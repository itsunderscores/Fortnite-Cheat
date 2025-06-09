#include "driver.h"
#include <stdio.h>
#include "commincat.h"
#include <TlHelp32.h>
#include <iostream>
#include <thread>
driverinfo driver;
std::wstring stringToWString(const std::string& str) {
	return std::wstring(str.begin(), str.end());
}
bool driverinfo::Dispatch(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped) {
	SPOOF_FUNC;

	BOOL cat = DeviceIoControl(hDevice, dwIoControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, lpBytesReturned, lpOverlapped);
	return cat;
}
bool driverinfo::ConnectToDriver()
{
	SPOOF_FUNC;
	pDevice = SPOOF_CALL(CreateFile)(L"\\\\.\\{fuckingniggerbitch}", GENERIC_WRITE | GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	if (pDevice == INVALID_HANDLE_VALUE) {
		printf("Error opening Handel (%u)\n", GetLastError());
		return false;
	}
	return true;
}
bool driverinfo::Authenticate(const std::string& authCode)
{
	userinput argument = {};
	useroutput alr = {};
	DWORD bytesReturned = 0;

	strncpy_s(argument.g_AuthCode, sizeof(argument.g_AuthCode), authCode.c_str(), _TRUNCATE);
	argument.g_AuthCode[sizeof(argument.g_AuthCode) - 1] = '\0';

	BOOL success = DeviceIoControl(
		pDevice,
		IOCTL_SET_AUTH_CODE,
		&argument, sizeof(argument),
		&alr, sizeof(alr),
		&bytesReturned,
		nullptr
	);

	if (!success) {
		printf("DeviceIoControl failed with error: %lu\n", GetLastError());
		return false;
	}

	return alr.g_IsAuthenticated;
}

uintptr_t driverinfo::GetBaseAddress(int pid) {
	SPOOF_FUNC;

	if (pid == 0) {
		return pid;
	}
	DWORD bytes;
	uintptr_t image_address = { NULL };
	userinput argument;
	argument.Proccssid = (HANDLE)pid;
	useroutput alr;
	BOOL result = DeviceIoControl(pDevice, IOCTL_MODULEADDRESS_PROCESS, &argument, sizeof(argument), &alr, sizeof(alr), &bytes, nullptr);
	if (!result) {
		DWORD error = GetLastError();
		return 0;
	}

	image_address = (uintptr_t)alr.HProcces;
	return image_address;

}

void driverinfo::read(uintptr_t address, PVOID buffer, DWORD size) {
	SPOOF_FUNC;

	userinput argument;
	useroutput alr;
	argument.Proccssid = (HANDLE)proccessid;
	argument.address = (PVOID)address;
	argument.buffer = buffer;
	argument.size = size;

	BOOL success = SPOOF_CALL(DeviceIoControl)(
		pDevice,
		IOCTL_READ_PROCESS,
		&argument, sizeof(argument),
		&alr, sizeof(alr),
		nullptr, nullptr
		);


}
DWORD driverinfo::get_proccess_id(LPCTSTR process_name)
{
	SPOOF_FUNC;

	PROCESSENTRY32 pt{};
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hsnap, &pt))
	{
		do {
			if (!lstrcmpi(pt.szExeFile, process_name))
			{
				CloseHandle(hsnap);
				return pt.th32ProcessID;
			}
		} while (Process32Next(hsnap, &pt));
	}
	CloseHandle(hsnap);
	return 0;
}

DWORD driverinfo::getcr3(int pid)
{
	SPOOF_FUNC;

	if (pid == 0) {
		return pid;
	}
	DWORD bytes;
	userinput argument;
	argument.Proccssid = (HANDLE)pid;
	useroutput alr;
	BOOL result = DeviceIoControl(pDevice, IOCTL_Cr3, &argument, sizeof(argument), &alr, sizeof(alr), &bytes, nullptr);
	if (!result) {
		DWORD error = GetLastError();
		return 0;
	}
	std::cout << alr.pml4_base << std::endl;
	return alr.pml4_base;
}

void driverinfo::movemouse(long x, long y)
{
	SPOOF_FUNC;
	userinput argument;

	useroutput alr;
	argument.inputx = x;
	argument.inputy = y;


	argument.button_flags = 0; // No key press
	BOOL success = SPOOF_CALL(DeviceIoControl)(
		pDevice,
		IOCTL_Movemouse,
		&argument, sizeof(argument),
		&alr, sizeof(alr),
		nullptr, nullptr
		);

}
void driverinfo::sendkey(unsigned short button)
{
	SPOOF_FUNC;
	userinput argument;
	useroutput alr;
	argument.inputx = 0;
	argument.inputy = 0;

	argument.button_flags = button;

	BOOL success = SPOOF_CALL(DeviceIoControl)(
		pDevice,
		IOCTL_Movemouse,
		&argument, sizeof(argument),
		&alr, sizeof(alr),
		nullptr, nullptr
		);
	argument.button_flags = 0;

}