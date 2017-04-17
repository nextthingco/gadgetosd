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
	static std::string findFileHereOrInParent(const std::string & fileName, const char * where= nullptr);
	static std::string prompt(const std::string& message, const std::string& defaultValue, bool (*validationFunc)(const std::string& val) = nullptr);
	static bool isLowerCase(const std::string& name);
	static std::string makeUuid();

};

#endif /* HELPERS_H_ */
