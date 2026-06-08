#include <cstdint>
#include <string>
#include <sys/socket.h>
#include <string_view>
#include <unistd.h>
#include <netinet/in.h>


/* TODO: https://www.rfc-editor.org/rfc/rfc9112.html */

namespace net {

class Listener final{
public:
    explicit Listener(std::string_view host);
    ~Listener();

    Listener(const Listener&) = delete;
    Listener& operator=(const Listener&) = delete;

    Listener(Listener&& src) noexcept;
    Listener& operator=(Listener&& rhs) noexcept;

    auto listen() -> void;
    auto accept() -> void;
    [[nodiscard]] auto recv() const -> std::string;
    auto send(const std::string& data) const -> void;

    auto close_client() -> void;

    constexpr auto get_addr() const -> std::string_view {
		return m_clientAddr;
	}

    constexpr auto get_port() const -> uint16_t {
        return m_port;
    }

private:
    int m_fd                    = -1;
    int m_clientfd              = -1;
    uint16_t m_port = 0;
    std::string m_clientAddr    = {};
    std::string m_hostAddr      {"0.0.0.0"};

    [[nodiscard]] auto ntop_(const struct sockaddr_in *sa) const noexcept -> std::string;
    auto moveFrom(Listener& src) noexcept -> void;
    auto parse_addr_(std::string_view addr) -> void;
};
}
