/*
 * Device.h
 *
 *  Created on: Apr 11, 2017
 *      Author: howie
 */

#ifndef DEVICE_H_
#define DEVICE_H_
#include <string>

class Device {
friend class ProjectFactory;
public:
	Device(const std::string & from);

	const std::string& getFrom() const {
		return m_from;
	}

	void setFrom(const std::string& from) {
		m_from = from;
	}

	bool isMknod() const {
		return m_mknod;
	}

	void setMknod(bool mknod) {
		m_mknod = mknod;
	}

	bool isRead() const {
		return m_read;
	}

	void setRead(bool read) {
		m_read = read;
	}

	const std::string& getTo() const {
		return m_to;
	}

	void setTo(const std::string& to) {
		m_to = to;
	}

	bool isWrite() const {
		return m_write;
	}

	void setWrite(bool write) {
		m_write = write;
	}

private:
	std::string m_from;
	std::string m_to;
	bool m_read;
	bool m_write;
	bool m_mknod;
};

#endif /* DEVICE_H_ */
