#include <filesystem>
namespace fs = std::__fs::filesystem;
#include <functional>
#include <iostream>
#include <vector>
#include <string>

bool stdCondition(const fs::path &x) {
  return x.c_str()[2] != '.';
}

bool allCondition(const fs::path &x) {
  return true;
}

bool AllCondition(const fs::path &x) {
  return !strcmp(x.c_str(), "./..") &&
    !strcmp(x.c_str(), "./.");
}

int main(int argc, char **argv) {
  auto &args = *new std::vector<std::string>;
  for(int i = 1; i < argc; ++i) {
    args.push_back(argv[i]);
  }
  bool(*condition)(const fs::path &x) = &stdCondition;
  for(int i = 0; i < args.size(); ++i) {
    auto &src = *new std::string(args[i]);
    if(!src.compare("-a") ||
       !src.compare("--all")) {
      condition = &allCondition;
      continue;
    }
    for(const auto &entry: fs::directory_iterator(src)) {
      auto &p = entry.path();
      if(!condition(p)) {
	continue;
      }
      if(fs::is_directory(p)) {
	std::cout << "\033[1;32m";
      } else {
	std::cout << "\033[1;37m";
      }
      std::cout << *new std::string(p.c_str()) << std::endl;
    }
  }
  return 0;
}
