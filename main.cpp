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

enum Condition {
  STD = 0b1,
  DIR = 0b10,
};
Condition operator|(Condition a, Condition b) {
  return static_cast<Condition>( static_cast<u8>(a) | static_cast<u8>(b) );
}
Condition operator&(Condition a, Condition b) {
  return static_cast<Condition>( static_cast<u8>(a) & static_cast<u8>(b) );
}
Condition operator|=(Condition &a, Condition b) {
  return a = a | b;
}
Condition operator&=(Condition &a, Condition b) {
  return a = a & b;
}
Condition operator~(Condition a) {
  return static_cast<Condition>( ~static_cast<u8>(a) );
}
Condition const DEFAULT_CONDITION = STD;

//checks, print file or not
bool CheckCondition(const fs::path &path,
		    Condition condition = DEFAULT_CONDITION) {
  if(condition & STD && path.filename().u8string()[0] == '.' )
    return false;
  if(condition & DIR && fs::is_directory(path) )
    return false;
  return true;
}

//get extension of file
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

//print file
void DoHandlePath(fs::path &path,
		  u8 condition = DEFAULT_CONDITION) {
  path = path.filename();
  auto &pathStr = *new std::string(path.c_str());
  if(fs::is_directory(path)) {
    std::cout << "\033[36m";
  } else if(pathStr.back() == '~') {
    std::cout << "\033[37m";
  } else {
    std::cout << "\033[1m\033[37m";
  }
  std::cout << pathStr << std::endl << "\033[0m";
}
void HandlePath(fs::path &dir,
		u8 condition = DEFAULT_CONDITION) {
  try {
    DoHandlePath(dir, condition);
  } catch(...) {
    ReportError(*new std::string(boost::str(boost::format("Unexisting directory: %1%\n") % *new std::string(dir.c_str()) )), false);
  }
}

//return condition
std::pair<bool, Condition>
HandleOption(std::string &option) {
  if(!option.compare("a") || !option.compare("-all"))
    return {false, STD};
  if(!option.compare("d") || !option.compare("-dirs") || !option.compare("-directories"))
    return {true, DIR};
  throw boost::str(boost::format("Unknown option: \"%1%\"") % option);
}

void HandleProvided(fs::path const &path,
		    std::vector<size_t> &lengths,
		    Condition condition = DEFAULT_CONDITION,
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
  
  Condition condition  = DEFAULT_CONDITION;
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
	 !src.compare("--recurse") ||
	 !src.compare("--recursed") ||
	 !src.compare("--recursive")) {
	recursed = true;
	continue;
      }
      //other options check
      if(src[0] == '-') {
	//future result of function
	auto &tmp = *new std::pair<bool, Condition>;
	try {
	  tmp = HandleOption(*new std::string(src.substr(1)));
	} catch(std::string &e) {
	  ReportError(e, false);
	}
	//if has to add condition to condition
	if(tmp.first) {
	  condition |= tmp.second;
	} else {
	  condition &= ~tmp.second;
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
