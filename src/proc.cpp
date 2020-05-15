#include "proc.h"

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

DWORD GetProcessThreads(DWORD PID) {
  HANDLE thread_snap = INVALID_HANDLE_VALUE;
  THREADENTRY32 te32;

  // take a snapshot of all running threads
  thread_snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  if (thread_snap == INVALID_HANDLE_VALUE) {
    printf("Invalid Handle Value");
    return(FALSE);
  }

  // fill in the size of the structure before using it. 
  te32.dwSize = sizeof(THREADENTRY32);

  // retrieve information about the first thread,
  // and exit if unsuccessful
  if (!Thread32First(thread_snap, &te32)) {
    printf("Thread32First Error");
    CloseHandle(thread_snap);
    return 0;
  }

  // now walk the thread list of the system,
  // and display thread ids of each thread
  // associated with the specified process
  do {
    if (te32.th32OwnerProcessID == PID) {
      printf("THREAD ID: 0x%08X",te32.th32ThreadID);
      std::cout << std::endl;
    }
  } while (Thread32Next(thread_snap, &te32));

  // clean up the snapshot object.
  CloseHandle(thread_snap);
  return te32.th32ThreadID;
}