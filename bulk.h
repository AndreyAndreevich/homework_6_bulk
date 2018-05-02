#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <functional>

//--------<Observer>--------------------------------------

class Observer {
protected:
  std::vector<std::string> commands;
public:
  virtual void print() = 0;
  virtual void addCommand(std::string command) {
    commands.push_back(command);
  }
  virtual void clear() {
    commands.clear();
  }
};

//--------</Observer>-------------------------------------

//--------<Writers>---------------------------------------

class ConsoleWriter : public Observer {
  std::ostream* out;
public:
  ConsoleWriter() {
    out = &std::cout;
  }
  
  ConsoleWriter(std::ostream& out_stream) {
    out = &out_stream;
  }

  void print() override {
    *out << "bulk: ";
    for(auto command = commands.cbegin(); command < commands.cend(); command++) {
      if (command != commands.cbegin())
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
  FileWriter() {   
    name_generator = [](){return std::string{"aaaa.txt"};};
  }

  FileWriter(std::string filename) {
    name_generator = [filename](){return filename;};
  }

  void addCommand(std::string command) override {
    if (commands.empty())
      file.open(name_generator());
    Observer::addCommand(command);
  }

  void clear() override {
    file.close();
    Observer::clear();
  }

  void print() override {
    file << "bulk: ";
    for(auto command = commands.cbegin(); command < commands.cend(); command++) {
      if (command != commands.cbegin())
        file << ", ";
      file << *command;
    }
  }
};

//--------</Writers>--------------------------------------

//--------<Handler>---------------------------------------

class Handler {
  std::vector<std::unique_ptr<Observer>> writers;
  int N;
  int count = 0;

  void clear() {
    count = 0;
    for(auto& writer : writers)
      writer->clear();
  }

  void print() {
    for(auto& writer : writers)
      writer->print();
  };

public:
  void addWriter(Observer* obs) {
    writers.emplace_back(obs);
  }

  void setN(const int& n) {
    N = n;
  }

  void addCommand(std::string command) {
    for(auto& writer : writers)
      writer->addCommand(command);
    count++;
    if (count == N) {
      print();
      clear();
    }
  }
};

//--------</Handler>---------------------------------------