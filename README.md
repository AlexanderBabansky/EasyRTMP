# EasyRTMP
 Lightweight RTMP client/server library

## About RTMP
This is old standard, originally developed in 2003 as a proprietary protocol
for streaming between a *Flash player* and a server. In 2012 *Adobe* released
[specification](https://www.adobe.com/devnet/rtmp.html). Protocol is designed
to stream low latency realtime data. Today, when Flash is dead,
but **RTMP is de-facto standard for media streaming for first-mile delivery** (from
streamer to server). Protocol can be used to transfer any data, but in media world
it is used to stream **FLV packets**, often **AAC** audio with **H264** video. Codec parameters
are exchanged via *ActionScript* messages. Many say that RTMP is old and does not
support new formats 4K HDR 10bit video, but actually protocol supports everything.
New formats are not supported by agreements and *FLV specification*. During writing
library I found that some software and services violates or work differently with
protocol. Facebook violates some AMF serialization. YouTube accepted streams even
without codec parameters, but warn about 65k x 65k resolution. Twitch required
correct composition_offset parameter in video packets, or video jitters, while
YouTube playbacks smoothly. XSplit sends AMF codec data not as numbers defined
in FLV specification but as text. All this stuff I commented in code. So, you
can easily stream with new formats as long as client and server have an agreement
about format exchange.

## About library
Library is splitted to 3 parts: **transport, protocol, media streaming**.

**Protocol** contains communicating logic between two peers.

**Transport** contains logic for lower data transport and layering, such as TLS.
Library contains TCP client/server Windows/Linux transport and OpenSSL TLS.

**Media streaming** contains logic of media streaming. Is compatible with the most
of streaming services and software. You can use this to build media server or client
applications. Actually RTMP is bidirectional protocol so it can be used to stream
media in both directions.

## Documentation
The most important code is documented with doxygen.

## Dependencies
* C++11 compiler (tested on MSVCv14.29, gcc 8.3.0)
* OpenSSL (optional, to support RTMPS)

## Building
Project has CMake build script

## Example server

```C
using namespace librtmp;

//start tcp server on port 1935
TCPServer tcp_server(1935);

//accept connection
auto client = tcp_server.accept();

//initialize endpoint and server session
RTMPEndpoint rtmp_endpoint(client.get());
RTMPServerSession server_session(&rtmp_endpoint);

try{
   while(true){
      //receive media message
      auto message = server_session.GetRTMPMessage();
      //get received media codec parameters and streaming key
      auto params = server_session.GetClientParameters();
      switch (message.message_type){			
         case RTMPMessageType::VIDEO:
            HandleVideo(message,params);
            break;
         case RTMPMessageType::AUDIO:
            HandleAudio(message,params);
            break;
      }
   }
}catch(...){
   //connection terminated by peer or network conditions
}
```

## Other examples
Examples are available in next products: RTMP2NDI

## Acknowledgements
* [OpenSSL](https://www.openssl.org)