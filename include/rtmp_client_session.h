#include "framework.h"
#include "rtmp_endpoint.h"
#include "rtmp_proto.h"
#include <functional>

namespace librtmp {

/**
	* Media client. Contains common logic for streaming media to server
	*/
class RTMPClientSession
{
private:
    RTMPEndpoint *m_Endpoint;
    bool sent_params = false;

    bool GetCommandResponse(std::function<bool(AMFValue)> clb, int cmd_id);
    void SendAmfConnect(std::string app, std::string url);
    void SendReleaseStream(std::string key);
    void SendFCPublish(std::string key);
    void SendCreateStream();
    void SendPublish(std::string key);
    void SendDataFrameParameters(const ClientParameters *params);

public:
    /**
		* @param	endpoint	Established RTMP endpoint
		*/
    RTMPClientSession(RTMPEndpoint *endpoint);
    RTMPClientSession(const RTMPClientSession &) = delete;
    RTMPClientSession(RTMPClientSession &&) = delete;
    /**
		* Send media message. Blocks calling thread
		* until audio or video message is sent. You should SendClientParameters()
		* first, to tell receiver about media format
		* @exception	std::exception	Exceptions: network.
		* When throws, usually rtmp endpoint is not usable
		* any more and connection is terminated
		*/
    void SendRTMPMessage(RTMPMediaMessage message);
    /**
		* Send client, media codec parameters. It must be sent before any media message.
		* Sends all ActionScript commands to initialize media session
		* @params	parameters to send
		* @throw	std::exception network
		*/
    void SendClientParameters(const ClientParameters *params);

    //TODO: nice session termination with releaseStream
};
} // namespace librtmp
