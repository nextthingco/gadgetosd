/**
 * \file ProjectFactory.cpp
 * \brief Factory for created Projects
 * \copyright Next Thing Co. License GPL v2
 * \date Created on: Apr 11, 2017
 *
 * This static class creates Projects, either from a yaml file, from init()
 * license GPL v2
 */

#include "yaml-cpp/yaml.h"  // IWYU pragma: keep
#include "Project.h"
#include "Container.h"
#include "ProjectFactory.h"
#include "DockerContainer.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <exception>
#include <map>


using namespace std;


bool isLower(const std::string& s) {
	for (auto c : s) {
		if (tolower(c) != c)
			return false;
	}
	return true;
}
/**
 * Abstract Factory method to create appropriate container type. This is a bit out of place here.
 * @param name
 * @param type
 * @param id
 * @return a Container subclass
 */
std::shared_ptr<Container> ProjectFactory::newContainer(const std::string& type, const std::string& name, const std::string& id)
{
	if (type == DOCKER_TYPE) {
		return std::make_shared<DockerContainer>(name, id);
	} else {
		throw YAML::Exception(YAML::Mark::null_mark(),"Unknown container type: " + type);
	}
}

/**
 * Helper which gets an attribute from a parent. If doesn't exist, it throws an exception
 * @param parent
 * @param key
 * @return the found node
 * @throws Exception if not found
 */
const YAML::Node getRequiredAttribute(const YAML::Node& parent, const std::string& key)
{
	if (!parent[key])
		throw YAML::Exception(YAML::Mark::null_mark(),key + " is required");
	return parent[key];
}

/**
 * Serialize a project to YAML
 * @param project
 * @return std::string with resulting yaml
 */
std::string ProjectFactory::toYaml(const Project& project) {
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "spec-version" << YAML::Value << project.m_specVersion;
	out << YAML::Key << "name" << YAML::Value << project.m_name;
	out <<  "hello \n";
	if (!project.m_containers.empty()) {
		out << YAML::Key << "containers" << YAML::BeginSeq;
		for (auto container : project.m_containers) {
			out << container->m_name;
		}
		out << YAML::EndSeq;
		out << YAML::Literal << "\n";

		for (auto container : project.m_containers) {
			out << YAML::Key << container->m_name << YAML::BeginMap;
			out << YAML::Key << "name" << YAML::Value << container->getName();
			out << YAML::Key << "id" << YAML::Value << container->getId();
			out << YAML::Key << "type" << YAML::Value << container->getType();
			out << YAML::Key << "restart" << YAML::Value << container->getRestart();

			if (!container->m_volumes.empty()) {
				out << YAML::Key << "volumes" << YAML::BeginSeq;
				for (auto volume : container->m_volumes) {
					out << YAML::BeginMap;
					out << YAML::Key << "from" << YAML::Value << volume->getFrom();
					out << YAML::Key << "to" << YAML::Value << volume->getTo();
					out << YAML::Key << "read-only" << YAML::Value << volume->isReadOnly();
					out << YAML::EndMap;
				}
				out << YAML::EndSeq;
			}
			if (!container->m_devices.empty()) {
				out << YAML::Key << "devices" << YAML::BeginSeq;
				for (auto device : container->m_devices) {
					out << YAML::BeginMap;
					out << YAML::Key << "from" << YAML::Value << device->getFrom();
					out << YAML::Key << "to" << YAML::Value << device->getTo();
					out << YAML::Key << "read" << YAML::Value << device->isRead();
					out << YAML::Key << "write" << YAML::Value << device->isWrite();
					out << YAML::Key << "mknod" << YAML::Value << device->isMknod();
					out << YAML::EndMap;
				}
				out << YAML::EndSeq;
			}
			if (!container->m_privileges.empty()) {
				out << YAML::Key << "privileges" << YAML::BeginMap;
				for (auto const& privilege : container->m_privileges) {
					out << YAML::Key << privilege.first << YAML::Value << privilege.second;
				}
				out << YAML::EndMap;
			}
			out << YAML::EndMap;
		}
	}
	out << YAML::EndMap;
	return out.c_str();
}

/**
 * Creates a project from a yaml file
 * @param fileName of the yaml file
 * @return std::shared_ptr<Project> newly created Project
 */
std::shared_ptr<Project> ProjectFactory::fromYaml(const std::string& fileName)
{
	auto yaml = std::ifstream(fileName);
	return fromYaml(yaml);
}

/**
 *
 * @param yaml istream with yaml body
 * @return std::shared_ptr<Project> newly created Project
 */
std::shared_ptr<Project> ProjectFactory::fromYaml(std::istream& yaml)
{
	try {
		YAML::Node config = YAML::Load(yaml);
		auto specVersion = getRequiredAttribute(config,"spec-version").as<std::string>();
		auto projectName = getRequiredAttribute(config,"name").as<std::string>();

		auto project = std::make_shared<Project>(projectName, specVersion);


		auto containers = getRequiredAttribute(config,"containers");
		for (YAML::const_iterator c=containers.begin(); c!=containers.end(); ++c) {
			// Extract the Node corresponding to the container and get its primary attributes
			auto containerNode = getRequiredAttribute(config,c->as<std::string>());
			auto name = getRequiredAttribute(containerNode,"name").as<std::string>();
			if (name.empty()) {
				throw YAML::Exception(YAML::Mark::null_mark(),"Container name must not be empty");
			} else if (!isLower(name)) {
				throw YAML::Exception(YAML::Mark::null_mark(),"Container name: '" + name + "' must be all lower-case");

			}
			auto type = getRequiredAttribute(containerNode,"type").as<std::string>();
			auto id = getRequiredAttribute(containerNode,"id").as<std::string>();


			//TODO restart value

			// Use abstract factory to create container
			auto container = newContainer(type, name, id);
			auto restart = containerNode["restart"];
			if (restart)
				container->setRestart(restart.as<std::string>()); //TODO maybe check for allowed values

			// Process volumes
			auto volumesNode = containerNode["volumes"];
			if (volumesNode) {
				for (YAML::const_iterator v=volumesNode.begin(); v!=volumesNode.end(); ++v) {
					YAML::Node volumeNode = *v;
					auto from = getRequiredAttribute(volumeNode,"from").as<std::string>();
					auto volume = std::make_shared<Volume>(from);
					auto to = volumeNode["to"];
					if (to)
						volume->setTo(to.as<std::string>());
					auto readOnly = volumeNode["read-only"];
					if (readOnly)
						volume->setReadOnly(readOnly.as<bool>());
					container->m_volumes.push_back(volume);
				}
			}
			// Process devices
			auto devicesNode = containerNode["devices"];
			if (devicesNode) {
				for (YAML::const_iterator d=devicesNode.begin(); d!=devicesNode.end(); ++d) {
					YAML::Node deviceNode = *d;
					auto from = getRequiredAttribute(deviceNode,"from").as<std::string>();
					auto device = std::make_shared<Device>(from);
					auto to = deviceNode["to"];
					if (to)
						device->setTo(to.as<std::string>());
					auto read = deviceNode["read"];
					if (read)
						device->setRead(read.as<bool>());
					auto write = deviceNode["write"];
					if (write)
						device->setWrite(write.as<bool>());

					auto mknod = deviceNode["mknod"];
					if (mknod)
						device->setMknod(mknod.as<bool>());

					container->m_devices.push_back(device);
				}
			}

			// Process privileges as a map
			auto privilegesNode = containerNode["privileges"];
			if (privilegesNode) {
				container->m_privileges = privilegesNode.as<std::map<std::string, std::string>>();
			}

			project->m_containers.push_back(container);
		}

		return project;

	} catch (const YAML::Exception& e) {
		std::cerr << e.what() << std::endl;
		return nullptr;
	}
}

extern "C" {
/**
 * C-accessible for testing
 * @return
 */
int readYamlConfig()
{
	auto proj = ProjectFactory::fromYaml("tests/config.yaml");
//	std::cout << "Just read config" << std::endl;
//	auto yaml = ProjectFactory::toYaml(*proj);
//	std::cout << yaml;
//	auto yamlIstream = std::istringstream(yaml);
//	auto proj2 = ProjectFactory::fromYaml(yamlIstream);
//	auto yaml2 = ProjectFactory::toYaml(*proj2);
//	if (yaml2 == yaml)
//		std::cout << "serialization ok" << std::endl;
//	else
//		std::cerr << "serialization bad" << std::endl;


	return 1;
}
}
