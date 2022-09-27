#pragma once
#include <functional>
#include "framework.h"
#include "amf.h"
#include "rtmp_enums.h"
#include "rtmp_proto.h"
#include "EasyRtmpDLLAPI.h"

namespace librtmp {
enum class RTMPMessageType {
    SET_CHUNK_SIZE = 1,
    ABORT = 2,
    ACKNOWLEDGEMENT = 3,
    WINDOW_ACKNOWLEDGEMENT_SIZE = 5,
    SET_PEER_BANDWITH = 6,
    AMF0 = 20,
    AMF3 = 17,
    METADATA_AMF0 = 18,
    METADATA_AMF3 = 15,
    AUDIO = 8,
    VIDEO = 9,
    USER_CONTROL_MESSAGE = 4,
    UNKNOWN = 100
};

/**
	* Media message from RTMP streamer
	*/
struct RTMPMediaMessage
{
    RTMPMessageType message_type
        = RTMPMessageType::UNKNOWN; /**< Only AUDIO or VIDEO types available*/
    uint32_t message_stream_id
        = 0; /**< Id of the stream. RTMP session can transfer many audio/video streams simultaneously, but it is rarely implemented*/
    uint64_t timestamp
        = 0; /**< Timestamp of packet, for audio is presentation timestamp, for video - decoding timestamp*/
    rtmp_proto::AudioPacketAAC audio; /**< Audio packet. Is only valid, if message_type is AUDIO*/
    rtmp_proto::VideoPacket video;    /**< Video packet. Is only valid, if message_type is VIDEO*/
};

struct ChunkStream
{
    size_t message_length = 0;
    size_t message_bytes_read = 0;
    RTMPMessageType message_type = RTMPMessageType::UNKNOWN;
    uint32_t message_stream_id = UINT32_MAX;
    uint64_t timestamp = UINT64_MAX;
    uint32_t timestamp_delta = 0;
    std::vector<char> chunks_buffer;

    ChunkStream() = default;
    ChunkStream(size_t b_len) { chunks_buffer.resize(b_len); }
};

/**
	* Endpoint of communicating RTMP. Used by client or server
	*/
class EASYRTMP_DLLAPI RTMPEndpoint
{
private:
    DataLayer *m_LowerLevel = nullptr;

    //rtmp session parameters
    uint32_t m_MaxMessageSize = 512000;
    unsigned int m_ChunkSizeSend = 128;
    unsigned int m_ChunkSizeReceive = 128;
    std::map<uint8_t, ChunkStream> m_ChunkStreamsInput, m_ChunkStreamsOutput;

    //internal functions
    void DoHandshake();
    ChunkStream &GetChunkStreamInput(uint8_t id);
    void SendSerializable(const Serializable *data);
    void ReceiveSerializable(Serializable *data);

public:
    /**
		* Constructor
		* @param	lower_l		Data transfer layer
		*/
    RTMPEndpoint(DataLayer *lower_l);
    ~RTMPEndpoint();
    RTMPEndpoint(const RTMPEndpoint &o) = delete;
    RTMPEndpoint &operator=(const RTMPEndpoint &) = delete;
    RTMPEndpoint &operator=(RTMPEndpoint &&) = delete;

    //rtmp methods
    /**
		* Receive RTMP message. Method blocks calling thread until message is received.
		* Some messages are internal and not useful for caller. Caller must pass such
		* messages to HandleMessage method.
		* @param[out]	message_type		Type of received message
		* @param[out]	message_stream_id	Id of the stream
		* @param[out]	message_length		Length of the message. Size of returned container is not equal to payload length.
		* @param[out]	chunk_stream_id		Not useful. May be removed later
		* @param[out]	timestamp			Timestamp of packet in milliseconds. For audio packets is presentation timestamp. For video - decoding timestamp.
		* @exception	std::exception		Can throw from transport layers, parsing.
		*/
    DATA_BYTES GetRTMPMessage(RTMPMessageType *message_type, uint32_t *message_stream_id,
                              uint32_t *message_length, uint8_t *chunk_stream_id,
                              uint64_t *timestamp);
    /**
		* Handle internal message. Can be called by client for every received message,
		* but must called for every but not audio/video/amf message. Sometimes blocks
		* calling thread, because some messages requires send additional info to peer.
		* @exception	std::exception		Rarely throws. Can throw from transport layers.
		*/
    bool HandleMessage(DATA_BYTES data, RTMPMessageType message_type, uint32_t message_stream_id,
                       int message_length, int chunk_stream_id, uint64_t timestamp);
    /**
		* Sends WindowSize (bytes count) to remote peer.
		* Part of RTMP QoS, but mostly not used by modern client.
		* @exception	std::exception		Can throw from transport layers.
		*/
    void Send_WindowsAcknowledgementSize(uint32_t ack_size);
    /**
		* Sends Bandwith control policy.
		* Part of RTMP QoS, but mostly not used by modern client.
		* @exception	std::exception		Can throw from transport layers.
		*/
    void Send_SetPeerBandwith(uint32_t bandwith);
    /**
		* Changes chunk size to send, sends control message about this to peer.
		* Modern applications use high values such as 4096 (4kb) to increase
		* payload bandwith.
		* @exception	std::exception		Can throw from transport layers.
		*/
    void ChangeChunkSizeOutput(unsigned int chunk_size);

    //rtmp low level
    /**
		* Sends RTMP message to remote peer. This method blocks calling thread until message is transmited.
		* @exception	std::exception		Can throw from transport layers.
		*/
    void SendRTMPMessage(Serializable *data, uint8_t chunk_stream_id, uint32_t message_stream_id,
                         RTMPMessageType message_type, uint64_t timestamp = 0);
    /**
		* Sends RTMP AMF command to remote peer. Uses SendRTMPMessage internally.
		*/
    void SendCommand(AMFValue data, uint8_t chunk_stream_id = 3, uint32_t message_stream_id = 0,
                     RTMPMessageType message_type = RTMPMessageType::AMF0);
    /**
		* Sends internal RTMP control message. Uses SendRTMPMessage internally.
		*/
    void SendControlMessage(Serializable *data, RTMPMessageType message_type);
    /**
		* Sends User Control Message. Uses SendRTMPMessage internally.
		*/
    void SendUserControlMessage(Serializable *data);
};

} // namespace librtmp
