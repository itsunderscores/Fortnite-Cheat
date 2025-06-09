#include "includes.hpp"
#include <iostream>
#include <thread>
#include "xor.hpp"
#include <Windows.h>
#include <string>

/// <SUPPORT>
/// if you need help
/// discord: temu_manager
/// </SUPPORT>

void main()
{
	//FreeConsole();
	printf("Waiting for Fortnite...");

	while (game_wnd == 0)
	{
		Sleep(1);
		driver.proccessid = driver.get_proccess_id(_(L"FortniteClient-Win64-Shipping.exe"));
		game_wnd = get_process_wnd(driver.proccessid);
		Sleep(1);
	}

	printf("Discord: temu_manager");

	if (!input::init())
	{
		printf("input was not initialized");
		Sleep(3000);
		exit(0);
	}

	if (!gui::init())
	{
		printf("gui was not initialized"); 
		Sleep(3000);
		exit(0);
	}

	if (!driver.ConnectToDriver()) {
		printf("driver was not initialized");
		Sleep(3000);
		exit(0);
	}


	driver.BASEaddress = driver.GetBaseAddress(driver.proccessid);

	auto cr3 = driver.getcr3(driver.proccessid);
	printf("CR3: %p\n", cr3);

	if (cr3 == 0) {
		printf("cr3 was not correct");
		Sleep(2000);
		exit(1);
	}

	if (!driver.GetBaseAddress(driver.proccessid))
	{
		printf("The driver couldn't get the base address");
		Sleep(3000);
		exit(0);
	}

	float aspect_ratio = static_cast<float>(settings::width) / settings::height;
	printf("Screen Resolution: %dx%d | Aspect Ratio: %.2f\n", settings::width, settings::height, aspect_ratio);

	LoadDefaultConfig();
	create_overlay(); 
	directx_init();
	render_loop();

}


