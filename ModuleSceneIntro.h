#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "p2SString.h"
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
	bool canthrow;

	PhysBody* sensorball_lost;
	bool sensoredball_lost;

	SDL_Texture* lefttube_hotel_entry;
	SDL_Texture* lefttube_hotel;
	SDL_Texture* lefttube_below_exit;
	SDL_Texture* lefttube_above_exit;

	PhysBody* lefttube;
	PhysBody* lefttube_intermediate;
	PhysBody* sensorball_enter_left;
	bool sensoredball_enter_left; 
	PhysBody* sensorball_enterext_left;
	bool sensoredball_enterext_left;
	PhysBody* sensorball_points_left;
	bool sensoredball_points_left;
	PhysBody* sensorball_end_left;
	bool sensoredball_end_left;
	uint fx_lefttube;
	bool lefttube_active;

	PhysBody* cylinder;
	PhysBody* sensor_cylinder;
	bool sensored_cylinder;

	SDL_Texture* righttube_up;
	SDL_Texture* righttube_down;

	PhysBody* righttube;
	PhysBody* sensorball_enter_RT;
	bool sensoredball_enter_RT;
	PhysBody* sensorball_end_RT;
	bool sensoredball_end_RT;
	uint fx_righttube;

	p2List<PhysBody*> reboters;
	SDL_Texture* imgreboter;
	uint fx_reboter;
	bool reboted;

	PhysBody* triangleleft;
	PhysBody* triangleleft_bouncer;
	SDL_Texture* imgtriangleleft;

	PhysBody* triangleright;
	PhysBody* triangleright_bouncer;
	SDL_Texture* imgtiangleright;
	uint fx_triangle;
	bool triangled;

	short CATEGORY_MAIN_PINBALL;
	short CATEGORY_NOTMAIN_PINBALL;
};
