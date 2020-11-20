#pragma once

#include "Entity.h"

#include <math/Box.h>

#include <geodata/Geodata.h>

#include <cstdint>
#include <vector>

class GeodataEntityFactory {
public:
  auto make_entity(const geodata::Geodata &geodata,
                   const math::Box &bounding_box,
                   const std::uint64_t surface_type) const
      -> Entity<GeodataMesh>;
};
