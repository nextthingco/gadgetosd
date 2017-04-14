/*
 * Container.h
 *
 *  Created on: Apr 11, 2017
 *      Author: howie
 */
#ifndef CONTAINER_H_
#define CONTAINER_H_
#include "Device.h"
#include "Volume.h"
#include <vector>
#include <map>
#include <memory>
#include <string>

class Container { //abstract base class
friend class ProjectFactory;
public:
	Container(const std::string& name, const std::string& id);
	virtual ~Container();
	virtual const std::string& getType()=0; // makes this class abstract
	virtual int build(const std::string& path="", std::string name="")=0;
	virtual int save(const std::string& path)=0;


	const std::string& getId() const {
		return m_id;
	}

	void setId(const std::string& id) {
		m_id = id;
	}

	const std::string& getName() const {
		return m_name;
	}

	void setName(const std::string& name) {
		m_name = name;
	}

	const std::map<std::string, std::string>& getPrivileges() const {
		return m_privileges;
	}

	void setPrivileges(const std::map<std::string, std::string>& privileges) {
		m_privileges = privileges;
	}

	const std::string& getRestart() const {
		return m_restart;
	}

	void setRestart(const std::string& restart) {
		m_restart = restart;
	}


	static const std::string RESTART_ALWAYS;

protected:
	std::string m_name;
	std::string m_id;
	std::string m_restart;
	std::vector<std::shared_ptr<Volume>> m_volumes;
	std::vector<std::shared_ptr<Device>> m_devices;
	std::map<std::string, std::string> m_privileges;
};

#endif /* CONTAINER_H_ */
