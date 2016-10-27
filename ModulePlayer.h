#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "ModulePhysics.h"

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

	void RestorePosBall(bool reset = false);

	void LeftTubeBallEnter();
	void LeftTubeBallExit();
	void Reset();
	void RightTubeBallEnter();
	void RightTubeBallExit();

public:
	PhysBody* ball;
	SDL_Texture* imgball;

	PhysBody* flipperleft;
	PhysBody* flipperleft_wheel;
	SDL_Texture* imgflipperleft;
	b2RevoluteJointDef revoluteJointDef_left;

	PhysBody* flipperright;
	PhysBody* flipperright_wheel;
	SDL_Texture* imgflipperright;
	b2RevoluteJointDef revoluteJointDef_right;


	float veloy;
	bool trower;

	uint fx_throw;

	uint fx_flipper;

	uint live;
	SDL_Texture* imgscore;
	bool gameover;
	uint score;
	uint best_score;
};