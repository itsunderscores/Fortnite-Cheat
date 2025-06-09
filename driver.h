#pragma once
#pragma once
#include <Windows.h>


#include "commincat.h"
#include <stdio.h>
#include <TlHelp32.h>
#include <string_view>
#include "spoog.h"
#include <iostream>
#include "xor.hpp"

class driverinfo
{
public:
    uintptr_t BASEaddress;
    uintptr_t guard;
    HANDLE pDevice;
    int proccessid;
    uintptr_t GetBaseAddress(int proccessid);
    bool ConnectToDriver();
    bool Authenticate(const std::string& cat);
    bool Dispatch(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);
    void read(uintptr_t address, PVOID buffer, DWORD size);
    bool is_guarded(uintptr_t pointer);
    uint64_t validate_pointer(uint64_t address);
    uintptr_t getpdm4();
    DWORD getcr3(int pid);
    void movemouse(long y, long x);
    void sendkey(unsigned short button);
    template <typename T>
    T readdriver(uintptr_t address) {
        T temp = {};
        read(address, &temp, sizeof(T));
        return temp;
    }
    void readchainarray(uintptr_t address, void* buffer, size_t size) {
        read(address, buffer, size);
    }
    template <typename T>T readguard(uintptr_t address)
    {
        SPOOF_FUNC;
        T temp = {};

        read(address, &temp, sizeof(T));

        if (is_guarded(temp))
        {
            temp = validate_pointer(temp);
        }
        return temp;  // Return the value stored in temp
    }
    typedef struct _exemreadwrite
    {
        INT32 lexemveprocess_id;
        ULONGLONG lexemveaddress;
        ULONGLONG lexemvebuffer;
        ULONGLONG lexemvesize;
        BOOLEAN lexemvewrite;
    } rw, * prw;


    typedef struct _ba {
        INT32 lexemveprocess_id;
        ULONGLONG* lexemveaddress;
    } ba, * pba;

    typedef struct _SYSTEM_BIGPOOL_ENTRY
    {
        union {
            PVOID VirtualAddress;
            ULONG_PTR NonPaged : 1;
        };
        ULONG_PTR SizeInBytes;
        union {
            UCHAR Tag[4];
            ULONG TagUlong;
        };
    } SYSTEM_BIGPOOL_ENTRY, * PSYSTEM_BIGPOOL_ENTRY;

    typedef struct _SYSTEM_BIGPOOL_INFORMATION {
        ULONG Count;
        SYSTEM_BIGPOOL_ENTRY AllocatedInfo[ANYSIZE_ARRAY];
    } SYSTEM_BIGPOOL_INFORMATION, * PSYSTEM_BIGPOOL_INFORMATION;

    typedef enum _SYSTEM_INFORMATION_CLASSS {
        SystemBigPoolInformation = 0x42
    } SYSTEM_INFORMATION_CLASSS;

    typedef NTSTATUS(WINAPI* pNtQuerySystemInformation)(
        IN _SYSTEM_INFORMATION_CLASSS SystemInformationClass,
        OUT PVOID                   SystemInformation,
        IN ULONG                    SystemInformationLength,
        OUT PULONG                  ReturnLength
        );

    __forceinline auto query_bigpools() -> PSYSTEM_BIGPOOL_INFORMATION
    {
        static const pNtQuerySystemInformation NtQuerySystemInformation =
            (pNtQuerySystemInformation)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQuerySystemInformation");

        DWORD length = 0;
        DWORD size = 0;
        LPVOID heap = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0);
        heap = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, heap, 0xFF);
        NTSTATUS ntLastStatus = NtQuerySystemInformation(SystemBigPoolInformation, heap, 0x30, &length);
        heap = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, heap, length + 0x1F);
        size = length;
        ntLastStatus = NtQuerySystemInformation(SystemBigPoolInformation, heap, size, &length);

        return reinterpret_cast<PSYSTEM_BIGPOOL_INFORMATION>(heap);
    }
    __forceinline auto get_guarded_reg() -> uintptr_t
    {
        auto pool_information = query_bigpools();
        uintptr_t guarded = 0;

        if (pool_information)
        {
            auto count = pool_information->Count;
            for (auto i = 0ul; i < count; i++)
            {
                SYSTEM_BIGPOOL_ENTRY* allocation_entry = &pool_information->AllocatedInfo[i];
                const auto virtual_address = (PVOID)((uintptr_t)allocation_entry->VirtualAddress & ~1ull);
                /*    if (allocation_entry->NonPaged && allocation_entry->SizeInBytes == 0x200000)
                        if (guarded == 0 && allocation_entry->TagUlong == 'TnoC')
                            guarded = reinterpret_cast<uintptr_t>(virtual_address);*/
                if (allocation_entry->NonPaged && allocation_entry->TagUlong == 'TnoC' && allocation_entry->SizeInBytes == 0x200000)
                {
                    if (readdriver<uintptr_t>(reinterpret_cast<uintptr_t>(virtual_address) + 0x60) != 0)
                    {
                        guarded = reinterpret_cast<uintptr_t>(virtual_address);
                    }
                }
            }
        }

        return guarded;
    }

    template<typename T>void writedriver(uintptr_t address, T value)
    {
        SPOOF_FUNC;

        userinput argument;
        useroutput alr;
        argument.Proccssid = (HANDLE)proccessid;
        argument.address = (PVOID)address;
        argument.buffer = &value;  // Expecting kernel to fill this
        argument.size = sizeof(T);

        useroutput output = {};  // Ensure this is used properly for output

        SPOOF_CALL(DeviceIoControl)(pDevice, IOCTL_WRITE_PROCESS, &argument, sizeof(argument), &alr, sizeof(alr), nullptr, nullptr);
        //value = (T)alr.returns;
        SPOOF_CALL(memcpy)(&value, alr.returns, sizeof(T));

        //if (!works) {
        //    printf("Failed to read\n");
        //}
        //else {
        //    printf("Successfully writen data: %p\n", value);  // Add a log to see what temp holds
        //}

    }


    std::uintptr_t GetModuleAddress(int pid, std::wstring_view moduleName) const noexcept
    {
        MODULEENTRY32W entry = {}; // Use MODULEENTRY32W for Unicode
        entry.dwSize = sizeof(MODULEENTRY32W);

        const HANDLE snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);

        std::uintptr_t result = 0;

        if (snapShot != INVALID_HANDLE_VALUE) {
            while (::Module32Next(snapShot, reinterpret_cast<MODULEENTRY32*>(&entry))) // Cast for compatibility
            {
                // Use wide string comparison
                if (moduleName.compare(entry.szModule) == 0)
                {
                    result = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
                    break;
                }
            }
            ::CloseHandle(snapShot);
        }

        return result;
    }


    DWORD get_proccess_id(LPCTSTR process_name);
    /* template<typename T> T read2(uintptr_t address, DWORD size)
     {
         T buffer{};
         read(proccessid,(PVOID)address, &buffer, size);
         return buffer;
     }*/
private:

};
extern driverinfo driver;