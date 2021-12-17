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
  padded_string(string_view sv) : m_string_view(sv), m_padding(padding::none){};
  padded_string(string_view sv, padding pad)
      : m_string_view(sv), m_padding(pad){};

  string_view to_string_view() const { return m_string_view; }
  padding padding_kind() const { return m_padding; }

 private:
  string_view m_string_view;
  padding m_padding;
};
