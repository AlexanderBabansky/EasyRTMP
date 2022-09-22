#include "framework.h"
#include "rtmp_endpoint.h"
#include "rtmp_proto.h"
#include "EasyRtmpDLLAPI.h"

namespace librtmp {

/**
	* Media server. Contains common logic for accepting media stream
	*/
class EASYRTMP_DLLAPI RTMPServerSession
{
private:
    RTMPEndpoint *m_Endpoint;
    ClientParameters m_ClientParameters;
    void HandleAMF(AMFValue data);
    void SendAMFResult(std::list<AMFValue> data, int id);

public:
    /**
		* @param	endpoint	Established RTMP endpoint
		*/
    RTMPServerSession(RTMPEndpoint *endpoint);
    RTMPServerSession(const RTMPServerSession &) = delete;
    RTMPServerSession(RTMPServerSession &&) = delete;
    /**
		* Get media message. Updates client parameters. Blocks calling thread
		* until audio or video message is received.
		* @exception	std::exception	Exceptions: network,
		* parsing, rtmp logic. When throws, usually rtmp endpoint is not usable
		* any more and connection is terminated
		*/
    RTMPMediaMessage GetRTMPMessage();
    /**
		* Get client parameters received by streamer. This parameters is seted
		* after first GetRTMPMessage() call, before it it does not have any sense.
		* Caller can update parameters by data received from media packets.
		* For example, NGINX RTMP module pushes ActionScript metadata without
		* audio channels count and samplerate
		*/
    ClientParameters *GetClientParameters();
};
} // namespace librtmp
