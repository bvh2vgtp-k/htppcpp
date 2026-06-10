#pragma once

#include <string>
#include <arpa/inet.h>
#include <array>
#include <netinet/in.h>
#include <string_view>
#include <optional>
#include <sys/socket.h>
#include <unistd.h>

namespace net {
    constexpr size_t MAX_BUFF_SIZE = 4096;

    class Acceptor {
    public:
        explicit Acceptor(int sock);
        ~Acceptor();

        Acceptor(const Acceptor&) = delete;
        Acceptor& operator=(const Acceptor&) = delete;

        Acceptor(Acceptor&& src) noexcept;
        Acceptor& operator=(Acceptor&& rhs) noexcept;

        [[nodiscard]] auto recv() -> std::optional<std::string_view>;

        auto send(std::string_view data) const -> void;

        [[nodiscard]] auto get_addrstr() const -> std::string;
        auto get_fd() const noexcept -> int {
            return m_clientfd;
        }

    private:
        [[nodiscard]] auto ntop_(const sockaddr* sa) const -> std::string;

        int m_clientfd = -1;
        size_t m_bytesStored = 0; //может вместо того чтобы мозшги ебать уже легче вектор использоватьь
        std::array<char, MAX_BUFF_SIZE> m_buff;
    };
}
