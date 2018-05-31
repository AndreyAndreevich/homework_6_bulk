#include "Parser.h"

#include <stdexcept>

int start_parsing(int argc, char *argv[]) {
  if (argc < 2) {
    throw std::runtime_error("The parameter is missing");
  }
  auto N = std::atoi(argv[1]);
  if (N == 0) {
    throw std::runtime_error("Incorrect parameter");
  }
  return N;
}

BlockParser::Block BlockParser::parsing(const std::string& line) {
  if (line.size() == 0) 
    return Block::Command;

  if (line.find("{") != std::string::npos && line.size() == 1) {
    blocks_count++;
    if (blocks_count > 1) 
      return Block::Empty;
    else 
      return Block::StartBlock;
  }

  if(line.find("}") != std::string::npos && line.size() == 1) {
    if (blocks_count == 0) 
      return Block::Command;

    blocks_count--;
    if (blocks_count != 0) 
      return Block::Empty;
    else 
      return Block::CancelBlock;
  }
  return Block::Command;
}