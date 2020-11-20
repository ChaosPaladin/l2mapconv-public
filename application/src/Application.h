#pragma once

#include <filesystem>
#include <string>
#include <vector>

class Application {
public:
  explicit Application(const std::vector<std::string> &arguments);

  auto run() -> int;

private:
  const std::vector<std::string> m_arguments;

  void usage() const;

  void preview(const std::filesystem::path &root_path,
               const std::vector<std::string> &map_names) const;
  void build(const std::filesystem::path &root_path,
             const std::vector<std::string> &map_names) const;
};
