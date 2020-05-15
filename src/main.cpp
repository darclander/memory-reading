#include <iostream>
#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
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
    uintptr_t dynamicPtrLastAddr = moduleBase + moduleSize;
    std::cout << "Base: " << dynamicPtrBaseAddr << std::endl;
    std::cout << "Last: " << dynamicPtrLastAddr << std::endl;

    // Require all access and checks if hProcess fails
    HANDLE hProcess = 0;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
    if (hProcess == NULL) {
        std::cout << GetLastError();
    }

    // Initializes values
    int number = 502;
    std::cout << std::endl;
    std::cout << &number << std::endl;

    int value = 0;
    std::ofstream myfile;
    myfile.open("values.txt"); // textfile to write to

    int search; 
    std::cin >> search;

    // Reads and writes values from all memory addresses which are within 0 < value < 1000 range to file.
    uintptr_t values[100];
    int k = 0;
    if (myfile.is_open()) {
        for (uintptr_t i = dynamicPtrBaseAddr; dynamicPtrBaseAddr <= dynamicPtrLastAddr; i++) {
            dynamicPtrBaseAddr += 4;
            ReadProcessMemory(hProcess, (BYTE*)dynamicPtrBaseAddr, &value, sizeof(value),0);
            if(value == search) {
                myfile << dynamicPtrBaseAddr << ": " << value << "\n";
                values[k] = dynamicPtrBaseAddr;
                k++;
            }
        }
    }
    myfile.close();
    std::cout << k;
    std::cin >> search;
    std::cout << search;
    number = 503;

    for (int i = 0; i < (sizeof(values)/sizeof(*values)); i++) {
        dynamicPtrBaseAddr = values[i];
        ReadProcessMemory(hProcess, (BYTE*)dynamicPtrBaseAddr, &value, sizeof(value),0);
        if (value == search) {
            std::cout << dynamicPtrBaseAddr << std::endl;
        }
    }

    system("pause");
}


