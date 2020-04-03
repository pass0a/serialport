#ifndef __DPZ__SERIALPORT__HPP__
#define __DPZ__SERIALPORT__HPP__
#include <windows.h>
#include <vector>

struct SerialPortOption {
    std::string name;
    int baud;
    int stopBits;
    int byteSize;
    int parity;
    size_t capacity;
};
struct SPMsg {
    std::vector<BYTE> data;
    DWORD len;
};
class SerialPort {
public:
    SerialPort();
    int open(SerialPortOption opts);
    int read(SPMsg& msg);
    int write(BYTE* buf, DWORD len);
    int close();
private:
    int readMore(SPMsg& msg);
    int readUart(SPMsg& msg);
private:
    HANDLE g_hCom;
    DWORD moreBytes;
};
#endif
