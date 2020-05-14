#pragma once

#include <iostream>
#include <windows.h>
#include <TlHelp32.h>

DWORD GetProcessThreads(DWORD PID);

DWORD GetProcId(const wchar_t* procName);

uintptr_t GetModuleSize(DWORD procId, const wchar_t* modName);

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);