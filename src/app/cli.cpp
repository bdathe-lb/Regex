#include "cli.hpp"
#include "re/error.hpp"

#include <string_view>
#include <vector>
#include <cstddef>

namespace re {

namespace {

/**
 * Helper function
 *
 * @brief Convert argv to a list of string_views.
 *
 * @param argc Number of arguments.
 * @param argv Array of argument strings.
 */
std::vector<std::string_view> to_args(int argc, char **argv) {
  std::vector<std::string_view> out;

  size_t reserve_space = static_cast<size_t>(argc - 1);
  out.reserve(reserve_space);

  for (int i = 1; i < argc; ++ i) {
    out.emplace_back(argv[i]);
  }

  return out;
}

/**
 * Helper function
 *
 * @brief Construct a parsing error.
 *
 * @param msg Error message.
 * 
 * @return Parsing Error.
 */
Error cli_error(std::string_view msg) {
  return Error{0, std::string(msg)};
}

} // namespace

Result<Command> parse_args(int argc, char **argv) {
  auto args = to_args(argc, argv);

  // Convention: no arguments is equivalent to `--help`.
  if (args.empty()) {
    return Result<Command>::ok(Command{CommandKind::Help});
  }

  // Convention: only one primary option is allowed.
  const auto opt = args[0];

  if (opt == "--help") {
    if (args.size() != 1) {
      Error err = cli_error("`--help` takes no arguments");
      return Result<Command>::fail(err);
    }
    return Result<Command>::ok(Command{CommandKind::Help});
  }

  if (opt == "--version") {
    if (args.size() != 1) {
      Error err = cli_error("`--version` takes no arguments");
      return Result<Command>::fail(err);
    }
    return Result<Command>::ok(Command{CommandKind::Version});
  }

  // TODO: 
  if (opt == "--lex") {
    if (args.size() != 2) {
      Error err = cli_error("usage: re --lex <pattern>");
      return Result<Command>::fail(err);
    }
    return Result<Command>::ok(Command{CommandKind::Lex, args[1], {}});
  }

  // TODO: 
  if (opt == "--ast") {
      if (args.size() != 2) {
        Error err = cli_error("usage: re --ast <pattern>");
        return Result<Command>::fail(err);
      }
      return Result<Command>::ok(Command{CommandKind::Ast, args[1], {}});
  }

  // TODO:
  if (opt == "--nfa") {
      if (args.size() != 2) {
        Error err = cli_error("usage: re --nfa <pattern>");
        return Result<Command>::fail(err);
      }
      return Result<Command>::ok(Command{CommandKind::Nfa, args[1], {}});
  }

  // TODO:
  if (opt == "--match") {
    if (args.size() != 3) {
      Error err = cli_error("usage: re --match <pattern> <text>");
      return Result<Command>::fail(err);
    }
    return Result<Command>::ok(Command{CommandKind::MatchFull, args[1], args[2]});
  }

  // TODO:
  if (opt == "--search") {
    if (args.size() != 3) {
      Error err = cli_error("usage: re --search <pattern> <text>");
      return Result<Command>::fail(err);
    }
    return Result<Command>::ok(Command{CommandKind::Search, args[1], args[2]});
  }

  return Result<Command>::fail(cli_error("unknown option (use --help)"));
}

/**
 * @brief Help message.
 *
 * @return Help message string view.
 */
std::string_view help_text() {
  return
R"(re - a tiny regex engine

Usage:
  re --help
  re --version

  re --lex   <pattern>        (not implemented yet)
  re --ast   <pattern>        (not implemented yet)
  re --nfa   <pattern>        (not implemented yet)
  re --match <pattern> <text> (not implemented yet)
  re --search <pattern> <text> (not implemented yet)

Exit codes:
  0 success
  2 CLI usage error
  3 regex parse/compile error
  4 not implemented
)";
}

/**
 * @brief Version message.
 *
 * @return Version message string view.
 */
std::string_view version_text() {
  return "re 0.0.1\n";
}

} // namespace re
