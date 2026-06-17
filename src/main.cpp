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
        srv.run();
    } catch(std::exception& e){
        std::println("[FATAL] {}", e.what());
    }
}
