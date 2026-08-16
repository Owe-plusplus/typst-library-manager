#pragma once

#include "library.hpp"
#include <optional>
#include <string>
namespace subCommands {
class Add {
  Library library;

public:
  Add(const Library &library) : library(library) {}
  ~Add() {}

  bool add_document(std::string file_name,
                    std::optional<std::string> lib_name) {
    std::string document_name = file_name;
    std::string library_name =
        lib_name.value_or(library.library_data[0].config_data.name);
    std::filesystem::path document_path =
        std::filesystem::current_path() / (document_name + ".typ");
    if (document_path.has_parent_path()) {
      try {
        std::filesystem::create_directories(document_path.parent_path());
      } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        return false;
      }
    }
    if (std::filesystem::exists(document_path)) {
      std::cerr << "Error: File already exists: " << document_path << std::endl;
      return false;
    }
    std::ofstream document_file(document_path);
    if (!document_file.is_open()) {
      std::cerr << "Error creating file: " << document_path << std::endl;
      return false;
    }
    Library::libraryData selected_library;
    bool library_found = false;
    for (const auto &lib_data : library.library_data) {
      if (lib_data.config_data.name == library_name) {
        selected_library = lib_data;
        library_found = true;
        break;
      }
    }
    if (!library_found) {
      std::cerr << "Error: Library not found: " << library_name << std::endl;
      selected_library = library.library_data[0];
    }
    std::filesystem::path relative_library_path =
        std::filesystem::relative(selected_library.source_path,
                                  document_path.parent_path())
            .lexically_normal();
    document_file << "#import \"" << relative_library_path.string() << "\" : *"
                  << "\n"
                  << "#show : setup" << "\n";
    document_file.close();
    std::cout << "Document " << file_name << " created successfully at "
              << document_path << std::endl;
    return true;
  }

  bool add_project(std::string project_name,
                   std::optional<std::string> file_name,
                   std::optional<std::string> lib_name) {
    std::string floder_name = project_name;
    std::string document_name = file_name.value_or("main.typ");
    std::string library_name =
        lib_name.value_or(library.library_data[0].config_data.name);
    std::filesystem::path document_path = std::filesystem::current_path() /
                                          floder_name /
                                          (document_name + ".typ");
    if (document_path.has_parent_path()) {
      try {
        std::filesystem::create_directories(document_path.parent_path());
      } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        return false;
      }
    }
    if (std::filesystem::exists(document_path)) {
      std::cerr << "Error: File already exists: " << document_path << std::endl;
      return false;
    }
    std::ofstream document_file(document_path);
    if (!document_file.is_open()) {
      std::cerr << "Error creating file: " << document_path << std::endl;
      return false;
    }
    Library::libraryData selected_library;
    bool library_found = false;
    for (const auto &lib_data : library.library_data) {
      if (lib_data.config_data.name == library_name) {
        selected_library = lib_data;
        library_found = true;
        break;
      }
    }
    if (!library_found) {
      std::cerr << "Error: Library not found: " << library_name << std::endl;
      selected_library = library.library_data[0];
    }
    std::filesystem::path relative_library_path =
        std::filesystem::relative(selected_library.source_path,
                                  document_path.parent_path())
            .lexically_normal();
    document_file << "#import \"" << relative_library_path.string() << "\" : *"
                  << "\n"
                  << "#show : setup" << "\n";
    document_file.close();
    std::cout << "Project " << project_name << " created successfully at "
              << document_path << std::endl;
    return true;
  }
};
} // namespace subCommands