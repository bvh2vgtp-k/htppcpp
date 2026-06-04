#include <asm-generic/socket.h>

#include <charconv>
#include <net/net.hpp> 
#include <error/error.hpp>

#include <print>
#include <stdexcept>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <utility>


//TODO: надо нормально потом мож обрабатывать ато чё эот
namespace net {
	Listener::Listener(std::string host)
	{
		parse_addr_(host);
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
		if(m_fd != -1){
			::close(m_fd);
		}
	}
	
	auto Listener::moveFrom(Listener& src) noexcept -> void {
		m_clientAddr = std::move(src.m_clientAddr); // ну такто...
		m_fd = std::exchange(src.m_fd, -1);
		m_clientfd = std::exchange(src.m_clientfd, -1);
		m_port = std::exchange(src.m_port, 0);
	}

	Listener::Listener(Listener&& src) noexcept{
		moveFrom(src);
	}

	Listener& Listener::operator=(Listener&& rhs) noexcept {
		if(this == &rhs){
			return *this;
		}
		
		m_fd = m_clientfd = -1;
		m_port = 0;
		m_clientAddr.clear();
		moveFrom(rhs);

		return *this;
	}

	auto Listener::close_client() -> void{
		::close(m_clientfd);
		m_clientAddr.clear();
	}

	auto Listener::listen() -> void {
	//TODO: есть идея по лучше
		struct sockaddr_in addr; // <- вот это сунуть как мембер
		socklen_t addr_len = sizeof(addr);
		addr.sin_family = AF_INET;
		auto err = inet_pton(AF_INET, m_hostAddr.c_str(), &(addr.sin_addr));
		if(err != 1){ // ))))))))))
			throw err::socket_error{"bad addr: "};
		}
		
		addr.sin_port = htons(m_port);

		if(bind(m_fd, (struct sockaddr*)&addr, addr_len) != 0){
			throw err::socket_error{"bind: "};
		}

		if(::listen(m_fd, SOMAXCONN) != 0){
			throw err::socket_error{"listen: "};
		}

		std::println("listening on: {}:{}", m_hostAddr, m_port);
	}

	auto Listener::accept() -> void {
		struct sockaddr_in their_addr;
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

		m_clientAddr = ntop_(&their_addr);
	}

	auto Listener::recv() const -> std::string{
		std::string buff;
		/* max len около 617 мне удалось получить такчто похуй...*/
		buff.resize(1024);

		auto size = ::recv(m_clientfd, buff.data(), buff.size(), 0);
		if(size == -1){
			throw err::socket_error{"recv: "};
		}
		std::println("recived {} bytes", size);
		return buff;
	}

	auto Listener::send(const std::string& data) const -> void {
		auto size = ::send(m_clientfd, data.c_str(), data.size(), 0);
		if(size == -1){
			throw err::socket_error{"send: "};
		}
		std::println("sended: {} bytes", size);
	}

	auto Listener::ntop_(const struct sockaddr_in* sa) const noexcept -> std::string {
		char s[INET_ADDRSTRLEN];
		const char* dst = inet_ntop(AF_INET, &sa->sin_addr.s_addr, s, INET_ADDRSTRLEN);
		return std::string(dst);
	}
	
	auto Listener::parse_addr_(std::string_view addr) -> void {
		auto colon = addr.find(':');
		if (colon == std::string_view::npos){
			throw std::invalid_argument("invalid address format, missing :");
		}
		auto host = addr.substr(0, colon);
		if(!host.empty()){
			m_hostAddr = host;
		}

		auto port = addr.substr(colon + 1);
		if(port.empty()){
			throw std::invalid_argument("invalid adress, missing port");
		}

		auto [ptr, err] = std::from_chars(port.data(), port.data() + port.size(), m_port);
		if(err != std::errc{}){
			if(err == std::errc::invalid_argument){
				throw std::invalid_argument("port is not a valid number");
			}
			if(err == std::errc::result_out_of_range){
				throw std::out_of_range("port value is out of range");
			}

		}
		if(ptr != port.data() + port.size()){
			throw std::invalid_argument("port contains trailing garbage");
		}
	}
} //NAMESPACE NET
