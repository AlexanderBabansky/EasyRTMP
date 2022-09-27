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
    OUT_OF_BUFFER,
    INTERNAL
};

class rtmp_exception : public std::exception
{
private:
    RTMPError m_Code;

public:
    rtmp_exception(RTMPError code) : m_Code(code) {}
    RTMPError get_code() { return m_Code; }
    virtual char const *what() const noexcept override
    {
        switch (m_Code) {
        case librtmp::RTMPError::NETWORK: return "network";
        case librtmp::RTMPError::BAD_URL: return "bad url";
        case librtmp::RTMPError::PARSING: return "parsing";
        case librtmp::RTMPError::PROTOCOL: return "protocol";
        case librtmp::RTMPError::AMF: return "amf";
        case librtmp::RTMPError::NETSTREAM_CONNECTION: return "netstream connection";
        case RTMPError::OUT_OF_BUFFER: return "out of buffer";
        case RTMPError::ENDPOINT_STATUS: return "endpoint status";
        case RTMPError::INTERNAL: break;
        }
        return "librtmp error";
    }
};
}; // namespace librtmp
