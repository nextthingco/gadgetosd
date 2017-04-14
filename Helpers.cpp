/*
 * Helpers.cpp
 *
 *  Created on: Apr 13, 2017
 *      Author: howie
 */

#include "Helpers.h"
#include <boost/process.hpp>
#include <boost/log/trivial.hpp>
namespace bp = ::boost::process;


int Helpers::process(const std::string& command) {
	bp::ipstream stdoutStream;
	bp::ipstream stderrStream;
	bp::child process(command,  bp::std_out > stdoutStream, bp::std_err > stderrStream);
	std::string line;
	BOOST_LOG_TRIVIAL(trace) << command;
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
