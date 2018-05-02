#include <iostream>

#include "bulk.h"
#include "writers.h"

int main() 
{
  int N;
  std::cin >> N;

  Handler handler;
  ConsoleWriter consoleWriter(&handler);
  FileWriter fileWriter(&handler);

  handler.setN(N);

  std::string line;
  while (std::getline(std::cin, line)) {
    handler.addCommand(line);
  }
  handler.stop();

  return 0;
}