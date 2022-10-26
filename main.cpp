#include <filesystem>
namespace fs = std::__fs::filesystem;
#include <functional>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <map>

typedef u_int8_t u8;

enum Condition {
  STD = 0b1,
  DIR = 0b10,
};

bool CheckCondition(u8 condition, const fs::path &x) {
  if(!condition) return true;
  if(condition & STD && x.c_str()[2] == '.' )
    return false;
  if(condition & DIR && fs::is_directory(x) )
    return false;
  return true;
}

int main(int argc, char **argv) {
  auto &args = *new std::vector<std::string>();
  for(int i = 1; i < argc; ++i) {
    args.push_back(argv[i]);
  }
  u8 condition = STD;
  bool d_option = false;
  for(int i = 0; i < args.size(); ++i) {
    auto &src = *new std::string(args[i]);
    if(!src.compare("-a") ||
       !src.compare("--all")) {
      condition &= ~STD;
      continue;
    }
    if(!src.compare("-d") ||
       !src.compare("--dirs") ||
       !src.compare("--directories")) {
      condition |= DIR;
      continue;
    }
    if(src[0] == '-') {
      std::cout << "Unknown option: " << src << std::endl;
      continue;
    }
    for(const auto &entry: fs::directory_iterator(src)) {
      auto &p = entry.path();
      auto &ps = *new std::string(p.c_str());
      if(!CheckCondition(condition, p)) {
	continue;
      }
      if(fs::is_directory(p)) {
	std::cout << "\033[36m";
      } else if(ps.back() == '~') {
	std::cout << "\033[37m";
      } else {
	std::cout << "\033[1m\033[37m";
      }
      std::cout <<
	*new std::string(p.c_str()) <<
	std::endl <<
	"\033[0m";
    }
  }
  return 0;
}
