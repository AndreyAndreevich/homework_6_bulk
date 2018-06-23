#include <iostream>

#include "Writers.h"
#include "Parser.h"

int main(int argc, char *argv[]) 
{
  try {
    auto handler = std::make_shared<Handler>(start_parsing(argc,argv));
    auto consoleWriter = std::make_shared<ConsoleWriter>();
    auto fileWriter = std::make_shared<FileWriter>();
    consoleWriter->subscribe(handler);
    fileWriter->subscribe(handler);  
    std::string line;
    while (std::getline(std::cin, line)) {
      handler->addCommand(line);
    }
    handler->stop();
  } catch(const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}