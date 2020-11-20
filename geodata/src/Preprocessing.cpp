#include "pch.h"

#include "Preprocessing.h"

// Оставь надежду всяк сюда смотрящий.
namespace geodata {

static constexpr auto RC_FLAT_AREA = 1;
static constexpr auto RC_STEEP_AREA = 2;
static constexpr auto RC_WALL_AREA = 3;

void mark_triangles(float walkable_angle, float wall_angle,
                    const float *vertices, const int *triangles,
                    std::size_t triangle_count, unsigned char *areas) {

  const auto walkable_angle_radians = std::cos(glm::radians(walkable_angle));
  const auto wall_angle_radians = std::cos(glm::radians(wall_angle));

  for (std::size_t i = 0; i < triangle_count; ++i) {
    const auto *triangle = &triangles[i * 3];
    const auto normal =
        glm::triangleNormal(glm::make_vec3(&vertices[triangle[0] * 3]),
                            glm::make_vec3(&vertices[triangle[1] * 3]),
                            glm::make_vec3(&vertices[triangle[2] * 3]));

    if (normal.y < wall_angle_radians) {
      areas[i] = RC_WALL_AREA;
    } else if (normal.y < walkable_angle_radians) {
      areas[i] = RC_STEEP_AREA;
    } else {
      areas[i] = RC_FLAT_AREA;
    }
  }
}

void merge_heightfields(rcContext *context, const rcHeightfield &source,
                        rcHeightfield &destination, int min_walkable_climb) {

  const auto x_ratio = source.width / destination.width;
  const auto y_ratio = source.height / destination.height;

  for (auto y = 0; y < source.height; ++y) {
    for (auto x = 0; x < source.width; ++x) {
      for (auto *span = source.spans[x + y * source.width]; span != nullptr;
           span = span->next) {

        rcAddSpan(context, destination, x / x_ratio, y / y_ratio, span->smin,
                  span->smax, span->area, 0, min_walkable_climb);
      }
    }
  }
}

void calculate_nswe(const rcHeightfield &hf, int walkable_height,
                    int min_walkable_climb, int max_walkable_climb) {

  const auto max_height = 0xffff;

  for (auto y = 0; y < hf.height; ++y) {
    for (auto x = 0; x < hf.width; ++x) {
      for (auto *span = hf.spans[x + y * hf.width]; span != nullptr;
           span = span->next) {

        const auto area = static_cast<int>(span->area) & 0xf;

        if (area == RC_NULL_AREA) {
          continue;
        }

        const auto bottom = static_cast<int>(span->smax);
        const auto top = span->next != nullptr
                             ? static_cast<int>(span->next->smin)
                             : max_height;

        span->area &= 0xf;

        for (auto direction = 0; direction < 4; ++direction) {
          const auto dx = x + rcGetDirOffsetX(direction);
          const auto dy = y + rcGetDirOffsetY(direction);

          if (dx < 0 || dy < 0 || dx >= hf.width || dy >= hf.height) {
            span->area |= 1 << (direction + 4);
            continue;
          }

          auto direction_allowed = false;

          for (auto *neighbour = hf.spans[dx + dy * hf.width];
               neighbour != nullptr; neighbour = neighbour->next) {

            const auto neighbour_area = static_cast<int>(neighbour->area) & 0xf;

            if (neighbour_area == RC_NULL_AREA) {
              direction_allowed = false;
              continue;
            }

            const auto neighbour_bottom = static_cast<int>(neighbour->smax);
            const auto merged_neighbour_bottom =
                static_cast<int>(neighbour->mmax);
            const auto neighbour_top =
                neighbour->next != nullptr
                    ? static_cast<int>(neighbour->next->smin)
                    : max_height;

            const auto height = std::min(top, neighbour_top) -
                                std::max(bottom, neighbour_bottom);

            if (height >= walkable_height) {
              if (neighbour_area == RC_WALL_AREA) {
                if (bottom < merged_neighbour_bottom) {
                  direction_allowed =
                      (merged_neighbour_bottom - bottom) <= min_walkable_climb;
                } else {
                  direction_allowed =
                      (neighbour_bottom - bottom) <= min_walkable_climb;
                }
              } else if (neighbour_area == RC_STEEP_AREA) {
                direction_allowed =
                    (neighbour_bottom - bottom) <= min_walkable_climb;
              } else {
                direction_allowed =
                    (neighbour_bottom - bottom) <= max_walkable_climb;
              }

              break;
            }
          }

          if (direction_allowed) {
            span->area |= 1 << (direction + 4);
          }
        }
      }
    }
  }
}

} // namespace geodata
