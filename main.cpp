#include <iostream>

#include "Writers.h"

int main(int argc, char *argv[]) 
{
  try {
    auto handler = std::make_shared<Handler>();
    auto consoleWriter = std::make_shared<ConsoleWriter>();
    auto fileWriter = std::make_shared<FileWriter>();
    consoleWriter->subscribe(handler);
    fileWriter->subscribe(handler);

    handler->setN(std::atoi(argv[1]));   
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