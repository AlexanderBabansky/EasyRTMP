/** @file */ 

/**
 * @mainpage
 *
 * @section About
 * EasyRTMP is a modern lightweight library for RTMP communication. Library contains
 * support for Linux/Windows TCP networking, OpenSSL TLS to support rtmps, Windows
 * certificate loading module for TLS validation. Contains media server module for receiving streams.
 * 
 * @section Server_setup
 * First of all, we need transport protocol. Library has TCP Windows/Linux network transport module TCPNetwork, but you can write your own. To get TCPNetwork, you need a factory for server or client part. Server factory is implemented in TCPServer. To start server on 1935 port and accept connection:
 * 
 * \code{.cpp}
 * TCPServer server(1935);
 * auto connection = server.accept();
 * \endcode
 * 
 * To begin RTMP communication you need librtmp::RTMPEndpoint class.
 * 
 * \code{.cpp}
 * librtmp::RTMPEndpoint endpoint(connection.get());
 * \endcode
 * 
 * Now RTMP connection is ready. To receive stream as server, you can use librtmp::RTMPServerSession class.
 * 
 * \code{.cpp}
 * librtmp::RTMPServerSession server_session(&endpoint);
 * try{
 *	while (true){
 *		librtmp::RTMPMediaMessage message = server_session.GetRTMPMessage();
 *		if (message.message_type == librtmp::RTMPMessageType::AUDIO){
 *			HandleAudio(message.audio);
 *		}else if (message.message_type == librtmp::RTMPMessageType::VIDEO){
 *			HandleVideo(message.video);
 *		}
 *	}
 * }catch(...){
 *	//connection terminated by peer or network conditions
 * }
 * \endcode
 */