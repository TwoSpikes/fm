#include <iostream>
#include <vector>
#include <string>

int main(int argc, char **argv) {
  auto &args = *new std::vector<std::string>;
  for(int i = 1; i < argc; ++i) {
    args.push_back(argv[i]);
  }
  if(args.size()) {
    if(!args[0].compare("ls")) {
      for(int i = 1; i < args.size(); ++i) {
	std::cout << args[i] << std::endl;
      }
      return 0;
    }
  }
  return 0;
}
