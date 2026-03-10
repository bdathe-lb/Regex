#ifndef RE_PARSER_HPP
#define RE_PARSER_HPP

#include "re/ast.hpp"
#include "re/error.hpp"
#include "re/lexer.hpp"

#include <cstddef>
#include <span>

namespace re {

class Parser {
private:
  std::span<const Token> tokens_;
  std::size_t pos_;  // The index of the current token in tokens stream.

public:
  explicit Parser(std::span<const Token> tokens) : tokens_(tokens), pos_(0) {}
  Result<Ast> parse();

private:
  // ----------- Recursive descent core function -----------
  Result<Ast> parse_expr();
  Result<Ast> parse_alt();
  Result<Ast> parse_concat();
  Result<Ast> parse_repeat();
  Result<Ast> parse_primary();

  // ----------- Cursor control auxiliary function ------------
  // Has the cursor reached the end.
  bool is_at_end() const noexcept;
  // View the current token.
  const Token* peek() const noexcept;
  // Consume the current token and return it, incrementing the cursor by 1.
  const Token& advance() noexcept;
  // Check if the current token is the specified kind.
  bool match(TokenKind kind) noexcept;

  // Help construct an error at the current cursor position.
  Error make_error(std::string message) const;
};

// Interfaces exposed to external calls.
Result<Ast> parse(std::span<const Token> tokens);

} // namespace re

#endif // !RE_PARSER_HPP
