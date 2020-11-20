#include "pch.h"

#include <rendering/GeodataMesh.h>

namespace rendering {

GeodataMesh::GeodataMesh(Context &context,
                         const std::vector<GeodataCell> &cells,
                         const MeshSurface &surface,
                         const math::Box &bounding_box)
    : m_mesh{context, cells.size(), {vertex_buffer(cells)}, {}},
      m_surfaces{surface}, m_bounding_box{bounding_box} {

  ASSERT(!cells.empty(), "Rendering", "Geodata must have at least one cell");
}

auto GeodataMesh::surfaces() const -> const std::vector<MeshSurface> & {
  return m_surfaces;
}

auto GeodataMesh::bounding_box() const -> const math::Box & {
  return m_bounding_box;
}

void GeodataMesh::draw(const MeshSurface & /*surface*/) const {
  m_mesh.draw(GL_POINTS, 1);
}

auto GeodataMesh::vertex_buffer(const std::vector<GeodataCell> &cells)
    -> VertexBuffer {

  VertexBuffer vertex_buffer{cells};
  vertex_buffer.int_layout(0, sizeof(GeodataCell), 0);
  return vertex_buffer;
}

} // namespace rendering
