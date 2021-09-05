#include <cassert>
#include "rtmp_server_session.h"
#include "rtmp_exception.h"
#include "rtmp_proto.h"

using namespace librtmp;
using namespace std;

#define OPTIONAL_META(arg) try{arg;}catch(...){}

RTMPServerSession::RTMPServerSession(RTMPEndpoint* endpoint) :
	m_Endpoint(endpoint) {
	assert(endpoint);
}

RTMPMediaMessage RTMPServerSession::GetRTMPMessage() {
	RTMPMessageType message_type = RTMPMessageType::UNKNOWN;
	uint32_t message_stream_id = 0;
	uint32_t message_length = 0;
	uint64_t timestamp = 0;
	uint8_t chunk_stream_id = 0;
	while (true) {
		auto data = m_Endpoint->GetRTMPMessage(&message_type, &message_stream_id, &message_length, &chunk_stream_id, &timestamp);
		switch (message_type)
		{
		case librtmp::RTMPMessageType::AMF0:
		case librtmp::RTMPMessageType::METADATA_AMF0:
		{
			AMFValue root(AMFType::ROOT);
			root.Parse(data.data(), message_length);
			HandleAMF(move(root));
		}
		break;
		case librtmp::RTMPMessageType::AUDIO:
		{
			RTMPMediaMessage mm;
			mm.audio.Parse(data.data(), message_length);
			mm.message_type = message_type;
			mm.message_stream_id = message_stream_id;
			mm.timestamp = timestamp;
			return mm;
		}
		break;
		case librtmp::RTMPMessageType::VIDEO:
		{
			RTMPMediaMessage mm;
			mm.video.Parse(data.data(), message_length);
			mm.message_type = message_type;
			mm.message_stream_id = message_stream_id;
			mm.timestamp = timestamp;
			return mm;
		}
		break;
		default:
			m_Endpoint->HandleMessage(data, message_type, message_stream_id, message_length, chunk_stream_id, timestamp);
			break;
		}
	}
}

const ClientParameters* RTMPServerSession::GetClientParameters() {
	return &m_ClientParameters;
}

void RTMPServerSession::HandleAMF(AMFValue data) {
	auto it = data.begin();
	string cmd_name = (*it).get_string();
	++it;
	int cmd_id = 0;
	if ((*it).GetType() == AMFType::NUMBER) {
		cmd_id = (*it).get_number();
		++it;
	}
	list<AMFValue> amf_result;
	if (cmd_name == "connect") {
		auto con_obj = *it;
		m_ClientParameters.app = con_obj.at("app").get_string();//use app name for logic
		m_Endpoint->Send_WindowsAcknowledgementSize(2500000);
		m_Endpoint->Send_SetPeerBandwith(10000000);
		rtmp_proto::UCM_StreamBegin ucm_streamBegin;
		ucm_streamBegin.stream_id = 0;
		m_Endpoint->SendUserControlMessage(&ucm_streamBegin);

		AMFValue obj1(AMFType::OBJECT);
		{
			//dummy properties, has no sense
			AMFValue fmsVer(AMFType::STRING);
			fmsVer.set_string("FMS/3,5,3,824");
			AMFValue capabs(AMFType::NUMBER);
			capabs.set_number(127);
			AMFValue mode(AMFType::NUMBER);
			mode.set_number(1);
			obj1["fmsVer"] = fmsVer;
			obj1["capabilities"] = capabs;
			obj1["mode"] = mode;
		}
		AMFValue obj2(AMFType::OBJECT);
		{
			AMFValue level(AMFType::STRING);
			level.set_string("status");
			AMFValue code(AMFType::STRING);
			code.set_string("NetConnection.Connect.Success");
			AMFValue description(AMFType::STRING);
			description.set_string("Connection succeeded.");
			AMFValue objectEncoding(AMFType::NUMBER);
			objectEncoding.set_number(0);
			AMFValue data(AMFType::ECMA_ARRAY);
			{
				AMFValue version(AMFType::STRING);
				version.set_string("3,5,3,824");
				data["version"] = version;
			}
			obj2["level"] = level;
			obj2["code"] = code;
			obj2["description"] = description;
			obj2["objectEncoding"] = objectEncoding;
			obj2["data"] = data;
		}
		amf_result.push_back(obj1);
		amf_result.push_back(obj2);
		SendAMFResult(move(amf_result), cmd_id);
	}
	else if (cmd_name == "createStream") {
		AMFValue nll(AMFType::NUL);
		AMFValue num(AMFType::NUMBER);
		num.set_number(1);
		amf_result.push_back(nll);
		amf_result.push_back(num);
		SendAMFResult(move(amf_result), cmd_id);
	}
	else if (cmd_name == "publish") {
		++it;
		m_ClientParameters.key = (*it).get_string();
		AMFValue amf_container(AMFType::ROOT);
		AMFValue amf_cmd(AMFType::STRING);
		amf_cmd.set_string("onStatus");
		AMFValue amf_id(AMFType::NUMBER);
		amf_id.set_number(cmd_id);
		AMFValue nl(AMFType::NUL);
		AMFValue obj1(AMFType::OBJECT);
		{
			AMFValue level(AMFType::STRING);
			level.set_string("status");
			AMFValue code(AMFType::STRING);
			code.set_string("NetStream.Publish.Start");
			obj1["level"] = level;
			obj1["code"] = code;
		}
		amf_container.push_back(amf_cmd);
		amf_container.push_back(amf_id);
		amf_container.push_back(nl);
		amf_container.push_back(obj1);
		m_Endpoint->SendCommand(amf_container, 3, 1);
	}
	else if (cmd_name == "@setDataFrame") {
		++it;
		auto obj = *it;
		try {
			m_ClientParameters.width = obj.at("width").get_number();
			m_ClientParameters.height = obj.at("height").get_number();
			//some software (XSplit) set this field as TEXT, not a NUMBER
			//this violates official specification, but nobody cares
			if (obj.at("videocodecid").GetType() == AMFType::NUMBER) {
				m_ClientParameters.video_codec = (RTMPVideoCodec)obj.at("videocodecid").get_number();
			}
			else if (obj.at("videocodecid").GetType() == AMFType::STRING) {
				if (obj.at("videocodecid").get_string() == "avc1") {
					m_ClientParameters.video_codec = RTMPVideoCodec::AVC;
				}
				else if (obj.at("videocodecid").get_string() == "hevc") {
					m_ClientParameters.video_codec = RTMPVideoCodec::HEVC;
				}
				else {
					//assume that streamer uses AVC
					m_ClientParameters.video_codec = RTMPVideoCodec::AVC;
				}
			}
			OPTIONAL_META(m_ClientParameters.video_datarate = obj.at("videodatarate").get_number());
			OPTIONAL_META(m_ClientParameters.framerate = obj.at("framerate").get_number());
			m_ClientParameters.has_video = true;
		}
		catch (exception& e) { m_ClientParameters.has_video = false; }

		try {
			//the same as for video
			if (obj.at("audiocodecid").GetType() == AMFType::NUMBER) {
				m_ClientParameters.audio_codec = (RTMPAudioCodec)obj.at("audiocodecid").get_number();
			}
			else if (obj.at("audiocodecid").GetType() == AMFType::STRING) {
				if (obj.at("audiocodecid").get_string() == "mp4a") {
					m_ClientParameters.audio_codec = RTMPAudioCodec::AAC;
				}
				else if (obj.at("audiocodecid").get_string() == "opus") {
					m_ClientParameters.audio_codec = RTMPAudioCodec::OPUS;
				}
				else {
					//assume that streamer uses AAC
					m_ClientParameters.audio_codec = RTMPAudioCodec::AAC;
				}
			}
			bool stereo = false;
			m_ClientParameters.samplerate = obj.at("audiosamplerate").get_number();
			OPTIONAL_META(stereo = obj.at("stereo").get_boolean());
			if (stereo) {
				m_ClientParameters.channels = 2;
			}
			OPTIONAL_META(m_ClientParameters.channels = obj.at("audiochannels").get_number());
			OPTIONAL_META(m_ClientParameters.audio_datarate = obj.at("audiodatarate").get_number());
			OPTIONAL_META(m_ClientParameters.samplesize = obj.at("audiosamplesize").get_number());
			if (!m_ClientParameters.channels)m_ClientParameters.channels = 2;
			m_ClientParameters.has_audio = true;
		}
		catch (exception& e) { m_ClientParameters.has_audio = false; }
	}
}

void RTMPServerSession::SendAMFResult(std::list<AMFValue> data, int id) {
	AMFValue amf_container(AMFType::ROOT);
	AMFValue amf_cmd(AMFType::STRING);
	amf_cmd.set_string("_result");
	AMFValue amf_id(AMFType::NUMBER);
	amf_id.set_number(id);
	amf_container.push_back(amf_cmd);
	amf_container.push_back(amf_id);
	for (auto& i : data) {
		amf_container.push_back(i);
	}
	m_Endpoint->SendCommand(amf_container);
}