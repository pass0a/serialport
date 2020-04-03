#include "serialport.h"
#include <iostream>

SerialPort::SerialPort() :moreBytes(0), g_hCom(nullptr)
{
}

int SerialPort::open(SerialPortOption opts)
{
    g_hCom = CreateFile(opts.name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
    if (g_hCom == INVALID_HANDLE_VALUE)
    {
        g_hCom = nullptr;
        char msg[100];
        sprintf(msg, "open serialport error: %d\r\n", GetLastError());
        return false;
    }

    //���ö���ʱ
    COMMTIMEOUTS timeouts;
    GetCommTimeouts(g_hCom, &timeouts);
    timeouts.ReadIntervalTimeout = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 60000;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    SetCommTimeouts(g_hCom, &timeouts);

    //���ö�д��������С
    static const int g_nZhenMax = 32768;
    if (!SetupComm(g_hCom, g_nZhenMax, g_nZhenMax))
    {
        std::cout << "SetupComm() failed" << std::endl;
        CloseHandle(g_hCom);
        return false;
    }

    //���ô���������Ϣ
    DCB dcb;
    if (!GetCommState(g_hCom, &dcb))
    {
        std::cout << "GetCommState() failed" << std::endl;
        CloseHandle(g_hCom);
        return false;
    }

    dcb.DCBlength = sizeof(DCB);
    dcb.BaudRate = opts.baud;//������Ϊ115200
    dcb.Parity = opts.parity;//У�鷽ʽΪ��У��
    dcb.ByteSize = opts.byteSize;//����λΪ8λ
    dcb.StopBits = opts.stopBits;//ֹͣλΪ1λ

    if (!SetCommState(g_hCom, &dcb))
    {
        std::cout << "SetCommState() failed" << std::endl;
        CloseHandle(g_hCom);
        return false;
    }

    //��ջ���
    PurgeComm(g_hCom, PURGE_RXCLEAR | PURGE_TXCLEAR);

    //�������
    DWORD dwError;
    COMSTAT cs;
    if (!ClearCommError(g_hCom, &dwError, &cs))
    {
        std::cout << "ClearCommError() failed" << std::endl;
        CloseHandle(g_hCom);
        return false;
    }

    //���ô��ڼ����¼�
    SetCommMask(g_hCom, EV_RXCHAR);

    /*HANDLE hThread1 = CreateThread(NULL, 0, ThreadSendMsg, NULL, 0, NULL);
    CloseHandle(hThread1);*/
    return 0;
}

int SerialPort::read(SPMsg& msg)
{
    if (moreBytes)
        return readMore(msg);
    else
        return readUart(msg);
}
int SerialPort::write(BYTE* buf, DWORD len)
{
    OVERLAPPED ov = { 0 };
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    DWORD bytesWritten;
    if (!WriteFile((HANDLE)g_hCom, buf, len, &bytesWritten, &ov)) {
        DWORD lastError = GetLastError();
        if (lastError != ERROR_IO_PENDING) {
            return -1;
        }

        if (WaitForSingleObject(ov.hEvent, 1000) != WAIT_OBJECT_0) {
            DWORD lastError = GetLastError();
            return -2;
        }

        if (!GetOverlappedResult((HANDLE)g_hCom, &ov, &bytesWritten, TRUE)) {
            return -3;
        }
    }
    return 0;
}

int SerialPort::close()
{
    if (g_hCom) {
        CloseHandle(g_hCom);
        g_hCom = nullptr;
    }
    return 0;
}

int SerialPort::readMore(SPMsg& msg)
{
    OVERLAPPED osRead;
    memset(&osRead, 0, sizeof(OVERLAPPED));
    osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    DWORD len = 0;
    if (moreBytes > msg.data.capacity()) {
        len = msg.data.capacity();
        moreBytes = moreBytes - msg.data.capacity();
    }
    else {
        len = moreBytes;
        moreBytes = 0;
    }
    BOOL bReadStatus = ReadFile(g_hCom, msg.data.data(), len, &msg.len, &osRead);
    if (!bReadStatus)
    {
        if (GetLastError() == ERROR_IO_PENDING)//�ص��������ڽ���
        {
            //GetOverlappedResult(g_hCom,&osRead2,&dwTrans,true);�ж��ص������Ƿ����

            //To do
        }
    }
    else//���������
    {
        //To do
    }
    return 0;
}

int SerialPort::readUart(SPMsg& msg)
{
    OVERLAPPED osWait;
    memset(&osWait, 0, sizeof(OVERLAPPED));
    osWait.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    DWORD dwEvtMask;

    if (!WaitCommEvent(g_hCom, &dwEvtMask, &osWait))
    {
        if (GetLastError() == ERROR_IO_PENDING)
        {
            WaitForSingleObject(osWait.hEvent, INFINITE);
        }
        else {
            return -2;
        }
    }
    if (dwEvtMask & EV_RXCHAR)
    {
        DWORD dwError;
        COMSTAT cs;
        if (!ClearCommError(g_hCom, &dwError, &cs))
        {
            std::cout << "ClearCommError() failed" << std::endl;
            CloseHandle(g_hCom);
            return -3;
        }

        OVERLAPPED osRead;
        memset(&osRead, 0, sizeof(OVERLAPPED));
        osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        DWORD len = 0;
        if (cs.cbInQue > msg.data.capacity()) {
            len = msg.data.capacity();
            moreBytes = cs.cbInQue - msg.data.capacity();
        }
        else {
            len = cs.cbInQue;
        }
        BOOL bReadStatus = ReadFile(g_hCom, msg.data.data(), len, &msg.len, &osRead);
        if (!bReadStatus)
        {
            if (GetLastError() == ERROR_IO_PENDING)//�ص��������ڽ���
            {
                //GetOverlappedResult(g_hCom,&osRead2,&dwTrans,true);�ж��ص������Ƿ����

                //To do
            }
        }
        else//���������
        {
            //To do
        }
        return 0;
    }
    return -1;
}