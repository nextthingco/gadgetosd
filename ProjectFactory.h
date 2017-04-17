/*
 * ProjectFactory.h
 *
 *  Created on: Apr 11, 2017
 *      Author: howie
 */
#ifndef PROJECT_FACTORY_H_
#define PROJECT_FACTORY_H_

#include <string>
#include <memory>
class Project;
class Container;
class ProjectFactory
{
public:
	static std::shared_ptr<Project> fromYaml(std::istream& yaml);
	static std::shared_ptr<Project> fromYaml(const std::string& fileName);
	static std::shared_ptr<Project> init(const std::string& fileName);

	static std::string toYaml(const Project& project);

	static std::shared_ptr<Container> newContainer(const std::string& type, const std::string& name, const std::string& id, const std::string& configFile); //TODO This could be a separate factory, but seems like overkill now

private:
	ProjectFactory() {} // don't allow instantiation
};
#endif
