#include "re/parser.hpp"
#include "re/ast.hpp"
#include "re/error.hpp"
#include "re/lexer.hpp"

#include <cassert>
#include <cstddef>
#include <vector>

namespace re {

/**
 * @brief Read tokens stream and build AST.
 */
Result<Ast> Parser::parse() {
  Result<Ast> ast_result = parse_expr();

  if (!ast_result.is_ok()) {
    return ast_result;
  }

  if (!is_at_end()) {
    return Result<Ast>::fail(make_error("Unexpected trailing characters"));
  }

  return ast_result;
}

/**
 * @brief Check if the cursor reached the end.
 *
 * @return Ture if reached, false otherwise.
 */
bool Parser::is_at_end() const noexcept {
  return pos_ >= tokens_.size();
}

/**
 * @brief Returns the address of the token currently pointed to by pos_.
 *
 * @return If the value is out of bounds, return nullptr;
 *         otherwise, return the token address pointed to by pos_.
 */
const Token* Parser::peek() const noexcept {
  if (is_at_end()) return nullptr;
  return &tokens_[pos_];
}

/**
 * @brief Consume the current token and return it, incrementing the cursor by 1.
 *
 * @return The token currently pointed to by pos_.
 */
const Token& Parser::advance() noexcept {
  assert(!is_at_end());
  return tokens_[pos_ ++];
}

/**
 * @brief Check if the current token is the specified kind.
 *
 * @param kind Token kind.
 *
 * @return If the type matches, consume the current token, increment pos by 1, and return true; 
 *         otherwise, return false.
 */
bool Parser::match(TokenKind kind) noexcept {
  if (is_at_end()) return false;

  if (tokens_[pos_].kind == kind) {
    advance();
    return true;
  }

  return false;
}

/**
 * @brief Help construct an error at the current cursor position.
 *
 * @param message Error message.
 *
 * @return Error object, tokens_.size() indicates crossing the boundary.
 */
Error Parser::make_error(std::string message) const {
  if (is_at_end()) {
    // If the boundary is exceeded, increment the position of the last token by 1 (indicating an error at the end).
    std::size_t last_pos = tokens_.empty() ? 0 : tokens_.back().pos + 1;
    return Error{last_pos, message};
  }

  // Retrieve the character position of the original string in the current Token record.
  return Error{tokens_[pos_].pos, message};
}

/**
 * @brief Interfaces exposed to external calls, Read tokens stream and build AST.
 *
 * @param tokens Tokens stream.
 */
Result<Ast> parse(std::span<const Token> tokens) {
  return Parser(tokens).parse();
}

// ---------------------------------------------------------
// EBNF:
// expr     := alt ;
// alt      := concat ('|' concat)* ;
// concat   := repeat+ ;
// repeat   := primary ('*' | '+' | '?') ;
// primary  := char | '.' | '(' expr ')' ;
// ---------------------------------------------------------
/**
 * @brief Analyzing the smallest unit.
 * primary  := char | '.' | '(' expr ')' ;
 */
Result<Ast> Parser::parse_primary() {
  if (!peek()) {
    return Result<Ast>::fail(make_error("Unexpected end of input"));
  }

  if (match(TokenKind::Char)) {
    return Result<Ast>::ok(make_literal(tokens_[pos_-1].ch));
  }

  if (match(TokenKind::Dot)) {
    return Result<Ast>::ok(make_dot());
  }

  if (match(TokenKind::LParen)) {
    auto expr_ast = parse_expr(); 
    if (!expr_ast.is_ok()) {
      return expr_ast;
    }

    if (!match(TokenKind::RParen)) {
      return Result<Ast>::fail(make_error("Expected ')' after expression"));
    }

    return expr_ast;
  }

  return Result<Ast>::fail(make_error("Unexpected token"));
}

/**
 * @brief Processing *, +, ?.
 * repeat   := primary ('*' | '+' | '?') ;
 */
Result<Ast> Parser::parse_repeat() {
  auto sub_ast = parse_primary();
  if (!sub_ast.is_ok()) {
    return sub_ast;
  }

  auto node = std::move(sub_ast.value());
  
  if (match(TokenKind::Star)) {
    return Result<Ast>::ok(make_repeat(std::move(node), Repeat::Op::Star)); 
  } else if (match(TokenKind::QMark)) {
    return Result<Ast>::ok(make_repeat(std::move(node), Repeat::Op::QMark)); 
  } else if (match(TokenKind::Plus)) {
    return Result<Ast>::ok(make_repeat(std::move(node), Repeat::Op::Plus)); 
  }

  return Result<Ast>::ok(std::move(node));
}


/**
 * @brief Handling implicit splicing.
 * concat   := repeat+ | ε;
 */
Result<Ast> Parser::parse_concat() {
  std::vector<NodePtr> parts;

  while (true) {
    // No tokens available to read
    if (is_at_end()) break;
    // The current connection operation (e.g. ab in (ab)) has ended
    if (peek()->kind == TokenKind::Or) break;
    // The current layer (e.g., ab in (ab)) has been closed
    if (peek()->kind == TokenKind::RParen) break;

    auto rep_ast = parse_repeat();
    if (!rep_ast.is_ok()) {
      return rep_ast;
    }

    parts.push_back(std::move(rep_ast.value()));
  }

  if (parts.size() == 1) {
    return Result<Ast>::ok(std::move(parts[0]));
  }

  // This includes two cases: parts.size() > 1 and parts.empty()
  return Result<Ast>::ok(make_concat(std::move(parts)));
}

/**
 * @brief Processing branch '|'.
 * alt      := concat ('|' concat)* ;
 */
Result<Ast> Parser::parse_alt() {
  std::vector<NodePtr> options; 

  auto concat_ast = parse_concat();
  if (!concat_ast.is_ok()) {
    return concat_ast;
  }

  options.push_back(std::move(concat_ast.value()));

  while (match(TokenKind::Or)) {
    auto concat_ast = parse_concat();
    if (!concat_ast.is_ok()) {
      return concat_ast;
    }
    options.push_back(std::move(concat_ast.value()));
  }

  if (options.size() == 1) {
    return Result<Ast>::ok(std::move(options[0]));
  }

  return Result<Ast>::ok(make_alt(std::move(options)));
}

/**
 * @brief Processing expression.
 * expr     := alt ;
 */
Result<Ast> Parser::parse_expr() {
  return parse_alt();
}

} // namespace re
