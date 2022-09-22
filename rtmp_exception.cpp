#include "rtmp_exception.h"

using namespace librtmp;

rtmp_exception::rtmp_exception(RTMPError code) : m_Code(code) {}

char const *rtmp_exception::what() const noexcept
{
    switch (m_Code) {
    case librtmp::RTMPError::NETWORK: return "network";
    case librtmp::RTMPError::BAD_URL: return "bad url";
    case librtmp::RTMPError::PARSING: return "parsing";
    case librtmp::RTMPError::PROTOCOL: return "protocol";
    case librtmp::RTMPError::AMF: return "amf";
    case librtmp::RTMPError::NETSTREAM_CONNECTION: return "netstream connection";
    case RTMPError::ENDPOINT_STATUS: return "endpoint status";
    case RTMPError::INTERNAL: break;
    }
    return "librtmp error";
}

RTMPError rtmp_exception::get_code() { return m_Code; }
