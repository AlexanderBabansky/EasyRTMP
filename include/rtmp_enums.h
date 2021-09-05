#pragma once

namespace librtmp {
	enum class RTMPVideoCodec {
		SORENSON_H263 = 2,
		SCREEN_VIDEO = 3,
		ON2_VP6 = 4,
		ON2_VP6_ALPHA = 5,
		SCREEN_VIDEO2 = 6,
		AVC = 7,/**< h264*/
		HEVC,/**< not part of official specification*/
		UNDEFINED
	};

	enum class RTMPAudioCodec {
		PCM = 0,
		ADPCM = 1,
		MP3 = 2,
		PCM_LE = 3,
		NELLYMOSER_16KHz = 4,
		NELLYMOSER_8KHz = 5,
		NELLYMOSER = 6,
		G711_A_law_PCM = 7,
		G711_Mu_law_PCM = 8,
		AAC = 10,
		SPEEX = 11,
		MP3_8KHz = 14,
		OPUS,/**< not part of official specification*/
		UNDEFINED
	};
	/**
	* Media parameters
	* 
	* Parameters of media transmission, needed by decoders
	*/
	struct ClientParameters {
		std::string app;
		std::string url;
		std::string key;

		bool has_video = false;/**< Does stream have video*/
		int width = 0;/**< Video width*/
		int height = 0;/**< Video height*/
		int video_datarate = 0;/**< Video bitrate in kb/s*/
		int framerate = 0;/**< Video framerate*/
		RTMPVideoCodec video_codec = RTMPVideoCodec::AVC;/**< Video codec*/

		bool has_audio = false;/**< Does stream have audio*/
		RTMPAudioCodec audio_codec = RTMPAudioCodec::AAC;/**< Audio codec*/
		int audio_datarate = 0;/**< Audio bitrate in kb/s*/
		int samplerate = 0;/**< Audio samplerate*/
		int samplesize = 16;/**< Audio samplesize @note Matters only when uncompressed audio*/
		int channels = 0;/**< Audio channels*/
	};
}