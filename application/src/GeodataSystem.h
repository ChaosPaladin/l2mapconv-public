#pragma once

#include "GeodataContext.h"
#include "Renderer.h"
#include "System.h"
#include "Timestep.h"
#include "UIContext.h"

class GeodataSystem : public System {
public:
  explicit GeodataSystem(GeodataContext &geodata_context, UIContext &ui_context,
                         const Renderer *renderer);

private:
  GeodataContext &m_geodata_context;
  UIContext &m_ui_context;
  const Renderer *m_renderer;

  void build() const;
};
