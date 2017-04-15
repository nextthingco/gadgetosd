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
 * hidden(hence static) helper function to recursively search for a file going up to parent each time
 * @param fileName
 * @param path
 * @return path found
 * @throws runtime error if not found
 */
static fs::path _findFileHereOrInParent(const std::string & fileName, fs::path path) {
	if (path.empty())
		throw std::runtime_error("Could not find " + fileName);

	path = fs::canonical(path); // to handle symlinks (do we want this?)
	auto parent = path.parent_path(); // save off parent path for later recursion if needed
	path /= fileName; // append fileName

	if (fs::is_regular_file(path))
		return path.c_str();
	return _findFileHereOrInParent(fileName, parent); // not found, so check parent
}

/**
 * Find a file either in current directory, or recursively in parent directories
 * @param fileName
 * @return path (as string) of found file
 * @throws std::exception if not found
 */
std::string Helpers::findFileHereOrInParent(const std::string & fileName) {
	auto path = _findFileHereOrInParent(fileName, fs::current_path());
	return path.c_str();
}
