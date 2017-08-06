/*
 * ConfigFile.h
 *
 *  Created on: Sep 8, 2015
 *      Author: root
 */

#ifndef CONFIGFILE_H_
#define CONFIGFILE_H_

#include <fstream>
#include <string>
#include <vector>
#include <string.h>

class ConfigFile {
 public:
  ConfigFile(std::string configFile, std::string port);
  virtual ~ConfigFile();

  bool m_skipLaunch;
  std::string m_port;
  char **m_args;
};

#endif /* CONFIGFILE_H_ */
