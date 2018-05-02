#ifndef writers_h
#define writers_h

#include "bulk.h"

#include <sstream>
#include <fstream>
#include <functional>
#include <ctime>

class ConsoleWriter : public Observer {
  std::ostream* out;
public:
  ConsoleWriter(Handler *handler = nullptr) {
    out = &std::cout;
    if (handler)
      handler->subscribe(this);
  }
  
  ConsoleWriter(std::ostream& out_stream, Handler *handler = nullptr) {
    out = &out_stream;
    if (handler)
      handler->subscribe(this);
  }

  void print() override {
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
  std::function<std::string()> name_generator;
  std::string name;
public:
  FileWriter(Handler *handler = nullptr) {   
    name_generator = [](){
      std::stringbuf out_buffer;
      std::ostream out_stream(&out_buffer);
      out_stream << "bulk" << std::time(nullptr) << ".log";
      return out_buffer.str();
    };
    if (handler)
      handler->subscribe(this);
  }

  FileWriter(std::string filename, Handler *handler = nullptr) {
    name_generator = [filename](){return filename;};
    name = name_generator();
    if (handler)
      handler->subscribe(this);
  }

  void update(Commands& commands) {
    if (commands.size() == 1)
      name = name_generator();
    Observer::update(commands);
  } 

  void print() override {
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