#include <print>
#include <exception>
#include <net/net.hpp>
//#include <http/http.hpp>

int main(){
    try {
        net::Listener srv{":8080"};
        srv.listen();
        for(;;){
            srv.accept();
            std::println("got conn from {}", srv.get_addr());
            std::string res = srv.recv();
            std::print("{}: {}", srv.get_addr(), res);
            //srv.send("hello");
        }
    } catch(std::exception& e){
        std::println("[FATAL] {}", e.what());
    }

}
