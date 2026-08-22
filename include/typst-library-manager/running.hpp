#pragma once
#include <cerrno>
#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <future>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

class Running {
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
  Running() = default;

  ~Running() = default;

  bool runGitClone(const std::string &url, const std::string &directory_name,
                   const std::filesystem::path &working_directory,
                   const std::string &loading_message,
                   const std::string &complete_message) {
    std::filesystem::path target_path(directory_name);
    if (directory_name.empty() || target_path.filename() != target_path ||
        directory_name == "." || directory_name == "..") {
      std::cerr << "Invalid library directory name: " << directory_name
                << std::endl;
      return false;
    }

    try {
      if (!std::filesystem::exists(working_directory)) {
        std::filesystem::create_directories(working_directory);
      }
    } catch (const std::filesystem::filesystem_error &e) {
      std::cerr << "Error creating directory: " << e.what() << std::endl;
      return false;
    }

    pid_t pid = fork();
    if (pid < 0) {
      std::cerr << "Error starting git: " << std::strerror(errno) << std::endl;
      return false;
    }

    if (pid == 0) {
      if (chdir(working_directory.c_str()) != 0) {
        _exit(127);
      }

      int null_fd = open("/dev/null", O_WRONLY);
      if (null_fd >= 0) {
        dup2(null_fd, STDOUT_FILENO);
        dup2(null_fd, STDERR_FILENO);
        close(null_fd);
      }

      execlp("git", "git", "clone", "--", url.c_str(),
             directory_name.c_str(), static_cast<char *>(nullptr));
      _exit(127);
    }

    auto future = std::async(std::launch::async, [pid]() {
      int status = 0;
      return waitpid(pid, &status, 0) == pid ? status : -1;
    });

    showSpinner(future, loading_message);

    int status = future.get();

    if (status != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0) {
      std::cout << "\r\033[K" << complete_message << std::endl;
      return true;
    }

    std::cerr << "\r\033[KError: git clone failed";
    if (status != -1 && WIFEXITED(status)) {
      std::cerr << " with exit code " << WEXITSTATUS(status);
    }
    std::cerr << std::endl;
    return false;
  }
};
