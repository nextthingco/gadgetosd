/*
 * Volume.h
 *
 *  Created on: Apr 12, 2017
 *      Author: howie
 */

#ifndef VOLUME_H_
#define VOLUME_H_
#include <string>
class Volume {
friend class ProjectFactory;
public:
	Volume(const std::string& from);

	const std::string& getFrom() const {
		return m_from;
	}

	void setFrom(const std::string& from) {
		m_from = from;
	}

	bool isReadOnly() const {
		return m_readOnly;
	}

	void setReadOnly(bool readOnly) {
		m_readOnly = readOnly;
	}

	const std::string& getTo() const {
		return m_to;
	}

	void setTo(const std::string& to) {
		m_to = to;
	}
private:
	std::string m_from;
	std::string m_to;
	bool m_readOnly;

};

#endif /* VOLUME_H_ */
