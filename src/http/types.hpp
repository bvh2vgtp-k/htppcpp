#pragma once
#include <cstdint>
#include <string_view>
#include <unordered_map>

namespace http {
    enum class Method : uint8_t {
        GET,
        HEAD,
        POST,
        PUT,
        DELETE,
    };

    enum class status_code : uint16_t {
        OK = 200,
        BAD_REQ = 400,
        NOT_FOUND = 404,
        NOT_ALLOWED = 405,
        INTERNAL_ERROR = 500
        /* ... */
    };

    struct Request {
        std::string_view method_uri;
        std::string_view ver;
        std::unordered_map<std::string_view, std::string_view> headers;
        std::string_view body;
    };

}
