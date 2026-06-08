#include <error/error.hpp>
#include <net/acceptor.hpp>
#include <string_view>
#include <sys/socket.h>
#include <unistd.h>

namespace net {
    Acceptor::Acceptor(int sock) : m_clientfd(sock)
    {

    }

    Acceptor::~Acceptor(){
        if(m_clientfd != -1){
            close(m_clientfd);
        }
    }

    Acceptor::Acceptor(Acceptor&& src) noexcept
        : m_clientfd(src.m_clientfd),
        m_buff{src.m_buff}
    {
        src.m_clientfd = -1;
    }

    auto Acceptor::recv() -> std::optional<std::string_view>{
        ssize_t size = ::recv(m_clientfd, m_buff.data(), m_buff.size(), 0);// TODO: посмотреть на флаги
        if(size == 0){
            return std::nullopt;
        } else if(size == -1){
            throw err::socket_error{"recv: "};
        }
        return std::string_view(m_buff.data(), size);
    }

    auto Acceptor::send(std::string_view data) -> void {
        ssize_t size = ::send(m_clientfd, data.data(), data.size(), 0); //TODO: посмотреть на флашги
        if(size == -1){//TODO: посмотреть на ошибки нормально обработать
            throw err::socket_error{"send: "};
        }
    }

}
