/*
 * Gadget.h
 *
 *  Created on: Apr 13, 2017
 *      Author: howie
 */

#ifndef GADGET_H_
#define GADGET_H_
#include "Project.h"
#include <memory>
class Gadget {
public:
	Gadget();
	std::shared_ptr<Project> loadProject();

};

#endif /* GADGET_H_ */
