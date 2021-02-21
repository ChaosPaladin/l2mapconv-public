#pragma once

#include "Archive.h"

#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace unreal {

class Package {
public:
  explicit Package(Archive &archive) : m_archive{archive} {}

  template <typename T>
  void load_objects(const std::string &class_name,
                    std::vector<std::shared_ptr<T>> &objects) const {

    m_archive.load_objects(class_name, objects);
  }

  auto name() const -> std::string { return std::string{m_archive.name}; }

  friend auto operator<<(std::ostream &output, const Package &package)
      -> std::ostream &;

private:
  Archive &m_archive;
};

} // namespace unreal
