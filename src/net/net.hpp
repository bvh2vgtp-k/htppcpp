#include <cstdint>
#include <string>
#include <sys/socket.h>
#include <string_view>
#include <unistd.h>
#include <netinet/in.h>


/* TODO: https://www.rfc-editor.org/rfc/rfc9112.html */

namespace net {

class Listener final{
public:
    Listener(uint16_t port);
    ~Listener();

    Listener(const Listener&) = delete;
    Listener& operator=(const Listener&) = delete;

    Listener(Listener&& src) noexcept;
    Listener& operator=(Listener&& rhs) noexcept;

    void listen();
    void accept();
    [[nodiscard]] std::string recv();
    void send(std::string&& data);
    void send(std::string_view data);

    void close_client();

    std::string_view get_addr() const {
		return m_clientAddr;
	}
    uint16_t get_port() const {
        return m_port;
    }

private:
    int m_fd = -1;
    int m_clientfd = -1;
    uint16_t m_port = 0;
    std::string m_clientAddr = {}; //maybe
    
    [[nodiscard]] std::string _ntop(const struct sockaddr_in *sa) const noexcept;
    void moveFrom(Listener& src) noexcept;
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
