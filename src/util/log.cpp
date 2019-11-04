/*
 * Andrew Smith
 * Logger Class
 *
 * 11/03/2019
 *
 */

#include "log.h"

Utility::Log::Log(std::string name) {
  fname = name;
  logfile.open(fname, std::ofstream::out | std::ofstream::out);
}

Utility::Log::~Log() {
  this->close();
}

void Utility::Log::write(std::string s) {
  logfile << s;
}

void Utility::Log::close() {
  if(logfile.is_open()) {
    logfile.close();
  }
}
