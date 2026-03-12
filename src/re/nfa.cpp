#include "re/nfa.hpp"
#include "re/ast.hpp"

#include <cstddef>
#include <variant>
#include <vector>

namespace re {

namespace {

struct Fragment {
  StateID start; // Entry state of local graph
  StateID end;   // Exit state of local graph
};

class NfaCompiler {
public:
  NFA nfa_;

public:
  // Receive an AST node, return the compiled local fragment.
  Fragment compile_node(const Node& node) {
    return std::visit([this](const auto& arg) -> Fragment {
      using T = std::decay_t<decltype(arg)>;

      if constexpr (std::is_same_v<T, Literal>) {
        return compile_literal(arg);
      }
      if constexpr (std::is_same_v<T, Dot>) {
        return compile_dot(arg);
      }
      if constexpr (std::is_same_v<T, Concat>) {
        return compile_concat(arg);
      }
      if constexpr (std::is_same_v<T, Alt>) {
        return compile_alt(arg);
      }
      if constexpr (std::is_same_v<T, Repeat>) {
        return compile_repeat(arg);
      }

      return Fragment{kNullState, kNullState};
    }, node.v);
  }

private:
  // (Fragment.start)
  //       │
  //       ▼
  // [ Consume('a') ] ──out1──> (Fragment.end) 
  Fragment compile_literal(const Literal& lit) {
    StateID id = nfa_.add_state(StateType::Consume, lit.c);
    return Fragment{id, id};
  }

  // (Fragment.start)
  //       │
  //       ▼
  // [ Dot(.) ] ──out1──> (Fragment.end) 
  Fragment compile_dot(const Dot& dot) {
    (void) dot;
    StateID id = nfa_.add_state(StateType::Dot);
    return Fragment{id, id};
  }

  // (Fragment.start)
  //       │
  //       ▼
  // [ sub_frag_1 ] ──out1──> [ sub_frag_2 ] ──out1──> [ sub_frag_? ] ──out1──> (Fragment.end)
  Fragment compile_concat(const Concat& concat) {
    // Case 1: Epsilon
    if (concat.parts.empty()) {
      StateID id = nfa_.add_state(StateType::Epsilon);
      return Fragment{id, id};
    }

    // Case 2: Otherwise
    // Received the first part, as the initial large block
    Fragment result = compile_node(*concat.parts[0]);
    for (std::size_t i = 1; i < concat.parts.size(); ++ i) {
      Fragment next_frag = compile_node(*concat.parts[i]);

      // The tail of the large block points to the Start of the new part
      nfa_.states[result.end].out1 = next_frag.start;
      // Update the tail of the large block
      result.end = next_frag.end;
    }

    return result;
  }

  // (Fragment.start)
  //       │
  //       ▼
  // [ in_id: Epsilon (Split) ]
  //       │
  //       ├──out1──> [ sub_frag_1 ] ──out1──┐
  //       │                                 │
  //       └──out2──> [ sub_frag_2 ] ──out1──┤
  //                                         ▼
  //                               [ out_id: Epsilon ] ──out1──> (Fragment.end)
  Fragment compile_alt(const Alt& alt) {
    Fragment result = compile_node(*alt.options[0]);
    for (std::size_t i = 1; i < alt.options.size(); ++ i) {
      Fragment next_frag = compile_node(*alt.options[i]);
      StateID out_id = nfa_.add_state(StateType::Epsilon);
      StateID in_id = nfa_.add_state(StateType::Epsilon, 0, result.start, next_frag.start);

      nfa_.states[result.end].out1 = out_id;
      nfa_.states[next_frag.end].out1 = out_id;

      result = Fragment{in_id, out_id};
    }

    return result;
  }

  // Star(*):
  // (Fragment.start)
  //       │
  //       ▼
  // [ in_id: Epsilon (Split) ] <────────────┐
  //       │                                 │
  //       ├──out1──> [ sub_frag ] ──out1────┘
  //       │
  //       └──out2──> [ out_id: Epsilon ] ──out1──> (Fragment.end)
  //
  // Plus(+)
  // (Fragment.start)
  //       │
  //       ▼
  // [ in_id: Epsilon ] <─────────────────┐
  //       │                              │
  //       ▼                              │
  // [ sub_frag ] ──out1──┐               │
  //       ┌──────────────┘               │
  //       ▼                              │
  // [ loop_id: Epsilon (Split) ] ──out1──┘
  //       │
  //       └──out2──> [ out_id: Epsilon ] ──out1──> (Fragment.end)
  // 
  // QMark(?)
  // (Fragment.start)
  //       │
  //       ▼
  // [ in_id: Epsilon (Split) ]
  //       │
  //       ├──out1──> [ sub_frag ] ──out1──────────────┐
  //       │                                           │
  //       └──out2─────────────────────────────────────┤
  //                                                   ▼
  //                                         [ out_id: Epsilon ] ──out1──> (Fragment.end)
  Fragment compile_repeat(const Repeat& rep) {
    Fragment sub_frag = compile_node(*rep.sub);
    StateID out_id = nfa_.add_state(StateType::Epsilon);
    StateID in_id = nfa_.add_state(StateType::Epsilon);

    switch (rep.op) {
      case Repeat::Op::Star: {
        nfa_.states[in_id].out1 = sub_frag.start;
        nfa_.states[in_id].out2 = out_id;

        nfa_.states[sub_frag.end].out1 = in_id;
        break;
      } 
      case Repeat::Op::Plus: {
        StateID loop_id = nfa_.add_state(StateType::Epsilon);

        nfa_.states[in_id].out1 = sub_frag.start;
        nfa_.states[sub_frag.end].out1 = loop_id;

        nfa_.states[loop_id].out1 = sub_frag.start;
        nfa_.states[loop_id].out2 = out_id;
        break;
      }
      case Repeat::Op::QMark: {
        nfa_.states[in_id].out1 = sub_frag.start;
        nfa_.states[in_id].out2 = out_id;

        nfa_.states[sub_frag.end].out1 = out_id;
        break;
      }
    }

    return Fragment{in_id, out_id};
  }
};

} // namespace

// Compile AST to NFA
NFA nfa(const Ast& ast) {
  NfaCompiler compiler;
  Fragment frag = compiler.compile_node(*ast);
 
  StateID end_id = compiler.nfa_.add_state(StateType::Match);
  compiler.nfa_.states[frag.end].out1 = end_id;

  compiler.nfa_.start = frag.start;
  compiler.nfa_.accept = end_id; 

  return std::move(compiler.nfa_);
}

std::ostream& operator<<(std::ostream& os, const NFA& nfa) {
  for (std::size_t i = 0; i < nfa.states.size(); ++ i) {
    auto& state = nfa.states[i];
    switch (state.type) {
      case StateType::Consume:
        os << "[" << i << "] " 
           << "Consume('" << state.c << "') -> " 
           << state.out1 << "\n";
        break;
      case StateType::Dot:
        os << "[" << i << "] " 
           << "Dot(.) -> "
           << state.out1 << "\n";
        break;
      case StateType::Epsilon:
        os << "[" << i << "] "
           << "Epsilon -> "
           << state.out1;
        if (state.out2 != kNullState) os << ", " << state.out2 << "\n";
        else os << "\n";
        break;
      case StateType::Match:
        os << "[" << i << "] "
           << "Match\n";
        break;
    }
  }

  return os;
}

} // namespace re
