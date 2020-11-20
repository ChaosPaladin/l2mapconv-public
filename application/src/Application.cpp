#include "pch.h"

#include "Application.h"
#include "ApplicationContext.h"
#include "CameraSystem.h"
#include "GeodataContext.h"
#include "GeodataSystem.h"
#include "LoadingSystem.h"
#include "RenderingContext.h"
#include "RenderingSystem.h"
#include "UIContext.h"
#include "UISystem.h"
#include "WindowContext.h"
#include "WindowSystem.h"

Application::Application(const std::vector<std::string> &arguments)
    : m_arguments{arguments} {}

auto Application::run() -> int {
  if (m_arguments.size() < 3) {
    usage();
    return EXIT_FAILURE;
  }

  const auto command = m_arguments[0];
  const auto root_path = m_arguments[1];

  if (!std::filesystem::exists(root_path)) {
    utils::Log(utils::LOG_ERROR)
        << "L2 root path not found: " << root_path << std::endl;
    return EXIT_FAILURE;
  }

  auto map_names = m_arguments;
  map_names.erase(map_names.begin());
  map_names.erase(map_names.begin());

  if (map_names.empty()) {
    usage();
    return EXIT_FAILURE;
  }

  if (command == "preview") {
    preview(root_path, map_names);
  } else if (command == "build") {
    build(root_path, map_names);
  } else {
    usage();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void Application::usage() const {
  std::cout << "Usage:" << std::endl;
  std::cout << "\tl2mapconv <L2 root path> <command> [<map package name>]"
            << std::endl
            << std::endl;
  std::cout << "Available commands:" << std::endl;
  std::cout << "\tpreview" << std::endl;
  std::cout << "\tbuild" << std::endl;
}

void Application::preview(const std::filesystem::path &root_path,
                          const std::vector<std::string> &map_names) const {

  // Make sure to remove systems & contexts before OpenGL context will be
  // destroyed.
  {

    // Initialize contexts.
    ApplicationContext application_context{};
    WindowContext window_context{};
    UIContext ui_context{};
    RenderingContext rendering_context{};
    GeodataContext geodata_context{};

    Renderer renderer{rendering_context};

    // Initialize systems.
    std::vector<std::unique_ptr<System>> systems;

    systems.push_back(
        std::make_unique<WindowSystem>(window_context, application_context,
                                       ui_context, "l2mapconv", 1440, 1000));
    systems.push_back(std::make_unique<UISystem>(ui_context, window_context,
                                                 rendering_context));
    systems.push_back(std::make_unique<RenderingSystem>(
        rendering_context, window_context, ui_context));
    systems.push_back(std::make_unique<CameraSystem>(
        rendering_context, window_context, ui_context));
    systems.push_back(std::make_unique<LoadingSystem>(
        geodata_context, &renderer, root_path, map_names));
    systems.push_back(std::make_unique<GeodataSystem>(geodata_context,
                                                      ui_context, &renderer));

    // Run application.
    application_context.running = true;

    for (const auto &system : systems) {
      system->start();
    }

    auto last_frmae_time = 0.0f;

    while (application_context.running) {
      // Shouldn't use glfwGetTime here, but it's the easiest way to get time.
      const auto time = static_cast<float>(glfwGetTime());
      Timestep frame_time{time - last_frmae_time};
      last_frmae_time = time;

      for (const auto &system : systems) {
        system->frame_begin(frame_time);
      }

      for (auto system = systems.rbegin(); system != systems.rend(); ++system) {
        (*system)->frame_end(frame_time);
      }
    }

    for (auto system = systems.rbegin(); system != systems.rend(); ++system) {
      (*system)->stop();
    }
  }
}

void Application::build(const std::filesystem::path &root_path,
                        const std::vector<std::string> &map_names) const {

  for (const auto &map_name : map_names) {
    UIContext ui_context{};
    GeodataContext geodata_context{};

    LoadingSystem loading_system{
        geodata_context, nullptr, root_path, {map_name}};
    GeodataSystem geodata_system{geodata_context, ui_context, nullptr};

    ui_context.geodata.export_ = true;
    ui_context.geodata.set_defaults();
    ui_context.geodata.build_handler();
  }

  std::cout << "Done!" << std::endl;
}
