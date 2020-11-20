#include "pch.h"

#include <rendering/EntityTree.h>

namespace rendering {

void EntityTree::add(const Entity &entity) {
  ASSERT(entity.mesh() != nullptr, "Rendering", "Entity must have mesh");

  m_entities.push_front(entity);
  const auto *inserted = &*m_entities.begin();

  for (const auto &surface : entity.mesh()->surfaces()) {
    m_tree[surface.type][entity.shader()][surface.material.texture]
          [entity.mesh()]
              .push_back({inserted, &surface});
  }
}

void EntityTree::remove(std::uint64_t surface_filter) {
  for (auto it = m_tree.cbegin(); it != m_tree.cend();) {
    if ((it->first & surface_filter) == it->first) {
      m_tree.erase(it++);
    } else {
      ++it;
    }
  }

  m_entities.remove_if([surface_filter](const Entity &entity) {
    for (const auto &surface : entity.mesh()->surfaces()) {
      if ((surface.type & surface_filter) == surface.type) {
        return true;
      }
    }

    return false;
  });
}

auto EntityTree::tree() const -> const Tree & { return m_tree; }

} // namespace rendering
