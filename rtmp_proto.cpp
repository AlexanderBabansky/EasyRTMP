#include "rtmp_proto.h"
#include <stdexcept>

using namespace std;
using namespace rtmp_proto;

void C0::Serialize(char *&data) const { WriteDataType<uint8_t>(data, d.version); }

void C0::ParseRef(const char *&data, int &data_len)
{
    ReadDataType<uint8_t>(&d.version, data, data_len);
}

int C0::GetLength() const { return sizeof(d); }

void C1::Serialize(char *&data) const
{
    WriteDataType<uint32_t>(data, d.time);
    WriteDataType<uint32_t>(data, d.zero);
    WriteDataBytes(data, d.random_bytes, 1528);
}

void C1::ParseRef(const char *&data, int &data_len)
{
    ReadDataType<uint32_t>(&d.time, data, data_len);
    ReadDataType<uint32_t>(&d.zero, data, data_len);
    ReadDataBytes(d.random_bytes, 1528, data, data_len);
}

int C1::GetLength() const { return sizeof(d); }

void C2::Serialize(char *&data) const
{
    WriteDataType<uint32_t>(data, d.time);
    WriteDataType<uint32_t>(data, d.time2);
    WriteDataBytes(data, d.random_bytes_echo, 1528);
}

void C2::ParseRef(const char *&data, int &data_len)
{
    ReadDataType<uint32_t>(&d.time, data, data_len);
    ReadDataType<uint32_t>(&d.time2, data, data_len);
    ReadDataBytes(d.random_bytes_echo, 1528, data, data_len);
}

int C2::GetLength() const { return sizeof(d); }

void ChunkBasicHeader::Serialize(char *&data) const
{
    uint8_t w = *(reinterpret_cast<const uint8_t *>(&d));
    WriteDataType<uint8_t>(data, w);
}

void ChunkBasicHeader::ParseRef(const char *&data, int &data_len)
{
    uint8_t r;
    ReadDataType<uint8_t>(&r, data, data_len);
    *(reinterpret_cast<uint8_t *>(&d)) = r;
}

int ChunkBasicHeader::GetLength() const { return sizeof(uint8_t); }

void ChunkMessageHeader0::Serialize(char *&data) const
{
    WriteDataType<uint32_t>(data, timestamp, 3);
    WriteDataType<uint32_t>(data, message_length, 3);
    WriteDataType<uint8_t>(data, message_type_id);
    WriteDataType<uint32_t>(data, message_stream_id, sizeof(uint32_t), true);
}

void ChunkMessageHeader0::ParseRef(const char *&data, int &data_len)
{
    ReadDataType<uint32_t>(&timestamp, data, data_len, 3);
    ReadDataType<uint32_t>(&message_length, data, data_len, 3);
    ReadDataType<uint8_t>(&message_type_id, data, data_len);
    ReadDataType<uint32_t>(&message_stream_id, data, data_len, 3, true);
}

int ChunkMessageHeader0::GetLength() const { return 3 + 3 + 1 + 4; }

void ChunkMessageHeader1::Serialize(char *&data) const
{
    WriteDataType<uint32_t>(data, timestamp_delta, 3);
    WriteDataType<uint32_t>(data, message_length, 3);
    WriteDataType<uint8_t>(data, message_type_id);
}

void ChunkMessageHeader1::ParseRef(const char *&data, int &data_len)
{
    ReadDataType<uint32_t>(&timestamp_delta, data, data_len, 3);
    ReadDataType<uint32_t>(&message_length, data, data_len, 3);
    ReadDataType<uint8_t>(&message_type_id, data, data_len);
}

int ChunkMessageHeader1::GetLength() const { return 3 + 3 + 1; }

void ChunkMessageHeader2::Serialize(char *&data) const
{
    WriteDataType<uint32_t>(data, timestamp_delta, 3);
}

void ChunkMessageHeader2::ParseRef(const char *&data, int &data_len)
{
    ReadDataType<uint32_t>(&timestamp_delta, data, data_len, 3);
}

int ChunkMessageHeader2::GetLength() const { return 3; }

void SetChunkSize::Serialize(char *&data) const { WriteDataType<uint32_t>(data, chunk_size); }

void SetChunkSize::ParseRef(const char *&data, int &data_len)
{
    ReadDataType<uint32_t>(&chunk_size, data, data_len);
}

int SetChunkSize::GetLength() const { return sizeof(chunk_size); }

void AbortMessage::Serialize(char *&data) const
{
    WriteDataType<uint32_t>(data, chunk_stream_id);
}

void AbortMessage::ParseRef(const char *&data, int &data_len)
{
    ReadDataType<uint32_t>(&chunk_stream_id, data, data_len);
}

int AbortMessage::GetLength() const { return sizeof(chunk_stream_id); }

void Acknowledgement::Serialize(char *&data) const
{
    WriteDataType<uint32_t>(data, sequence_number);
}

void Acknowledgement::ParseRef(const char *&data, int &data_len)
{
    ReadDataType<uint32_t>(&sequence_number, data, data_len);
}

int Acknowledgement::GetLength() const { return sizeof(sequence_number); }

void WindowsAcknowledgementSize::Serialize(char *&data) const
{
    WriteDataType<uint32_t>(data, acknowledgement_window_size);
}

void WindowsAcknowledgementSize::ParseRef(const char *&data, int &data_len)
{
    ReadDataType<uint32_t>(&acknowledgement_window_size, data, data_len);
}

int WindowsAcknowledgementSize::GetLength() const { return sizeof(acknowledgement_window_size); }

void SetPeerBandwith::Serialize(char *&data) const
{
    WriteDataType<uint32_t>(data, d.acknowledgement_window_size);
    WriteDataType<uint8_t>(data, d.limit_type);
}

void SetPeerBandwith::ParseRef(const char *&data, int &data_len)
{
    ReadDataType<uint32_t>(&d.acknowledgement_window_size, data, data_len);
    ReadDataType<uint8_t>(&d.limit_type, data, data_len);
}

int SetPeerBandwith::GetLength() const { return sizeof(d); }

void rtmp_proto::UCM_StreamBegin::Serialize(char *&data) const
{
    WriteDataType<uint16_t>(data, type);
    WriteDataType<uint32_t>(data, stream_id, sizeof(stream_id), true);
}

void UCM_StreamBegin::ParseRef(const char *&data, int &data_len)
{
    ReadDataType<uint16_t>(&type, data, data_len);
    ReadDataType<uint32_t>(&stream_id, data, data_len, sizeof(stream_id), true);
}

int UCM_StreamBegin::GetLength() const { return sizeof(type) + sizeof(stream_id); }

void AudioPacketAAC::Serialize(char *&data) const
{
    WriteDataType<uint8_t>(data, *(uint8_t *)(&d));
    WriteDataType<uint8_t>(data, aac_packet_type);
    WriteDataBytes(data, audio_data_send.data(), audio_data_send.size());
}

void AudioPacketAAC::ParseRef(const char *&data, int &data_len)
{
    ReadDataType<uint8_t>((uint8_t *)&d, data, data_len);
    ReadDataType<uint8_t>(&aac_packet_type, data, data_len);
    audio_data_send.resize(data_len);
    ReadDataBytes(audio_data_send.data(), data_len, data, data_len);
}

int AudioPacketAAC::GetLength() const { return sizeof(uint8_t) * 2 + audio_data_send.size(); }

void VideoPacket::Serialize(char *&data) const
{
    WriteDataType<uint8_t>(data, *(uint8_t *)(&d));
    WriteDataType<uint8_t>(data, d.avc_packet_type);
    WriteDataType<uint8_t>(data, d.composition_time, 3);
    WriteDataBytes(data, video_data_send.data(), video_data_send.size());
}

void VideoPacket::ParseRef(const char *&data, int &data_len)
{
    ReadDataType<uint8_t>((uint8_t *)&d, data, data_len);
    ReadDataType<uint8_t>(&d.avc_packet_type, data, data_len);
    ReadDataType<uint32_t>(&d.composition_time, data, data_len, 3);
    video_data_send.resize(data_len);
    ReadDataBytes(video_data_send.data(), data_len, data, data_len);
}

int VideoPacket::GetLength() const { return 2 * sizeof(uint8_t) + 3 + video_data_send.size(); }
