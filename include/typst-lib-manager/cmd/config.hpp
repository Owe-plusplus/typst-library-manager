#pragma once
#include "json/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
namespace cmd {

class Config {
  std::filesystem::path config_path;
  std::ofstream config_file;

public:
  Config(std::string config_path_str) : config_path(config_path_str) {
    createConfigFile();
  }
  ~Config() {}

private:
  bool createConfigFile() {
    if (std::filesystem::exists(config_path)) {
      return true;
    }
    if (config_path.has_parent_path()) {
      std::filesystem::create_directories(config_path.parent_path());
    }
    config_file.open(config_path);
    if (!config_file.is_open()) {
      std::cerr << "Failed to create config file: " << config_path << std::endl;
      return false;
    }
    std::cout << "Config file created at: " << config_path << std::endl;
    return true;
  }
};

}; // namespace cmd