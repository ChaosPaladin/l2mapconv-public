#pragma once

#include "BuilderSettings.h"
#include "Geodata.h"
#include "Map.h"

namespace geodata {

class Builder {
public:
  auto build(const Map &map, const BuilderSettings &settings) const -> Geodata;
};

} // namespace geodata
