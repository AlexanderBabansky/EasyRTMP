#include <string>
#include "EasyRtmpDLLAPI.h"

namespace librtmp {
enum class ProtoType { RTMP, RTMPS, UNKNOWN = 100 };
struct ParsedUrl
{
    ProtoType type = ProtoType::UNKNOWN;
    std::string url;
    uint16_t port = 1935;
    std::string app;
    std::string key;
};
ParsedUrl EASYRTMP_DLLAPI ParseURL(std::string url);
} // namespace librtmp
