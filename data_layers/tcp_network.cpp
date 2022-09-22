#include <cassert>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
WSADATA wsaData;
#else
//#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#endif

#include "data_layers/tcp_network.h"

using namespace std;

void InitNetwork()
{
#ifdef WIN32
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

TCPNetwork::TCPNetwork(unsigned int sock) : m_Socket(sock) {}

TCPNetwork &TCPNetwork::operator=(TCPNetwork &&o)
{
    destroy();
    m_Socket = o.m_Socket;
    o.m_Socket = 0;
    return *this;
}

void TCPNetwork::send_data(const char *data, size_t data_len)
{
#ifdef WIN32
    if (send(m_Socket, data, data_len, 0) == SOCKET_ERROR) {
#else
    if (write(m_Socket, data, data_len) < 0) {
#endif
        throw TCPNetworkException(TCPNetworkError::SENDING);
    }
}

void TCPNetwork::receive_data(char *out, int len)
{
    int ofs = 0;
    do {
#ifdef WIN32
        auto ofs_add = recv(m_Socket, out + ofs, len - ofs, 0);
#else
        auto ofs_add = read(m_Socket, out + ofs, len - ofs);
#endif
        if (ofs_add <= 0) {
            throw TCPNetworkException(TCPNetworkError::RECEIVING);
        }
        ofs += ofs_add;
    } while (ofs < len);
}

void TCPNetwork::destroy()
{
#ifdef WIN32
    shutdown(m_Socket, SD_BOTH);
    closesocket(m_Socket);
#else
    close(m_Socket);
#endif // WIN32
    m_Socket = 0;
}

TCPNetwork::~TCPNetwork() { destroy(); }

TCPServer::TCPServer(uint16_t port)
{
    InitNetwork();
    m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_Socket < 0) {
        throw TCPNetworkException(TCPNetworkError::CREATE_SOCKET);
    }
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    auto iResult = bind(m_Socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (iResult < 0) {
        throw TCPNetworkException(TCPNetworkError::BIND_SOCKET);
    }
    iResult = listen(m_Socket, SOMAXCONN);
    if (iResult < 0) {
        throw TCPNetworkException(TCPNetworkError::LISTEN_SOCKET);
    }
}

std::shared_ptr<TCPNetwork> TCPServer::accept()
{
    unsigned int accepted_sock = ::accept(m_Socket, NULL, NULL);
    if (accepted_sock < 0) {
        throw TCPNetworkException(TCPNetworkError::ACCEPTED_SOCKET);
    }
    return make_shared<TCPNetwork>(accepted_sock);
}

void TCPServer::destroy()
{
#ifdef WIN32
    closesocket(m_Socket);
#else
    close(m_Socket);
#endif // WIN32
}

TCPServer::~TCPServer() { destroy(); }

void TCPClient::cleanup() {}

TCPClient::TCPClient() { InitNetwork(); }

std::shared_ptr<TCPNetwork> TCPClient::ConnectToHost(const char *host, uint16_t port)
{
    cleanup();
    unsigned int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        throw TCPNetworkException(TCPNetworkError::CREATE_SOCKET);
    }
    struct hostent *server;
    struct sockaddr_in serv_addr;
    server = gethostbyname(host);
    if (server == NULL) {
        throw TCPNetworkException(TCPNetworkError::ADDRESS_TRANSLATION);
    }

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy((char *)&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    auto iResult = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (iResult < 0) {
        throw TCPNetworkException(TCPNetworkError::CONNECT_SOCKET);
    }
    return make_shared<TCPNetwork>(sock);
}

TCPClient::~TCPClient() { cleanup(); }
