#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <string_view>
#include <unistd.h>
#include <netinet/in.h>

/*
   Кароче в чём идея: я щас беру и забиваю просто ХУЯТИНУ на вот этот интерфейс
   там)) врапер блять вокрук сокетов. Просто ебанутые люди эту хуйню писали. 
   Я беру когда в следующий раз сажусь и делаю как человек. У меня будет весеть
   апи сокетов прямо нахуй в реализации http и похуй. ТАКЖЕ НЕ ЗАБЫТЬ ПРОЧИТАТЬ
   нормально про http. Мб http from scratch там)) но мало вероятно, лучше уж 
   чёто путнее прочитать. В следующий раз нормально распланировать реализацию

   ЕСЛИ чё надо подспиздить реализацию listen() из ver1. Там ПОЧТИ нормально 
 */

/*
   ~/cpp/network/explain_icmp_ping/src/cpp вот тут пиздато реализован класс 
   с сокетом, оттуда подспиздитьь надо. Я кстатип почти правильно пришёл даже
 */

/* TODO: https://www.rfc-editor.org/rfc/rfc9112.html */

namespace net {

//не выёюываться особо по беспонту
class Listener{
public:
    Listener(uint16_t port);
    ~Listener();

    void listen();
    void accept();
    void recv(std::vector<std::byte>& buff);
    void send(std::string&& data);

    void close();
    /*какого-то рода recv должна быть, хуй знает */

    std::string_view get_addr() const;

private:
    int m_fd = -1;
    int m_clientfd = -1;
    uint16_t m_port = 0;
    std::string m_endpoint = {}; //maybe
};
}
/*

#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io_context;

        // 1. Listen on a specific port (e.g., 12345)
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));

        std::cout << "Waiting for connections..." << std::endl;

        for (;;) {
            // 2. Create a socket for the new connection
            tcp::socket socket(io_context);
            // 3. Block until a client connects
            acceptor.accept(socket);
            
            std::cout << "Client connected!" << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}

 * */
