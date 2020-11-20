#pragma once

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>
#include <vector>

namespace geodata {

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
};

struct Mesh {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<glm::mat4> instance_matrices;
};

struct Entity {
  std::shared_ptr<Mesh> mesh;
  glm::mat4 model_matrix;
};

} // namespace geodata
