#include <filesystem>
namespace fs = std::__fs::filesystem;
#include <functional>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <deque>
#include <utility>
#include <boost/format.hpp>
#include <type_traits>

typedef u_int8_t u8;

void ReportError(std::string &error, bool toExit = true) {
  std::cout << error;
  if(toExit)
    exit(1);
};

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

bool CanCheckCondition(u8 condition,
		       const fs::path &x,
		       size_t lenCurDir = 0) {
  try {
    CheckCondition(condition,
		   x,
		   lenCurDir);
  } catch(...) {
    return false;
  }
  return true;
}


std::string GetExtension(std::string &filename) {
  auto &res = *new std::string;
  for(auto it = filename.begin(); it != filename.end(); ++it) {
    if(*it == '.') {
      res = "";
      continue;
    }
    res += *it;
  }
  return res;
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

std::pair<bool, Condition>
HandleOption(std::string &option) {
  if(!option.compare("a") || !option.compare("-all"))
    return {false, STD};
  if(!option.compare("d") || !option.compare("-dirs") || !option.compare("-directories"))
    return {true, DIR};
  throw boost::str(boost::format("Unknown option: %1%") % option);
}

void DoHandlePath(const fs::path &iterator,
		  u8 condition = 0b0,
		  size_t lenCurDir = 0) {
  try {
    HandlePath(iterator, condition, lenCurDir);
  } catch(...) {
    ReportError(*new std::string(boost::str(boost::format("Unexisting directory: %1%\n") % *new std::string(iterator.c_str()) )), false);
  }
}

int main(int argc, char **argv) {
  auto &args = *new std::vector<std::string>();
  for(int i = 1; i < argc; ++i) {
    args.push_back(argv[i]);
  }
  u8 condition  = STD;
  bool reversed = false;
  bool recursed = false;
  for(int i = 0; i < args.size(); ++i) {
    auto &src = *new std::string(args[i]);
    {
      auto &tmp = *new std::pair<bool, Condition>;
      if(!src.compare("-r") ||
	 !src.compare("--reversed")) {
	reversed = true;
	continue;
      }
      if(!src.compare("-R") ||
	 !src.compare("--recursed") ||
	 !src.compare("--recursive")) {
	recursed = true;
	continue;
      }
      if(src[0] == '-') {
	try {
	  tmp = HandleOption(*new std::string(src.substr(1)));
	} catch(std::string &e) {
	  ReportError(e, false);
	}
	if(tmp.first) {
	  condition |= tmp.second;
	} else {
	  condition &= ~tmp.second;
	}
	continue;
      }
    }

    const size_t lenCurDir = src.length()+1 - (src.back() == '/');
    auto &res = *new std::vector<const fs::path>;
    try {
      if(!recursed)
	for(const auto &entry: fs::directory_iterator(src))
	  res.push_back(entry.path());
      else
	for(const auto &entry: fs::recursive_directory_iterator(src))
	  res.push_back(entry.path());
    } catch(...) {
      ReportError(*new std::string(boost::str(boost::format("No such file or directory: %1%") % src)), false);
    }
    if(!reversed) {
      for(auto it = res.begin(); it != res.end(); ++it) {
	DoHandlePath(*it, condition, lenCurDir);
      }
    }
    for(auto it = res.rbegin(); it != res.rend(); ++it) {
      DoHandlePath(*it, condition, lenCurDir);
    }
  }
}
