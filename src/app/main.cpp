#include "cli.hpp"
#include "re/error.hpp"
#include "re/lexer.hpp"
#include "re/parser.hpp"
#include "re/nfa.hpp"
#include "re/match.hpp"

#include <iostream>
#include <string_view>

namespace {

constexpr int kExitOk = 0;
constexpr int kExitCliError = 2;
constexpr int kExitRegexError = 3;

/**
 * @brief Print the error message.
 *
 * @param e CLI error.
 */
void print_cli_error(const re::Error& e) {
  std::cerr << "error: " << e.message << "\n";
}

} // namespace

int main(int argc, char *argv[]) {
  auto cmdr = re::parse_args(argc, argv);

  if (!cmdr) {
    print_cli_error(cmdr.error());
    std::cerr << "hint: use --help\n";
    return kExitCliError;
  }

  const re::Command cmd = cmdr.value();

  switch (cmd.kind) {
    case re::CommandKind::Help:
      std::cout << re::help_text();
      return kExitOk; 

    case re::CommandKind::Version:
      std::cout << re::version_text();
      return kExitOk;
    case re::CommandKind::Lex: {
      // 1. Lexer
      auto tokens_res = re::lex(cmd.pattern);
      if (!tokens_res.is_ok()) {
        std::cerr << "Lex error at position " << tokens_res.error().pos
                  << ": " << tokens_res.error().message << "\n";
        return kExitRegexError;
      }

      // 2. Print tokens
      for (const auto& token : tokens_res.value()) {
        std::cout << token;
      }
      return kExitOk;
    }
    case re::CommandKind::Ast: {
      // 1. Lexer
      auto tokens_res = re::lex(cmd.pattern);
      if (!tokens_res.is_ok()) {
        std::cerr << "Lex error at position " << tokens_res.error().pos
                  << ": " << tokens_res.error().message << "\n";
        return kExitRegexError;
      }
      // 2. Parser
      auto ast_res = re::parse(tokens_res.value());
      if (!ast_res.is_ok()) {
        std::cerr << "Parse error at position " << ast_res.error().pos
                  << ": " << ast_res.error().message << "\n";
        return kExitRegexError;
      }
      // 3. Print AST
      std::cout << *(ast_res.value());
      return kExitOk;
    }
    case re::CommandKind::Nfa: {
      // 1. Lexer
      auto tokens_res = re::lex(cmd.pattern);
      if (!tokens_res.is_ok()) {
        std::cerr << "Lex error at position " << tokens_res.error().pos
                  << ": " << tokens_res.error().message << "\n";
        return kExitRegexError;
      }
      // 2. Parser
      auto ast_res = re::parse(tokens_res.value());
      if (!ast_res.is_ok()) {
        std::cerr << "Parse error at position " << ast_res.error().pos
                  << ": " << ast_res.error().message << "\n";
        return kExitRegexError;
      }
      // 3. Nfa
      auto nfa = re::nfa(ast_res.value());
      std::cout << nfa;

      return kExitOk;
    }
    case re::CommandKind::MatchFull: {
      // 1. Lexer
      auto tokens_res = re::lex(cmd.pattern);
      if (!tokens_res.is_ok()) {
        std::cerr << "Lex error at position " << tokens_res.error().pos
                  << ": " << tokens_res.error().message << "\n";
        return kExitRegexError;
      }
      // 2. Parser
      auto ast_res = re::parse(tokens_res.value());
      if (!ast_res.is_ok()) {
        std::cerr << "Parse error at position " << ast_res.error().pos
                  << ": " << ast_res.error().message << "\n";
        return kExitRegexError;
      }
      // 3. Nfa
      auto nfa = re::nfa(ast_res.value());

      // 4. Matcher
      bool matched = re::is_match(nfa, cmd.text);

      if (matched) {
        std::cout << "\033[1;32mMatch: TRUE\033[0m\n";
        return kExitOk;
      } else {
        std::cout << "\033[1;31mMatch: FALSE\033[0m\n";
        return kExitRegexError;
      }
    }
    case re::CommandKind::Search: {
      // 1. Lexer
      auto tokens_res = re::lex(cmd.pattern);
      if (!tokens_res.is_ok()) {
        std::cerr << "Lex error at position " << tokens_res.error().pos
                  << ": " << tokens_res.error().message << "\n";
        return kExitRegexError;
      }
      // 2. Parser
      auto ast_res = re::parse(tokens_res.value());
      if (!ast_res.is_ok()) {
        std::cerr << "Parse error at position " << ast_res.error().pos
                  << ": " << ast_res.error().message << "\n";
        return kExitRegexError;
      }
      // 3. Nfa
      auto nfa = re::nfa(ast_res.value());

      // 4. Matcher
      bool found = re::search(nfa, cmd.text);

      if (found) {
        std::cout << "\033[1;32mSearch: FOUND\033[0m\n";
        return kExitOk;
      } else {
        std::cout << "\033[1;31mSearch: NOT FOUND\033[0m\n";
        return 1;
      }
    }
  }

  return kExitCliError;
}
