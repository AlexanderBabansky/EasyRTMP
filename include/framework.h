#pragma once
#include <vector>
#include <stdexcept>
#include <cstring>
#define DATA_BYTES std::vector<char>

#define min(a,b) a<b?a:b
template<typename T>
T _ntohs(T t) {
	uint16_t v = *(reinterpret_cast<uint16_t*>(&t));
	v = (v >> 8) | (v << 8);
	return *(reinterpret_cast<T*>(&v));
}
template <typename T>
T _ntohll(T t) {
	uint64_t v = *(reinterpret_cast<uint64_t*>(&t));
	v = ((v << 56) & 0xff00000000000000) | ((v << 40) & 0x00ff000000000000) |
		((v << 24) & 0x0000ff0000000000) | ((v << 8) & 0x000000ff00000000) |
		((v >> 8) & 0x00000000ff000000) | ((v >> 24) & 0x0000000000ff0000) |
		((v >> 40) & 0x000000000000ff00) | ((v >> 56) & 0x00000000000000ff);
	return *(reinterpret_cast<T*>(&v));
}
template <typename T>
T _ntohl(T t) {
	uint32_t v = *(reinterpret_cast<uint32_t*>(&t));
	v = ((v << 24) & 0xff000000) | ((v << 8) & 0x00ff0000) |
		((v >> 8) & 0x0000ff00) | ((v >> 24) & 0x000000ff);
	return *(reinterpret_cast<T*>(&v));
}
template<typename T>
void ReadDataType(T* out, const char*& data, int& data_len, int new_size = sizeof(T), bool little_endian = false) {
	if (data_len < new_size) {
		throw std::runtime_error(0);//TODO: better exception
	}
	memcpy((char*)(out)+(sizeof(T) - new_size), data, new_size);
	data += new_size;
	data_len -= new_size;
	bool convert_endian = false;
#ifndef HOST_BIG_ENDIAN
	if (!little_endian)convert_endian = true;
#else
	if (little_endian)convert_endian = true;
#endif // !BIG_ENDIAN
	if (convert_endian) {
		if (sizeof(T) == 2) {
			*out = _ntohs<T>(*out);
		}
		else if (sizeof(T) == 4) {
			*out = _ntohl<T>(*out);
		}
		else if (sizeof(T) == 8) {
			*out = _ntohll<T>(*out);
		}
	}
}
template<typename T>
void WriteDataType(char*& data, T v, int new_size = sizeof(T), bool little_endian = false) {
	bool convert_endian = false;
#ifndef HOST_BIG_ENDIAN
	if (!little_endian)convert_endian = true;
#else
	if (little_endian)convert_endian = true;
#endif // !BIG_ENDIAN
	if (convert_endian) {
		if (sizeof(T) == 2) {
			v = _ntohs<T>(v);
		}
		else if (sizeof(T) == 4) {
			v = _ntohl<T>(v);
		}
		else if (sizeof(T) == 8) {
			v = _ntohll<T>(v);
		}
	}
	memcpy(data, ((char*)&v) + (sizeof(T) - new_size), new_size);
	data += new_size;
}
inline void ReadDataBytes(void* out, int out_len, const char*& data, int& data_len) {
	if (data_len < out_len) {
		throw std::runtime_error(0);//TODO: better exception
	}
	memcpy(out, data, out_len);
	data += out_len;
	data_len -= out_len;
}
inline void WriteDataBytes(char*& data, const void* src, int src_len) {
	memcpy(data, src, src_len);
	data += src_len;
}


/**
* Abstract layer to transform and move data
* 
* Used to build multilayer data transfering, such as TLS, TCP, HTTP
*/
class DataLayer {
public:
	void send_data(DATA_BYTES data);
	DATA_BYTES receive_data(int len);
	/**
	* Sends data
	* @exception std::exception	Is thrown when there is error in transport layer
	*/
	virtual void send_data(const char* data, size_t data_len) = 0;
	/**
	* Receives data
	* @exception std::exception	Is thrown when there is error in transport layer
	*/
	virtual void receive_data(char* out, int len) = 0;
	virtual void destroy() = 0;
};

/**
* Abstract class for binary serializable object
*/
class Serializable {
public:
	/**
	* Serialize object to binary
	* @param[out]	data	The allocated memory to put serialized binary. Must have size not less than GetLength() method returns.
	*/
	virtual void Serialize(char*& data) const = 0;
	DATA_BYTES SerializeToBuffer() const;
	/**
	* Get binary length of object
	*/
	virtual int GetLength() const = 0;
	/**
	* Initialize object by parsing binary data
	* @param[in,out]	data		The memory to get serialized binary from. After parsing complete, data points to after the end.
	* @param[in,out]	data_len	Size of memory pointed by data. After parsing
	*								complete, data_len is decreased by used data length. If data_len is less than required, implementation must throw exception.
	* @exception std::runtime_error	Is thrown when data_len less then needed, or data is bad.
	*/
	virtual void ParseRef(const char*& data, int& data_len) = 0;
	void Parse(const char* data, int data_len);
	void Parse(DATA_BYTES data_b);
	DATA_BYTES AllocateBuffer();
};