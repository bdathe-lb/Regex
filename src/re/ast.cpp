#include "re/ast.hpp"

#include <cstddef>
#include <memory>
#include <type_traits>
#include <variant>
#include <vector>
#include <utility>

namespace re {

NodePtr make_literal(char c) {
  return std::make_unique<Node>(Literal{c});
}

NodePtr make_dot() {
  return std::make_unique<Node>(Dot{});
}

NodePtr make_concat(std::vector<NodePtr> parts) {
  return std::make_unique<Node>(Concat{std::move(parts)});
}

NodePtr make_alt(std::vector<NodePtr> options) {
  return std::make_unique<Node>(Alt{std::move(options)});
}

NodePtr make_repeat(NodePtr sub, Repeat::Op op) {
  return std::make_unique<Node>(Repeat{std::move(sub), op});
}

void print_ast(std::ostream& os, const Node& n, std::size_t indent) {
  std::string ind(indent, ' ');

  // std::visit automatically deduces the type of auto& arg based on the 
  // actual type stored in the variant n.v at runtime.
  std::visit([&os, indent, &ind](const auto& arg) {
    // Remove type qualifiers (e.g., const, &, etc.) to facilitate subsequent comparisons
    // Note: Qualifiers like const, volatile, and references (&) can affect the result
    // decltype(arg): Get the declared type of the expression arg
    // decay_t<arg>: Perform type stripping to remove qualifiers
    using T = std::decay_t<decltype(arg)>;

    // If a regular if is used, when the code reaches the else if (std::is_same_v<T, Concat>) branch, 
    // it will attempt to compile arg.parts. However, at that point, arg could be of type Literal, 
    // and the Literal struct does not have a member named parts. The compiler would then report an 
    // error: "Literal has no member named parts".
    // Using if constexpr tells the compiler: if this condition is not met at compile time, 
    // treat the code within the braces as if it doesn't exist—don't even perform syntax checking!
    if constexpr (std::is_same_v<T, Literal>) {
      os << ind << "Literal('" << arg.c << "')\n";
    }
    else if constexpr (std::is_same_v<T, Dot>) {
      os << ind  << "Dot(.)\n";
    }
    else if constexpr (std::is_same_v<T, Concat>) {
      os << ind << "Concat:\n";
      for (const auto& part : arg.parts) {
        print_ast(os, *part, indent + 2);
      }
    }
    else if constexpr (std::is_same_v<T, Alt>) {
      os << ind << "Alt:\n";
      for (const auto& option : arg.options) {
        print_ast(os, *option, indent + 2);
      }
    }
    else if constexpr (std::is_same_v<T, Repeat>) {
      os << ind << "Repeat(";
      if (arg.op == Repeat::Op::Star)       os << "*):\n";
      else if (arg.op == Repeat::Op::Plus)  os << "+):\n";
      else if (arg.op == Repeat::Op::QMark) os << "?):\n";

      print_ast(os, *arg.sub, indent + 2);
    }
  }, n.v);
}

std::ostream& operator<<(std::ostream& os, const Node& n) {
  print_ast(os, n);
  return os;
}

}  // namespace re
