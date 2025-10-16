#include "udp_listener.h"
#include <iostream>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

UdpListener::UdpListener(int port) : port(port)
{
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

UdpListener::~UdpListener()
{
    stop();
#ifdef _WIN32
    WSACleanup();
#endif
}

void UdpListener::start()
{
    if (running)
        return;
    running = true;
    listenThread = std::thread(&UdpListener::listenLoop, this);
}

void UdpListener::stop()
{
    running = false;
    if (listenThread.joinable())
        listenThread.join();
}

bool UdpListener::popMessage(std::string &outMessage)
{
    std::lock_guard<std::mutex> lock(queueMutex);
    if (msgQueue.empty())
        return false;
    outMessage = std::move(msgQueue.front());
    msgQueue.pop_front();
    return true;
}

void UdpListener::listenLoop()
{
    SOCKET sockfd;
    struct sockaddr_in addr;
    char buffer[2048];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET)
    {
        std::cerr << "[UDP] Failed to create socket\n";
        return;
    }

    // Force blocking mode
    u_long mode = 0;
    ioctlsocket(sockfd, FIONBIO, &mode);

    // Allow rebinding even if the port is still in TIME_WAIT
    int reuse = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "[UDP] Bind failed on port " << port << "\n";
        closesocket(sockfd);
        return;
    }

    std::cout << "[UDP] Listening on port " << port << "...\n";

    sockaddr_in senderAddr{};
    socklen_t addrLen = sizeof(senderAddr);

    while (running)
    {
        int len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                           (struct sockaddr *)&senderAddr, &addrLen);

        if (len == SOCKET_ERROR)
        {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK || err == WSAETIMEDOUT)
                continue;
            if (!running)
                break;
            std::cerr << "[UDP] recvfrom() error: " << err << "\n";
            break;
        }

        std::cout << "[UDP DEBUG] recvfrom() returned len=" << len
                  << " err=" << WSAGetLastError() << "\n";

        if (len > 0)
        {
            buffer[len] = '\0';
            std::string msg(buffer);

            {
                std::lock_guard<std::mutex> lock(queueMutex);
                msgQueue.push_back(msg);
                if (msgQueue.size() > 100)
                    msgQueue.pop_front();
            }

            std::cout << "[UDP] Received: " << msg << "\n";
        }
    }

    closesocket(sockfd);
    std::cout << "[UDP] Listener stopped.\n";
}
