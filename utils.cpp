#include "utils.h"

using namespace librtmp;
using namespace std;


ParsedUrl librtmp::ParseURL(std::string url) {
	auto pos = url.find("://",0);
	/*if (!CHECK_POS(url)) {
		throw exception("Bad URL");
	}*/

	ParsedUrl res;
	return res;
}