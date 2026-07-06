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

            void register_handler(http::Method method, std::string uri, Handler&& handler);

            void run();
        private:
            net::Listener m_listenfd;
            std::unordered_map<std::string, Handler> m_routes;
            inline static volatile std::sig_atomic_t m_running = true;

            [[nodiscard]] static auto parse_req_str(std::string_view data) -> std::optional<Request>;
            static constexpr auto method_to_str(http::Method method) -> std::string_view;
            static void sighandler(int) {m_running = false;}
    };
}
