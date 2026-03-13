#include "re/match.hpp"
#include "re/nfa.hpp"
#include "re/parser.hpp"
#include "re/lexer.hpp"
#include "test.hpp"

#include <string_view>

namespace {

bool is_match(std::string_view pattern, std::string_view text) {
  auto lex_res = re::lex(pattern);
  if (!lex_res.is_ok()) return false;
  
  auto parse_res = re::parse(lex_res.value());
  if (!parse_res.is_ok()) return false;

  auto nfa = re::nfa(parse_res.value());
  return re::is_match(nfa, text);
}

TEST("matcher/basic_concat") {
  EXPECT_TRUE(is_match("abc", "abc"));
  EXPECT_FALSE(is_match("abc", "ab"));
  EXPECT_FALSE(is_match("abc", "abcd"));
  EXPECT_FALSE(is_match("abc", "xabc"));
}

TEST("matcher/dot") {
  EXPECT_TRUE(is_match("a.c", "abc"));
  EXPECT_TRUE(is_match("a.c", "a-c"));
  EXPECT_FALSE(is_match("a.c", "ac"));
  EXPECT_FALSE(is_match("a.c", "abbc"));
}

TEST("matcher/alt") {
  EXPECT_TRUE(is_match("a|b", "a"));
  EXPECT_TRUE(is_match("a|b", "b"));
  EXPECT_FALSE(is_match("a|b", "c"));
  EXPECT_FALSE(is_match("a|b", "ab"));
  
  EXPECT_TRUE(is_match("a(b|c)d", "abd"));
  EXPECT_TRUE(is_match("a(b|c)d", "acd"));
  EXPECT_FALSE(is_match("a(b|c)d", "ad"));
}

TEST("matcher/qmark") {
  EXPECT_TRUE(is_match("a?b", "b"));
  EXPECT_TRUE(is_match("a?b", "ab"));
  EXPECT_FALSE(is_match("a?b", "aab"));
}

TEST("matcher/plus") {
  EXPECT_TRUE(is_match("a+b", "ab"));
  EXPECT_TRUE(is_match("a+b", "aab"));
  EXPECT_TRUE(is_match("a+b", "aaaaab"));
  EXPECT_FALSE(is_match("a+b", "b"));
}

TEST("matcher/star") {
  EXPECT_TRUE(is_match("a*b", "b"));
  EXPECT_TRUE(is_match("a*b", "ab"));
  EXPECT_TRUE(is_match("a*b", "aaab"));
  EXPECT_FALSE(is_match("a*b", "c"));
}

TEST("matcher/complex") {
  EXPECT_TRUE(is_match("(a|b)*c", "c"));
  EXPECT_TRUE(is_match("(a|b)*c", "ac"));
  EXPECT_TRUE(is_match("(a|b)*c", "bc"));
  EXPECT_TRUE(is_match("(a|b)*c", "aabac"));
  EXPECT_FALSE(is_match("(a|b)*c", "aab"));
}

TEST("matcher/redos_immunity") {
  EXPECT_FALSE(is_match("(a*)*b", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaac"));
  EXPECT_TRUE(is_match("(a*)*b", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));
}

} // namespace
