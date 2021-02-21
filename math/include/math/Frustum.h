#pragma once

#include "Box.h"

#include <glm/glm.hpp>

#include <array>

namespace math {

class Frustum {
public:
  explicit Frustum(const glm::mat4 &projection_matrix,
                   const glm::mat4 &view_matrix);

  auto intersects(const Box &box) const -> bool;

private:
  std::array<glm::vec4, 6> m_planes;
};

} // namespace math
