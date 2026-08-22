#pragma once
#include "json/json.hpp"
#include <cstdlib>
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
#ifdef _WIN32
    if (home_dir == nullptr) {
      home_dir = getenv("USERPROFILE");
    }
#endif
    if (home_dir == nullptr) {
      std::cerr << "Error: home directory environment variable is not set."
                << std::endl;
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

  bool resetConfigFile() {
    if (std::filesystem::exists(config_path)) {
      try {
        std::filesystem::remove(config_path);
      } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Error deleting config file: " << e.what() << std::endl;
        return false;
      }
      std::cout << "Config file deleted: " << config_path << std::endl;

      std::filesystem::path config_dir = config_path.parent_path();

      std::error_code ec;
      if (!(std::filesystem::exists(config_dir, ec) &&
            !std::filesystem::is_empty(config_dir, ec))) {

        try {
          std::filesystem::remove(config_dir);
        } catch (const std::filesystem::filesystem_error &e) {
          std::cerr << "Error deleting config directory: " << e.what()
                    << std::endl;
          return false;
        }
        std::cout << "Config directory deleted: " << config_dir << std::endl;
      }
    }
    return true;
  }

  bool writeConfigFile() {
    if (!config_path.has_parent_path()) {
      std::cerr << "Config file has no parent directory: " << config_path
                << std::endl;
      return false;
    }

    try {
      std::filesystem::create_directories(config_path.parent_path());
    } catch (const std::filesystem::filesystem_error &e) {
      std::cerr << "Error creating config directory: " << e.what() << std::endl;
      return false;
    }

    std::filesystem::path temp_path =
        config_path.parent_path() / ".config.json.tmp";
    try {
      std::ofstream config_file(temp_path, std::ios::binary | std::ios::trunc);
      if (!config_file.is_open()) {
        std::cerr << "Failed to open temporary config file: " << temp_path
                  << std::endl;
        return false;
      }

      config_file << nlohmann::json(config_datas).dump(4);
      config_file.flush();
      if (!config_file) {
        std::cerr << "Failed to write config file: " << temp_path << std::endl;
        config_file.close();
        std::filesystem::remove(temp_path);
        return false;
      }
      config_file.close();

      std::error_code ec;
      if (std::filesystem::exists(config_path, ec)) {
        std::filesystem::remove(config_path, ec);
      }

      std::filesystem::rename(temp_path, config_path, ec);
      if (ec) {
        std::cerr << "Failed to replace config file: " << ec.message()
                  << std::endl;
        std::filesystem::remove(temp_path);
        return false;
      }
    } catch (const std::exception &e) {
      std::cerr << "Failed to write config file atomically: " << e.what()
                << std::endl;
      std::filesystem::remove(temp_path);
      return false;
    }

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
    if (!writeConfigFile()) {
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
    if (!writeConfigFile()) {
      return false;
    }
    std::cout << "Config data removed successfully." << std::endl;
    return true;
  }
};
}; // namespace subCommands
