#pragma once

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>
#include <array>
#include <string_view>
#include <optional>

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

        void send(std::string_view data) const;

        [[nodiscard]] auto get_addrstr() const -> std::string;
        [[nodiscard]] constexpr auto get_fd() const noexcept -> int32_t {
            return m_clientfd;
        }



    private:
        [[nodiscard]] auto ntop_(const sockaddr* sa) const -> std::string;

        int32_t m_clientfd = -1;
        size_t m_bytesStored = 0;
        std::array<char, MAX_BUFF_SIZE> m_buff;
    };
}//NAMESPACE NET
