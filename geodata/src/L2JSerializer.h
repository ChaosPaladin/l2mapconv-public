#pragma once

#include <geodata/ExportBuffer.h>
#include <geodata/Geodata.h>

#include <iostream>

namespace geodata {

class L2JSerializer {
public:
  auto deserialize(std::istream &input) const -> Geodata;
  void serialize(ExportBuffer &buffer, std::ostream &output) const;

private:
  auto read_complex_block_cell(std::istream &input, std::uint8_t type, int x,
                               int y, int cx, int cy) const -> Cell;
  void write_complex_block_cell(std::ostream &output, const Cell &cell) const;

  void round_height(std::int16_t &height) const;

  void write(std::ostream &output, std::int16_t value) const;
};

} // namespace geodata
