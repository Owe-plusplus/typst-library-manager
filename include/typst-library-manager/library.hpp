#pragma once
#include "subCommands/config.hpp"
#include "subCommands/init.hpp"
#include <filesystem>
#include <optional>
#include <vector>

class Library {
  subCommands::Config config;
  subCommands::Init init;

public:
  struct libraryData {
    subCommands::Config::ConfigData config_data;
    std::filesystem::path source_path;
  };

  std::vector<libraryData> library_data;
  Library(const subCommands::Config &config, const subCommands::Init &init)
      : config(config), init(init) {
    updateLibraryData();
  }

  ~Library() {}

  std::optional<std::filesystem::path>
  searchOneOfSourceFilePath(const std::filesystem::path &library_path,
                            const std::string &library_name) {
    std::optional<std::filesystem::path> result;

    if (!std::filesystem::exists(library_path) ||
        !std::filesystem::is_directory(library_path)) {
      std::cerr << "Directory does not exist: " << library_path << std::endl;
      return result;
    }

    for (const auto &entry :
         std::filesystem::directory_iterator(library_path)) {
      if (entry.is_regular_file()) {
        auto filename = entry.path().stem().string();
        auto ext = entry.path().extension().string();
        if ((ext == ".typ" || ext == ".typst") && filename == library_name) {
          result = entry.path();
          return result;
        }
      }
    }
    for (const auto &entry :
         std::filesystem::directory_iterator(library_path)) {
      if (entry.is_regular_file()) {
        auto ext = entry.path().extension().string();
        if (ext == ".typ" || ext == ".typst") {
          result = entry.path();
          return result;
        }
      }
    }
    result.reset();
    return result;
  }
  void updateLibraryData() {
    library_data.clear();
    for (subCommands::Config::ConfigData config_data : config.config_datas) {
      std::filesystem::path library_path =
          init.library_directory / config_data.name;
      std::optional<std::filesystem::path> source_file_path =
          searchOneOfSourceFilePath(library_path, config_data.name);
      if (!source_file_path.has_value()) {
        continue;
      }

      libraryData data{config_data, source_file_path.value()};
      library_data.push_back(data);
    }
  }
  bool checkLibraryInitialization() {
    if (library_data.empty()) {
      std::cerr << "No libraries found. Please run 'init' first." << std::endl;
      return false;
    }
    for (auto &lib_data : library_data) {
      if (!std::filesystem::exists(lib_data.source_path)) {
        std::cerr << "This directory hasn't been initialized. : "
                  << lib_data.source_path << std::endl;
        return false;
      }
    }
    return true;
  };
};