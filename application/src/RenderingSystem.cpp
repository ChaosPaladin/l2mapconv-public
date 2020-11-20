#include "pch.h"

#include "Entity.h"
#include "RenderingSystem.h"

RenderingSystem::RenderingSystem(RenderingContext &rendering_context,
                                 WindowContext &window_context,
                                 UIContext &ui_context)
    : m_rendering_context{rendering_context}, m_window_context{window_context},
      m_ui_context{ui_context}, m_entity_renderer{rendering::EntityRenderer{
                                    m_rendering_context.context,
                                    m_rendering_context.camera}} {

  ASSERT(glewInit() == GLEW_OK, "App", "Can't initialize GLEW");

  GL_CALL(const auto gl_version = glGetString(GL_VERSION));
  GL_CALL(const auto gl_vendor = glGetString(GL_VENDOR));

  utils::Log(utils::LOG_INFO, "App")
      << "GL Version: " << gl_version << std::endl;
  utils::Log(utils::LOG_INFO, "App") << "GL Vendor: " << gl_vendor << std::endl;

  GL_CALL(glEnable(GL_DEPTH_TEST));
  GL_CALL(glEnable(GL_BLEND));
  GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}

void RenderingSystem::frame_begin(Timestep /*frame_time*/) {
  resize();
  clear();
}

void RenderingSystem::frame_end(Timestep /*frame_time*/) {
  rendering::FrameSettings settings{};
  settings.wireframe = m_ui_context.rendering.wireframe;
  settings.culling = m_ui_context.rendering.culling;

  if (m_ui_context.rendering.passable) {
    settings.surface_filter |= SURFACE_PASSABLE;
  }

  if (m_ui_context.rendering.terrain) {
    settings.surface_filter |= SURFACE_TERRAIN;
  }

  if (m_ui_context.rendering.static_meshes) {
    settings.surface_filter |= SURFACE_STATIC_MESH;
  }

  if (m_ui_context.rendering.csg) {
    settings.surface_filter |= SURFACE_CSG;
  }

  if (m_ui_context.rendering.bounding_boxes) {
    settings.surface_filter |= SURFACE_BOUNDING_BOX;
  }

  if (m_ui_context.rendering.imported_geodata) {
    settings.surface_filter |= SURFACE_IMPORTED_GEODATA;
  }

  if (m_ui_context.rendering.exported_geodata) {
    settings.surface_filter |= SURFACE_EXPORTED_GEODATA;
  }

  if (settings.wireframe) {
    GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
  } else {
    GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
  }

  m_ui_context.rendering.draws = 0;

  m_entity_renderer.render(m_rendering_context.entity_tree, settings,
                           m_ui_context.rendering.draws);
}

void RenderingSystem::resize() const {
  const auto height = m_window_context.framebuffer.size.height;
  const auto width = m_window_context.framebuffer.size.width;

  auto &framebuffer = m_rendering_context.context.framebuffer;

  if (framebuffer.size.width != width || framebuffer.size.height != height) {
    GL_CALL(glViewport(0, 0, width, height));
    framebuffer.size = {width, height};
  }
}

void RenderingSystem::clear() const {
  GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
  GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}
