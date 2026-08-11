#pragma once
#include "json/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
namespace cmd {

class Config {
  std::filesystem::path config_path;
  std::ofstream config_file;

public:
  struct ConfigData {
    std::string name;
    std::string url;
  };
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(ConfigData, name, url);
  std::vector<ConfigData> config_datas;

  Config(std::string config_path_str) : config_path(config_path_str) {
    createConfigFile();
  }
  ~Config() {}

  bool readConfigFile() {
    if (!std::filesystem::exists(config_path)) {
      std::cerr << "Config file does not exist: " << config_path << std::endl;
      return false;
    }
    std::ifstream config_file(config_path);
    if (!config_file.is_open()) {
      std::cerr << "Failed to open config file: " << config_path << std::endl;
      return false;
    }
    nlohmann::json config_json;
    try {
      config_file >> config_json;
    } catch (const nlohmann::json::parse_error &e) {
      std::cerr << "Failed to parse config file: " << e.what() << std::endl;
      return false;
    }

    try {
      config_datas = config_json.get<std::vector<ConfigData>>();
    } catch (const nlohmann::json::type_error &e) {
      std::cerr << "Config file has invalid format: " << e.what() << std::endl;
      return false;
    }
    // Process the JSON data as needed
    return true;
  }

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