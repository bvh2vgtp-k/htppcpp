#include <http/response/Response.hpp>

namespace http {
    auto Response::build() const -> std::string {
        std::string_view status_lie = get_status_line();
        std::string length_str = m_body.empty() ? "" : std::to_string(m_body.size());

        size_t total_size = status_lie.size() + 2; // \r\n
        if(!m_content_type.empty()){
            total_size += 14 + m_content_type.size() + 2; //content-length: + \r\n
        }
        total_size += 26; // connection: keep-alive\r\n\r\n
        total_size += m_body.size();

        std::string res;
        res.reserve(total_size);

        res.append(status_lie).append("\r\n");
        if(!m_content_type.empty()){
            res.append("Content-type: ").append(m_content_type).append("\r\n");
        }
        if(!m_body.empty()){
            res.append("Content-length: ").append(length_str).append("\r\n");
        }
        res.append("Connection: keep-alive\r\n\r\n");
        res.append(m_body);

        return res;
    }


    constexpr auto Response::get_status_line() const noexcept -> std::string_view {
        switch (m_code) {
            case http::status_code::OK:          return "HTTP/1.1 200 OK";
            case http::status_code::BAD_REQ:     return "HTTP/1.1 400 Bad Request";
            case http::status_code::NOT_FOUND:   return "HTTP/1.1 404 Not Found";
            case http::status_code::NOT_ALLOWED: return "HTTP/1.1 405 Method Not Allowed";
            default:                             return "HTTP/1.1 500 Internal Server Error";
        }
    }

}
