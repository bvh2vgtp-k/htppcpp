#include <cassert>
#include <charconv>
#include <optional>
#include <print>
#include <stdexcept>
#include <string>
#include <string_view>

#include <http/http.hpp>
#include <http/types.hpp>
#include <net/acceptor.hpp>

namespace http {

    auto Server::run() -> void {
        #if 0
        std::string respone =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain; charset=utf-8\r\n"
            "Content-Length: 11\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Hello World";
        std::string bad_req =
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n";

        std::string not_allowed =
            "HTTP/1.1 405 Method Not Allowed\r\n"
            "Content-Length: 0\r\n"
            "\r\n"
            "Allow: GET";

        std::string not_found =
            "HTTP/1.1 404 Not found\r\n";
            //"Content-Length: 0\r\n"
            //"\r\n";
        #endif
        m_listenfd.listen();
        for(;;){
            auto peer = m_listenfd.accept();
            if(peer.has_value()) {
                std::println("got connection from: {}", peer->get_addrstr());
                auto raw_request = peer->recv();
                if(raw_request.has_value()){
                    auto request = parse_req_str(*raw_request);
                    //WARN: Костылирование
                    std::string method_uri = std::string{request->method} + " " + std::string{request->uri};
                    if(m_routes.contains(method_uri)){
                        //WARN: костылирование 2
                        auto res = m_routes.at(method_uri)();
                        peer->send(res);
                    } else {
                        peer->send(
                            Response()
                                .status(http::status_code::BAD_REQ)
                                .type("text/plain; charset=utf-8")
                                .build()
                        );
                    }
                }

            }
        }
    }

    auto Server::register_handler(http::Method method, std::string uri, Handler&& handler) -> void{
        assert(!uri.empty() && uri[0] == '/' && "URI must not be empty and must start with '/'");
        std::string key {method_to_str(method)};
        if(!key.empty()){
            key += uri;
            auto [it, inserted] = m_routes.emplace(std::move(key), std::move(handler));
            if(!inserted){
                throw std::runtime_error("Duplicate route registration: " + key + " already exist");
            }
        } else {
            throw std::invalid_argument("Invalid HTTP method provided");
        }
    }


    constexpr auto Server::method_to_str(http::Method method) -> std::string_view{
        switch (method) {
            case http::Method::GET:     return "GET ";
            case http::Method::HEAD:    return "HEAD ";
            case http::Method::POST:    return "POST ";
            case http::Method::PUT:     return "PUT ";
            case http::Method::DELETE:  return "DELETE ";
            default:                    return "";
        }
    }

    auto Server::parse_req_str(std::string_view data) const -> std::optional<Request> {
        /*вообще можно expected возрващать с конкретным чемто*/
        Request req;

        size_t header_end = data.find("\r\n\r\n");
        if(header_end == std::string_view::npos){
            return std::nullopt;
        }

        std::string_view header = data.substr(0, header_end);
        std::string_view body = data.substr(header_end + 4);

        size_t line_end = header.find("\r\n");
        if(line_end == std::string_view::npos){
            return std::nullopt;
        }

        std::string_view request_line = header.substr(0, line_end);


        size_t space1 = data.find(' ');
        if(space1 == std::string_view::npos){
            return std::nullopt;
        }
        //TODO: поменять на объедеённый
        req.method = request_line.substr(0, space1);

        size_t space2 = data.find(' ', space1 + 1);
        if(space2 == std::string_view::npos){
            return std::nullopt;
        }
        req.uri = request_line.substr(space1 + 1, space2 - (space1 + 1));
        req.ver = request_line.substr(space2 + 1);

        size_t end = data.find("\r\n", space2 + 1);
        if(end == std::string_view::npos){
            return std::nullopt;
        }

        size_t curr_pos = line_end + 2;
        while(curr_pos < header.size()){
            size_t next_line = header.find("\r\n", curr_pos);
            if(next_line == std::string_view::npos){
                next_line = header.size();
            }
            std::string_view header_line = header.substr(curr_pos, next_line - curr_pos);
            if(!header_line.empty()){
                size_t col = header_line.find(';');
                if(col != std::string_view::npos){
                    std::string_view key = header_line.substr(0, col);
                    std::string_view value = header_line.substr(col + 1);

                    while(!value.empty() && value.front() == ' ') value.remove_prefix(1);
                    while(!value.empty() && value.back() == ' ') value.remove_suffix(1);

                    req.headers[key] = value;
                }
            }
            curr_pos = next_line + 2;
        }

        auto it = req.headers.find("Content-Length");
        if(it != req.headers.end()){
            std::string_view sv = it->second;
            size_t content_length = 0;

            auto [ptr, err] = std::from_chars(sv.data(), sv.data() + sv.size(), content_length);
            if(err != std::errc{}){
                return std::nullopt;
            }

            if(body.size() < content_length){
                return std::nullopt;
            }
            req.body = body.substr(0, content_length);
        }

        return req;
    }
}
