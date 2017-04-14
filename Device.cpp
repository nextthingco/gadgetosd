/*
 * Device.cpp
 *
 *  Created on: Apr 11, 2017
 *      Author: howie
 */
#include "Device.h"

/**
 * Make a device
 * @param from Required argument. All others are optional
 */
Device::Device(const std::string & from)
	: m_from(from)
	, m_to(from) // defaults to from value
	, m_read(true)
	, m_write(true)
	, m_mknod(true)
{
}
