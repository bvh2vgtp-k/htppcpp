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

    Listener(Listener&& src) noexcept;
    Listener& operator=(Listener&& src) noexcept;

    void listen();
    [[nodiscard]] auto accept() const -> std::optional<Acceptor>;

    [[nodiscard]] constexpr uint16_t get_port() const noexcept {
        return m_port;
    }

    [[nodiscard]] constexpr int32_t get_fd() const noexcept {
        return m_fd;
    }

private:
    int32_t m_fd                = -1;
    uint16_t m_port             = 0;
    std::string m_hostAddr      {"0.0.0.0"};

    [[nodiscard]] std::string ntop_(const struct sockaddr_in *sa) const noexcept;
    void parse_addr_(std::string_view addr);
};
} //NAMESPACE NET
