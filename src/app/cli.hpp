#ifndef RE_CLI_HPP
#define RE_CLI_HPP

#include "re/error.hpp"
#include <string_view>

namespace re {

enum class CommandKind {
  Help,
  Version,
  Lex,
  Ast,
  Nfa,
  MatchFull,
  Search,
};

struct Command {
  CommandKind kind{};
  std::string_view pattern{};
  std::string_view text{};
};

// Parse the input arguments and return a Command.
Result<Command> parse_args(int argc, char **argv);
// Help information for `--help`.
std::string_view help_text();
// Version information for `--version`.
std::string_view version_text();

} // namespace re

#endif // !RE_CLI_HPP
