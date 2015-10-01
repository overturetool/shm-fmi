/*
 * ConfigFile.cpp
 *
 *  Created on: Sep 8, 2015
 *      Author: root
 */

#include "ConfigFile.h"

ConfigFile::ConfigFile(std::string configFile,std::string port)
{
	std::ifstream file(configFile);
	int index = 0;

	std::vector<std::string> arguments;

	std::string str;
	while (std::getline(file, str))
	{
		//printf("Read line: %s\n",str.c_str());

		if (index == 0)
		{
			this->m_skipLaunch = str.compare("true") == 0;
		} else if (this->m_skipLaunch && index == 1)
		{
			this->m_port = str;
		} else
		{
			arguments.push_back(str);
		}
		index++;
	}

	int size = arguments.size()+2;

	this->m_args = new char*[size];
	for (int i = 0; i < arguments.size(); i++)
	{
		//printf("Copying line: %s\n",arguments.at(i).c_str());

		this->m_args[i] = new char[arguments.at(i).size()];
		strcpy(this->m_args[i], arguments.at(i).c_str() );


		//printf("Inserted char* %s\n",this->m_args[i]);
	}
	this->m_args[size-2]=strdup(port.c_str());
	this->m_args[size-1]=NULL;

}

ConfigFile::~ConfigFile()
{
	// TODO Auto-generated destructor stub
}
