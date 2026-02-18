#pragma once
#include <string>
#include <iostream>

class Buffer
{
private:
    std::string buf;

public:
    Buffer();
    ~Buffer();
    void append(const char *_str, int _size);
    ssize_t size();
    const char *c_str();
    void clear();
    void getline();
};

inline Buffer::Buffer() {}
inline Buffer::~Buffer() {}

inline void Buffer::append(const char *_str, int _size)
{
    for (int i = 0; i < _size; ++i)
    {
        if (_str[i] == '\0')
            break;
        buf.push_back(_str[i]);
    }
}

inline ssize_t Buffer::size()
{
    return buf.size();
}

inline const char *Buffer::c_str()
{
    return buf.c_str();
}

inline void Buffer::clear()
{
    buf.clear();
}

inline void Buffer::getline()
{
    buf.clear();
    std::getline(std::cin, buf);
}
