/*
 * Helpers.h
 *
 *  Created on: Apr 13, 2017
 *      Author: howie
 */

#ifndef HELPERS_H_
#define HELPERS_H_
#include <string>
class Helpers {
public:
	static int process(const std::string& command);
	static std::string findFileHereOrInParent(const std::string & fileName);

};

#endif /* HELPERS_H_ */
