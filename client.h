#ifndef CLIENT_H
#define CLIENT_H

#include <string>

class Client {
public:
    Client(const std::string& name, const std::string& server_address, int port, int period);
    void run();

private:
    std::string name;
    std::string server_address;
    int port;
    int period;
};

#endif // CLIENT_H