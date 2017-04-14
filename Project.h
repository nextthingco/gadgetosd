/*
 * Project.h
 *
 *  Created on: Apr 11, 2017
 *      Author: howie
 */
#ifndef PROJECT_H_
#define PROJECT_H_
#include "Container.h"
#include "boost/spimpl.h"
#include <vector>
#include <memory>

class Project {
	friend class ProjectFactory;
public:
	Project(const std::string& name, const std::string& specVersion);

	const std::string& getName() const {
		return m_name;
	}

	void setName(const std::string& name) {
		m_name = name;
	}

	const std::vector<std::shared_ptr<Container> >& getContainers() const {
		return m_containers;
	}
	bool createPayloadDirectory();

private:
	std::string m_name;
	std::string m_specVersion;
	std::vector<std::shared_ptr<Container>> m_containers;
private:
	class Impl;
	spimpl::unique_impl_ptr<Impl> pimpl;  // Movable Smart PIMPL
};
#endif
