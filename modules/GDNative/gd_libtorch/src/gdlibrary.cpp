#include <pch.h>

#include "gdlibrary.h"

#include <gdnative/gdnative.h>
#include <core/GodotGlobal.hpp>

#include <classes/PyBridge.h>
#include <classes/SnakeGame.h>
#include <classes/SnakeMap.h>

extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options* o)
{
	godot::Godot::gdnative_init(o);

	std::printf("godot_gdnative_init\n");
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options* o)
{
	godot::Godot::gdnative_terminate(o);

	std::printf("godot_gdnative_terminate\n");
}

extern "C" void GDN_EXPORT godot_nativescript_init(void* handle)
{
	godot::Godot::nativescript_init(handle);

	std::printf("godot_nativescript_init\n");

	godot::register_class<smidi::PyBridge>();
	godot::register_class<smidi::SnakeGame>();
	godot::register_class<smidi::SnakeMap>();
}