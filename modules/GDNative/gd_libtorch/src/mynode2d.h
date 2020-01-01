#pragma once

#include <core/Godot.hpp>
#include <gen/Node2D.hpp>

namespace smidi
{
	class MyNode2D
		: public godot::Node2D
	{
		GODOT_CLASS(MyNode2D, Node2D);
	public:
		MyNode2D() = default;
		~MyNode2D() = default;

		static void _register_methods();

		void _init();
		void _process(float delta);
	};
}