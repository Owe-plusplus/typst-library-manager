#pragma once
#include "json/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
namespace subCommands {

class Config {
  std::filesystem::path config_path;

public:
  struct ConfigData {
    std::string name;
    std::string url;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ConfigData, name, url)
  };
  std::vector<ConfigData> config_datas;

  Config() {
    const char *home_dir = getenv("HOME");
    if (home_dir == nullptr) {
      std::cerr << "Error: HOME environment variable is not set." << std::endl;
      exit(EXIT_FAILURE);
    }
    std::filesystem::path home_path(home_dir);
    config_path = home_path / ".typst_library_manager" / "config.json";
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
    } catch (const nlohmann::json::exception &e) {
      std::cerr << "Config file has invalid format: " << e.what() << std::endl;
      return false;
    } catch (const std::exception &e) {
      std::cerr << "Unexpected error:  " << e.what() << std::endl;
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
      try {
        std::filesystem::create_directories(config_path.parent_path());
      } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        return false;
      }
    }
    std::ofstream config_file;
    config_file.open(config_path);
    if (!config_file.is_open()) {
      std::cerr << "Failed to create config file: " << config_path << std::endl;
      return false;
    }
    config_file << "[]";
    config_file.close();
    std::cout << "Config file created at: " << config_path << std::endl;
    return true;
  }

  bool deleteConfigFile() {
    if (!std::filesystem::exists(config_path)) {
      std::cerr << "Config file does not exist: " << config_path << std::endl;
      return false;
    }
    try {
      std::filesystem::remove(config_path);
    } catch (const std::filesystem::filesystem_error &e) {
      std::cerr << "Error deleting config file: " << e.what() << std::endl;
      return false;
    }
    std::cout << "Config file deleted: " << config_path << std::endl;

    try {
      std::filesystem::remove(config_path.parent_path());
    } catch (const std::filesystem::filesystem_error &e) {
      std::cerr << "Error deleting config directory: " << e.what() << std::endl;
      return false;
    }
    std::cout << "Config directory deleted: " << config_path.parent_path()
              << std::endl;
    return true;
  }

  bool addConfigData(std::string url, std::string name) {
    for (const auto &data : config_datas) {
      if (data.name == name) {
        std::cerr << "Config data with name '" << name << "' already exists."
                  << std::endl;
        return false;
      }
    }
    ConfigData new_config_data{name, url};
    config_datas.push_back(new_config_data);
    nlohmann::json config_json = config_datas;
    std::ofstream config_file(config_path);
    if (!config_file.is_open()) {
      std::cerr << "Failed to open config file for writing: " << config_path
                << std::endl;
      return false;
    }
    try {
      config_file << config_json.dump(4);
    } catch (const nlohmann::json::type_error &e) {
      std::cerr << "Failed to write to config file: " << e.what() << std::endl;
      return false;
    }
    std::cout << "Config data added successfully." << std::endl;
    return true;
  }

  bool removeConfigData(std::string name) {
    auto it = std::remove_if(
        config_datas.begin(), config_datas.end(),
        [&](const ConfigData &data) { return data.name == name; });
    if (it == config_datas.end()) {
      std::cerr << "Config data with name '" << name << "' not found."
                << std::endl;
      return false;
    }
    config_datas.erase(it, config_datas.end());
    nlohmann::json config_json = config_datas;
    std::ofstream config_file(config_path);
    if (!config_file.is_open()) {
      std::cerr << "Failed to open config file for writing: " << config_path
                << std::endl;
      return false;
    }
    try {
      config_file << config_json.dump(4);
    } catch (const nlohmann::json::type_error &e) {
      std::cerr << "Failed to write to config file: " << e.what() << std::endl;
      return false;
    }
    std::cout << "Config data removed successfully." << std::endl;
    return true;
  }
};
}; // namespace subCommands