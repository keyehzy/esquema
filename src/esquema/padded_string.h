#pragma once

#include "string_view.h"

enum class padding {
  none,
  left,
  right,
  both,
};

class padded_string {
 public:
  padded_string(string_view sv) : m_string(sv), m_padding(padding::none){};
  padded_string(string_view sv, padding pad) : m_string(sv), m_padding(pad){};

  string to_string() const { return m_string; }
  padding padding_kind() const { return m_padding; }

 private:
  string m_string;
  padding m_padding;
};
