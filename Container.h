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
#include "boost/spimpl.h"


class Container { //abstract base class
friend class ProjectFactory;
public:
	Container(const std::string& name, const std::string& id, const std::string& configFile);
	virtual ~Container();
	virtual const std::string& getType()=0; // makes this class abstract
	virtual int build(const std::string& path="", std::string name="")=0;
	virtual int save(const std::string& path)=0;
	virtual bool isValidName(const std::string& name);

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

	const std::string& getConfigFile() const {
		return m_configFile;
	}

	void setConfigFile(const std::string& configFile) {
		m_configFile = configFile;
	}

	const std::string& getBuild() const {
		return m_build;
	}

	void setBuild(const std::string& build) {
		m_build = build;
	}

	static const std::string RESTART_ALWAYS;

	bool createPayloadDirectory();

protected:
	std::string m_name;
	std::string m_id;
	std::string m_configFile;
	std::string m_restart;
	std::string m_build;
	std::vector<std::shared_ptr<Volume>> m_volumes;
	std::vector<std::shared_ptr<Device>> m_devices;
	std::map<std::string, std::string> m_privileges;
private:
	class Impl;
	spimpl::unique_impl_ptr<Impl> pimpl;  // Movable Smart PIMPL

};

#endif /* CONTAINER_H_ */
