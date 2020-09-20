#pragma once

#include <Godot.hpp>
#include <KinematicBody2D.hpp>
#include <Input.hpp>

namespace godot {
	class Player : public KinematicBody2D 
	{
	
	// Godot structure
	private:
		GODOT_CLASS(Player, KinematicBody2D)
	public:
		static void _register_methods();
		void _init();
		void _process(float delta);

		Player();
		~Player();


	// Gameplay variables
	public:
		const int speed = 300;

	private:
		Vector2 motion;



	// Gameplay methods
	public:
		void UpdateMotionFromInput();
	
	private:




	};
}
