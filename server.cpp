#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <fstream>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

std::mutex log_mutex;

void handle_client(int socket_fd) {
    char buffer[1024] = {0};
    read(socket_fd, buffer, 1024);

    std::ofstream log_file;
    log_mutex.lock();
    log_file.open("log.txt", std::ios::app);
    log_file << buffer << std::endl;
    log_file.close();
    log_mutex.unlock();

    close(socket_fd);
}

class Server {
public:
    Server(int port) : port(port) {}

    void run() {
        int server_fd, new_socket;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);

        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            std::cerr << "Socket creation failed" << std::endl;
            return;
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            std::cerr << "Setsockopt failed" << std::endl;
            return;
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            std::cerr << "Bind failed" << std::endl;
            return;
        }

        if (listen(server_fd, 3) < 0) {
            std::cerr << "Listen failed" << std::endl;
            return;
        }

        while (true) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                std::cerr << "Accept failed" << std::endl;
                return;
            }

            std::thread client_thread(handle_client, new_socket);
            client_thread.detach();
        }
    }

private:
    int port;
};

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: server <port>" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);
    Server server(port);
    server.run();

    return 0;
}