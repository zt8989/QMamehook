#ifndef XINPUT_WRAPPER_H
#define XINPUT_WRAPPER_H

#ifdef Q_OS_WIN
#include <Windows.h>
#include <Xinput.h>
#endif

class XInputWrapper
{
public:
    XInputWrapper();
    ~XInputWrapper();

#ifdef Q_OS_WIN
    bool IsAvailable();
    bool SetVibration(int userIndex, int leftMotor, int rightMotor);
#endif

private:
#ifdef Q_OS_WIN
    HMODULE xinputDll = nullptr;
    using XInputSetState_t = DWORD (WINAPI *)(DWORD, XINPUT_VIBRATION *);
    XInputSetState_t xinputSetState = nullptr;

    bool EnsureLoaded();
    void Cleanup();
#endif
};

#endif // XINPUT_WRAPPER_H
