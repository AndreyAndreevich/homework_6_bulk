#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <functional>

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
  int N;
  int count = 0;

  void clear() {
    count = 0;
    commands.clear();
  }

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
    commands.push_back(command);
    update();
    count++;
    if (count == N) {
      print();
      clear();
    }
  }
};

//--------</Handler>---------------------------------------

//--------<Writers>---------------------------------------

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
  }
};

//---------------------------------------------------------------------------------

class FileWriter : public Observer {
  std::ofstream file;
  std::function<std::string()> name_generator;
public:
  FileWriter(Handler *handler = nullptr) {   
    name_generator = [](){return std::string{"aaaa.txt"};};
    if (handler)
      handler->subscribe(this);
  }

  FileWriter(std::string filename, Handler *handler = nullptr) {
    name_generator = [filename](){return filename;};
    if (handler)
      handler->subscribe(this);
  }

  void update(Commands& commands) {
    if (!commands.empty() && !file.is_open())
      file.open(name_generator());
    Observer::update(commands);
  } 

  void print() override {
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