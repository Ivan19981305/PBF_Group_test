#include <iostream>
#include <string>
#include <cstring>
#include <chrono>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iomanip>

class Client {
public:
    Client(const std::string& name, const std::string& server_address, int port, int period) 
        : name(name), server_address(server_address), port(port), period(period) {}

    void run() {
        while (true) {
            int sock = 0;
            struct sockaddr_in serv_addr;

            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                std::cerr << "Socket creation error" << std::endl;
                return;
            }

            memset(&serv_addr, '0', sizeof(serv_addr));

            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(port);

            if(inet_pton(AF_INET, server_address.c_str(), &serv_addr.sin_addr)<=0) {
                std::cerr << "Invalid address/ Address not supported" << std::endl;
                return;
            }

            if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
                std::cerr << "Connection Failed" << std::endl;
                return;
            }

            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&in_time_t), "[%Y-%m-%d %H:%M:%S");
            ss << "." << std::setfill('0') << std::setw(3) << std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
            ss << "] \"" << name << "\"";

            std::string message = ss.str();
            send(sock , message.c_str() , message.length() , 0 );
            close(sock);

            std::this_thread::sleep_for(std::chrono::seconds(period));
        }
    }

private:
    std::string name;
    std::string server_address;
    int port;
    int period;
};

int main(int argc, char const *argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: client <name> <server_address> <port> <period>" << std::endl;
        return 1;
    }

    std::string name = argv[1];
    std::string server_address = argv[2];
    int port = std::stoi(argv[3]);
    int period = std::stoi(argv[4]);

    Client client(name, server_address, port, period);
    client.run();

    return 0;
}