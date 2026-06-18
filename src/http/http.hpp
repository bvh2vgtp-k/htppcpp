#include <unistd.h>

#include <csignal>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <functional>

#include <net/listener.hpp>
#include <net/acceptor.hpp>
#include <http/types.hpp>
#include <http/response/Response.hpp>

namespace http {
    class Server {
        public:
            using Handler = std::function<std::string()>; //TODO: добаить в реквест приколы
            [[nodiscard]] explicit Server(std::string_view host);
            ~Server();

            auto register_handler(http::Method method, std::string uri, Handler&& fn) -> void;

            auto run() -> void;
        private:
            net::Listener m_listenfd;
            std::unordered_map<std::string, Handler> m_routes;
            inline static volatile std::sig_atomic_t m_running = true;

            constexpr auto method_to_str(http::Method method) -> std::string_view;
            auto parse_req_str(std::string_view data) const -> std::optional<Request>;
            static void sighandler(int) {m_running = false;}
    };
}
