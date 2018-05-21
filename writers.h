#ifndef writers_h
#define writers_h

#include "bulk.h"

#include <sstream>
#include <fstream>
#include <functional>
#include <ctime>

class ConsoleWriter : public Observer {
  std::ostream* out;
  std::shared_ptr<Observer> thisPtr;
public:
  ConsoleWriter(Handler *handler) {
    thisPtr = std::shared_ptr<Observer>(this,[](auto p){});
    out = &std::cout;
    if (handler)
      handler->subscribe(thisPtr);
  }
  
  ConsoleWriter(std::ostream& out_stream, Handler *handler = nullptr) {
    thisPtr = std::shared_ptr<Observer>(this,[](auto p){});
    out = &out_stream;
    if (handler)
      handler->subscribe(thisPtr);
  }

  void print() override {
    if (_commands.expired()) {
      throw std::runtime_error("commands do not exist");
    }
    auto commands = _commands.lock();
    *out << "bulk: ";
    for(auto command = commands->cbegin(); command < commands->cend(); command++) {
      if (command != commands->cbegin())
        *out << ", ";
      *out << *command;
    }
    *out << std::endl;
  }
};

//---------------------------------------------------------------------------------

class FileWriter : public Observer {
  std::ofstream file;
  std::shared_ptr<Observer> thisPtr;
  std::weak_ptr<std::time_t> time;
  std::string name;
public:
  FileWriter(Handler *handler) { 
    thisPtr = std::shared_ptr<Observer>(this,[](auto p){});
    if (handler)
      handler->subscribe(thisPtr);
  }

  FileWriter(std::weak_ptr<std::time_t> time_, Handler *handler = nullptr) {
    thisPtr = std::shared_ptr<Observer>(this,[](auto p){});
    time = time_;
    if (handler)
      handler->subscribe(thisPtr);
  }

  void update(std::weak_ptr<Commands> commands) {
    if (commands.expired()) {
      throw std::runtime_error("commands do not exist");
    }
    if (commands.lock()->size() == 1) {
      std::stringbuf out_buffer;
      std::ostream out_stream(&out_buffer);
      std::time_t* time_ = time.expired() ? nullptr : time.lock().get();
      out_stream << "bulk" << std::time(time_) << ".log";
      name = out_buffer.str();
    }
    Observer::update(commands);
  } 

  void print() override {
    if (_commands.expired()) {
      throw std::runtime_error("commands do not exist");
    }
    auto commands = _commands.lock();
    file.open(name);
    file << "bulk: ";
    for(auto command = commands->cbegin(); command < commands->cend(); command++) {
      if (command != commands->cbegin())
        file << ", ";
      file << *command;
    }
    file.close();
  }
};
//--------</Writers>--------------------------------------

#endif