#pragma once
#include <string>
#include <list>
#include <vector>
#include <map>
#include "framework.h"
#include "EasyRtmpDLLAPI.h"

void OffsetParser(int o, const char *&data, int &data_len);
void OffsetSerializer(int o, const char *&data);

enum class AMFType {
    STRING = 2,
    NUMBER = 0,
    BOOLEAN = 1,
    NUL = 5,
    ECMA_ARRAY = 8,
    STRICT_ARRAY = 10,
    OBJECT = 3,
    END_OF_OBJECT = 9,
    UNDEFINED = 6,

    UNKNOWN = 100,
    ROOT = 101
};

/**
* AMF container
* 
* Is a compact binary format that is used to serialize ActionScript object graphs.
* Allows two endpoints to communicate through the exchange of stronly typed data.
*/
class EASYRTMP_DLLAPI AMFValue : public Serializable
{
public:
    /*
	* AMF iterator
	*
	* Valid only for types: ECMA_ARRAY, OBJECT, ROOT
	*/
    class EASYRTMP_DLLAPI iterator
    {
    private:
        AMFType type;
        std::list<AMFValue>::iterator list_iterator;
        std::map<std::string, AMFValue>::iterator map_iterator;

    public:
        iterator(const iterator &iterator);
        iterator(AMFType t, std::list<AMFValue>::iterator it);
        iterator(AMFType t, std::map<std::string, AMFValue>::iterator it);
        iterator &operator++();
        bool operator==(const iterator &o) const;
        AMFValue operator*() const;
        ~iterator();
    };

private:
    AMFType type = AMFType::UNKNOWN;
    std::string data_string;
    double data_number = 0;
    bool data_boolean = false;
    std::list<AMFValue> data_array;
    std::map<std::string, AMFValue> data_object;

public:
    AMFValue() = default;
    AMFValue(AMFValue &&) noexcept;
    AMFValue(const AMFValue &o) = default;
    AMFValue &operator=(const AMFValue &o) = default;
    AMFValue(AMFType t);
    ~AMFValue();

    void set_string(const std::string &s);
    void set_number(double num);
    void set_boolean(bool b);
    std::string get_string();
    double get_number();
    bool get_boolean();

    AMFValue::iterator begin();
    AMFValue::iterator end();
    AMFValue &operator[](std::string idx);
    AMFValue &at(std::string idx);
    void push_back(const AMFValue &v);

    AMFType GetType();
    int GetLength() const override;
    void Serialize(char *&data) const override;
    void ParseRef(const char *&data, int &data_len) override;
};
