/*
 * Helpers.cpp
 *
 *  Created on: Apr 13, 2017
 *      Author: howie
 */

#include "Helpers.h"
#include <boost/process.hpp>
#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>
#include <iostream>

namespace fs = ::boost::filesystem;
namespace bp = ::boost::process;

/**
 * Run a subprocess. Output is logged.
 * @param command string with the command line (no need for an array)
 * @return process exit code
 */
int Helpers::process(const std::string& command) {
	bp::ipstream stdoutStream;
	bp::ipstream stderrStream;
	bp::child process(command,  bp::std_out > stdoutStream, bp::std_err > stderrStream); // start process
	std::string line;
	BOOST_LOG_TRIVIAL(trace) << command;

	// Log stdout and stderr
	while (stdoutStream && std::getline(stdoutStream, line) && !line.empty()) {
		BOOST_LOG_TRIVIAL(trace) << line;
	}
	while (stderrStream && std::getline(stderrStream, line) && !line.empty()) {
		BOOST_LOG_TRIVIAL(fatal) << line;
	}

	process.wait();

	if (process.exit_code() != 0) {
		BOOST_LOG_TRIVIAL(fatal) << "Failed to execute " + command;
	}
	return process.exit_code();
}


/**
 * Find a file either in current directory, or in a parent directory
 * @param fileName
 * @param where root of where to search (optional). If omitted, uses current working directory
 * @return path (as string) of found file, or empty string if not found
 */
std::string Helpers::findFileHereOrInParent(const std::string & fileName, const char * where) {
	for (auto path = where ? fs::path(where): fs::current_path(); !path.empty(); path = path.parent_path()) {
		auto filePath = path / fileName;
		if (fs::is_regular_file(filePath))
			return filePath.string();
	}
	return "";
}

std::string Helpers::prompt(const std::string& message, const std::string& defaultValue, bool (*validationFunc)(const std::string& val)) {
	bool ok = false;
	std::string userValue;
	while (!ok) {
		std::cout << message << ": ";
		if (!defaultValue.empty())
			std::cout << "(" << defaultValue << ") ";
		std::getline(std::cin,userValue);
		boost::trim(userValue);
		if (userValue.empty())
			userValue = defaultValue;
		if (validationFunc) {
			try {
				ok = validationFunc(userValue);
			} catch (std::runtime_error & e) {
				std::cout << e.what() << std::endl;
			}
		} else {
			ok = true;
		}
	}
	return userValue;
}

bool Helpers::isLowerCase(const std::string& name)
{
	auto ok = boost::to_lower_copy<std::string>(name) == name;
	if (!ok) {
		throw std::runtime_error("The value must be all lower case");
	}
	return ok;
}

std::string Helpers::makeUuid() {
	boost::uuids::uuid uuid = boost::uuids::random_generator()();
	return boost::lexical_cast<std::string>(uuid);
}

