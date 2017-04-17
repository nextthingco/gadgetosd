/*
 * DockerContainer.h
 *
 *  Created on: Apr 12, 2017
 *      Author: howie
 */

#ifndef DOCKERCONTAINER_H_
#define DOCKERCONTAINER_H_

#include "Container.h"
const std::string DOCKER_TYPE = "docker";

class DockerContainer: public Container {
public:
	DockerContainer(const std::string& name, const std::string& id, const std::string configFile);
	virtual ~DockerContainer();
	virtual const std::string& getType() override;
	virtual int build(const std::string& path="", std::string name="") override;
	virtual int save(const std::string& path) override;
	virtual bool isValidName(const std::string& name) override;

};

#endif /* DOCKERCONTAINER_H_ */
