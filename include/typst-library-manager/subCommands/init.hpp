#pragma once
#include <filesystem>
#include <iostream>
#include <running.hpp>
#include <subCommands/config.hpp>

namespace subCommands {
class Init {
  subCommands::Config &config;
  Running running;

public:
  std::filesystem::path library_directory;
  Init(subCommands::Config &config) : config(config) {
    running = Running();
    library_directory = std::filesystem::current_path() / "libraries";
  }
  ~Init() {}

  bool workspaceInit(bool force_init) {
    for (const auto &config_data : config.config_datas) {
      if (std::filesystem::exists(library_directory / config_data.name)) {
        std::cout << "Directory " << library_directory / config_data.name
                  << " already exists. Skipping clone." << std::endl;
        continue;
      }
      if (!running.runGitClone(
              config_data.url, config_data.name, library_directory,
              "Cloning " + config_data.name + "...",
              "Cloned " + config_data.name + " successfully.")) {
        std::cerr << "Failed to clone " << config_data.name << std::endl;
        std::cerr << "Workspace initialization failed." << std::endl;
        return false;
      }
      bool found_source_file = force_init;
      for (const auto &entry : std::filesystem::directory_iterator(
               library_directory / config_data.name)) {
        if (entry.is_regular_file()) {
          auto ext = entry.path().extension().string();
          if (ext == ".typ" || ext == ".typst") {
            found_source_file = true;
            break;
          }
        }
      }
      if (!found_source_file) {
        std::cerr << "No source file found in "
                  << library_directory / config_data.name << std::endl;
        std::cerr << "Workspace initialization failed." << std::endl;
        return false;
      }
    }
    std::cout << "Workspace initialized successfully." << std::endl;
    return true;
  }
};
} // namespace subCommands
