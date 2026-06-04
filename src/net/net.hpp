#include <cstdint>
#include <string>
#include <sys/socket.h>
#include <string_view>
#include <unistd.h>
#include <netinet/in.h>
#include <utility>


/* TODO: https://www.rfc-editor.org/rfc/rfc9112.html */

namespace net {

class Listener final{
public:
    explicit Listener(std::string host);
    ~Listener();

    Listener(const Listener&) = delete;
    Listener& operator=(const Listener&) = delete;

    Listener(Listener&& src) noexcept;
    Listener& operator=(Listener&& rhs) noexcept;

    void listen();
    void accept();
    [[nodiscard]] std::string recv();
    void send(const std::string& data);

    void close_client();

    constexpr auto get_addr() ->std::string_view const {
		return m_clientAddr;
	}
    constexpr auto get_port() -> uint16_t const {
        return m_port;
    }

private:
    int m_fd = -1;
    int m_clientfd = -1;
    uint16_t m_port = 0;
    std::string m_clientAddr = {}; // их бы такто хранить в network order чтоб удобнее было туда сюда давать а в гетере проводить в человеческий
    std::string m_hostAddr = "0.0.0.0";
    
    [[nodiscard]] auto ntop_(const struct sockaddr_in *sa) const noexcept -> std::string;
    auto moveFrom(Listener& src) noexcept -> void;
    auto parse_addr_(std::string_view addr) -> void;
};
}
