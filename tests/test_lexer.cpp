#include "re/error.hpp"
#include "test.hpp"
#include "re/lexer.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace {

static std::vector<re::Token> lex_ok(std::string_view pattern) {
  auto r = re::lex(pattern);
  ASSERT_TRUE(r);
  return r.value();
}

static re::Error lex_fail(std::string_view pattern) {
  auto r = re::lex(pattern);
  ASSERT_TRUE(!r);
  return r.error();
}

static void expect_tokens(const std::vector<re::Token>& got,
                          const std::vector<re::Token>& exp) {
  ASSERT_EQ(got.size(), exp.size());
  for (std::size_t i = 0; i < exp.size(); ++i) {
    EXPECT_EQ(got[i].kind, exp[i].kind);
    EXPECT_EQ(got[i].ch, exp[i].ch);
    EXPECT_EQ(got[i].pos, exp[i].pos);
  }
}

TEST("lex/empty") {
  auto toks = lex_ok("");
  EXPECT_EQ(toks.size(), 0u);
}

TEST("lex/single-char") {
  auto toks = lex_ok("a");
  EXPECT_EQ(toks.size(), 1u);
  expect_tokens(toks, {{re::TokenKind::Char, 'a', 0}});
}

TEST("lex/simple-mix") {
  // pattern: (ab|c)*
  auto toks = lex_ok("(ab|c)*");

  expect_tokens(toks, {
    {re::TokenKind::LParen, 0, 0},
    {re::TokenKind::Char,   'a', 1},
    {re::TokenKind::Char,   'b', 2},
    {re::TokenKind::Or,     0, 3},
    {re::TokenKind::Char,   'c', 4},
    {re::TokenKind::RParen, 0, 5},
    {re::TokenKind::Star,   0, 6},
  });
}

TEST("lex/metacharacters") {
  auto toks = lex_ok("()|*+?.");

  expect_tokens(toks, {
    {re::TokenKind::LParen, 0, 0},
    {re::TokenKind::RParen, 0, 1},
    {re::TokenKind::Or, 0, 2},
    {re::TokenKind::Star, 0, 3},
    {re::TokenKind::Plus, 0, 4},
    {re::TokenKind::QMark, 0, 5},
    {re::TokenKind::Dot, 0, 6},
  });
} 

TEST("lex/escape") {
  auto toks = lex_ok("a\\*b");

  expect_tokens(toks, {
    {re::TokenKind::Char, 'a', 0},
    {re::TokenKind::Char, '*', 1},
    {re::TokenKind::Char, 'b', 3},
  });
}

TEST("lex/dangling-escape") {
  auto err = lex_fail("\\");
  ASSERT_TRUE(err.pos == 0);

  ASSERT_TRUE(err.message.find("escape") != std::string::npos);
}

} // namespace
