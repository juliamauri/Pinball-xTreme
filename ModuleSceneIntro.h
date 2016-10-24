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

public:

	p2List<PhysBody*> pinballtable;
	SDL_Texture* backgound_shape;
	SDL_Texture* backgound_border;

	short CATEGORY_MAIN_PINBALL;
	short CATEGORY_NOTMAIN_PAINBALL;
};
