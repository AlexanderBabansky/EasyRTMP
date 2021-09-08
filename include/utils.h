#include <string>

namespace librtmp {
	enum class ProtoType {
		RMTP,
		RTMPS
	};
	struct ParsedUrl {
		ProtoType type;
		std::string url;
		uint16_t port = 1935;
		std::string app;
		std::string key;
	};
	ParsedUrl ParseURL(std::string url);
}