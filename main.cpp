#include <iostream>

#include "bulk.h"
#include "writers.h"

int main() 
{
  try {
    int N;
    std::cin >> N;

    std::unique_ptr<Handler> handler(new Handler);
    std::unique_ptr<ConsoleWriter> consoleWriter(new ConsoleWriter(handler.get()));
    std::unique_ptr<FileWriter> fileWriter(new FileWriter(handler.get()));

    handler->setN(N);

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