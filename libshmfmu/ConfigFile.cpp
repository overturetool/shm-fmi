/*
 *
 * Shared Memory FMI
 * 
 * Copyright (C) 2015 - 2017 Overture
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/gpl-3.0.html>.
 *
 * Author: Kenneth Lausdahl
 */

#include "ConfigFile.h"

ConfigFile::ConfigFile(std::string configFile, std::string port) {
  this->m_skipLaunch = true;

  std::ifstream file(configFile);
  int index = 0;

  std::vector<std::string> arguments;

  std::string str;
  while (std::getline(file, str)) {
    // printf("Read line: %s\n",str.c_str());

    if (index == 0) {
      this->m_skipLaunch = !(str.compare("false") == 0);
    } else if (this->m_skipLaunch && index == 1) {
      this->m_port = str;
    } else {
      arguments.push_back(str);
    }
    index++;
  }

  int size = arguments.size() + 2;

  this->m_args = new char *[size];
  for (int i = 0; i < arguments.size(); i++) {
    // printf("Copying line: %s\n",arguments.at(i).c_str());

    this->m_args[i] = new char[arguments.at(i).size() + 1];
    strcpy(this->m_args[i], arguments.at(i).c_str());

    for (int j = 0; j < arguments.at(i).length(); j++) {
      char *cp = this->m_args[i];
      char c = cp[j];
      if (c == '\r') cp[j] = 0;
    }

    // printf("Inserted char* %s\n",this->m_args[i]);
  }
  this->m_args[size - 2] = strdup(port.c_str());
  this->m_args[size - 1] = NULL;
}

ConfigFile::~ConfigFile() {
  delete[] this->m_args;
  this->m_args = NULL;
}
