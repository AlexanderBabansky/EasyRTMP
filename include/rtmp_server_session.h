#include "framework.h"
#include "rtmp_endpoint.h"
#include "rtmp_proto.h"

namespace librtmp {
	/**
	* Media message received from RTMP streamer
	*/
	struct RTMPMediaMessage {
		RTMPMessageType message_type = RTMPMessageType::UNKNOWN;/**< Only AUDIO or VIDEO types available*/
		uint32_t message_stream_id = 0;/**< Id of the stream. RTMP session can transfer many audio/video streams simultaneously, but it is rarely implemented*/
		uint64_t timestamp = 0;/**< Timestamp of packet, for audio is presentation timestamp, for video - decoding timestamp*/
		rtmp_proto::AudioPacketAAC audio;/**< Audio packet. Is only valid, if message_type is AUDIO*/
		rtmp_proto::VideoPacket video;/**< Video packet. Is only valid, if message_type is VIDEO*/
	};

	/**
	* Media server. Contains common logic for accepting media stream
	*/
	class RTMPServerSession {
	private:
		RTMPEndpoint* m_Endpoint;
		ClientParameters m_ClientParameters;
		void HandleAMF(AMFValue data);
		void SendAMFResult(std::list<AMFValue> data, int id);
	public:
		/**
		* @param	endpoint	Established RTMP endpoint
		*/
		RTMPServerSession(RTMPEndpoint* endpoint);
		RTMPServerSession(const RTMPServerSession&) = delete;
		RTMPServerSession(RTMPServerSession&&) = delete;
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
		* after first GetRTMPMessage() call, before it it does not have any sense
		*/
		const ClientParameters* GetClientParameters();
	};
}