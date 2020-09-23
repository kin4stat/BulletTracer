#include "CLiteRegedit.h"
#include <string>
#include <stdio.h>

CLiteRegedit::CLiteRegedit(HKEY hKey, LPCSTR lpSubKey)
{
    _hKey = 0;
    RegCreateKeyExA(hKey, lpSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &_hKey, NULL);
}
CLiteRegedit::~CLiteRegedit()
{
    if (_hKey != NULL)
        RegCloseKey(_hKey);
}

bool CLiteRegedit::GetRegister(LPCSTR lpName, DWORD dwType, void* outBuf, DWORD maxSize)
{
    if (outBuf == nullptr || _hKey == NULL)
        return false;
    if (!strlen(lpName))
        return false;
    if (!AutoSizeRead(dwType, outBuf, maxSize))
        return false;

    return !RegQueryValueExA(_hKey, lpName, NULL, &dwType, (byte*)outBuf, &maxSize);
}
bool CLiteRegedit::SetRegister(LPCSTR lpName, DWORD dwType, void* inBuf, DWORD size)
{
    if (inBuf == nullptr || _hKey == NULL)
        return false;
    if (!strlen(lpName))
        return false;
    if (!AutoSizeWrite(dwType, inBuf, size))
        return false;

    return !RegSetValueExA(_hKey, lpName, 0, dwType, (byte*)inBuf, size);
}

bool CLiteRegedit::GetRegisterDefault(LPSTR outBuf, LONG maxSize)
{
    if (outBuf == nullptr || _hKey == NULL)
        return false;

    return !RegQueryValueA(_hKey, NULL, (LPSTR)outBuf, &maxSize);
}
bool CLiteRegedit::SetRegisterDefault(LPCSTR inBuf)
{
    if (inBuf == nullptr || _hKey == NULL)
        return false;

    return !RegSetValueA(_hKey, NULL, REG_SZ, inBuf, strlen(inBuf));
}

bool CLiteRegedit::DeleteRegister(LPCSTR lpName)
{
    if (_hKey == NULL)
        return false;
    if (!strlen(lpName))
        return false;

    return !RegDeleteKeyValueA(_hKey, NULL, lpName);
}
bool CLiteRegedit::AutoSizeWrite(DWORD dwType, void* inBuf, DWORD& size)
{
    if (!size) {
        switch (dwType)
        {
        case REG_BINARY:
            size = 1;
            break;
        case REG_DWORD:
        case REG_DWORD_BIG_ENDIAN:
            size = 4;
            break;
        case REG_QWORD:
            size = 8;
            break;
        case REG_EXPAND_SZ:
        case REG_LINK:
        case REG_MULTI_SZ:
        case REG_SZ:
            size = strlen((LPCSTR)inBuf);
            break;
        case REG_NONE:
        default:
            return false;
        }
    }
    return true;
}
bool CLiteRegedit::AutoSizeRead(DWORD dwType, void* outBuf, DWORD& size)
{
    if (!size) {
        switch (dwType)
        {
        case REG_BINARY:
            size = 1;
            break;
        case REG_DWORD:
        case REG_DWORD_BIG_ENDIAN:
            size = 4;
            break;
        case REG_QWORD:
            size = 8;
            break;
        case REG_EXPAND_SZ:
        case REG_LINK:
        case REG_MULTI_SZ:
        case REG_SZ:
        case REG_NONE:
        default:
            return false;
        }
    }
    return true;
}



CLiteRegeditEasy::CLiteRegeditEasy(HKEY hKey, LPCSTR lpSubKey) : CLiteRegedit(hKey, lpSubKey)
{
    no_error = true;
}

void CLiteRegeditEasy::WriteString(LPCSTR lpName, LPCSTR lpString, ...)
{
    va_list ap;
    size_t szArray = strlen(lpString) * 2 + 1024;
    char* szStr = new char[szArray];
    va_start(ap, lpString);
    vsprintf_s(szStr, szArray, lpString, ap);
    va_end(ap);

    no_error = SetRegister(lpName, REG_SZ, szStr, strlen(szStr));
    delete[] szStr;
}
std::string CLiteRegeditEasy::ReadString(LPCSTR lpName)
{
    char szStr[0x1000];
    no_error = GetRegister(lpName, REG_SZ, szStr, 0x1000);
    return szStr;
}

void CLiteRegeditEasy::WriteInteger(LPCSTR lpName, int value)
{
    no_error = SetRegister(lpName, REG_DWORD, &value);
}
int CLiteRegeditEasy::ReadInteger(LPCSTR lpName)
{
    int value;
    no_error = GetRegister(lpName, REG_DWORD, &value);
    return value;
}

void CLiteRegeditEasy::WriteFloat(LPCSTR lpName, float value)
{
    no_error = SetRegister(lpName, REG_DWORD, &value);
}
float CLiteRegeditEasy::ReadFloat(LPCSTR lpName)
{
    float value;
    no_error = GetRegister(lpName, REG_DWORD, &value);
    return value;
}

void CLiteRegeditEasy::WriteLongLong(LPCSTR lpName, long long value)
{
    no_error = SetRegister(lpName, REG_QWORD, &value);
}
long long CLiteRegeditEasy::ReadLongLong(LPCSTR lpName)
{
    long long value;
    no_error = GetRegister(lpName, REG_QWORD, &value);
    return value;
}

void CLiteRegeditEasy::WriteDouble(LPCSTR lpName, double value)
{
    no_error = SetRegister(lpName, REG_QWORD, &value);
}
double CLiteRegeditEasy::ReadDouble(LPCSTR lpName)
{
    double value;
    no_error = GetRegister(lpName, REG_QWORD, &value);
    return value;
}

bool CLiteRegeditEasy::IsError()
{
    return !no_error;
}