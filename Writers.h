#ifndef writers_h
#define writers_h

#include <sstream>
#include <fstream>
#include <ctime>

#include "Observer.h"

class ConsoleWriter : public Observer {
  std::ostream* out;
public:
  ConsoleWriter();
  ConsoleWriter(std::ostream& out_stream);
  void print() override;
};

//---------------------------------------------------------------------------------

class FileWriter : public Observer {
  std::ofstream file;
  std::time_t time;
  std::string name;
  int section = 0;
public:
  FileWriter();
  void update(const std::weak_ptr<Commands>& commands) override;
  void print() override;
  std::string getName();
  std::time_t getTime();
};

#endif