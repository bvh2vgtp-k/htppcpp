#include <print>
#include <exception>
#include <http/http.hpp>

int main(){
    try {
        http::Server srv{":8080"};
        srv.register_handler(http::Method::GET, "/", [](){
            return http::Response()
                .status(http::status_code::OK)
                .type("text/plain; charset=utf-8")
                .body("Hello")
                .build();
        });

        srv.register_handler(http::Method::GET, "/hello", [](){
            std::string html = R"(
            <!DOCTYPE html>
            <html lang="en">
            <head>
                <meta charset="UTF-8">
                <title>My page</title>
            </head>
            <body>
                <h1 class="main-title">hello httpsrv</h1>
                <p>example of html formated text</p>
            </body>
            </html>
            )";

            return http::Response()
                .status(http::status_code::OK)
                .type("text/html; charset=utf-8")
                .body(html)
                .build();
        });
        srv.run();
    } catch(std::exception& e){
        std::println("[FATAL] {}", e.what());
    }

    return 0;
}
