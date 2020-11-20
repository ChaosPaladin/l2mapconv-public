#pragma once

#include <functional>

struct UIContext {
  struct {
    int draws;
    bool culling;
    bool wireframe;
    bool passable;
    bool terrain;
    bool static_meshes;
    bool csg;
    bool bounding_boxes;
    bool imported_geodata;
    bool exported_geodata;
  } rendering;

  struct {
    float cell_size;
    float cell_height;
    float walkable_height;
    float wall_angle;
    float walkable_angle;
    float min_walkable_climb;
    float max_walkable_climb;

    std::function<void()> build_handler;
    bool export_;

    void set_defaults() {
      cell_size = 8.0f;
      cell_height = 1.0f;
      walkable_height = 46.0f;
      wall_angle = 87.5f;
      walkable_angle = 45.0f;
      min_walkable_climb = 10.0f;
      max_walkable_climb = 16.0f;
    }
  } geodata;
};
