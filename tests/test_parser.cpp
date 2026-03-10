#include "re/error.hpp"
#include "re/lexer.hpp"
#include "re/parser.hpp"
#include "test.hpp"

#include <sstream>
#include <string_view>

namespace {

std::string parse_to_string(std::string_view pattern) {
  // 1. Lexer
  auto lex_result = re::lex(pattern);
  if (!lex_result.is_ok()) {
    return "LEX_ERROR";
  }

  // 2. Parser
  auto parse_result = re::parse(lex_result.value());
  if (!parse_result.is_ok()) {
    return "PARSE_ERROR" + parse_result.error().message;
  }

  // 3. To string
  std::ostringstream oss;
  oss << *(parse_result.value());
  return oss.str();
}

TEST("parser/literal") {
  std::string expected = "Literal('a')\n";
  EXPECT_EQ(parse_to_string("a"), expected);
}

TEST("parser/concat") {
  std::string expected = 
      "Concat:\n"
      "  Literal('a')\n"
      "  Literal('b')\n"
      "  Literal('c')\n";
  EXPECT_EQ(parse_to_string("abc"), expected);
}

TEST("parser/precedence_star_concat") {
  std::string expected = 
      "Concat:\n"
      "  Repeat(*):\n"
      "    Literal('a')\n"
      "  Literal('b')\n";
  EXPECT_EQ(parse_to_string("a*b"), expected);
}

TEST("parser/precedence_alt_concat") {
  std::string expected = 
      "Alt:\n"
      "  Literal('a')\n"
      "  Concat:\n"
      "    Literal('b')\n"
      "    Literal('c')\n";
  EXPECT_EQ(parse_to_string("a|bc"), expected);
}

TEST("parser/error_unclosed_paren") {
  std::string res = parse_to_string("(ab");
  ASSERT_TRUE(res.find("Expected ')'") != std::string::npos);
}

} // namespace
