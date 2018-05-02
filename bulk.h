#ifndef bulk_h
#define bulk_h

#include "parser.h"

#include <iostream>
#include <vector>


//--------<Observer>--------------------------------------

using Commands = std::vector<std::string>;

class Observer {
protected:
  Commands* commands = nullptr;
public:
  virtual void update(Commands& commands) {
    this->commands = &commands;
  }
  virtual void print() = 0;
};

//--------</Observer>-------------------------------------


//--------<Handler>---------------------------------------

class Handler {
  std::vector<Observer*> writers;
  Commands commands;
  BlockParser parser;
  int N;

  void print() {
    for(auto& writer : writers)
      writer->print();
  };

  void update() {
    for(auto& writer : writers)
      writer->update(commands);
  }

public:
  void subscribe(Observer *obs) {
    writers.push_back(obs);
  }

  void setN(const int& n) {
    N = n;
  }

  void addCommand(const std::string& command) { 
    switch(parser.pars(command))
    {
      case BlockParser::Empty: 
        break;

      case BlockParser::StartBlock: 
        N = -1; 
        commands.clear();
        break;

      case BlockParser::CancelBlock:
        N = commands.size();
        break;

      case BlockParser::Command:
        commands.push_back(command);
        update();
        break;

      default: break;
    }
    
    if (commands.size() == N) {
      print();
      commands.clear();
    }
  }

  void stop() {
    if (N != -1 && commands.size())
      print();
    commands.clear();
  }
};

//--------</Handler>---------------------------------------


#endif
