#pragma once

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace net {

class Acceptor;

class Listener {
public:
    explicit Listener(std::string_view host);
    ~Listener();

    Listener(const Listener&) = delete;
    Listener& operator=(const Listener&) = delete;

    Listener(Listener&& src);
    Listener& operator=(Listener&& rhs);

    auto listen() -> void;
    auto accept() -> std::optional<Acceptor>;

    constexpr auto get_port() const noexcept -> uint16_t {
        return m_port;
    }

    constexpr auto get_fd() const noexcept -> int32_t {
        return m_fd;
    }

private:
    int32_t m_fd                = -1;
    uint16_t m_port             = 0;
    std::string m_hostAddr      {"0.0.0.0"};

    [[nodiscard]] auto ntop_(const struct sockaddr_in *sa) const noexcept -> std::string;
    auto parse_addr_(std::string_view addr) -> void;
};
} //NAMESPACE NET
