#ifndef SERVER_H
#define SERVER_H

class Server {
public:
    Server(int port);
    void run();

private:
    int port;
};

#endif // SERVER_H