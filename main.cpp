#include <filesystem>
namespace fs = std::__fs::filesystem;
#include <functional>
#include <algorithm>
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

struct Condition {
  bool STD = true;
  // 0 - Print only dirs
  // 1 - Print dirs and files
  // 2 - Print only files
  enum Dir {
    DIRS,
    ALL,
    FILES,
  };
  Dir DIR = ALL;
};

//checks, print file or not
bool CheckCondition(const fs::path &path,
		    Condition condition = {}) {
  return !(condition.STD && path.filename().u8string()[0] == '.' ||
	   condition.DIR == Condition::FILES && fs::is_directory(path) ||
	   condition.DIR == Condition::DIRS && !fs::is_directory(path));
}

void PrepareColor(fs::path &path) {
  auto ext = path.extension().u8string();
  if(ext.length()) {
    if(!ext.compare(".sh")) {
      std::cout << "\033[38;05;215m";
      return;
    }
    if(!ext.compare(".cpp")) {
      std::cout << "\033[38;05;033m";
      return;
    }
    if(!ext.compare(".out")) {
      std::cout << "\033[38;05;010m";
      return;
    }
  }
}
//print file
void DoHandlePath(fs::path &path,
		  Condition condition = {}) {
  std::cout << "\033[0m";
  path = path.filename();
  auto &pathStr = *new std::string(path.c_str());
  if(fs::is_directory(path)) {
    std::cout << "\033[01m\033[38;05;049m";
  } else if(pathStr.back() == '~') {
    std::cout << "\033[38;05;241m";
  } else if(path.extension().u8string().length()) {
    PrepareColor(path);
  } else {
    std::cout << "\033[1m\033[37m";
  }
  std::cout << pathStr << std::endl << "\033[0m";
}
void HandlePath(fs::path &dir,
		Condition condition = {}) {
  try {
    DoHandlePath(dir, condition);
  } catch(...) {
    ReportError(*new std::string(boost::str(boost::format("Unexisting directory: %1%\n") % *new std::string(dir.c_str()) )), false);
  }
}

//return condition
void DoHandleOption(std::string &option,
			 auto &condition = *new Condition) {
  if(!option.compare("a") ||
     !option.compare("all"))
    condition.STD = false;
  else if(!option.compare("D") ||
     !option.compare("not-dirs") ||
     !option.compare("only-files"))
    condition.DIR = Condition::FILES;
  else if(!option.compare("d") ||
     !option.compare("dirs"))
    condition.DIR = Condition::DIRS;
  else throw boost::str(boost::format("Unknown option: \"%1%\"\n") % option);
}
void HandleOption(std::string &option,
		  auto &condition = *new Condition) {
  try {
    DoHandleOption(option, condition);
  } catch(std::string &e) {
    ReportError(e, false);
  }
}
void HandleOption(char option,
		  auto &condition = *new Condition) {
  HandleOption(*new std::string(1, option), condition);
}

void HandleProvided(fs::path const &path,
		    std::vector<size_t> &lengths,
		    Condition condition = {},
		    bool reversed = false,
		    bool recursed = false) {
  std::vector<fs::path> tmp;
  
  //handle recursed
  if(!recursed) {
    for(auto it: fs::directory_iterator(path)) {
      tmp.push_back(it.path());
    }
  } else {
    for(auto it: fs::recursive_directory_iterator(path)) {
      tmp.push_back(it.path());
    }
  }

  //handle reversed
  if(reversed) {
    std::reverse(tmp.begin(), tmp.end());
  }
  
  {
    for( auto it = tmp.begin();
	 it != tmp.end();
	 ++it ) {
      if(CheckCondition(*it, condition)) {
	HandlePath(*it, condition);
      }
    }
  }
}

int main(int argc, char **argv) {
  //fill args
  auto &args = *new std::vector<std::string>();
  for(int i = 1; i < argc; ++i) {
    args.push_back(argv[i]);
  }

  //provided directories
  auto &provided  = *new std::vector<fs::path const>;
  //lengths of directories prefixes (like "./" in "./build.sh")
  auto &lengths = *new std::vector<size_t>;
  
  Condition condition{};
  bool reversed = false;
  bool recursed = false;
  //for every arg
  for(int i = 0; i < args.size(); ++i) {
    //current arg
    auto &src = *new std::string(args[i]);
    {
      //-r option check
      if(!src.compare("-r") ||
	 !src.compare("--reversed")) {
	reversed = true;
	continue;
      }
      //-R option check
      if(!src.compare("-R") ||
	 !src.compare("--recursive")) {
	recursed = true;
	continue;
      }
      //other options check
      if(src[0] == '-') {
	if(src[1] == '-') {
	  HandleOption(*new std::string(src.substr(2)), condition);
	  continue;
	}
	//if one-symbol option
	auto optbuf = src.substr(1);
        for(char tmp: optbuf) {
	  HandleOption(tmp, condition);
	}
	continue;
      }
    }

    provided.push_back(fs::u8path(src));
  }

  if(!provided.size()) {
    provided.push_back(fs::u8path("./"));
  }

  //handle every provided
  for(auto it: provided) {
    // void HandleProvided(std::vector<fs::path const> &provided,
    // 			std::vector<size_t> &lengths,
    // 			Condition condition = DEFAULT_CONDITION,
    // 			bool reversed = false,
    // 			bool recursed = false);
    HandleProvided(it, lengths, condition, reversed, recursed);
  }
  
  return 0;
}
