#include <string_view>
#include <string>

#include <http/types.hpp>

namespace http{
    class Response{
        public:
            auto status(status_code code) noexcept-> Response& {
                m_code = code;
                return *this;
            }

            auto type(std::string_view ct) noexcept -> Response& {
                m_content_type = ct;
                return *this;
            }

            auto body(std::string_view data) noexcept -> Response& {
                m_body = data;
                return *this;
            }

            auto build() const -> std::string;

        private:
            constexpr auto get_status_line() const noexcept -> std::string_view;

            status_code m_code = status_code::INTERNAL_ERROR;
            std::string_view m_content_type = "text/plain; charset=utf-8";
            std::string_view m_body;
    };
}
