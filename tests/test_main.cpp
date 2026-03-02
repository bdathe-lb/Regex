#include "test.hpp"

#include <algorithm>
#include <cstdlib>
#include <exception>
#include <string_view>
#include <vector>
#include <iostream>

namespace re::test {

namespace {

/**
 * @brief Print the help message for the test framework.
 *
 * @param argv0 CLI name.
 */
void print_help(const char* argv0) {
  std::cout
      << "Usage: " << argv0 << " [--list] [--filter SUBSTR]\n"
      << "  --list          List all tests\n"
      << "  --filter SUBSTR Run only tests whose name contains SUBSTR\n";
}

/**
 * @brief Check if the string s contains the substring sub.
 *
 * @param s String.
 * @param sub Substring.
 *
 * @return Returns true if contain, otherwise returns false.
 */
bool contains(std::string_view s, std::string_view sub) {
  return s.find(sub) != std::string_view::npos;
}

} // namespace

/**
 * @brief Create a test function registry.
 *
 * @return A static TestCase vector.
 */
std::vector<TestCase>& registry() {
  static std::vector<TestCase> reg;
  return reg;
}

/**
 * @brief Register test functions.
 *
 * @param name Test name.
 * @param fn Test function.
 */
void register_test(std::string_view name, void (*fn)()) {
  registry().push_back(TestCase{std::string(name), fn});
}

/**
 * @brief Get the number of test failures.
 *
 * @return The number of test failures.
 */
int& current_failures() {
  static int fails = 0;
  return fails;
}

/**
 * @brief Log a failure and print it.
 *
 * @param file File where the error occurred.
 * @param line Line number where the error occurred.
 * @param expr Expression that caused the error.
 * @param msg Error message.
 */
void record_failure(const char* file, int line, std::string_view expr, std::string_view msg) {
  ++current_failures();
  std::cerr << " " << file << ":" << line << "\n"
            << "    " << expr << "\n"
            << "    " << msg << "\n";
}

} // namespace re::test

// Main logic
int main(int argc, char *argv[]) {
  using namespace re::test;

  bool list_only = false;
  std::string filter;

  for (int i = 1; i < argc; ++ i) {
    std::string_view opt = argv[i];

    if (opt == "--help") {
      if (argc != 2) {
        std::cerr << "`--help` takes no arguments";
        return 2;
      }
      print_help(argv[0]);
      return 0;
    }

    if (opt == "--list") {
      if (argc != 2) {
        std::cerr << "`--list` takes no arguments";
        return 2;
      }
      list_only = true;
      continue;
    }

    if (opt == "--filter") {
      if (i + 1 >= argc) {
        std::cerr << "`--filter` requires an argument\n";
        return 2;
      }
      filter = argv[++ i];
      continue;
    }

    std::cerr << "Unknown arg: " << opt << "\n";
    print_help(argv[0]);
    return 2;
  }

  auto& reg = registry();
  std::sort(reg.begin(), reg.end(), 
            [](const TestCase& x, const TestCase& y){ return x.name < y.name; });

  if (list_only) {
    for (auto& t : reg) std::cout << t.name << "\n";
    return 0;
  }

  int total = 0;
  int failed_tests = 0;

  for (auto& t : reg) {
    if (!filter.empty() && !contains(t.name, filter)) continue;

    ++ total;
    current_failures() = 0;

    std::cerr << "[RUN ]" << t.name << "\n";
    try {
      t.fn();
    } catch (const AbortTest&) {
      // Nothing to do
    } catch (const std::exception& e) {
      record_failure(__FILE__, __LINE__, "unexpected std::exception", e.what());
    } catch (...) {
      record_failure(__FILE__, __LINE__, "unexpected exception", "unknown");
    }

    if (current_failures() == 0) {
      std::cerr << "[ OK ]" << t.name << "\n";
    } else {
      ++ failed_tests;
      std::cerr << "[FAIL]" << t.name << " (" << current_failures()
                << " failures)\n";
    }
  }

  std::cerr << "\n============ Summary ============\n"
            << "  tests run: " << total << "\n"
            << "  failed   : " << failed_tests << "\n";

  return failed_tests == 0 ? 0 : 1;
}
