#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModulePlayer.h"
#include "ModuleInput.h"
#include "ModulePhysics.h"
#include "ModuleSceneIntro.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	imgball = NULL;
	ball = nullptr;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	imgball = App->textures->Load("pinball/ball.png");

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");
	App->textures->Unload(imgball);

	return true;
}

void ModulePlayer::RestorePosBall()
{
	ball->body->SetLinearVelocity(b2Vec2(0, 0));
	ball->body->SetAngularVelocity(0);
	ball->body->SetTransform(b2Vec2(PIXEL_TO_METERS(320), PIXEL_TO_METERS(485)), 0);
}

// Update: draw background
update_status ModulePlayer::Update()
{
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		if (ball == nullptr)
		{
			ball = App->physics->CreateCircle(320, 485, 7, App->scene_intro->CATEGORY_NOTMAIN_PAINBALL);
			ball->listener = App->scene_intro;
		}
		else if (ball != nullptr)
		{
			ball->body->SetLinearVelocity(b2Vec2(0, 0));
			ball->body->SetAngularVelocity(0);
			ball->body->SetTransform(b2Vec2(PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY())), 0);
		}
	}


	return UPDATE_CONTINUE;
}



