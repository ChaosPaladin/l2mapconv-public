#include "pch.h"

#include "GeodataEntityFactory.h"

auto GeodataEntityFactory::make_entity(const geodata::Geodata &geodata,
                                       const math::Box &bounding_box,
                                       std::uint64_t surface_type) const
    -> Entity<GeodataMesh> {

  const auto mesh = std::make_shared<GeodataMesh>();

  mesh->cells = geodata.cells;
  mesh->bounding_box = math::Box{{0.0f, 0.0f, bounding_box.min().z},
                                 bounding_box.max() - bounding_box.min()};

  mesh->surface.type = surface_type;
  mesh->surface.material.color = {0.0f, 1.0f, 1.0f};

  Entity entity{mesh};
  entity.position = {bounding_box.min().x, bounding_box.min().y, 0.0f};

  return entity;
}
