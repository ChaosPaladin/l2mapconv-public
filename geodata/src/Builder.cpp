#include "pch.h"

#include "Preprocessing.h"

#include <geodata/Builder.h>

namespace geodata {

static constexpr auto destination_cell_size = 16.0f;

auto Builder::build(const Map &map, const BuilderSettings &settings) const
    -> Geodata {

  // Configuration.
  const auto source_cell_size = settings.cell_size;
  const auto cell_height = settings.cell_height;
  const auto walkable_height =
      static_cast<int>(std::ceil(settings.walkable_height / cell_height));
  const auto wall_angle = settings.wall_angle;
  const auto walkable_angle = settings.walkable_angle;
  const auto min_walkable_climb =
      static_cast<int>(std::floor(settings.min_walkable_climb / cell_height));
  const auto max_walkable_climb =
      static_cast<int>(std::floor(settings.max_walkable_climb / cell_height));

  // Flip bounding box for Recast (Y <-> Z).
  const auto *source_bb_min = glm::value_ptr(map.bounding_box().min());
  const auto *source_bb_max = glm::value_ptr(map.bounding_box().max());
  float bb_min[3] = {source_bb_min[0], source_bb_min[2], source_bb_min[1]};
  float bb_max[3] = {source_bb_max[0], source_bb_max[2], source_bb_max[1]};

  // Calculate grid size.
  auto source_width = 0;
  auto source_height = 0;
  rcCalcGridSize(static_cast<float *>(bb_min), static_cast<float *>(bb_max),
                 source_cell_size, &source_width, &source_height);

  // Destination grid size.
  auto destination_width = 0;
  auto destination_height = 0;
  rcCalcGridSize(static_cast<float *>(bb_min), static_cast<float *>(bb_max),
                 destination_cell_size, &destination_width,
                 &destination_height);

  rcContext context{};

  // Create source heightfield.
  auto *source_hf = rcAllocHeightfield();
  rcCreateHeightfield(&context, *source_hf, source_width, source_height,
                      static_cast<float *>(bb_min),
                      static_cast<float *>(bb_max), source_cell_size,
                      cell_height);

  // Prepare geometry data.
  const auto *vertices = glm::value_ptr(map.vertices().front());
  const auto vertex_count = map.vertices().size();
  const auto *triangles = reinterpret_cast<const int *>(map.indices().data());
  const auto triangle_count = map.indices().size() / 3;

  // Rasterize triangles.
  std::vector<unsigned char> areas(triangle_count);
  mark_triangles(walkable_angle, wall_angle, vertices, triangles,
                 triangle_count, &areas.front());
  rcRasterizeTriangles(&context, vertices, vertex_count, triangles,
                       &areas.front(), triangle_count, *source_hf, 0);

  // Create destination heightfield.
  auto *destination_hf = rcAllocHeightfield();
  rcCreateHeightfield(&context, *destination_hf, destination_width,
                      destination_height, static_cast<float *>(bb_min),
                      static_cast<float *>(bb_max), destination_cell_size,
                      cell_height);
  merge_heightfields(&context, *source_hf, *destination_hf, min_walkable_climb);
  rcFreeHeightField(source_hf);

  // Filter low height spans.
  rcFilterWalkableLowHeightSpans(&context, walkable_height, *destination_hf);

  // Calculate NSWE.
  calculate_nswe(*destination_hf, walkable_height, min_walkable_climb,
                 max_walkable_climb);

  // Convert heightfield to geodata.
  Geodata geodata;

  const auto depth = static_cast<int>((bb_max[2] - bb_min[2]) / cell_height);

  std::vector<int> columns(destination_hf->width * destination_hf->height);
  auto black_holes = 0;

  for (auto y = 0; y < destination_hf->height; ++y) {
    for (auto x = 0; x < destination_hf->width; ++x) {
      for (auto *span = destination_hf->spans[x + y * destination_hf->width];
           span != nullptr; span = span->next) {

        const auto area = static_cast<int>(span->area) & 0xf;
        const auto nswe = static_cast<int>(span->area) >> 4;

        if (area == RC_NULL_AREA) {
          continue;
        }

        if (nswe == 0) {
          black_holes++;
        }

        const auto z = static_cast<int>(span->smax) - depth / 2 + 1;

        geodata.cells.push_back({
            static_cast<std::int16_t>(x),
            static_cast<std::int16_t>(y),
            static_cast<std::int16_t>(static_cast<float>(z) * cell_height + 28),
            BLOCK_MULTILAYER,
            (nswe & DIRECTION_N) != 0,
            (nswe & DIRECTION_W) != 0,
            (nswe & DIRECTION_E) != 0,
            (nswe & DIRECTION_S) != 0,
        });

        columns[y * destination_hf->width + x]++;
      }
    }
  }

  // Add fake cells to columns with no layers.
  for (auto y = 0; y < destination_hf->height; ++y) {
    for (auto x = 0; x < destination_hf->width; ++x) {
      if (columns[y * destination_hf->width + x] > 0) {
        continue;
      }

      geodata.cells.push_back({
          static_cast<std::int16_t>(x),
          static_cast<std::int16_t>(y),
          -0x4000,
          BLOCK_COMPLEX,
          false,
          false,
          false,
          false,
      });
    }
  }

  if (black_holes > 0) {
    utils::Log(utils::LOG_WARN, "Geodata")
        << "Black holes (points of no return): " << black_holes << std::endl;
  }

  rcFreeHeightField(destination_hf);

  return geodata;
}

} // namespace geodata
