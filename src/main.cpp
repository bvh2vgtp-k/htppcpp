#include <print>
#include <exception>
#include <net/net.hpp>

int main(){
    try {

    net::Listener srv{1488};
    srv.listen();

    for(;;){
        srv.accept();
        srv.send("hello");
    }
    } catch(std::exception& e){
        std::println("Error: ", e.what());
    }

}
