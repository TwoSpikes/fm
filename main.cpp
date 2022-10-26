#include <filesystem>
namespace fs = std::__fs::filesystem;
#include <functional>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <deque>
#include <map>

typedef u_int8_t u8;

enum Condition {
  STD = 0b1,
  DIR = 0b10,
};

bool CheckCondition(u8 condition,
		    const fs::path &x,
		    size_t lenCurDir = 0) {
  if(!condition) return true;
  if(condition & STD && x.c_str()[lenCurDir] == '.' )
    return false;
  if(condition & DIR && fs::is_directory(x) )
    return false;
  return true;
}

void HandlePath(const fs::path &path,
		u8 condition = 0b0,
		size_t lenCurDir = 0) {
  auto &pathStr = *new std::string(path.c_str());
  pathStr = pathStr.substr(lenCurDir);
  if(!CheckCondition(condition, path, lenCurDir)) {
    return;
  }
  if(fs::is_directory(path)) {
    std::cout << "\033[36m";
  } else if(pathStr.back() == '~') {
    std::cout << "\033[37m";
  } else {
    std::cout << "\033[1m\033[37m";
  }
  std::cout << pathStr << std::endl << "\033[0m";
}

int main(int argc, char **argv) {
  auto &args = *new std::vector<std::string>();
  for(int i = 1; i < argc; ++i) {
    args.push_back(argv[i]);
  }
  u8 condition  = STD;
  bool reversed = false;
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
    if(!src.compare("-r") ||
       !src.compare("--reverse")) {
      reversed = true;
      continue;
    }
    if(src[0] == '-') {
      std::cout << "Unknown option: " << src << std::endl;
      continue;
    }

    const size_t lenCurDir = src.length() - (src.back() == '/');
    if(!reversed)
      for(const auto &entry: fs::directory_iterator(src))
	HandlePath(entry.path(), condition, lenCurDir);
    auto &res = *new std::deque<const fs::path>;
    for(const auto &entry: fs::directory_iterator(src))
      res.push_front(entry.path());
    for(auto it = res.rbegin(); it != res.rbegin(); --it)
      HandlePath(*it, condition, lenCurDir);
  }
  return 0;
}
