/*
 * include/re/lexer.hpp -- Lexical annalyzer, converts a character stream into a token stream.
*/

#ifndef RE_LEXER_HPP
#define RE_LEXER_HPP

#include "re/error.hpp"

#include <cstddef>
#include <ostream>
#include <string_view>
#include <vector>

namespace re {

enum class TokenKind {
  Char,
  LParen,
  RParen,
  Star,
  Plus,
  Or,
  Dot,
  QMark,
};

struct Token {
  TokenKind kind;   // Token kind.
  char ch;          // Used only for Char, set to 0 for others.
  std::size_t pos;  // The position of the token in the original string.
};

// Print the characters in a Token to a string, taking special characters (e.g., \n, \t) into account.
void print_char_escaped(std::ostream& os, const char ch);
// Human-readable output.
std::ostream& operator<<(std::ostream& os, const Token& token);
// Lexical annalyzer.
Result<std::vector<Token>> lex(std::string_view pattern);

}

#endif // !RE_LEXER_HPP
