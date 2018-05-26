#ifndef parser_h
#define parser_h

#include <string>

class BlockParser {
  int blocks_count = 0;
public:

  enum Block {
    StartBlock,
    CancelBlock,
    Command,
    Empty
  };

  Block pars(const std::string& line) {
    if (line.size() == 0) 
      return Block::Empty;

    if (line.find("{") != std::string::npos) {
      blocks_count++;
      if (blocks_count > 1) 
        return Block::Empty;
      else 
        return Block::StartBlock;
    }

    if(line.find("}") != std::string::npos) {
      if (blocks_count == 0) 
        return Block::Empty;

      blocks_count--;
      if (blocks_count != 0) 
        return Block::Empty;
      else 
        return Block::CancelBlock;
    }
    return Block::Command;
  }
};

#endif
