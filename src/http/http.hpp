#include <optional>
#include <string_view>
#include <unordered_map>
#include <cstdint>
#include <unistd.h>

#include <net/listener.hpp>
#include <net/acceptor.hpp>


namespace http {

    enum class Method : uint8_t {
        GET,
        HEAD,
        POST,
        PUT,
        DELETE,
        /* ... */
    };

    enum class Answer : uint8_t {

    };

class Server {
public:
	[[nodiscard]] explicit Server(std::string_view host);

	~Server();

	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;

	/*TODO: пока так но потом незнаю надо будет передлать мб */
	Server(const Server&&) = delete;
	Server& operator=(const Server&&) = delete;

	void handle_request(std::string uri, Method method/*чёто типо std::function мож чёто поинтереснее в языке для этого есть*/ );

	auto run() -> void;
private:
	net::Listener m_conn;

	struct request {
		std::string_view method;
		std::string_view uri;
		std::string_view ver;
		std::unordered_map<std::string_view, std::string_view> headers;
		std::string_view body;
	};

	auto send(std::string_view data /**/) const -> void; //??

    auto str_to_metthod(std::string_view method) -> std::optional<Method>{ //>? мож unnecesary
        if(method == "GET"){
            return Method::GET;
        } else if (method == "HEAD"){
            return Method::HEAD;
        } else if (method == "POST"){
            return Method::POST;
        } else if (method == "PUT"){
            return Method::PUT;
        } else if (method == "DELETE"){
            return Method::DELETE;
        } else {
            return std::nullopt; //ващето поидее нам ничё и не дожно плохого приийти
        }
    }

	auto parse_req_str(std::string_view data) const -> std::optional<request>;

};
}

/*ПРИМЕР GET ЗАПРОСА

GET /resource HTTP/1.1
Host: localhost:8080
User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:150.0) Gecko/20100101 Firefox/150.0
Accept: text/html,...
Accept-Language: en-US,en;q=0.9
Accept-Encoding: gzip, deflate, br, zstd
DNT: 1
Sec-GPC: 1
Connection: keep-alive
Upgrade-Insecure-Requests: 1
Sec-Fetch-Dest: document
Sec-Fetch-Mode: navigate
Sec-Fetch-Site: none
Sec-Fetch-User: ?1
Priority: u=0, i
*/


/* example ansewr мне кстати всегда надо отдавать content-length
 * иначе у мя бразуер будет ждать ответа всю жизнь

TODO:
HTTP/1.1 200 OK\r\n
Content-Type: text/html; charset=utf-8\r\n
Content-Length: 44\r\n
Connection: keep-alive\r\n\r\n

<html><body><h1>Hello World!</h1></body></html> <-- вот это
читаем откудато там до этого ещё долго
вконце \0\r\n\r\n

*/
