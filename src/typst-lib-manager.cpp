#include "CLI/CLI11.hpp"
#include "subCommands/config.hpp"
#include "subCommands/init.hpp"
#include <optional>
#include <string>
#include <unistd.h>

bool add_project(std::string project_name, std::optional<std::string> file_name,
                 std::optional<std::string> lib_name) {
  return true;
}

int main(int argc, char *argv[]) {
  std::string config_path =
      std::string(getenv("HOME")) + "/.typst_library_manager/config.json";
  subCommands::Config config = subCommands::Config(config_path);
  if (!config.createConfigFile() || !config.readConfigFile()) {
    exit(EXIT_FAILURE);
  }
  std::vector<subCommands::Config::ConfigData> config_datas;
  config_datas = config.config_datas;
  subCommands::Init init = subCommands::Init(config);

  CLI::App app{"Typst Manager"};
  CLI::App *init_subCommands =
      app.add_subcommand("init", "Initialize a new Typst project");
  init_subCommands->callback([&]() {
    if (!init.workspaceInit())
      exit(EXIT_FAILURE);
  });

  std::string project_name;
  std::optional<std::string> filename;
  std::optional<std::string> libname;
  CLI::App *add_subCommands =
      app.add_subcommand("add", "Add a new Typst project");
  add_subCommands
      ->add_option("project_name", project_name, "Name of the project to add")
      ->required();
  auto filename_opt = add_subCommands->add_option(
      "-n,--filename", filename, "Name of the Typst file to create");
  auto libname_opt = add_subCommands->add_option(
      "-l,--library", libname, "Name of the Typst library to use");
  add_subCommands->callback(
      [&]() { add_project(project_name, filename, libname); });

  CLI11_PARSE(app, argc, argv);
  return 0;
}