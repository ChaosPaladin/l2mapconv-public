#pragma once

#include "Entity.h"

#include <utils/NonCopyable.h>

#include <math/Box.h>

#include <glm/glm.hpp>

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace geodata {

class Map : public utils::NonCopyable {
public:
  explicit Map(const std::string &name, const math::Box &bounding_box)
      : m_name{name}, m_bounding_box{bounding_box} {}

  Map(Map &&other) noexcept
      : m_name{std::move(other.m_name)},
        m_vertices{std::move(other.m_vertices)}, m_indices{std::move(
                                                     other.m_indices)},
        m_bounding_box{std::move(other.m_bounding_box)} {}

  void add(const Entity &entity);

  auto name() const -> const std::string &;

  auto vertices() const -> const std::vector<glm::vec3> &;
  auto indices() const -> const std::vector<unsigned int> &;

  auto bounding_box() const -> const math::Box &;

private:
  std::string m_name;
  std::vector<glm::vec3> m_vertices;
  std::vector<unsigned int> m_indices;
  math::Box m_bounding_box;
};

} // namespace geodata
