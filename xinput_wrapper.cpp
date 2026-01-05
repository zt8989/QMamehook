#include "xinput_wrapper.h"
#include <cstdio>

XInputWrapper::XInputWrapper()
{
}

XInputWrapper::~XInputWrapper()
{
#ifdef Q_OS_WIN
    Cleanup();
#endif
}

#ifdef Q_OS_WIN
bool XInputWrapper::EnsureLoaded()
{
    if(xinputSetState)
        return true;

    const char *dllNames[] = { "xinput1_4.dll", "xinput1_3.dll", "xinput9_1_0.dll" };
    char dllPath[MAX_PATH] = {};
    UINT sysLen = GetSystemDirectoryA(dllPath, MAX_PATH);
    if(sysLen == 0 || sysLen >= MAX_PATH)
        return false;

    for(const char *dllName : dllNames) {
        char fullPath[MAX_PATH] = {};
        int written = snprintf(fullPath, MAX_PATH, "%s\\%s", dllPath, dllName);
        if(written <= 0 || written >= MAX_PATH)
            continue;

        HMODULE dll = LoadLibraryA(fullPath);
        if(!dll)
            continue;

        auto setState = reinterpret_cast<XInputSetState_t>(GetProcAddress(dll, "XInputSetState"));
        if(setState) {
            xinputDll = dll;
            xinputSetState = setState;
            return true;
        }

        FreeLibrary(dll);
    }

    return false;
}

void XInputWrapper::Cleanup()
{
    if(xinputDll) {
        FreeLibrary(xinputDll);
        xinputDll = nullptr;
        xinputSetState = nullptr;
    }
}

bool XInputWrapper::IsAvailable()
{
    return EnsureLoaded();
}

bool XInputWrapper::SetVibration(int userIndex, int leftMotor, int rightMotor)
{
    if(userIndex < 0 || userIndex >= XUSER_MAX_COUNT)
        return false;

    if(!EnsureLoaded())
        return false;

    if(leftMotor < 0) leftMotor = 0;
    if(leftMotor > 65535) leftMotor = 65535;
    if(rightMotor < 0) rightMotor = 0;
    if(rightMotor > 65535) rightMotor = 65535;

    XINPUT_VIBRATION vibration;
    ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
    vibration.wLeftMotorSpeed = static_cast<WORD>(leftMotor);
    vibration.wRightMotorSpeed = static_cast<WORD>(rightMotor);

    return xinputSetState(static_cast<DWORD>(userIndex), &vibration) == ERROR_SUCCESS;
}
#endif
