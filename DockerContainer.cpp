/*
 * DockerContainer.cpp
 *
 *  Created on: Apr 12, 2017
 *      Author: howie
 */

#include "DockerContainer.h"
#include "Helpers.h"
#include <iostream>
#include <sstream>


DockerContainer::DockerContainer(const std::string& name, const std::string& id, const std::string configFile)
	: Container(name, id, configFile)
{
}

DockerContainer::~DockerContainer() {
}

const std::string& DockerContainer::getType() //override
{
	return DOCKER_TYPE;
}

int DockerContainer::build(const std::string& path, std::string name)
{
	if (name.empty())
		name = m_name;
	std::string command = "docker build -t " + name + " " + path;
	return Helpers::process(command);
}

int DockerContainer::save(const std::string& path) {
	std::string command = "docker save " + m_name + " -o " + path;
	return Helpers::process(command);
}
