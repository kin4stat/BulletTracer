#pragma once
#include <windows.h>
#include <string>
#pragma comment (lib, "advapi32")

/* 
    Взято отсюда: https://www.blast.hk/threads/13855/

    Спасибо SR_Team!
*/

class CLiteRegedit
{
public:
    CLiteRegedit(HKEY hKey, LPCSTR lpSubKey);
    ~CLiteRegedit();

    bool GetRegister(LPCSTR lpName, DWORD dwType, void* outBuf, DWORD maxSize = 0);
    bool SetRegister(LPCSTR lpName, DWORD dwType, void* inBuf, DWORD size = 0);

    bool GetRegisterDefault(LPSTR outBuf, LONG maxSize);
    bool SetRegisterDefault(LPCSTR inBuf);

    bool DeleteRegister(LPCSTR lpName);

private:
    HKEY _hKey;

    bool AutoSizeWrite(DWORD dwType, void* inBuf, DWORD& size);
    bool AutoSizeRead(DWORD dwType, void* outBuf, DWORD& size);
};

class CLiteRegeditEasy : public CLiteRegedit
{
public:
    CLiteRegeditEasy(HKEY hKey, LPCSTR lpSubKey);
    void WriteString(LPCSTR lpName, LPCSTR lpString, ...);
    std::string ReadString(LPCSTR lpName);

    void WriteInteger(LPCSTR lpName, int value);
    int ReadInteger(LPCSTR lpName);

    void WriteFloat(LPCSTR lpName, float value);
    float ReadFloat(LPCSTR lpName);

    void WriteLongLong(LPCSTR lpName, long long value);
    long long ReadLongLong(LPCSTR lpName);

    void WriteDouble(LPCSTR lpName, double value);
    double ReadDouble(LPCSTR lpName);

    bool IsError();

private:
    bool no_error;
};