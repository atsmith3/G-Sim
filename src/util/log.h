/*
 * Andrew Smith
 * Logger Class
 *
 * 11/03/2019
 *
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <iostream>

namespace Utility {

class Log {
private:
  std::ofstream logfile;
  std::string fname;

public:
  Log(std::string name);
  ~Log();

  void write(std::string s);
  void close();
};

}; // namespace Utility

namespace SimObj {

inline Utility::Log sim_out("simulator_output.log");

}; // namespace SimObj

#endif // LOGGER_H
