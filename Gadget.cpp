/*
 * Gadget.cpp
 *
 *  Created on: Apr 13, 2017
 *      Author: howie
 */

#include "Gadget.h"
#include "ProjectFactory.h"
#include "Project.h"
#include "Container.h"
#include <boost/program_options.hpp>

namespace po = boost::program_options;
Gadget::Gadget() {
	// TODO Auto-generated constructor stub

}

int main(int argc, char **argv)
{
	auto project = ProjectFactory::fromYaml("tests/config.yaml");
	if (!project)
		exit(-1);
	auto container = project->getContainers()[0];
	container->build();
}
