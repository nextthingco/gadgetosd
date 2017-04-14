/*
 * Container.cpp
 *
 *  Created on: Apr 11, 2017
 *      Author: howie
 */
#include "Container.h"

const std::string Container::RESTART_ALWAYS = "always";

Container::Container(const std::string& name, const std::string& id)
	: m_name(name)
	, m_id(id)
	, m_restart(RESTART_ALWAYS)
{
}

Container::~Container() {
}

