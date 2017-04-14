/*
 * Project.cpp
 *
 *  Created on: Apr 11, 2017
 *      Author: howie
 */

#include "Project.h"
#include <boost/filesystem.hpp>
namespace fs = ::boost::filesystem;

/**
 * Using PIMPL pattern to avoid putting boost in the header
 */
class Project::Impl {
public:
	fs::path m_payloadPath;
	bool createPayloadDirectory() {
		m_payloadPath = fs::temp_directory_path();
		//fs::unique_path("/tmp/gadget_build_%%%%%%");
		return fs::create_directory(m_payloadPath);
	}
};

Project::Project(const std::string& name, const std::string& specVersion)
	: m_name(name)
	, m_specVersion(specVersion)
	, pimpl(spimpl::make_unique_impl<Impl>())
{
}

bool Project::createPayloadDirectory() {
	return pimpl->createPayloadDirectory();
}
