#ifndef RE_ERROR_HPP
#define RE_ERROR_HPP

#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <variant>

namespace re {

struct Error {
  std::size_t pos{};
  std::string message;
};

template <class T>
class Result {
private:
  std::variant<T, Error> data_;

private:
  explicit Result(T value) : data_(std::move(value)) {}
  explicit Result(Error err) : data_(std::move(err)) {}

public:
  // --------- Factory function ---------
  static Result ok(T value) {
    return Result(std::move(value));
  }

  static Result fail(Error err) {
    return Result(std::move(err));
  }

  // --------- Query status ---------
  bool is_ok() const noexcept {
    return std::holds_alternative<T>(data_);
  }

  explicit operator bool() const noexcept {
    return is_ok();
  }

  // --------- Value retrieval ---------
  T& value() {
    assert(is_ok());
    return std::get<T>(data_);
  }

  const T& value() const {
    assert(is_ok());
    return std::get<T>(data_);
  }

  Error& error() {
    assert(!is_ok());
    return std::get<Error>(data_);
  }

  const Error& error() const { 
    assert(!is_ok());
    return std::get<Error>(data_); 
  }

  T* value_if() {
    if (is_ok()) return std::get_if<T>(&data_);
    else return nullptr;
  }

  const T* value_if() const {
    if (is_ok()) return std::get_if<T>(&data_);
    else return nullptr;
  }

  Error* error_if() {
    if (!is_ok()) return std::get_if<Error>(&data_);
    else return nullptr;
  }

  const Error* error_if() const {
    if (!is_ok()) return std::get_if<Error>(&data_);
    else return nullptr;
  }
};

} // namespace re

#endif // !RE_ERROR_HPP
