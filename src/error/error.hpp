#pragma once

#include <string>
#include <system_error>

namespace err{

class socket_error : public std::exception
{
public:
	explicit socket_error(std::string msg)
		: m_msg{std::move(msg)}
		{
			m_msg = m_msg + ec.message();
		}
	virtual ~socket_error() noexcept override {};
	virtual const char* what() const noexcept override{
		return m_msg.c_str();
	}
private:
	std::error_code ec{errno, std::generic_category()};
	std::string m_msg;
};

}
