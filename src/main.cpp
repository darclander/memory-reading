#include <iostream>
#include <Windows.h>
#include <Psapi.h>
#include <stdio.h>
#include <fstream>
#include "proc.h"

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

    // Require all access and checks if hProcess fails
    HANDLE hProcess = 0;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
    if (hProcess == NULL) {
        std::cout << GetLastError();
    }

    DWORD tid = GetProcessThreads(procId);

    HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, false, tid);
    if (hThread == NULL) {
        std::cout << GetLastError();
    }

    // Initializes values
    int number = 502;
    std::cout << std::endl;
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


