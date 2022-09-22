#include <cassert>
#include "amf.h"
#include "rtmp_exception.h"

using namespace std;
using namespace librtmp;

void OffsetParser(int o, const char *&data, int &data_len)
{
    data += o;
    data_len -= o;
}

void OffsetSerializer(int o, const char *&data) { data += o; }

AMFValue::AMFValue(AMFValue &&o) noexcept
{
    type = o.type;
    switch (type) {
    case AMFType::STRING: data_string = std::move(o.data_string); break;
    case AMFType::NUMBER: data_number = o.data_number; break;
    case AMFType::BOOLEAN: data_boolean = o.data_boolean; break;
    case AMFType::ECMA_ARRAY:
    case AMFType::OBJECT: data_object = std::move(o.data_object); break;
    case AMFType::UNKNOWN: break;
    case AMFType::ROOT: data_array = std::move(o.data_array); break;
    default: break;
    }
}

AMFValue::~AMFValue() {}

AMFValue::AMFValue(AMFType t) : type(t) {}

void AMFValue::set_string(const std::string &s)
{
    assert(type == AMFType::STRING);
    data_string = s;
}

void AMFValue::set_number(double num)
{
    assert(type == AMFType::NUMBER);
    data_number = num;
}

void AMFValue::set_boolean(bool b)
{
    assert(type == AMFType::BOOLEAN);
    data_boolean = b;
}

std::string AMFValue::get_string()
{
    assert(type == AMFType::STRING);
    return data_string;
}

double AMFValue::get_number()
{
    assert(type == AMFType::NUMBER);
    return data_number;
}

bool AMFValue::get_boolean()
{
    assert(type == AMFType::BOOLEAN);
    return data_boolean;
}

AMFType AMFValue::GetType() { return type; }

int AMFValue::GetLength() const
{
    switch (type) {
    case AMFType::STRING: return data_string.size() + sizeof(uint16_t) + 1;
    case AMFType::NUMBER: return sizeof(double) + 1;
    case AMFType::BOOLEAN: return 2;
    case AMFType::NUL: return 1;
    case AMFType::ECMA_ARRAY:
    case AMFType::OBJECT: {
        int res = 1 + data_object.size() * (sizeof(uint16_t)) + 3;
        if (type == AMFType::ECMA_ARRAY)
            res += sizeof(uint32_t);
        for (auto &i : data_object) {
            res += i.first.size() + i.second.GetLength();
        }
        return res;
    }
    case AMFType::STRICT_ARRAY: throw rtmp_exception(RTMPError::AMF); break;
    case AMFType::ROOT: {
        int res = data_object.size();
        for (auto &i : data_array) {
            res += i.GetLength();
        }
        return res;
    }
    default: throw rtmp_exception(RTMPError::AMF);
    }
}

void AMFValue::Serialize(char *&data) const
{
    switch (type) {
    case AMFType::STRING: {
        *data = (char)type;
        data++;
        uint16_t str_len = data_string.size();
        WriteDataType<uint16_t>(data, str_len);
        WriteDataBytes(data, data_string.c_str(), str_len);
    } break;
    case AMFType::NUMBER: {
        *data = (char)type;
        data++;
        WriteDataType<double>(data, data_number);
    } break;
    case AMFType::BOOLEAN: {
        *data = (char)type;
        data++;
        if (data_boolean) {
            *data = 1;
        } else
            *data = 0;
        data++;
    } break;
    case AMFType::NUL: {
        *data = (char)type;
        data++;
    } break;
    case AMFType::ECMA_ARRAY:
    case AMFType::OBJECT: {
        *data = (char)type;
        data++;
        if (type == AMFType::ECMA_ARRAY) {
            uint32_t arr_len = data_object.size();
            WriteDataType<uint32_t>(data, arr_len);
        }
        for (auto &i : data_object) {
            uint16_t str_len = i.first.size();
            WriteDataType<uint16_t>(data, str_len);
            WriteDataBytes(data, i.first.c_str(), str_len);
            i.second.Serialize(data);
        }
        uint16_t str_len = 0;
        WriteDataType<uint16_t>(data, str_len);
        *data = (char)AMFType::END_OF_OBJECT;
        data++;
    } break;
    case AMFType::UNKNOWN:
        *data = (char)type;
        data++;
        break;
    case AMFType::ROOT:
        for (auto &i : data_array) {
            i.Serialize(data);
        }
        break;
    default: throw rtmp_exception(RTMPError::AMF);
    }
}

void AMFValue::ParseRef(const char *&data, int &data_len)
{
    if (type == AMFType::ROOT) {
        while (data_len > 0) {
            AMFValue am;
            am.ParseRef(data, data_len);
            data_array.push_back(am);
        }
        return;
    }
    switch (data[0]) {
    case 0: type = AMFType::NUMBER; break;
    case 1: type = AMFType::BOOLEAN; break;
    case 2: type = AMFType::STRING; break;
    case 3: type = AMFType::OBJECT; break;
    case 5: type = AMFType::NUL; break;
    case 8: type = AMFType::ECMA_ARRAY; break;
    case 10: type = AMFType::STRICT_ARRAY; break;
    case 6: type = AMFType::UNDEFINED; break;
    default: assert(false); throw rtmp_exception(RTMPError::AMF);
    }
    OffsetParser(1, data, data_len);
    switch (type) {
    case AMFType::STRING: {
        uint16_t str_len = 0;
        ReadDataType<uint16_t>(&str_len, data, data_len);
        char *str_data = new char[str_len + 1];
        str_data[str_len] = 0;
        ReadDataBytes(str_data, str_len, data, data_len);
        data_string = str_data;
        delete[] str_data;
    } break;
    case AMFType::NUMBER: {
        ReadDataType<double>(&data_number, data, data_len);
    } break;
    case AMFType::OBJECT:
    case AMFType::ECMA_ARRAY: {
        if (type == AMFType::ECMA_ARRAY) {
            uint32_t array_size = 0; //some services (Facebook) set it to 0 despite real array size
            ReadDataType<uint32_t>(&array_size, data, data_len);
        }
        while (true) {
            uint16_t str_len = 0;
            ReadDataType<uint16_t>(&str_len, data, data_len);
            if (!str_len)
                break;
            char *prop_name = new char[str_len + 1];
            prop_name[str_len] = 0;
            ReadDataBytes(prop_name, str_len, data, data_len);
            AMFValue am;
            am.ParseRef(data, data_len);
            data_object.insert(make_pair(prop_name, am));
            delete[] prop_name;
        }
        OffsetParser(1, data, data_len);
    } break;
    case AMFType::BOOLEAN: {
        unsigned char b_d = 0;
        ReadDataType<unsigned char>(&b_d, data, data_len);
        if (b_d)
            data_boolean = true;
        else
            data_boolean = false;
    } break;
    }
}

AMFValue::iterator AMFValue::begin()
{
    switch (type) {
    case AMFType::ECMA_ARRAY:
    case AMFType::OBJECT: return AMFValue::iterator(type, data_object.begin());
    case AMFType::ROOT: return AMFValue::iterator(type, data_array.begin());
    default: throw rtmp_exception(RTMPError::AMF);
    }
}

AMFValue::iterator AMFValue::end()
{
    switch (type) {
    case AMFType::ECMA_ARRAY:
    case AMFType::OBJECT: return AMFValue::iterator(type, data_object.end());
    case AMFType::ROOT: return AMFValue::iterator(type, data_array.end());
    default: throw rtmp_exception(RTMPError::AMF);
    }
}

AMFValue &AMFValue::operator[](std::string idx)
{
    switch (type) {
    case AMFType::ECMA_ARRAY:
    case AMFType::OBJECT: return data_object[idx];
    default: throw rtmp_exception(RTMPError::AMF);
    }
}

AMFValue &AMFValue::at(std::string idx)
{
    switch (type) {
    case AMFType::ECMA_ARRAY:
    case AMFType::OBJECT: return data_object.at(idx);
    default: throw rtmp_exception(RTMPError::AMF);
    }
}

void AMFValue::push_back(const AMFValue &v)
{
    switch (type) {
    case AMFType::ROOT: data_array.push_back(v); return;
    default: throw rtmp_exception(RTMPError::AMF);
    }
}

AMFValue::iterator::iterator(const iterator &o)
{
    type = o.type;
    switch (type) {
    case AMFType::ECMA_ARRAY:
    case AMFType::OBJECT: map_iterator = o.map_iterator; break;
    case AMFType::ROOT: list_iterator = o.list_iterator; break;
    default: throw rtmp_exception(RTMPError::AMF);
    }
}

AMFValue::iterator::iterator(AMFType t, std::list<AMFValue>::iterator it)
    : list_iterator(it), type(t)
{
    assert(type == AMFType::ECMA_ARRAY || type == AMFType::OBJECT || type == AMFType::ROOT);
}

AMFValue::iterator::iterator(AMFType t, std::map<std::string, AMFValue>::iterator it)
    : type(t), map_iterator(it)
{
    assert(type == AMFType::ECMA_ARRAY || type == AMFType::OBJECT || type == AMFType::ROOT);
}

AMFValue::iterator &AMFValue::iterator::operator++()
{
    switch (type) {
    case AMFType::ECMA_ARRAY:
    case AMFType::OBJECT: map_iterator++; return *this;
    case AMFType::ROOT: list_iterator++; return *this;
    default: throw rtmp_exception(RTMPError::AMF);
    }
}

AMFValue::iterator::~iterator() {}

bool AMFValue::iterator::operator==(const iterator &o) const
{
    switch (type) {
    case AMFType::ECMA_ARRAY:
    case AMFType::OBJECT: return map_iterator == o.map_iterator;
    case AMFType::ROOT: return list_iterator == o.list_iterator; ;
    default: throw rtmp_exception(RTMPError::AMF);
    }
}

AMFValue AMFValue::iterator::operator*() const
{
    switch (type) {
    case AMFType::ECMA_ARRAY:
    case AMFType::OBJECT: return (*map_iterator).second;
    case AMFType::ROOT: return (*list_iterator);
    default: throw rtmp_exception(RTMPError::AMF);
    }
}
