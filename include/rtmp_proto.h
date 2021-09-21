#pragma once
#include <stdint.h>
#include "framework.h"

namespace rtmp_proto {

#pragma pack(push,1)
	struct C0 : public Serializable {
		struct {
			uint8_t version = 3;
		}d;
		void Serialize(char*& data) const override;
		void ParseRef(const char*& data, int& data_len) override;
		int GetLength() const override;
	};

	struct C1 :public Serializable {
		struct {
			uint32_t time = 0;
			uint32_t zero = 0;
			uint8_t random_bytes[1528] = { 177,123,229,159,235,197,239,83,203,12,152,1,207,36,119,50,56,249,114,113,191,217,169,189,60,229,195,224,127,237,131,61,83,49,108,155,129,240,46,154,100,170,190,223,83,149,1,9,118,254,246,6,164,53,129,181,19,180,148,19,157,67,245,205,137,227,57,4,136,188,80,152,255,182,248,22,231,192,165,21,194,99,214,239,188,221,192,214,136,2,51,175,137,87,4,165,182,246,12,55,152,70,34,74,160,71,50,115,135,204,12,148,227,25,121,172,2,94,139,100,156,112,153,141,150,116,81,38,131,11,162,183,191,45,149,125,137,42,61,224,202,111,188,57,71,94,50,220,237,238,180,88,90,234,64,18,50,4,7,19,90,53,242,6,110,162,123,236,125,255,187,58,131,25,72,25,48,172,69,96,221,190,123,69,251,251,97,209,59,121,101,127,30,79,184,116,109,207,46,151,95,34,87,253,19,196,242,233,63,74,96,62,64,219,95,17,93,199,23,55,191,181,182,32,24,96,153,31,101,40,142,210,128,113,10,161,162,166,41,100,149,26,148,128,52,182,223,151,40,154,46,191,216,25,241,101,178,236,190,18,63,123,93,127,51,181,160,54,50,241,179,104,156,149,116,246,82,213,124,174,172,157,174,213,56,119,26,228,88,209,15,83,7,161,6,21,230,230,149,171,212,32,213,83,191,233,197,54,207,16,200,239,144,191,246,207,54,45,118,171,111,248,247,88,91,232,55,153,119,202,110,148,102,161,191,53,76,128,164,188,182,13,235,146,237,124,247,227,145,74,35,28,74,45,117,170,155,63,12,57,235,7,245,1,205,89,202,108,22,47,121,211,250,207,239,240,248,211,216,7,140,238,59,161,228,197,18,235,164,246,168,57,81,234,160,93,4,10,0,97,89,6,154,39,180,136,236,31,35,229,130,146,77,148,255,47,155,145,168,105,176,148,223,12,32,190,117,18,233,59,212,54,60,27,40,158,88,59,8,7,48,223,80,23,75,25,27,73,108,175,138,51,27,166,215,225,27,66,84,106,142,188,111,165,7,222,9,31,94,152,182,85,170,155,26,143,154,103,136,16,215,81,251,66,52,183,184,222,242,218,150,94,70,76,206,154,137,68,213,250,16,249,22,112,91,191,175,188,95,155,35,195,2,175,21,91,228,229,72,247,79,238,141,61,203,3,199,94,11,93,59,57,188,192,208,9,231,205,171,89,238,95,188,4,78,24,230,233,131,107,154,64,103,227,28,102,167,178,219,90,18,178,164,119,74,180,123,121,81,253,10,112,195,45,167,192,0,200,47,114,160,109,5,239,38,203,232,253,221,174,233,161,51,253,118,235,221,75,42,225,199,201,40,15,251,16,126,167,181,14,218,186,4,112,80,130,185,116,195,219,120,165,135,125,217,213,173,112,99,114,27,131,246,156,101,181,88,78,58,47,77,216,24,7,55,21,198,153,33,248,145,79,160,134,157,186,223,22,241,229,141,245,191,210,147,241,124,209,237,8,116,216,47,170,51,252,234,149,88,194,111,62,156,194,31,112,203,95,131,8,86,146,130,252,100,0,238,63,127,35,53,141,240,54,91,106,240,175,238,149,183,235,219,1,139,48,110,154,65,118,124,42,77,132,65,105,3,120,45,9,172,95,78,162,49,101,67,186,227,214,146,254,52,139,64,158,33,144,234,229,25,225,85,229,248,179,85,121,234,173,254,40,187,209,219,58,84,97,149,167,31,252,194,90,227,123,233,124,246,196,132,107,67,10,59,35,12,36,26,222,110,23,49,224,182,153,38,131,170,34,29,204,230,210,235,212,93,77,195,118,68,102,156,175,186,110,180,134,12,9,115,93,209,223,14,57,152,101,133,16,21,139,68,215,101,230,97,143,55,75,65,106,80,251,27,168,25,89,77,238,83,197,118,59,127,0,33,254,37,187,173,39,129,86,102,45,82,116,204,46,240,50,60,228,131,249,86,3,241,121,49,119,2,51,239,165,78,153,147,83,195,253,250,35,59,13,199,215,230,141,115,24,36,32,205,247,147,253,217,151,14,31,104,253,100,236,3,158,173,220,241,112,194,69,83,18,193,94,38,162,209,84,224,13,204,240,117,147,219,208,193,29,183,248,234,144,124,79,223,7,177,9,28,216,14,172,131,62,62,185,103,20,165,201,24,253,72,11,11,87,186,239,200,254,110,237,29,0,100,90,69,127,191,137,50,135,54,212,92,106,103,214,63,184,228,149,185,246,145,165,61,155,102,49,180,166,131,51,216,222,77,46,223,79,209,141,80,209,35,203,59,49,89,153,186,166,145,101,223,230,192,3,8,202,85,247,60,35,121,11,174,66,119,203,28,171,162,254,18,115,25,173,16,76,226,105,196,239,223,49,104,114,21,80,33,137,102,19,99,110,87,48,101,188,10,81,200,204,173,65,106,107,132,54,195,235,114,204,191,152,217,171,80,111,222,17,197,118,73,63,221,197,227,232,40,197,34,192,94,94,224,209,137,22,106,170,84,126,24,199,210,23,154,153,170,131,124,219,65,99,128,148,160,65,132,124,176,167,216,212,238,97,107,135,131,87,78,196,245,28,53,120,245,56,40,241,244,94,224,85,139,80,196,45,24,24,189,132,185,221,241,75,118,228,195,213,86,241,147,153,129,130,183,121,15,54,221,80,252,1,186,65,158,77,153,89,74,232,17,229,22,147,43,94,136,121,90,59,13,15,11,168,253,40,146,195,91,167,110,239,71,13,53,173,3,153,12,189,144,86,62,173,43,4,96,21,149,55,176,4,132,115,15,115,225,148,118,250,29,183,81,67,81,244,179,91,16,160,41,238,247,31,46,56,11,218,199,91,77,95,8,208,86,31,8,103,120,200,168,206,148,88,153,126,85,41,158,217,35,168,203,229,83,3,214,11,117,234,32,105,194,206,78,205,56,228,182,232,38,108,216,82,192,30,227,72,40,42,89,141,111,63,48,0,116,14,7,35,4,248,221,178,118,136,112,6,154,92,223,212,35,244,221,100,188,64,80,80,7,233,49,125,140,126,117,115,39,101,185,229,146,61,252,195,176,197,4,140,125,43,175,161,73,151,199,76,164,43,168,246,177,24,160,184,62,9,16,0,33,0,214,199,247,163,34,59,139,218,191,114,159,188,113,8,234,135,241,23,237,102,133,182,109,233,113,235,251,58,253,43,159,122,19,11,208,218,179,197,228,19,186,3,42,226,117,41,60,67,227,169,178,241,14,92,181,211,100,35,93,10,86,47,145,4,134,102,207,228,163,71,87,34,251,40,48,235,17,83,250,9,8,203,12,9,17,7,106,216,196,0,64,37,40,68,33,192,27,88,122,120,101,47,234,106,73,145,57,193,122,177,86,66,144,158,179,77,186,121,126,73,42,147,250,222,69,220,102,219,242,247,119,141,194,166,53,124,226,214 };
		}d;

		void Serialize(char*& data) const override;
		void ParseRef(const char*& data, int& data_len) override;
		int GetLength() const override;
	};

	struct C2 : public Serializable {
		struct {
			uint32_t time = 0;
			uint32_t time2 = 0;
			uint8_t random_bytes_echo[1528];
		}d;

		void Serialize(char*& data) const override;
		void ParseRef(const char*& data, int& data_len) override;
		int GetLength() const override;
	};

	struct ChunkBasicHeader : public Serializable {
		struct {
			uint8_t	cs_id : 6;
			uint8_t fmt : 2;
		}d;

		void Serialize(char*& data) const override;
		void ParseRef(const char*& data, int& data_len) override;
		int GetLength() const override;
	};
#pragma pack(pop)

	struct ChunkMessageHeader0 : Serializable {
		uint32_t timestamp = 0;//3b
		uint32_t message_length = 0;//3b
		uint8_t message_type_id = 0;
		uint32_t message_stream_id = 0;//le

		void Serialize(char*& data) const override;
		void ParseRef(const char*& data, int& data_len) override;
		int GetLength() const override;
	};

	struct ChunkMessageHeader1 : Serializable {
		uint32_t timestamp_delta = 0;//3b
		uint32_t message_length = 0;//3b
		uint8_t message_type_id = 0;

		void Serialize(char*& data) const override;
		void ParseRef(const char*& data, int& data_len) override;
		int GetLength() const override;
	};

	struct ChunkMessageHeader2 : Serializable {
		uint32_t timestamp_delta = 0;//3b

		void Serialize(char*& data) const override;
		void ParseRef(const char*& data, int& data_len) override;
		int GetLength() const override;
	};
	struct SetChunkSize : Serializable {
		uint32_t chunk_size = 0;//31 bits
		void Serialize(char*& data) const override;
		void ParseRef(const char*& data, int& data_len) override;
		int GetLength() const override;
	};
	struct AbortMessage : Serializable {
		uint32_t chunk_stream_id = 0;
		void Serialize(char*& data) const override;
		void ParseRef(const char*& data, int& data_len) override;
		int GetLength() const override;
	};
	struct Acknowledgement : Serializable {
		uint32_t sequence_number = 0;
		void Serialize(char*& data) const override;
		void ParseRef(const char*& data, int& data_len) override;
		int GetLength() const override;
	};
	struct WindowsAcknowledgementSize : Serializable {
		uint32_t acknowledgement_window_size = 0;

		void Serialize(char*& data) const override;
		void ParseRef(const char*& data, int& data_len) override;
		int GetLength() const override;
	};
	struct SetPeerBandwith : Serializable {
		struct {
			uint32_t acknowledgement_window_size = 0;
			uint8_t limit_type = 0;
		}d;

		void Serialize(char*& data) const override;
		void ParseRef(const char*& data, int& data_len) override;
		int GetLength() const override;
	};
	struct UCM_StreamBegin : Serializable {
		uint16_t type = 0;
		uint32_t stream_id = 0;

		void Serialize(char*& data) const override;
		void ParseRef(const char*& data, int& data_len) override;
		int GetLength() const override;
	};
	
	/**
	* Audio packet. Contains audio parameters, that is not useful, because all
	* parameters are transfered via AMF commands
	*/
	struct AudioPacketAAC : Serializable {
#pragma pack(push,1)
		struct {
			uint8_t channels : 1;
			uint8_t sample_size : 1;
			uint8_t sample_rate : 2;
			uint8_t format : 4;
		}d;
#pragma pack(pop)
		uint8_t aac_packet_type = 0;/**< Packet type. Is only valid for compressed audio. If 0 - packets contains headers to initialize decoder, if 1 - audio data*/
		DATA_BYTES audio_data_send;/**< Audio payload*/

		void Serialize(char*& data) const override;
		void ParseRef(const char*& data, int& data_len) override;
		int GetLength() const override;
	};
	/**
	* Video packet
	*/
	struct VideoPacket : Serializable {
#pragma pack(push,1)
		struct {
			uint8_t codec_id : 4;/**< Is not useful, because codec is transfered via AMF commands*/
			uint8_t frame_type : 4;/**< Frame type. 1=key frame, 2=interframe. Others values are described in FLV specification*/
			uint8_t avc_packet_type = 0;/**< Packet type. If 0 - packets contains headers to initialize decoder, if 1 - video data (NALUs)*/
			//3b
			uint32_t composition_time = 0;/**< Offset between decoding (DTS) and presentation
				timestamp (PTS). In streams without B frames usually is 0.
				PTS = DTS + composition_time, where DTS is RTMP packet timestamp*/
		}d;
#pragma pack(pop)
		DATA_BYTES video_data_send;/**< Video payload*/

		void Serialize(char*& data) const override;
		void ParseRef(const char*& data, int& data_len) override;
		int GetLength() const override;
	};

}