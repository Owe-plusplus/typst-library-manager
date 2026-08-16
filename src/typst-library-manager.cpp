#include "CLI/CLI11.hpp"
#include "library.hpp"
#include "subCommands/add.hpp"
#include "subCommands/config.hpp"
#include "subCommands/init.hpp"
#include <optional>
#include <string>
#include <unistd.h>

int main(int argc, char *argv[]) {
  subCommands::Config config = subCommands::Config();
  if (!config.createConfigFile() || !config.readConfigFile()) {
    exit(EXIT_FAILURE);
  }
  std::vector<subCommands::Config::ConfigData> config_datas;
  config_datas = config.config_datas;
  subCommands::Init init = subCommands::Init(config);

  Library library = Library(config, init);

  subCommands::Add add = subCommands::Add(library);

  CLI::App app{"Typst Manager"};
  CLI::App *init_subCommands =
      app.add_subcommand("init", "Initialize a new Typst project");
  init_subCommands->callback([&]() {
    if (!init.workspaceInit())
      exit(EXIT_FAILURE);
  });

  std::string project_name;
  std::optional<std::string> file_name;
  std::optional<std::string> lib_name;
  bool create_doc_only = false;
  CLI::App *add_subCommands =
      app.add_subcommand("add", "Add a new Typst project");
  add_subCommands
      ->add_option("project_name", project_name, "Name of the project to add")
      ->required();
  add_subCommands->add_option("-n,--docname", file_name,
                              "Name of the Typst file to create");
  add_subCommands->add_option("-l,--library", lib_name,
                              "Name of the Typst library to use");
  add_subCommands->add_flag("-d, --doc", create_doc_only,
                            "Create only new typst document");
  add_subCommands->callback([&]() {
    if (!library.checkLibraryInitialization()) {
      exit(EXIT_FAILURE);
    }
    if (!create_doc_only) {
      add.add_project(project_name, file_name, lib_name);
    } else {
      add.add_document(project_name, lib_name);
    }
  });

  CLI11_PARSE(app, argc, argv);
  return 0;
}