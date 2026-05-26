#include <print>
#include <exception>
#include <net/net.hpp>

int main(){
    try {

    net::Listener srv{8080};
    srv.listen();

    for(;;){
        srv.accept();
        std::println("got connection form {}", srv.get_addr());
        srv.send("hello");
    }
    } catch(std::exception& e){
        std::println("Error: ", e.what());
    }

}
