#ifndef RE_NFA_HPP
#define RE_NFA_HPP

#include "re/ast.hpp"

#include <cstddef>
#include <limits>
#include <ostream>

// Thompson Alggorithm construct NFA:
// 1. empty: ε
// ○ ──ε──> ◎
//
// 2. single character: a
// ○ ──a──> ◎
//
// 3. concat: ab
// ○ ──a──> ○ ──ε──> ○ ──b──> ◎
//
// 4. select: a|b
//
//    ┌──ε──> ○ ──a──> ○ ──ε──┐
// ○ ─┤                       ├─> ◎
//    └──ε──> ○ ──b──> ○ ──ε──┘
//
// 5. closure: a*
//           ┌────ε─────┐
//           v          │
// ○ ──ε───> ○ ───a───> ○ ──ε───> ◎
// │                              ^
// └───────────ε──────────────────┘
//
// 6. closure: a+
//           ┌────ε─────┐
//           v          │
// ○ ──ε───> ○ ───a───> ○ ──ε───> ◎
//
// 7. closure: a?
// ○ ──ε───> ○ ───a───> ○ ──ε───> ◎
// │                              ^
// └───────────ε──────────────────┘

namespace re {

// Use integer indices to represent the position of a state in the NFA::states array
using StateID = std::size_t;
// Define a constant to represent an "invalid state"
constexpr StateID kNullState = std::numeric_limits<StateID>::max();

enum class StateType {
  Consume, // Consume the specified character (c) and transition to out1
  Dot,     // Consume any single character and transition to out1
  Epsilon, // Do not consume any characters. Unconditional jump
  Match    // Final accepting state
};

struct State {
  StateType type;
  char c;
  StateID out1{kNullState};
  StateID out2{kNullState};
};

struct NFA {
  std::vector<State> states;
  StateID start{kNullState};  // First outgoing edge
  StateID accept{kNullState}; // Second outgoing edge

  // Helper function: Add a new state into state pool and returns it's ID.
  StateID add_state(StateType type, char c = 0, 
                    StateID out1 = kNullState, 
                    StateID out2 = kNullState) {
    states.push_back({type, c, out1, out2});
    return states.size() - 1;
  }
};

// Compile AST to NFA
NFA nfa(const Ast& ast);

// Debug
std::ostream& operator<<(std::ostream& os, const NFA& nfa);

} // namespace re

#endif // !RE_NFA_HPP
