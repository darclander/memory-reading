#include <iostream>
#include <Windows.h>
#include <string>
#include <Psapi.h>
#include <process.h>
#include <tlhelp32.h>
#include <vector>
#include <process.h>
#include <stdio.h>
#include <tchar.h>
#include <fstream>

DWORD GetProcId(const wchar_t* procName) {
    DWORD pid = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);
        if(Process32First(hSnap, &procEntry)) {
            do {
                if(!_wcsicmp(procEntry.szExeFile, procName)) {
                    pid = procEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
    }
    CloseHandle(hSnap);
    return pid;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName) {
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | 0x10, procId);
    if(hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if(Module32First(hSnap, &modEntry)) {
            do {
                if(!_wcsicmp(modEntry.szModule, modName)) {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while(Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

uintptr_t GetModuleSize(DWORD procId, const wchar_t* modName) {
    uintptr_t modBaseSize = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | 0x10, procId);
    if(hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if(Module32First(hSnap, &modEntry)) {
            do {
                if(!_wcsicmp(modEntry.szModule, modName)) {
                    modBaseSize = (uintptr_t)modEntry.modBaseSize;
                    break;
                }
            } while(Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseSize;
}

int main() {

    // Get PID, moduleBase and moduleSize
    DWORD procId = GetProcId(L"x.exe");
    uintptr_t moduleBase = GetModuleBaseAddress(procId, L"x.exe");
    uintptr_t moduleSize = GetModuleSize(procId, L"x.exe");

    // Set base and last address and print them for user
    uintptr_t dynamicPtrBaseAddr = moduleBase;
    uintptr_t dynamicPtrLastAddr = (moduleBase + moduleSize);
    std::cout << "Base: " << dynamicPtrBaseAddr << std::endl;
    std::cout << "Last: " << dynamicPtrLastAddr << std::endl;

    // Require all access
    HANDLE hProcess = 0;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);

    // Initializes values
    int number = 502;
    std::cout << &number;

    int value = 0;
    std::ofstream myfile;
    myfile.open("values.txt"); // textfile to write to

    // Reads and writes values from all memory addresses which are within 0 < value < 1000 range to file.
    if (myfile.is_open()) {
        for (uintptr_t i = dynamicPtrBaseAddr; dynamicPtrBaseAddr <= dynamicPtrLastAddr; i++) {
            dynamicPtrBaseAddr += 4;
            ReadProcessMemory(hProcess, (BYTE*)dynamicPtrBaseAddr, &value, sizeof(value),0);
            if(value > 0 && value < 1000) {
                myfile << dynamicPtrBaseAddr << ": " << value << "\n";
            }
        }
    }
    myfile.close();
    system("pause");
}
