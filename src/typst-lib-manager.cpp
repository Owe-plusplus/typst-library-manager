#include "CLI/CLI11.hpp"
#include "cmd/config.hpp"
#include <optional>
#include <string>
#include <unistd.h>

bool workspaceInit(std::string config_path) { return true; }
bool add_project(std::string project_name, std::optional<std::string> file_name,
                 std::optional<std::string> lib_name) {
  return true;
}

int main(int argc, char *argv[]) {
  std::string config_path =
      std::string(getenv("HOME")) + "/.typst_library_manager/config.json";
  cmd::Config config(config_path);
  CLI::App app{"Typst Manager"};

  CLI::App *init_cmd =
      app.add_subcommand("init", "Initialize a new Typst project");
  init_cmd->callback([&]() { workspaceInit(config_path); });

  std::string project_name;
  std::optional<std::string> filename;
  std::optional<std::string> libname;
  CLI::App *add_cmd = app.add_subcommand("add", "Add a new Typst project");
  add_cmd
      ->add_option("project_name", project_name, "Name of the project to add")
      ->required();
  auto filename_opt = add_cmd->add_option("-n,--filename", filename,
                                          "Name of the Typst file to create");
  auto libname_opt = add_cmd->add_option("-l,--library", libname,
                                         "Name of the Typst library to use");
  add_cmd->callback([&]() { add_project(project_name, filename, libname); });
}