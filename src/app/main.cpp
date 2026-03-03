#include "cli.hpp"
#include "re/error.hpp"
#include "re/lexer.hpp"

#include <iostream>
#include <string_view>
#include <vector>

namespace {

constexpr int kExitOk = 0;
constexpr int kExitCliError = 2;
constexpr int kExitRegexError = 3;
constexpr int kExitNotImplemented = 4;

/**
 * @brief Print the error message.
 *
 * @param e CLI error.
 */
void print_cli_error(const re::Error& e) {
  std::cerr << "error: " << e.message << "\n";
}

/**
 * @brief Print tokens.
 *
 * @param tokens Token stream.
 */
void print_tokens(std::vector<re::Token>& tokens) {
  for (auto token : tokens) {
    std::cout << token;
  }
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
      auto res = re::lex(cmd.pattern);
      if (res.is_ok()) print_tokens(res.value());
      return kExitOk;
    }
    case re::CommandKind::Ast:
    case re::CommandKind::Nfa:
    case re::CommandKind::MatchFull:
    case re::CommandKind::Search:
      std::cerr << "not implemented yet\n";
      return kExitNotImplemented;
  }

  return kExitCliError;
}
