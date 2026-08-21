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

  bool force_init = false;
  CLI::App app{"Typst Manager"};
  CLI::App *init_subCommands =
      app.add_subcommand("init", "Initialize a new Typst project");
  init_subCommands->add_flag("-f, --force", force_init,
                             "Force re-initialization of the workspace");
  init_subCommands->callback([&]() {
    if (!init.workspaceInit(force_init)) {
      exit(EXIT_FAILURE);
    }
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

  CLI::App *reset_subcommand =
      app.add_subcommand("reset", "Reset a library configuration");
  reset_subcommand->callback([&]() {
    if (!config.deleteConfigFile()) {
      exit(EXIT_FAILURE);
    }
  });
  CLI::App *config_subcommand =
      app.add_subcommand("config", "Manage Typst library configuration");

  std::string config_url;
  std::string config_name;
  CLI::App *config_add_subcommand = config_subcommand->add_subcommand(
      "add", "Add a new library configuration");
  config_add_subcommand
      ->add_option("-u,--url", config_url, "URL of the library to add")
      ->required();
  config_add_subcommand
      ->add_option("-n,--name", config_name, "Name of the library to add")
      ->required();
  config_add_subcommand->callback([&]() {
    if (!config.addConfigData(config_url, config_name)) {
      exit(EXIT_FAILURE);
    }
  });

  CLI::App *config_remove_subcommand = config_subcommand->add_subcommand(
      "remove", "Remove a library configuration");
  config_remove_subcommand
      ->add_option("-n,--name", config_name, "Name of the library to remove")
      ->required();
  config_remove_subcommand->callback([&]() {
    if (!config.removeConfigData(config_name)) {
      exit(EXIT_FAILURE);
    }
  });

  CLI11_PARSE(app, argc, argv);
  return 0;
}