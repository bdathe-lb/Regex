#include "re/lexer.hpp"
#include "re/error.hpp"

#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <vector>

namespace re {

namespace {
/**
 * Helper function
 *
 * @brief Get TokenKind name.
 *
 * @param kind Token kind.
 */
std::string_view token_kind_name(const TokenKind kind) {
  switch (kind) {
    case TokenKind::Char:   return "Char";
    case TokenKind::LParen: return "LParen";
    case TokenKind::RParen: return "RParen";
    case TokenKind::Star:   return "Star";
    case TokenKind::Plus:   return "Plus";
    case TokenKind::Or:     return "Or";
    case TokenKind::Dot:    return "Dot";
    case TokenKind::QMark:  return "QMark";
    default:                return "Unknown";
  }
  
  std::abort();
}

/**
 * Helper function
 *
 * @brief Get the hexadecimal character corresponding to the decimal value.
 *
 * @param v Decimal number (0-15).
 *
 * @return Hexadecimal character.
 */
char hex_digit(int v) {
  switch (v) {
    case 0:  return '0';
    case 1:  return '1';
    case 2:  return '2';
    case 3:  return '3';
    case 4:  return '4';
    case 5:  return '5';
    case 6:  return '6';
    case 7:  return '7';
    case 8:  return '8';
    case 9:  return '9';
    case 10: return 'A';
    case 11: return 'B';
    case 12: return 'C';
    case 13: return 'D';
    case 14: return 'E';
    case 15: return 'F';
    default: return 'X';
  }

  std::abort();
}

} // namespace


/**
 * @brief Print the characters in a Token to a string, 
 *        taking special characters (e.g., \n, \t) into account.
 *
 * @param os Standard output stream.
 * @param ch Characters stored in the Token.
 */
void print_char_escaped(std::ostream& os, const char ch) {
  if (ch == '\n') { os << "\\n";  return; }
  if (ch == '\t') { os << "\\t";  return; }
  if (ch == '\r') { os << "\\r";  return; }
  if (ch == '\\') { os << "\\\\"; return; }
  if (ch == '\'') { os << "\\'";  return; }
  if (ch == '\0') { os << "\\0";  return; }

  unsigned char uc = static_cast<unsigned char>(ch);

  if (std::isprint(uc)) {
    os << ch;
    return;
  } else {
    int hi = (uc >> 4) &0xF;
    int lo = uc & 0xF;
    os << "\\x";
    os << hex_digit(hi);
    os << hex_digit(lo);
  }
}

// Human-readable output.
std::ostream& operator<<(std::ostream& os, const re::Token& token) {
  switch (token.kind) {
    case TokenKind::Char:
      os << std::setw(2) << token.pos << ": ";
      os << token_kind_name(token.kind);
      os << "('";
      print_char_escaped(os, token.ch);
      os << "')";
      os << "\n";
      break;
    case TokenKind::LParen:
      os << std::setw(2) << token.pos << ": ";
      os << token_kind_name(token.kind);
      os << "\n";
      break;
    case TokenKind::RParen:
      os << std::setw(2) << token.pos << ": ";
      os << token_kind_name(token.kind);
      os << "\n";
      break;
    case TokenKind::Star:
      os << std::setw(2) << token.pos << ": ";
      os << token_kind_name(token.kind);
      os << "\n";
      break;
    case TokenKind::Plus:
      os << std::setw(2) << token.pos << ": ";
      os << token_kind_name(token.kind);
      os << "\n";
      break;
    case TokenKind::Or:  
      os << std::setw(2) << token.pos << ": ";
      os << token_kind_name(token.kind);
      os << "\n";
      break;
    case TokenKind::Dot: 
      os << std::setw(2) << token.pos << ": ";
      os << token_kind_name(token.kind);
      os << "\n";
      break;
    case TokenKind::QMark: 
      os << std::setw(2) << token.pos << ": ";
      os << token_kind_name(token.kind);
      os << "\n";
      break;
  }

  return os;
}

/**
 * @brief Converts a character stream into a token stream.
 *
 * @param pattern Characters stream.
 *
 * @return Token stream.
 */
re::Result<std::vector<Token>> lex(std::string_view pattern) {
  std::vector<Token> tokens;

  // Prevent frequent capacity expansion
  tokens.reserve(pattern.size());
  for (size_t i = 0; i < pattern.size(); ++ i) {
    if (pattern[i] == '(')       tokens.push_back(Token{TokenKind::LParen, 0, i});
    else if (pattern[i] == ')')  tokens.push_back(Token{TokenKind::RParen, 0, i});
    else if (pattern[i] == '|')  tokens.push_back(Token{TokenKind::Or, 0, i});
    else if (pattern[i] == '*')  tokens.push_back(Token{TokenKind::Star, 0, i});
    else if (pattern[i] == '+')  tokens.push_back(Token{TokenKind::Plus, 0, i});
    else if (pattern[i] == '.')  tokens.push_back(Token{TokenKind::Dot, 0, i});
    else if (pattern[i] == '?')  tokens.push_back(Token{TokenKind::QMark, 0, i});
    else if (pattern[i] == '\\') {
      if (i + 1 == pattern.size()) return Result<std::vector<Token>>::fail(Error{i, "dangling escape"});
      else {
        char c = pattern[i+1];
        tokens.push_back(Token{TokenKind::Char, c, i});
        ++ i;
      }
    }
    else tokens.push_back(Token{TokenKind::Char, pattern[i], i});
  }

  return Result<std::vector<Token>>::ok(tokens);
}

} // namespace re
