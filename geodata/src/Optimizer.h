#pragma once

#include <geodata/ExportBuffer.h>

#include <utility>

namespace geodata {

class Optimizer {
public:
  explicit Optimizer(ExportBuffer &buffer) : m_buffer{buffer} {}

  void optimize();

private:
  ExportBuffer &m_buffer;

  auto is_multilayer_block(int x, int y) const -> bool;
  auto is_simple_block(int x, int y) const -> std::pair<bool, int>;
};

} // namespace geodata
