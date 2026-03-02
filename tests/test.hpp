#ifndef TEST_HPP
#define TEST_HPP

#include <string>
#include <string_view>
#include <vector>

namespace re::test {

struct TestCase {
  std::string name;
  void (*fn)();
};

// Create a test function registry.
std::vector<TestCase>& registry();
// Register test functions.
void register_test(std::string_view name, void (*fn)());

// Get the total number of test failures.
int& current_failures();
// Log a failure and print it.
void record_failure(const char* file, int line, std::string_view expr, std::string_view msg);

// Used to abort the current test after an ASSERT_* failure.
struct AbortTest final {};
} // namespace re::test

// ------------ Helper for unique names ------------
#define RE_TEST_JOIN2(a, b) a##b
#define RE_TEST_JOIN(a, b) RE_TEST_JOIN2(a, b)

// ----------- TEST registration macro -----------
#define TEST(name_literal)                                                    \
  static void RE_TEST_JOIN(re_test_fn_, __LINE__)();                          \
  namespace {                                                                 \
  struct RE_TEST_JOIN(re_test_reg_, __LINE__) {                               \
    RE_TEST_JOIN(re_test_reg_, __LINE__)() {                                  \
      ::re::test::register_test((name_literal),                               \
                               &RE_TEST_JOIN(re_test_fn_, __LINE__));         \
    }                                                                         \
  } RE_TEST_JOIN(re_test_reg_inst_, __LINE__);                                \
  }                                                                           \
  static void RE_TEST_JOIN(re_test_fn_, __LINE__)()

// ----------- EXPECT / ASSERT ------------
#define EXPECT_TRUE(expr)                                                    \
  do {                                                                       \
    if (!(expr)) {                                                           \
      ::re::test::record_failure(__FILE__, __LINE__, #expr,                  \
                                 "expected true");                           \
    }                                                                        \
  } while (0)
  
#define EXPECT_FALSE(expr)                                                   \
  do {                                                                       \
    if ((expr)) {                                                            \
      ::re::test::record_failure(__FILE__, __LINE__, #expr,                  \
                                 "expected false");                          \
    }                                                                        \
  } while (0)

#define EXPECT_EQ(a, b)                                                      \
  do {                                                                       \
    auto _a = (a);                                                           \
    auto _b = (b);                                                           \
    if (!(_a == _b)) {                                                       \
      ::re::test::record_failure(__FILE__, __LINE__,                         \
                                 #a " == " #b, "not equal");                 \
    }                                                                        \
  } while (0)

#define ASSERT_TRUE(expr)                                                    \
  do {                                                                       \
    if (!(expr)) {                                                           \
      ::re::test::record_failure(__FILE__, __LINE__, #expr,                  \
                                 "assert true failed");                      \
      throw ::re::test::AbortTest{};                                         \
    }                                                                        \
  } while (0)

#define ASSERT_EQ(a, b)                                                      \
  do {                                                                       \
    auto _a = (a);                                                           \
    auto _b = (b);                                                           \
    if (!(_a == _b)) {                                                       \
      ::re::test::record_failure(__FILE__, __LINE__,                         \
                                 #a " == " #b, "assert eq failed");          \
      throw ::re::test::AbortTest{};                                         \
    }                                                                        \
  } while (0)

#endif // !TEST_HPP
