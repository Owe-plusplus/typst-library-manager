#pragma once
#include <filesystem>
#include <future>
#include <iostream>
#include <string>
#include <vector>

class Running {

  std::filesystem::path current_path;

  void showSpinner(const std::future<int> &future,
                   const std::string &loading_message) {
    const std::vector<std::string> spinner = {"⠋", "⠙", "⠹", "⠸", "⠼",
                                              "⠴", "⠦", "⠧", "⠇", "⠏"};
    int i = 0;

    while (future.wait_for(std::chrono::milliseconds(80)) ==
           std::future_status::timeout) {
      std::cout << "\r\033[K[" << spinner[i % spinner.size()] << "] "
                << loading_message << std::flush;
      i++;
    }
  }

public:
  Running() { current_path = std::filesystem::current_path(); }

  ~Running() {}

  bool runCommand(const std::string &command, const std::string &path_dir,
                  const std::string &loading_message,
                  const std::string &complete_message) {

    try {
      if (!std::filesystem::exists(path_dir)) {
        std::filesystem::create_directories(path_dir);
      }
    } catch (const std::filesystem::filesystem_error &e) {
      std::cerr << "Error creating directory: " << e.what() << std::endl;
      return false;
    }

    try {
      std::filesystem::current_path(path_dir);
    } catch (const std::filesystem::filesystem_error &e) {
      std::cerr << "Error changing directory: " << e.what() << std::endl;
      return false;
    }

    auto future = std::async(std::launch::async, [command]() {
#ifdef _WIN32
      std::string quiet_command = command + " > nul 2>&1";
#else
      std::string quiet_command = command + " > /dev/null 2>&1";
#endif
      return system(quiet_command.c_str());
    });

    showSpinner(future, loading_message);

    int result = future.get();

    if (result == 0) {
      std::cout << "\r\033[K" << complete_message << std::endl;

      try {
        std::filesystem::current_path(current_path);
      } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Error changing directory back: " << e.what() << std::endl;
        return false;
      }

      return true;
    } else {
      std::cerr << "\r\033[KError: Command failed with exit code " << result
                << std::endl;

      try {
        std::filesystem::current_path(current_path);
      } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Error changing directory back: " << e.what() << std::endl;
        return false;
      }
      return false;
    }
  }
};