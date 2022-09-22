#pragma once
#include <exception>

namespace librtmp {
enum class RTMPError {
    NETWORK,
    BAD_URL,
    PARSING,
    PROTOCOL,
    AMF,
    NETSTREAM_CONNECTION,
    ENDPOINT_STATUS,
    INTERNAL
};

class rtmp_exception : public std::exception
{
private:
    RTMPError m_Code;

public:
    rtmp_exception(RTMPError code);
    RTMPError get_code();
    virtual char const *what() const noexcept override;
};
}; // namespace librtmp
