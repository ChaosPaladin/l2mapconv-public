#pragma once

#include <geodata/Geodata.h>

#include <cstdint>
#include <vector>

namespace geodata {

class ExportBuffer {
public:
  struct Block {
    BlockType type : 4;
  };

  struct Column {
    std::uint8_t layers;
  };

  struct PackedCell {
    std::int16_t height;
    bool north : 1;
    bool south : 1;
    bool west : 1;
    bool east : 1;
  };

  explicit ExportBuffer();

  void reset(const Geodata &geodata);

  auto block(int x, int y) const -> Block;
  auto column(int x, int y, int cx, int cy) const -> Column;
  auto cell(int x, int y, int cx = 0, int cy = 0, int layer = 0) const -> Cell;

  void set_block_type(int x, int y, BlockType type);
  void set_block_height(int x, int y, std::int16_t height);

private:
  std::vector<Block> m_blocks;
  std::vector<Column> m_columns;
  std::vector<PackedCell> m_cells;

  auto pack_cell(const Cell &cell) const -> PackedCell;
  auto unpack_cell(PackedCell packed_cell, BlockType type, int x, int y) const
      -> Cell;
};

} // namespace geodata
