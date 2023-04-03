#include "utils.h"
#include <cassert>
#include "rtmp_exception.h"

using namespace librtmp;
using namespace std;

#define CHECK_POS(pos, url) (pos != url.npos) ? true : false

ParsedUrl librtmp::ParseURL(std::string url)
{
    ParsedUrl res;

    auto pos_proto = url.find("://", 0);
    if (!CHECK_POS(pos_proto, url)) {
        throw librtmp::rtmp_exception(RTMPError::BAD_URL);
    }
    if (pos_proto != 5 && pos_proto != 4) {
        throw librtmp::rtmp_exception(RTMPError::BAD_URL);
    }
    auto proto_name = url.substr(0, pos_proto);

    if (proto_name == "rtmp") {
        res.type = ProtoType::RTMP;
    } else if (proto_name == "rtmps") {
        res.type = ProtoType::RTMPS;
    } else {
        throw librtmp::rtmp_exception(RTMPError::BAD_URL);
    }

    auto pos_port = url.find(":", pos_proto + 3);
    auto pos_app = url.find("/", pos_proto + 3);
    auto pos_key = url.find("/", pos_app + 1);

    if (!CHECK_POS(pos_app, url) || !CHECK_POS(pos_key, url)) {
        throw librtmp::rtmp_exception(RTMPError::BAD_URL);
    }

    if (!CHECK_POS(pos_port, url)) {
        if (res.type == ProtoType::RTMP) {
            res.port = 1935;
        } else if (res.type == ProtoType::RTMPS) {
            res.port = 443;
        } else {
            assert(false);
            //TODO: add new proto port
        }
        res.url = url.substr(pos_proto + 3, pos_app - pos_proto - 3);
    } else {
        auto port_name = url.substr(pos_port + 1, pos_app - pos_port - 1);
        res.port = atoi(port_name.c_str());
        if (!res.port) {
            throw librtmp::rtmp_exception(RTMPError::BAD_URL);
        }
        res.url = url.substr(pos_proto + 3, pos_port - pos_proto - 3);
    }

    res.app = url.substr(pos_app + 1, pos_key - pos_app - 1);
    res.key = url.substr(pos_key + 1);
    return res;
}
