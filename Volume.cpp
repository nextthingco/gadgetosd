/*
 * Volume.cpp
 *
 *  Created on: Apr 12, 2017
 *      Author: howie
 */

#include "Volume.h"

Volume::Volume(const std::string& from)
	: m_from(from)
	, m_to(from) // to defaults to from
	, m_readOnly(false)
{
}

