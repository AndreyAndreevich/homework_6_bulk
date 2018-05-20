#ifndef bulk_h
#define bulk_h

#include "parser.h"

#include <iostream>
#include <vector>
#include <memory>


//--------<Observer>--------------------------------------

using Commands = std::vector<std::string>;

class Observer {
protected:
  std::weak_ptr<Commands> _commands;
public:
  virtual void update(std::weak_ptr<Commands> commands) {
    _commands = commands;
  }
  virtual void print() = 0;
};

//--------</Observer>-------------------------------------


//--------<Handler>---------------------------------------

class Handler {
  std::vector<std::weak_ptr<Observer>> writers;
  std::shared_ptr<Commands> commands;
  BlockParser parser;
  int N;

  void print() {
    for(auto& writer : writers) {
      if (!writer.expired()) {
        writer.lock()->print();
      }
    }
  };

  void update() {
    for(auto& writer : writers) {
      if (!writer.expired()) {
        writer.lock()->update(commands);
      }
    }
  }

public:
  Handler() {
    commands = std::make_shared<Commands>();
  }
  void subscribe(std::weak_ptr<Observer> obs) {
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
        commands->clear();
        break;

      case BlockParser::CancelBlock:
        N = commands->size();
        break;

      case BlockParser::Command:
        commands->push_back(command);
        update();
        break;

      default: break;
    }
    
    if (commands->size() == N) {
      print();
      commands->clear();
    }
  }

  void stop() {
    if (N != -1 && commands->size())
      print();
    commands->clear();
  }
};

//--------</Handler>---------------------------------------


#endif
