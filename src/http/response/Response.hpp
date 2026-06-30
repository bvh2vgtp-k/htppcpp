#include <string_view>
#include <string>

#include <http/types.hpp>

namespace http{
    class Response{
        public:
            Response& status(status_code code) noexcept {
                m_code = code;
                return *this;
            }

            Response& type(std::string_view ct) noexcept {
                m_content_type = ct;
                return *this;
            }

            Response& body(std::string_view data) noexcept {
                m_body = data;
                return *this;
            }

            std::string build() const;

        private:
            constexpr std::string_view get_status_line() const noexcept;

            status_code m_code = status_code::INTERNAL_ERROR;
            std::string_view m_content_type = "text/plain; charset=utf-8";
            std::string_view m_body;
    };
}
