#include <asm-generic/socket.h>
#include <cstdint>

#include <net/net.hpp> 
#include <error/error.hpp>

#include <print>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>


//TODO: надо нормально потом мож обрабатывать ато чё эот
namespace net {
	Listener::Listener(uint16_t port) : m_port(port)
	{
		int yes = 1;
		m_fd = socket(AF_INET, SOCK_STREAM, 0);
		if(m_fd == -1){
			throw err::socket_error{"socket: "};
		}

		if(setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0){
			throw err::socket_error{"setsockopt: "};
		}
	}

	Listener::~Listener(){
		close();
	}

	void Listener::close(){
		::close(m_fd);
	}

	void Listener::listen(){
		struct sockaddr_in addr;
		socklen_t addr_len = sizeof(addr);
		char s[INET_ADDRSTRLEN];
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY; //INADDR_ANY долэен быть 0.0.0.0
		addr.sin_port = htons(m_port);

		if(bind(m_fd, (struct sockaddr*)&addr, addr_len) != 0){
			throw err::socket_error{"bind: "};
		}

		if(::listen(m_fd, SOMAXCONN) != 0){
			throw err::socket_error{"listen: "};
		}
		auto host = _ntop(&addr);

		//TODO: переделать под нормалоные enp адресс, можно исполььзовать ifчётоам в интернете ест
		std::println("listening on: {}:{}", host, m_port);
	}

	void Listener::accept(){
		struct sockaddr_in their_addr;
		char s[INET_ADDRSTRLEN];
		socklen_t sin_size = sizeof(their_addr);
		m_clientfd = ::accept(m_fd, (struct sockaddr*)&their_addr, &sin_size);
		if(m_clientfd == -1){
			/*Нормальная проверка std::error_code может сделать потом как мембер класса и тут его првоерятть
			а модет на стеке создавать тут ипроверять тоже */

			/*тут нужна нормальная обработка ошибки, так как accept() может вернуть очень многое
			ECONNABORTED к примеру возникает когда мой пир обрубил соединение, сервер не должен умерать
			EPERM  Firewall rules forbid connection. -- надо будет это тоже ловит и выводит ошибку ро фаервоел*/ 
			throw err::socket_error{"accept: "};
		}

		m_clientAddr = _ntop(&their_addr);
	}

	void Listener::recv(std::vector<std::byte>& buff){ //ПЕРЕДЕЛАТЬ МБ ПОТОМ С ВОЗВРАЩЗАЮЩИМ ЗЩНАЧЕНИЕМ
		auto size = ::recv(m_fd, buff.data(), buff.size(), 0);
		std::println("reieved: {} bytes", size);
	}

	void Listener::send(std::string&& data){
		auto size = ::send(m_clientfd, data.c_str(), data.size(), 0);
		if(size == -1){
			throw err::socket_error{"send: "};
		}
		std::println("sended: {} bytes", size);
	}

	std::string Listener::_ntop(const struct sockaddr_in* sa) const {
		char s[INET_ADDRSTRLEN];
		const char* dst = inet_ntop(AF_INET, &sa->sin_addr.s_addr, s, INET_ADDRSTRLEN);
		return std::string(dst);
	}



} //NAMESPACE NET
