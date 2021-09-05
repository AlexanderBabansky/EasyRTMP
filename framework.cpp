#include "framework.h"

using namespace std;

DATA_BYTES Serializable::SerializeToBuffer() const {
	DATA_BYTES buffer;
	buffer.resize(GetLength());
	char* d = buffer.data();
	Serialize(d);
	return buffer;
}

void Serializable::Parse(const char* data, int data_len) {
	ParseRef(data, data_len);
}

void Serializable::Parse(DATA_BYTES data_b)
{
	Parse(data_b.data(),data_b.size());
}

DATA_BYTES Serializable::AllocateBuffer()
{
	DATA_BYTES buffer;
	buffer.resize(GetLength());
	return buffer;
}

void DataLayer::send_data(DATA_BYTES data)
{
	send_data(data.data(),data.size());
}

DATA_BYTES DataLayer::receive_data(int len)
{
	DATA_BYTES b;
	b.resize(len);
	receive_data(b.data(), len);
	return b;
}
