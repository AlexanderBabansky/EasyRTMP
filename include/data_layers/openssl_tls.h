#pragma once
#include <exception>
#include <memory>
#include "../framework.h"

struct ssl_st;
struct x509_st;
struct evp_pkey_st;
struct ssl_ctx_st;
struct bio_method_st;
struct bio_st;

enum class OpenSSLError {
    ALLOC,
    FILE_OPEN,
    PEM_READ,
    INVALID_CERT,
    INVALID_PRIVATE_KEY,
    HANDSHAKE,
    SYSTEM_CERT_STORE,
    IO
};
class OpenSSLException : public std::exception
{
private:
    OpenSSLError m_Error;

public:
    OpenSSLException(OpenSSLError err) : m_Error(err) {}
    inline OpenSSLError get_error() { return m_Error; }
    char const *what() const noexcept override { return "openssl_exception"; }
};

class OpenSSL_TLS : public DataLayer
{
private:
    ssl_st *m_SSL = nullptr;
    bio_method_st *m_BioMethod = nullptr;
    bio_st *m_BIO = nullptr;

public:
    OpenSSL_TLS(ssl_st *ssl, bio_method_st *bio_method, bio_st *bio);
    void send_data(const char *data, size_t data_len) override;
    void receive_data(char *out, int len) override;
    void destroy() override;

    OpenSSL_TLS(const OpenSSL_TLS &) = delete;
    OpenSSL_TLS &operator=(const OpenSSL_TLS &) = delete;
    OpenSSL_TLS &operator=(OpenSSL_TLS &&o) noexcept;

    ~OpenSSL_TLS();
};

class OpenSSL_TLS_Server
{
private:
    DataLayer *m_LowerLevel = nullptr;
    ssl_ctx_st *ctx = nullptr;
    x509_st *m_Cert = nullptr;
    evp_pkey_st *m_PrivateKey = nullptr;
    ssl_st *m_SSL = nullptr;
    bio_method_st *meth = nullptr;
    bio_st *bio = nullptr;

    void cleanup();

public:
    OpenSSL_TLS_Server() = default;
    std::shared_ptr<OpenSSL_TLS> handshake(DataLayer *lower_l, const char *certificate_filepath,
                                           const char *private_key_filepath);
    OpenSSL_TLS_Server(const OpenSSL_TLS_Server &) = delete;
    OpenSSL_TLS_Server &operator=(const OpenSSL_TLS_Server &) = delete;
    ~OpenSSL_TLS_Server();
};

class OpenSSL_TLS_Client
{
private:
    DataLayer *m_LowerLevel = nullptr;
    ssl_ctx_st *ctx = nullptr;
    ssl_st *m_SSL = nullptr;
    bio_method_st *meth = nullptr;
    bio_st *bio = nullptr;

    void LoadTrustedCerts();
    void cleanup();

public:
    OpenSSL_TLS_Client();
    std::shared_ptr<OpenSSL_TLS> handshake(DataLayer *lower_l, const char *host_name);
    ~OpenSSL_TLS_Client();
};
