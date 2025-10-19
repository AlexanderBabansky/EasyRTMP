#include <cstdlib>
#include "easyrtmp/rtmp_endpoint.h"

int main(void)
{
    class DummyDataLayer : public DataLayer
    {
    public:
        virtual ~DummyDataLayer() = default;

        virtual void send_data(const char *data, size_t data_len) override { throw 0; }
        virtual void receive_data(char *out, int len) override { throw 0; }
        virtual void destroy() override { return; }
    };
    DummyDataLayer dummy;

    try {
        librtmp::RTMPEndpoint endpoint(&dummy);
    } catch (...) {
        return EXIT_SUCCESS;
    }
    return 1;
}
