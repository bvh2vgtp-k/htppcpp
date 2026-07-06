#include <array>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>
#include <string_view>
#include <utility>

#include <error/error.hpp>
#include <net/acceptor.hpp>

namespace net {
    Acceptor::Acceptor(int sock) : m_clientfd(sock)
    {

    }

    Acceptor::~Acceptor(){
        if(m_clientfd != -1){
            ::close(m_clientfd);
        }
    }

    Acceptor::Acceptor(Acceptor&& src) noexcept
    : m_clientfd{std::exchange(src.m_clientfd, -1)}, m_bytesStored{std::exchange(src.m_bytesStored, 0)}
    {
        if(m_bytesStored != 0) {
            std::move(src.m_buff.begin(), src.m_buff.begin() + m_bytesStored, m_buff.begin());
        }
    }

    Acceptor& Acceptor::operator=(Acceptor&& src) noexcept {
        if(this != &src){
            if(m_clientfd != -1){
                ::close(m_clientfd);
            }
            m_clientfd = std::exchange(src.m_clientfd, -1);
            m_bytesStored = std::exchange(src.m_bytesStored, 0);
            if(m_bytesStored != 0) {
                std::move(src.m_buff.begin(), src.m_buff.begin() + m_bytesStored, m_buff.begin());
            }
        }
        return *this;
    }

    auto Acceptor::recv() -> std::optional<std::string_view> {
        ssize_t size = ::recv(m_clientfd, m_buff.data(), m_buff.size(), 0);
        if(size == 0){
            return std::nullopt;
        } else if(size == -1){
            /*Нормально обрабатыватьь вместо этого вот, потом надо будет */
            throw err::socket_error{"recv: "};
        }
        return std::string_view(m_buff.data(), size);
    }

    void Acceptor::send(std::string_view data) const {
        ssize_t size = ::send(m_clientfd, data.data(), data.size(), 0);
        if(size == -1){
            throw err::socket_error{"send: "};
        }
    }

    auto Acceptor::get_addrstr() const -> std::string{
        struct sockaddr addr;
        socklen_t len = sizeof(addr);
        int res = getpeername(m_clientfd, &addr, &len);
        if(res == -1){
            return std::string{};
        }
        return ntop_(&addr);
    }

    auto Acceptor::ntop_(const sockaddr* sa) -> std::string {
        if(sa == nullptr) return {};

        if(sa->sa_family == AF_INET){
            const auto* sin = reinterpret_cast<const sockaddr_in*>(sa);
            std::array<char, INET_ADDRSTRLEN> buffer;

            const char* dst = inet_ntop(AF_INET, &sin->sin_addr, buffer.data(), INET_ADDRSTRLEN);
            if(dst == nullptr) {
                return {};
            }
            return std::string{dst};
        }
        return {};
    }
} // NAMESPACE NET
