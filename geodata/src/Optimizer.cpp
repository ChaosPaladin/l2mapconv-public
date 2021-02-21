#include "pch.h"

#include "Optimizer.h"

namespace geodata {

static constexpr auto MAP_WIDTH_BLOCKS = 256;
static constexpr auto MAP_HEIGHT_BLOCKS = 256;
static constexpr auto BLOCK_WIDTH_CELLS = 8;
static constexpr auto BLOCK_HEIGHT_CELLS = 8;
static constexpr auto SIMPLE_BLOCK_MAX_HEIGHT_DIFFERENCE = 32;

void Optimizer::optimize() {
  for (auto x = 0; x < MAP_WIDTH_BLOCKS; ++x) {
    for (auto y = 0; y < MAP_HEIGHT_BLOCKS; ++y) {
      if (is_multilayer_block(x, y)) {
        m_buffer.set_block_type(x, y, BLOCK_MULTILAYER);
      } else {
        const auto [is_simple, new_z] = is_simple_block(x, y);

        if (is_simple) {
          m_buffer.set_block_type(x, y, BLOCK_SIMPLE);
          m_buffer.set_block_height(x, y, new_z);
        } else {
          m_buffer.set_block_type(x, y, BLOCK_COMPLEX);
        }
      }
    }
  }
}

auto Optimizer::is_multilayer_block(int x, int y) const -> bool {
  for (auto cx = 0; cx < BLOCK_WIDTH_CELLS; ++cx) {
    for (auto cy = 0; cy < BLOCK_HEIGHT_CELLS; ++cy) {
      const auto column = m_buffer.column(x, y, cx, cy);

      ASSERT(column.layers > 0, "Geodata",
             "Column must have at least one layer: "
                 << x * MAP_WIDTH_BLOCKS + cx << " "
                 << y * MAP_HEIGHT_BLOCKS + cy);

      if (column.layers > 1) {
        return true;
      }
    }
  }

  return false;
}

auto Optimizer::is_simple_block(int x, int y) const
    -> std::pair<bool, std::int16_t> {

  auto min_z = std::numeric_limits<std::int16_t>::max();
  auto max_z = std::numeric_limits<std::int16_t>::min();

  for (auto cx = 0; cx < BLOCK_WIDTH_CELLS; ++cx) {
    for (auto cy = 0; cy < BLOCK_HEIGHT_CELLS; ++cy) {
      const auto cell = m_buffer.cell(x, y, cx, cy);

      if (!cell.north || !cell.south || !cell.west || !cell.east) {
        return {false, 0};
      }

      min_z = std::min(min_z, cell.z);
      max_z = std::max(max_z, cell.z);

      if (max_z - min_z > SIMPLE_BLOCK_MAX_HEIGHT_DIFFERENCE) {
        return {false, 0};
      }
    }
  }

  return {true, max_z};
}

} // namespace geodata
