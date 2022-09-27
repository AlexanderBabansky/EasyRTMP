#include <iostream>
#include <cassert>
#include <mutex>
#include "rtmp_endpoint.h"
#include "rtmp_exception.h"

using namespace rtmp_proto;
using namespace librtmp;
using namespace std;

ChunkStream &RTMPEndpoint::GetChunkStreamInput(uint8_t id)
{
    if (m_ChunkStreamsInput.find(id) == m_ChunkStreamsInput.end()) {
        m_ChunkStreamsInput.insert(make_pair(id, ChunkStream(m_MaxMessageSize)));
    }
    return m_ChunkStreamsInput.at(id);
}

//amf functions
void RTMPEndpoint::SendSerializable(const Serializable *data)
{
    m_LowerLevel->send_data(data->SerializeToBuffer());
}

void RTMPEndpoint::ReceiveSerializable(Serializable *data)
{
    data->Parse(m_LowerLevel->receive_data(data->GetLength()));
}

RTMPEndpoint::RTMPEndpoint(DataLayer *lower_l) : m_LowerLevel(lower_l)
{
    assert(lower_l);
    DoHandshake();
    ChangeChunkSizeOutput(4096);
}

RTMPEndpoint::~RTMPEndpoint() {}

void RTMPEndpoint::DoHandshake()
{
    C0 c0;
    SendSerializable(&c0);
    C1 c1;
    SendSerializable(&c1);

    C0 s0;
    ReceiveSerializable(&s0);
    if (s0.d.version != 3) {
        throw rtmp_exception(RTMPError::PROTOCOL);
    }
    C1 s1;
    ReceiveSerializable(&s1);

    C2 c2;
    c2.d.time = s1.d.time;
    c2.d.time2 = 0;
    memcpy(c2.d.random_bytes_echo, s1.d.random_bytes, 1528);
    SendSerializable(&c2);

    C2 s2;
    ReceiveSerializable(&s2);
    /*should check random bytes, but isn't useful
	if (memcmp(c1.d.random_bytes, s2.d.random_bytes_echo, 1528)) {
		protocol error
	}*/
}

DATA_BYTES RTMPEndpoint::GetRTMPMessage(RTMPMessageType *message_type,
                                        uint32_t *message_stream_id, uint32_t *message_length,
                                        uint8_t *_o_chunk_stream_id, uint64_t *timestamp)
{
    assert(message_stream_id && message_type && timestamp && message_length);
    do {
        ChunkBasicHeader basic_header;
        ReceiveSerializable(&basic_header);
        uint8_t chunk_steam_id = basic_header.d.cs_id;
        auto &cs = GetChunkStreamInput(chunk_steam_id);

        switch (basic_header.d.fmt) {
        case 0: {
            ChunkMessageHeader0 mes_header0;
            ReceiveSerializable(&mes_header0);

            if (!cs.message_bytes_read) {
                cs.timestamp_delta = mes_header0.timestamp - cs.timestamp;
            }
            cs.timestamp = mes_header0.timestamp;
            cs.message_length = mes_header0.message_length;
            cs.message_type = (RTMPMessageType)mes_header0.message_type_id;
            cs.message_stream_id = mes_header0.message_stream_id;
        } break;
        case 1: {
            ChunkMessageHeader1 mes_header1;
            ReceiveSerializable(&mes_header1);
            if (!cs.message_bytes_read) {
                cs.timestamp_delta = mes_header1.timestamp_delta;
                cs.timestamp += mes_header1.timestamp_delta;
            }
            cs.message_length = mes_header1.message_length;
            cs.message_type = (RTMPMessageType)mes_header1.message_type_id;
        } break;
        case 2: {
            ChunkMessageHeader2 mes_header2;
            ReceiveSerializable(&mes_header2);
            if (!cs.message_bytes_read) {
                cs.timestamp_delta = mes_header2.timestamp_delta;
                cs.timestamp += mes_header2.timestamp_delta;
            }
        } break;
        case 3:
            if (!cs.message_bytes_read) {
                cs.timestamp += cs.timestamp_delta;
            }
            break;
        }

        if (m_MaxMessageSize < cs.message_length) {
            cs.chunks_buffer.resize(cs.message_length);
            m_MaxMessageSize = cs.message_length;
        }
        auto to_receive_bytes = std::min(cs.message_length - cs.message_bytes_read,
                                         size_t(m_ChunkSizeReceive));
        auto rec_data = m_LowerLevel->receive_data(to_receive_bytes);
        memcpy(cs.chunks_buffer.data() + cs.message_bytes_read, rec_data.data(), to_receive_bytes);
        cs.message_bytes_read += to_receive_bytes;

        if (cs.message_bytes_read == cs.message_length) {
            cs.message_bytes_read = 0;
            *message_length = cs.message_length;
            *message_stream_id = cs.message_stream_id;
            *message_type = cs.message_type;
            *timestamp = cs.timestamp;
            *_o_chunk_stream_id = chunk_steam_id;
            DATA_BYTES data_return;
            data_return.resize(cs.message_length);
            memcpy(data_return.data(), cs.chunks_buffer.data(), cs.message_length);
            return move(data_return);
        }
    } while (true);
}

bool RTMPEndpoint::HandleMessage(DATA_BYTES data, RTMPMessageType message_type,
                                 uint32_t message_stream_id, int message_length,
                                 int chunk_stream_id, uint64_t timestamp)
{
    switch (message_type) {
    case RTMPMessageType::SET_CHUNK_SIZE: {
        SetChunkSize scz;
        scz.Parse(data);
        m_ChunkSizeReceive = scz.chunk_size;
    } break;
    case RTMPMessageType::ABORT: {
        m_ChunkStreamsInput.at(chunk_stream_id).message_bytes_read = 0;
    } break;
    case RTMPMessageType::ACKNOWLEDGEMENT: {
        //TODO
    } break;
    case RTMPMessageType::WINDOW_ACKNOWLEDGEMENT_SIZE: {
        WindowsAcknowledgementSize was;
        was.Parse(data);
        //TODO
    } break;
    case RTMPMessageType::SET_PEER_BANDWITH: {
        SetPeerBandwith spb;
        spb.Parse(data);
        //TODO
    } break;
    case RTMPMessageType::USER_CONTROL_MESSAGE: {
    } break;
    default: break;
    }
    return true;
}

void RTMPEndpoint::Send_WindowsAcknowledgementSize(uint32_t ack_size)
{
    rtmp_proto::WindowsAcknowledgementSize was;
    was.acknowledgement_window_size = ack_size;
    SendControlMessage(&was, RTMPMessageType::WINDOW_ACKNOWLEDGEMENT_SIZE);
}

void RTMPEndpoint::Send_SetPeerBandwith(uint32_t bandwith)
{
    rtmp_proto::SetPeerBandwith spb;
    spb.d.limit_type = 2;
    spb.d.acknowledgement_window_size = bandwith;
    SendControlMessage(&spb, RTMPMessageType::SET_PEER_BANDWITH);
}

void RTMPEndpoint::ChangeChunkSizeOutput(unsigned int chunk_size)
{
    SetChunkSize scz;
    scz.chunk_size = chunk_size;
    SendControlMessage(&scz, RTMPMessageType::SET_CHUNK_SIZE);
    m_ChunkSizeSend = chunk_size;
}

void RTMPEndpoint::SendRTMPMessage(Serializable *data, uint8_t chunk_stream_id,
                                   uint32_t message_stream_id, RTMPMessageType message_type,
                                   uint64_t timestamp)
{
    auto bb = data->SerializeToBuffer();
    int bytes_sent = 0;
    ChunkStream &cs = m_ChunkStreamsOutput[chunk_stream_id];

    while (bytes_sent != bb.size()) {
        ChunkBasicHeader cbh;
        cbh.d.cs_id = chunk_stream_id;
        if (bytes_sent) {
            cbh.d.fmt = 3;
            SendSerializable(&cbh);
        } else if (cs.message_length == bb.size() && cs.message_type == message_type
                   && cs.message_stream_id == message_stream_id && timestamp >= cs.timestamp) {
            cbh.d.fmt = 2;
            ChunkMessageHeader2 header2;
            header2.timestamp_delta = timestamp - cs.timestamp;

            SendSerializable(&cbh);
            SendSerializable(&header2);
        } else if (cs.message_stream_id == message_stream_id && timestamp >= cs.timestamp) {
            cbh.d.fmt = 1;
            ChunkMessageHeader1 header1;
            header1.message_length = bb.size();
            header1.message_type_id = (uint8_t)message_type;
            header1.timestamp_delta = timestamp - cs.timestamp;

            SendSerializable(&cbh);
            SendSerializable(&header1);
        } else {
            cbh.d.fmt = 0;
            ChunkMessageHeader0 header0;
            header0.message_length = bb.size();
            header0.message_stream_id = message_stream_id;
            header0.message_type_id = (uint8_t)message_type;
            header0.timestamp = timestamp;

            SendSerializable(&cbh);
            SendSerializable(&header0);
        }

        int bytes_to_send = std::min(size_t(m_ChunkSizeSend), bb.size() - bytes_sent);
        m_LowerLevel->send_data(bb.data() + bytes_sent, bytes_to_send);
        bytes_sent += bytes_to_send;
    }
    cs.timestamp = timestamp;
    cs.message_length = bb.size();
    cs.message_type = message_type;
    cs.message_stream_id = message_stream_id;
}

void RTMPEndpoint::SendCommand(AMFValue data, uint8_t chunk_stream_id, uint32_t message_stream_id,
                               RTMPMessageType message_type)
{
    SendRTMPMessage(&data, chunk_stream_id, message_stream_id, message_type, 0);
}

void RTMPEndpoint::SendControlMessage(Serializable *data, RTMPMessageType message_type)
{
    SendRTMPMessage(data, 2, 0, message_type);
}

void RTMPEndpoint::SendUserControlMessage(Serializable *data)
{
    SendControlMessage(data, RTMPMessageType::USER_CONTROL_MESSAGE);
}
