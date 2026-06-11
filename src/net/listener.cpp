#include <asm-generic/socket.h>

#include <bits/types/struct_timeval.h>
#include <charconv>
#include <net/listener.hpp>
#include <net/acceptor.hpp>
#include <error/error.hpp>

#include <optional>
#include <print>
#include <stdexcept>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <netinet/in.h>
#include <system_error>
#include <unistd.h>
#include <arpa/inet.h>
#include <utility>


namespace net {
    Listener::Listener(std::string_view host)
    {
        parse_addr_(host);
        constexpr int yes = 1;
        struct timeval timeout;
        timeout.tv_sec = 60;
        timeout.tv_usec = 0;
        m_fd = socket(AF_INET, SOCK_STREAM, 0);
        if(m_fd == -1){
            throw err::socket_error{"socket: "};
        }

        if(setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0){
            throw err::socket_error{"setsockopt: "};
        }

        if(setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)) < 0){
            throw err::socket_error{"setsockopt: "};
        }

        if(setsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0){
            throw err::socket_error{"setsockopt: "};
        }

        if(setsockopt(m_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0){
            throw err::socket_error{"setsockopt: "};
        }
    }

    Listener::~Listener(){
        if(m_fd != -1){
            ::close(m_fd);
        }
    }

    Listener::Listener(Listener&& src) {
        m_fd = std::exchange(src.m_fd, -1);
        m_port = std::exchange(src.m_port, 0);
        m_hostAddr = std::move(src.m_hostAddr);
    }

    Listener& Listener::operator=(Listener&& src) {
        if(this != &src){
            if(m_fd != -1){
                ::close(m_fd);
            }

            m_fd = std::exchange(src.m_fd, -1);
            m_port = std::exchange(src.m_port, 0);
            m_hostAddr = std::move(src.m_hostAddr);
            return *this;
        }


        return *this;
    }

    auto Listener::listen() -> void {
        struct sockaddr_in addr;
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

    auto Listener::accept() -> std::optional<Acceptor>{
        struct sockaddr_in their_addr;
        socklen_t sin_size = sizeof(their_addr);
        int clientfd = ::accept(m_fd, (struct sockaddr*)&their_addr, &sin_size);
        if(clientfd == -1){
            std::error_code ec(errno, std::generic_category());
            if(ec == std::errc::resource_unavailable_try_again){
                return std::nullopt;
            }
            throw err::socket_error{"accept: "};
        }
        return std::optional<Acceptor>{clientfd};
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
