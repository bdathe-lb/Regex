#include "re/parser.hpp"
#include "re/lexer.hpp"
#include "re/nfa.hpp"
#include "test.hpp"

#include <sstream>
#include <string>
#include <string_view>

namespace {

// Helper function：Lex -> Parse -> Compile -> String
std::string compile_to_string(std::string_view pattern) {
  auto lex_res = re::lex(pattern);
  if (!lex_res.is_ok()) return "LEX_ERROR";
  
  auto parse_res = re::parse(lex_res.value());
  if (!parse_res.is_ok()) return "PARSE_ERROR";

  auto nfa = re::nfa(parse_res.value());
  
  std::ostringstream oss;
  oss << nfa;
  return oss.str();
}

TEST("nfa/literal_and_dot") {
  std::string expected_a = 
      "[0] Consume('a') -> 1\n"
      "[1] Match\n";
  EXPECT_EQ(compile_to_string("a"), expected_a);

  std::string expected_dot = 
      "[0] Dot(.) -> 1\n"
      "[1] Match\n";
  EXPECT_EQ(compile_to_string("."), expected_dot);
}

TEST("nfa/concat") {
  std::string expected = 
      "[0] Consume('a') -> 1\n"
      "[1] Consume('b') -> 2\n"
      "[2] Match\n";
  EXPECT_EQ(compile_to_string("ab"), expected);
}

TEST("nfa/alt") {
  std::string expected = 
      "[0] Consume('a') -> 2\n"
      "[1] Consume('b') -> 2\n"
      "[2] Epsilon -> 4\n"
      "[3] Epsilon -> 0, 1\n"
      "[4] Match\n";
  EXPECT_EQ(compile_to_string("a|b"), expected);
}

TEST("nfa/repeat_star") {
  std::string expected = 
      "[0] Consume('a') -> 2\n"
      "[1] Epsilon -> 3\n"
      "[2] Epsilon -> 0, 1\n"
      "[3] Match\n";
  EXPECT_EQ(compile_to_string("a*"), expected);
}

TEST("nfa/repeat_plus") {
  std::string expected = 
      "[0] Consume('a') -> 3\n"
      "[1] Epsilon -> 4\n"
      "[2] Epsilon -> 0\n"
      "[3] Epsilon -> 0, 1\n"
      "[4] Match\n";
  EXPECT_EQ(compile_to_string("a+"), expected);
}

TEST("nfa/repeat_qmark") {
  std::string expected = 
      "[0] Consume('a') -> 1\n"
      "[1] Epsilon -> 3\n"
      "[2] Epsilon -> 0, 1\n"
      "[3] Match\n";
  EXPECT_EQ(compile_to_string("a?"), expected);
}

} // namespace
