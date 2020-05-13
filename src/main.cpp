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


uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets) {
    uintptr_t addr = ptr;
    for (unsigned int i = 0; i < offsets.size(); ++i) {
        ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
        addr += offsets[i];
    }
    return addr;
}


int main() {

    DWORD procId = GetProcId(L"x.exe");
    uintptr_t moduleBase = GetModuleBaseAddress(procId, L"x.exe");
    uintptr_t moduleSize = GetModuleSize(procId, L"x.exe");

    HANDLE hProcess = 0;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);

    uintptr_t dynamicPtrBaseAddr = moduleBase;
    uintptr_t dynamicPtrLastAddr = 2*(moduleBase + moduleSize);

    std::cout << "Base: " << dynamicPtrBaseAddr << std::endl;
    std::cout << "Last: " << dynamicPtrLastAddr << std::endl;

    int number = 502;
    std::cout << &number;
    int value = 0;
    int arr [100000] = {};
    uintptr_t addresses[100000] = {};
    int k=0;
    int search = 0;
    std::cin >> search;
    DWORD oldProtect;
    std::ofstream myfile;
    myfile.open("values2.txt");
    if (myfile.is_open()) {
        for (uintptr_t i = dynamicPtrBaseAddr; dynamicPtrBaseAddr <= dynamicPtrLastAddr; i++) {
            dynamicPtrBaseAddr += 4;
            ReadProcessMemory(hProcess, (BYTE*)dynamicPtrBaseAddr, &value, sizeof(value),0);
            if(value > 0 && value < 1000) {
                myfile << dynamicPtrBaseAddr << ": " << value << "\n";
            }
            // if (value == search) {
            //     VirtualProtectEx(hProcess, (LPVOID)dynamicPtrBaseAddr, sizeof(value), PAGE_READWRITE, &oldProtect);
            //     value = 3000;
            //     if(WriteProcessMemory(hProcess, (BYTE*)dynamicPtrBaseAddr, &value, sizeof(value),0) == 0) {
            //         std::cout << GetLastError();
            //         system("pause");
            //     } 
            //     arr[k] = value;
            //     addresses[k] = dynamicPtrBaseAddr;
            //     std::cout << k << std::endl;
            //     std::cout << dynamicPtrBaseAddr << std::endl;
            //     std::cout << arr[k] << std::endl;
            //     k++;
            // }
        }
    }
    myfile.close();
    std::cout << dynamicPtrBaseAddr << std::endl;
    std::cout << k;
    std::cout << "hello" << std::endl;
    system("pause");
    for (int i = 0; i <= 10; i++) {
        std::cout << addresses[i] << std::endl;
    }

    
    // system("pause");
    // for (int i =0; i <= 10; i++) {
    // ReadProcessMemory(hProcess, (BYTE*)addresses[i], &value, sizeof(value),0);
    //     std::cout << addresses[i] << std::endl;
    //     std::cout << value << std::endl;
    // }
    // system("pause");
}


    // std::vector<unsigned int> xOffsets = {0};
    // uintptr_t xaddr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, xOffsets);
    // std::cout << "xaddr: " << "0x" << std::hex << xaddr << std::endl;