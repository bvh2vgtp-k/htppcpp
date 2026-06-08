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
        auto send(std::string_view data) -> void;
    private:
        int m_clientfd = -1;
        std::array<char, MAX_BUFF_SIZE> m_buff;
        //std::string m_clientAddr = {};
    };
}
