#include <print>
#include <exception>
#include <net/listener.hpp>
#include <net/acceptor.hpp>
//#include <http/http.hpp>

int main(){
    std::string respone =
        "HTTP/1.1 404\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "Content-Length: 11\r\n"
        "Connection: close\r\n"
        "\r\n"
        "Hello World";

    try {
        net::Listener srv{":8080"};

        srv.listen();
        for(;;){
            auto conn = srv.accept();
            if(!conn){
                continue;
            }
            auto res = conn->recv();
            if(!res){
                std::println("Client disconencted");
                continue;
            }
            std::print("{}", *res);
            conn->send(respone);
        }
    } catch(std::exception& e){
        std::println("[FATAL] {}", e.what());
    }

}
