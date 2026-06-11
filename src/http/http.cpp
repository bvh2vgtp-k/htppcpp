#include <charconv>
#include <http/http.hpp>
#include <print>
#include <string_view>

namespace http {

    Server::Server(std::string_view host) : m_conn(host)
    {
        std::println("Server started with pid [{}]", getpid());
    }

    Server::~Server(){
        std::println("Sever stopped");
    }

    auto Server::run() -> void {
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
        m_conn.listen();
        for(;;){
            auto peer = m_conn.accept();
            if(!peer){
                continue;
            }
            std::println("got connection from {}", peer->get_addrstr());
            auto res = peer->recv();
            if(!res){
                std::println("peer {} disconnected", peer->get_addrstr());
                continue;
            }
            auto req = parse_req_str(*res);
            if(!req){
                peer->send(bad_req);
            } else if (req->method != "GET") {
                peer->send(not_allowed);
            } else if (req->method == "GET" && req->uri == "/"){
                peer->send(respone);
            } else if(req->uri != "/"){
                peer->send(not_found);
            }
            std::println("[{}]'s request: {}", peer->get_addrstr(), req->uri);
        }
    }


    auto Server::parse_req_str(std::string_view data) const -> std::optional<request> {
        /*вообще можно expected возрващать с конкретным чемто*/
        request req;

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
