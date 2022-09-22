#pragma once
#include <exception>
#include <memory>
#include "framework.h"
#include "EasyRtmpDLLAPI.h"

enum class TCPNetworkError {
    SENDING,
    RECEIVING,
    CREATE_SOCKET,
    BIND_SOCKET,
    LISTEN_SOCKET,
    ADDRESS_TRANSLATION,
    ACCEPTED_SOCKET,
    CONNECT_SOCKET
};

class TCPNetworkException : public std::exception
{
private:
    TCPNetworkError m_Error;

public:
    TCPNetworkException(TCPNetworkError err) : m_Error(err) {}
    inline TCPNetworkError get_error() { return m_Error; }
    char const *what() const noexcept override
    {
        return "tcp_network_exception"; //TODO: describe
    }
};

class EASYRTMP_DLLAPI TCPNetwork : public DataLayer
{
private:
    unsigned int m_Socket = 0;

public:
    TCPNetwork() {}
    TCPNetwork(unsigned int sock);
    TCPNetwork(const TCPNetwork &o) = delete;
    TCPNetwork &operator=(TCPNetwork &&o);
    TCPNetwork &operator=(const TCPNetwork &) = delete;
    void send_data(const char *data, size_t data_len) override;
    void receive_data(char *out, int len) override;
    void destroy() override;
    ~TCPNetwork();
};

class EASYRTMP_DLLAPI TCPServer
{
private:
    unsigned int m_Socket = 0;

public:
    TCPServer(uint16_t port);
    TCPServer(const TCPServer &) = delete;
    TCPServer &operator=(const TCPServer &) = delete;
    std::shared_ptr<TCPNetwork> accept();
    void destroy();
    ~TCPServer();
};

class EASYRTMP_DLLAPI TCPClient
{
private:
    void cleanup();

public:
    TCPClient();
    std::shared_ptr<TCPNetwork> ConnectToHost(const char *host, uint16_t port);
    ~TCPClient();
};
