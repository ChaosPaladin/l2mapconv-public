#include "pch.h"

#include "Entity.h"
#include "GeodataEntityFactory.h"
#include "GeodataSystem.h"

GeodataSystem::GeodataSystem(GeodataContext &geodata_context,
                             UIContext &ui_context, const Renderer *renderer)
    : m_geodata_context{geodata_context}, m_ui_context{ui_context},
      m_renderer{renderer} {

  m_ui_context.geodata.build_handler = [this] { build(); };
}

void GeodataSystem::build() const {
  geodata::BuilderSettings settings{};
  settings.cell_size = m_ui_context.geodata.cell_size;
  settings.cell_height = m_ui_context.geodata.cell_height;
  settings.walkable_height = m_ui_context.geodata.walkable_height;
  settings.wall_angle = m_ui_context.geodata.wall_angle;
  settings.walkable_angle = m_ui_context.geodata.walkable_angle;
  settings.min_walkable_climb = m_ui_context.geodata.min_walkable_climb;
  settings.max_walkable_climb = m_ui_context.geodata.max_walkable_climb;

  geodata::Builder geodata_builder;
  geodata::Exporter geodata_exporter{"output"};
  GeodataEntityFactory geodata_entity_factory;

  if (m_renderer != nullptr) {
    m_renderer->remove(SURFACE_EXPORTED_GEODATA);
  }

  for (const auto &map : m_geodata_context.maps) {
    utils::Log(utils::LOG_INFO, "App")
        << "Building geodata for map: " << map.name() << std::endl;

    const auto geodata = geodata_builder.build(map, settings);
    const auto geodata_entity = geodata_entity_factory.make_entity(
        geodata, map.bounding_box(), SURFACE_EXPORTED_GEODATA);

    if (m_renderer != nullptr) {
      m_renderer->render_geodata({geodata_entity});
    }

    if (m_ui_context.geodata.export_) {
      utils::Log(utils::LOG_INFO, "App")
          << "Exporting geodata for map: " << map.name() << std::endl;

      geodata_exporter.export_l2j_geodata(map.name(), geodata);
    }
  }
}
