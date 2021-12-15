#pragma once

#include <string>
#include <string_view>

using string = std::basic_string<char>;
using string_view = std::basic_string_view<char>;

constexpr string_view operator""_sv(const char* string, std::size_t length) {
  return string_view(string, length);
}
