/*
 * Container.cpp
 *
 *  Created on: Apr 11, 2017
 *      Author: howie
 */
#include "Container.h"
#include <boost/filesystem.hpp>
namespace fs = ::boost::filesystem;

const std::string Container::RESTART_ALWAYS = "always";

/**
 * Using PIMPL pattern to avoid putting boost in the header
 */
class Container::Impl {
public:
	fs::path m_payloadPath;
	bool createPayloadDirectory() {
		m_payloadPath = fs::temp_directory_path();
		//fs::unique_path("/tmp/gadget_build_%%%%%%");
		return fs::create_directory(m_payloadPath);
	}
};

Container::Container(const std::string& name, const std::string& id, const std::string& configFile)
	: m_name(name)
	, m_id(id)
	, m_configFile(configFile)
	, m_restart(RESTART_ALWAYS)
	, pimpl(spimpl::make_unique_impl<Impl>())

{
}

bool Container::createPayloadDirectory() {
	return pimpl->createPayloadDirectory();
}

Container::~Container() {
}

