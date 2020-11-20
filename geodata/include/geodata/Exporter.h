#pragma once

#include "ExportBuffer.h"
#include "Geodata.h"

#include <filesystem>
#include <string>

namespace geodata {

class Exporter {
public:
  explicit Exporter(const std::filesystem::path &root_path);

  void export_l2j_geodata(const std::string &name,
                          const Geodata &geodata) const;

private:
  std::filesystem::path m_root_path;

  mutable ExportBuffer m_export_buffer;
};

} // namespace geodata
