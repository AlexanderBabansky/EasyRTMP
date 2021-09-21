#include <cassert>
#include "rtmp_client_session.h"
#include "rtmp_exception.h"

using namespace librtmp;
using namespace std;

bool RTMPClientSession::GetCommandResponse(std::function<bool(AMFValue)> clb, int cmd_id)
{
	while (true) {
		RTMPMessageType message_type = RTMPMessageType::UNKNOWN;
		uint32_t message_stream_id = 0;
		uint32_t message_length = 0;
		uint64_t timestamp = 0;
		uint8_t chunk_stream_id = 0;
		auto data = m_Endpoint->GetRTMPMessage(&message_type, &message_stream_id, &message_length, &chunk_stream_id, &timestamp);
		if (message_type == RTMPMessageType::AMF0) {
			AMFValue amf_container(AMFType::ROOT);
			const char* p = data.data();
			int ml = message_length;
			amf_container.ParseRef(p, ml);
			auto it = amf_container.begin();
			++it;
			if ((*it).get_number() == cmd_id) {				
				return clb(amf_container);
			}
		}
		else
			m_Endpoint->HandleMessage(move(data), message_type, message_stream_id, message_length, chunk_stream_id, timestamp);
	}
	return false;
}

void RTMPClientSession::SendAmfConnect(string app, string url)
{
	AMFValue amf_container(AMFType::ROOT);
	AMFValue amf_cmd_name(AMFType::STRING);
	amf_cmd_name.set_string("connect");
	AMFValue amf_transaction_id(AMFType::NUMBER);
	amf_transaction_id.set_number(1);
	AMFValue amf_object(AMFType::OBJECT);
	{
		AMFValue amf_app(AMFType::STRING);
		amf_app.set_string(app);
		AMFValue amf_flashver(AMFType::STRING);
		amf_flashver.set_string("FMLE/3.0 (compatible; FMSc/1.0)");
		AMFValue amf_type(AMFType::STRING);
		amf_type.set_string("nonprivate");
		AMFValue amf_tcurl(AMFType::STRING);
		amf_tcurl.set_string(url);

		amf_object["app"] = amf_app;
		amf_object["type"] = amf_type;
		amf_object["flashVer"] = amf_flashver;
		amf_object["swfUrl"] = amf_tcurl;
		amf_object["tcUrl"] = amf_tcurl;
	}
	amf_container.push_back(amf_cmd_name);
	amf_container.push_back(amf_transaction_id);
	amf_container.push_back(amf_object);
	m_Endpoint->SendCommand(amf_container);
	if (GetCommandResponse([&](AMFValue v) {
		auto it = v.begin();
		if ((*it).get_string() == "_result") {
			++it; ++it; ++it;
			if ((*it)["code"].get_string() == "NetConnection.Connect.Success") {
				return true;
			}
		}
		return false;
		}, 1)==false) {
			throw rtmp_exception(RTMPError::NETSTREAM_CONNECTION);
	}
}

void RTMPClientSession::SendReleaseStream(string key)
{
	AMFValue amf_container(AMFType::ROOT);
	AMFValue amf_cmd(AMFType::STRING);
	amf_cmd.set_string("releaseStream");
	AMFValue amf_id(AMFType::NUMBER);
	amf_id.set_number(2);
	AMFValue amf_null(AMFType::NUL);
	AMFValue amf_key(AMFType::STRING);
	amf_key.set_string(key);

	amf_container.push_back(amf_cmd);
	amf_container.push_back(amf_id);
	amf_container.push_back(amf_null);
	amf_container.push_back(amf_key);

	m_Endpoint->SendCommand(amf_container);
}

void RTMPClientSession::SendFCPublish(std::string key)
{
	AMFValue amf_container(AMFType::ROOT);
	AMFValue amf_cmd(AMFType::STRING);
	amf_cmd.set_string("FCPublish");
	AMFValue amf_id(AMFType::NUMBER);
	amf_id.set_number(3);
	AMFValue amf_null(AMFType::NUL);
	AMFValue amf_key(AMFType::STRING);
	amf_key.set_string(key);

	amf_container.push_back(amf_cmd);
	amf_container.push_back(amf_id);
	amf_container.push_back(amf_null);
	amf_container.push_back(amf_key);

	m_Endpoint->SendCommand(amf_container);
	/*if (GetCommandResponse([&](AMFValue v) {
		auto it = v.begin();
		if ((*it).get_string() == "onFCPublish") {
			return true;
		}
		return false;
		}, 0) == false) {
		throw rtmp_exception(RTMPError::NETSTREAM_CONNECTION);
	}*/
}

void RTMPClientSession::SendCreateStream()
{
	AMFValue amf_container(AMFType::ROOT);
	AMFValue amf_cmd(AMFType::STRING);
	amf_cmd.set_string("createStream");
	AMFValue amf_id(AMFType::NUMBER);
	amf_id.set_number(4);
	AMFValue amf_null(AMFType::NUL);

	amf_container.push_back(amf_cmd);
	amf_container.push_back(amf_id);
	amf_container.push_back(amf_null);

	m_Endpoint->SendCommand(amf_container);
	if (GetCommandResponse([&](AMFValue v) {
		auto it = v.begin();
		if ((*it).get_string() == "_result") {
			return true;
		}
		return false;
		}, 4) == false) {
		throw rtmp_exception(RTMPError::NETSTREAM_CONNECTION);
	}
}

void RTMPClientSession::SendPublish(string key)
{
	AMFValue amf_container(AMFType::ROOT);
	AMFValue amf_cmd(AMFType::STRING);
	amf_cmd.set_string("publish");
	AMFValue amf_id(AMFType::NUMBER);
	amf_id.set_number(0);
	AMFValue amf_null(AMFType::NUL);
	AMFValue amf_key(AMFType::STRING);
	amf_key.set_string(key);
	AMFValue amf_live(AMFType::STRING);
	amf_live.set_string("live");

	amf_container.push_back(amf_cmd);
	amf_container.push_back(amf_id);
	amf_container.push_back(amf_null);
	amf_container.push_back(amf_key);
	amf_container.push_back(amf_live);

	m_Endpoint->SendCommand(amf_container,4,1);
	if (GetCommandResponse([&](AMFValue v) {
		auto it = v.begin();
		if ((*it).get_string() == "onStatus") {
			++it; ++it; ++it;
			if ((*it)["code"].get_string() == "NetStream.Publish.Start") {
				return true;
			}
		}
		return false;
		}, 0) == false) {
		throw rtmp_exception(RTMPError::NETSTREAM_CONNECTION);
	}
}

void RTMPClientSession::SendDataFrameParameters(const ClientParameters* params)
{
	AMFValue amf_container(AMFType::ROOT);
	AMFValue amf_cmd(AMFType::STRING);
	amf_cmd.set_string("@setDataFrame");
	AMFValue amf_meta(AMFType::STRING);
	amf_meta.set_string("onMetaData");
	AMFValue amf_props(AMFType::ECMA_ARRAY);
	{
		amf_props["duration"] = AMFValue(AMFType::NUMBER);
		amf_props["fileSize"] = AMFValue(AMFType::NUMBER);
		if (params->has_video) {
			amf_props["width"] = AMFValue(AMFType::NUMBER);
			amf_props.at("width").set_number(params->width);
			amf_props["height"] = AMFValue(AMFType::NUMBER);
			amf_props.at("height").set_number(params->height);
			amf_props["videocodecid"] = AMFValue(AMFType::NUMBER);
			amf_props.at("videocodecid").set_number((int)params->video_codec);
			amf_props["videodatarate"] = AMFValue(AMFType::NUMBER);
			amf_props.at("videodatarate").set_number(params->video_datarate);
			amf_props["framerate"] = AMFValue(AMFType::NUMBER);
			amf_props.at("framerate").set_number(params->framerate);
		}
		if (params->has_audio) {
			amf_props["audiocodecid"] = AMFValue(AMFType::NUMBER);
			amf_props.at("audiocodecid").set_number((int)params->audio_codec);
			amf_props["audiodatarate"] = AMFValue(AMFType::NUMBER);
			amf_props.at("audiodatarate").set_number(params->audio_datarate);
			amf_props["audiosamplerate"] = AMFValue(AMFType::NUMBER);
			amf_props.at("audiosamplerate").set_number(params->samplerate);
			amf_props["audiosamplesize"] = AMFValue(AMFType::NUMBER);
			amf_props.at("audiosamplesize").set_number(params->samplesize);
			amf_props["audiochannels"] = AMFValue(AMFType::NUMBER);
			amf_props.at("audiochannels").set_number(params->channels);
			amf_props["stereo"] = AMFValue(AMFType::BOOLEAN);
			amf_props["2.1"] = AMFValue(AMFType::BOOLEAN);
			amf_props["3.1"] = AMFValue(AMFType::BOOLEAN);
			amf_props["4.0"] = AMFValue(AMFType::BOOLEAN);
			amf_props["4.1"] = AMFValue(AMFType::BOOLEAN);
			amf_props["5.1"] = AMFValue(AMFType::BOOLEAN);
			amf_props["7.1"] = AMFValue(AMFType::BOOLEAN);
			switch (params->channels) {
			case 2:
				amf_props.at("stereo").set_boolean(true);
				break;
			}
		}
		amf_props["encoder"] = AMFValue(AMFType::STRING);
		amf_props.at("encoder").set_string("FMLE");
	}
	amf_container.push_back(amf_cmd);
	amf_container.push_back(amf_meta);
	amf_container.push_back(amf_props);
	m_Endpoint->SendCommand(amf_container, 4, 1, RTMPMessageType::METADATA_AMF0);
}

RTMPClientSession::RTMPClientSession(RTMPEndpoint* endpoint):
	m_Endpoint(endpoint)
{
	assert(endpoint);	
}

void RTMPClientSession::SendRTMPMessage(RTMPMediaMessage message)
{
	assert(sent_params);//you should send client parameters first
	if (message.message_type == RTMPMessageType::AUDIO) {
		m_Endpoint->SendRTMPMessage(&message.audio, 4, message.message_stream_id,message.message_type,message.timestamp);
	}
	else if (message.message_type == RTMPMessageType::VIDEO) {
		m_Endpoint->SendRTMPMessage(&message.video, 4, message.message_stream_id, message.message_type, message.timestamp);
	}
	else assert(false);
}

void RTMPClientSession::SendClientParameters(const ClientParameters* params)
{
	//TODO: check responses
	assert(params);
	SendAmfConnect(params->app,params->url);
	SendReleaseStream(params->key);
	SendFCPublish(params->key);
	SendCreateStream();
	SendPublish(params->key);
	SendDataFrameParameters(params);
	sent_params = true;
}
