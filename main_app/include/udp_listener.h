#ifndef udp_listener_h
#define udp_listener_h

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <deque>

class UdpListener
{
public:
    UdpListener(int port);
    ~UdpListener();

    void start();
    void stop();

    bool popMessage(std::string &outMessage);

private:
    void listenLoop();

    int port;
    std::atomic<bool> running{false};
    std::thread listenThread;

    std::mutex queueMutex;
    std::deque<std::string> msgQueue;
};

#endif