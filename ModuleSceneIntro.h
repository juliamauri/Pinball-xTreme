#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

	void SetMainPinballChain();
	void SetLeftTubeChain();
	void SetRightTubeChain();

public:

	p2List<PhysBody*> pinballtable;
	SDL_Texture* backgound_shape;
	SDL_Texture* backgound_border;

	PhysBody* thrower;
	PhysBody* throwercomplement;
	SDL_Texture* imgthrower;
	SDL_Texture* imgthrowercomplement;

	PhysBody* sensorball_lost;
	bool sensoredball_lost;

	SDL_Texture* lefttube_up_hotel;
	SDL_Texture* lefttube_down_hotel;
	//SDL_Texture* lefttube_up_exit;
	//SDL_Texture* lefttube_down_exit;

	SDL_Texture* righttube_up;
	SDL_Texture* righttube_down;

	PhysBody* lefttube;
	PhysBody* sensorball_enter_left;
	bool sensoredball_enter_left;
	PhysBody* sensorball_end_left;
	bool sensoredball_end_left;

	PhysBody* righttube;
	PhysBody* sensorball_enter_RT;
	bool sensoredball_enter_RT;
	PhysBody* sensorball_end_RT;
	bool sensoredball_end_RT;

	short CATEGORY_MAIN_PINBALL;
	short CATEGORY_NOTMAIN_PINBALL;
};
