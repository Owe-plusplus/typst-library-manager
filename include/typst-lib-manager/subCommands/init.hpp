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
  Init(subCommands::Config &config) : config(config) { running = Running(); }
  ~Init() {}

  bool workspaceInit() {
    for (const auto &config_data : config.config_datas) {
      std::string git_clone_subCommands =
          "git clone " + config_data.url + " " + "\"" + config_data.name + "\"";
      std::filesystem::path target_directory =
          std::filesystem::current_path() / "libraries";
      if (std::filesystem::exists(target_directory / config_data.name)) {
        std::cout << "Directory " << target_directory / config_data.name
                  << " already exists. Skipping clone." << std::endl;
        continue;
      }
      if (!running.runCommand(git_clone_subCommands, target_directory,
                              "Cloning " + config_data.name + "...",
                              "Cloned " + config_data.name +
                                  " successfully.")) {
        std::cerr << "Failed to clone " << config_data.name << std::endl;
        std::cerr << "Workspace initialization failed." << std::endl;
        return false;
      }
    }
    std::cout << "Workspace initialized successfully." << std::endl;
    return true;
  }
};
} // namespace subCommands