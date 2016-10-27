#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

	void RestorePosBall();

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
	PhysBody* impulseflipperlesft;
	SDL_Texture* imgflipperleft;

	PhysBody* flipperright;
	PhysBody* flipperright_wheel;
	PhysBody* impulseflipperright;
	SDL_Texture* imgflipperright;


};