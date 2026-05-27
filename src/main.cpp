#include <print>
#include <exception>
#include <net/net.hpp>
#include <vector>

int main(){
    try {

    net::Listener srv{8080};
    srv.listen();
    std::vector<char> buff;

    for(;;){
        srv.accept();
        std::println("got connection form {}", srv.get_addr());
        srv.recv(buff);
        for(auto i : buff){
            std::println("{}", i);
            }
        srv.send("hello");
    }
    } catch(std::exception& e){
        std::println("Error: {}", e.what());
    }

}
