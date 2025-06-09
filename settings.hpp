//SIMPLE ASS SETTINGS HERE
#pragma once
#include "imgui/imgui.h"

namespace settings
{
	static bool showSaveMessage = false;
	static float saveMessageTimer = 0.0f;
	static bool showLoadMessage = false;
	inline int x_slider = 0;
	inline float fov_value = 89.95555f;

	inline int width_offset = 0;
	inline int height_offset = 0;

	inline int width_offset2 = 0;
	inline int width_offset3 = 0;

	inline int width = GetSystemMetrics(SM_CXSCREEN);
	inline int height = GetSystemMetrics(SM_CYSCREEN);

	inline int screen_center_x = (width / 2) + width_offset;
	inline int screen_center_y = (height / 2) + width_offset;

	inline int get_screen_center_x() {
		return (width / 2) + width_offset;
	}

	inline int get_screen_center_x2() {
		return (width / 2) + width_offset2;
	}

	inline int get_screen_center_x3() {
		return (width / 2) + width_offset3;
	}

	inline int get_screen_center_y() {
		return (height / 2) + height_offset;
	}


	inline bool show_menu = true;
	inline int tab = 0;
	namespace aimbot
	{
		inline bool enable = false;
		inline bool show_fov = false;
		inline float fov = 160;
		inline float smoothness = 6;
		inline bool visible_check = false;
		inline static int current_key = VK_RBUTTON;
	}
	namespace visuals
	{
		inline bool enable = false;
		inline bool CorneredBox = false;
		inline bool fill_box = false;
		inline bool fill_wcbox = false;
		inline bool line = false;
		inline bool distance = false;
		inline bool Box = false;
		inline bool skeleton = false;
		float aspect_ratio = 1.0f;
		float box_height = 1.0f;

		// Correct usage of ImVec4 after including imgui.h
		inline ImVec4 boxColor = ImVec4(36 / 255.0f, 207 / 255.0f, 2 / 255.0f, 100 / 255.0f);  // Initial color (green with some transparency)
	}
	namespace Features
	{
		inline bool Watermark = true;
		inline bool info = false;
		inline bool crosshair = false;
		inline bool fpswater = true;
		inline bool ultrawidefix = false;
		inline bool changeresolution = false;
	}
	namespace radar
	{
		inline bool radar = false;
		inline bool cross = true;
		inline bool local_player = true;
		inline float range = 50.0f;
	}
}