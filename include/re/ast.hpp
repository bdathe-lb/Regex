#ifndef RE_AST_HPP
#define RE_AST_HPP

#include <cstddef>
#include <memory>
#include <utility>
#include <variant>
#include <vector>

namespace re {

// Forward declaration
struct Node;
// Owning pointer type for chiild nodes
using NodePtr = std::unique_ptr<Node>;

// ------------ Leaf nodes ------------
struct Literal { 
  char c; 
};

struct Dot { };

// --------- Composite nodes ---------
struct Concat { 
  std::vector<NodePtr> parts; 
};

struct Alt { 
  std::vector<NodePtr> options; 
};

struct Repeat {
  enum class Op { 
    Star, // *
    Plus, // +
    QMark // ?
  };

  NodePtr sub;
  Op op;
};

// ------------ Node wrapper ------------
struct Node {
  std::variant<Literal, Dot, Concat, Alt, Repeat> v;

  template<class T>
  explicit Node(T&& x) : v(std::forward<T>(x)) {}
};

using Ast = NodePtr;

// ------------ Factor helper ------------
NodePtr make_literal(char c);
NodePtr make_dot();
NodePtr make_concat(std::vector<NodePtr> parts);
NodePtr make_alt(std::vector<NodePtr> options);
NodePtr make_repeat(NodePtr sub, Repeat::Op op);

// ------------ Debug printing ------------
void print_ast(std::ostream& os, const Node& n, std::size_t indent = 0);
std::ostream& operator<<(std::ostream& os, const Node& n);

} // namespace re

#endif // !RE_AST_HPP
