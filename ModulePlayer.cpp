#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModulePlayer.h"
#include "ModuleInput.h"
#include "ModulePhysics.h"
#include "ModuleSceneIntro.h"
#include "ModuleRender.h"
#include "ModuleAudio.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	imgball = imgflipperleft = imgflipperright = imgscore = NULL;
	ball = nullptr;
	veloy = 0;
	trower = gameover = false;
	live = 0;
	score = 0;
	best_score = 0;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	imgball = App->textures->Load("pinball/ball.png");
	
	int flipperleft_coords[20] = {
		1, 7,
		1, 10,
		5, 14,
		7, 15,
		50, 26,
		53, 25,
		52, 22,
		6, 1,
		2, 3,
		1, 6
	};
	flipperleft = App->physics->CreateChain(93, 529, flipperleft_coords, 20, App->scene_intro->CATEGORY_MAIN_PINBALL, true);
	flipperleft_wheel = App->physics->CreateCircle(100,537,3, App->scene_intro->CATEGORY_NOTMAIN_PINBALL, false, false);
	imgflipperleft = App->textures->Load("pinball/flipperleft.png");
	App->physics->CreateRevoluteJoint(flipperleft,flipperleft_wheel, 8,8,0,0,60,0);

	int flipperright_coords[20] = {
		50, 2,
		53, 4,
		54, 7,
		54, 10,
		52, 14,
		45, 16,
		4, 25,
		2, 24,
		3, 21,
		46, 1
	};
	flipperright = App->physics->CreateChain(171, 530, flipperright_coords, 20, App->scene_intro->CATEGORY_MAIN_PINBALL);
	flipperright_wheel = App->physics->CreateCircle(218, 538, 3, App->scene_intro->CATEGORY_NOTMAIN_PINBALL, false, false);
	imgflipperright = App->textures->Load("pinball/flipperright.png");
	App->physics->CreateRevoluteJoint(flipperright, flipperright_wheel, 47, 8, 0, 0, 0, -60);

	//Init Ball
	ball = App->physics->CreateCircle(320, 485, 7, App->scene_intro->CATEGORY_NOTMAIN_PINBALL, 0.1f);
	ball->listener = App->scene_intro;
	live++;

	//Score
	imgscore = App->textures->Load("pinball/Game_Over.png");
	
	//fxs
	fx_throw = App->audio->LoadFx("pinball/Audio/Thrower.wav");

	fx_flipper = App->audio->LoadFx("pinball/Audio/Flippers.wav");

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");
	App->textures->Unload(imgball);

	App->textures->Unload(imgflipperleft);
	App->textures->Unload(imgflipperright);

	App->textures->Unload(imgscore);

	return true;
}

void ModulePlayer::RestorePosBall(bool reset)
{
	if (++live >= 6 && reset == false)
		gameover = true;
	else if (reset == true)
	{
		ball->body->SetLinearVelocity(b2Vec2(0, 0));
		ball->body->SetAngularVelocity(0);
		ball->body->SetTransform(b2Vec2(PIXEL_TO_METERS(320), PIXEL_TO_METERS(485)), 0);
		score = 0;
		live = 1;
	}
	else
	{
		ball->body->SetLinearVelocity(b2Vec2(0, 0));
		ball->body->SetAngularVelocity(0);
		ball->body->SetTransform(b2Vec2(PIXEL_TO_METERS(320), PIXEL_TO_METERS(485)), 0);
	}
}

/*void ModulePlayer::LeftTubeBallEnter()
{
	App->physics->ChangeFilter(ball->body, App->scene_intro->CATEGORY_MAIN_PINBALL);
}*/

/*void ModulePlayer::LeftTubeBallExit()
{
	App->physics->ChangeFilter(ball->body, App->scene_intro->CATEGORY_NOTMAIN_PINBALL);
}*/

void ModulePlayer::Reset()
{
	App->physics->ChangeFilter(ball->body, App->scene_intro->CATEGORY_NOTMAIN_PINBALL);
}

void ModulePlayer::RightTubeBallEnter()
{
	App->physics->ChangeFilter(ball->body, App->scene_intro->CATEGORY_MAIN_PINBALL);

	b2Vec2 vel = ball->body->GetLinearVelocity();

	for (uint timer = SDL_GetTicks(); SDL_GetTicks() - timer < 1000; timer)
		ball->body->SetTransform(b2Vec2(PIXEL_TO_METERS(280), PIXEL_TO_METERS(200)), 0);

	ball->body->SetLinearVelocity(b2Vec2(vel.x, vel.y*-1));
}

void ModulePlayer::RightTubeBallExit()
{
	App->physics->ChangeFilter(ball->body, App->scene_intro->CATEGORY_NOTMAIN_PINBALL);

	b2Vec2 vel = ball->body->GetLinearVelocity();

	ball->body->SetLinearVelocity(b2Vec2(vel.x, vel.y / 4));
}

// Update: draw background
update_status ModulePlayer::Update()
{
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		if (ball == nullptr)
		{
			ball = App->physics->CreateCircle(320, 485, 7, App->scene_intro->CATEGORY_NOTMAIN_PINBALL,0.1f);
			ball->listener = App->scene_intro;
		}
		else if (ball != nullptr)
		{
			ball->body->SetLinearVelocity(b2Vec2(0, 0));
			ball->body->SetAngularVelocity(0);
			ball->body->SetTransform(b2Vec2(PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY())), 0);
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		if (ball == nullptr)
		{
			
		}
		else if (ball != nullptr)
		{
			ball->body->SetLinearVelocity(b2Vec2(0, -10));
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		flipperleft->body->ApplyAngularImpulse(DEGTORAD * -90, true);
		App->audio->PlayFx(fx_flipper);
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		flipperright->body->ApplyAngularImpulse(DEGTORAD * 90, true);
		App->audio->PlayFx(fx_flipper);
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		veloy += -0.5f;
		trower = true;

	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP)
	{
		if (ball != nullptr && App->scene_intro->canthrow == true)
			ball->body->SetLinearVelocity(b2Vec2(0, veloy));

		App->audio->PlayFx(fx_throw);
		veloy = 0;
		trower = false;
	}

	if (gameover == true)
	{
		App->renderer->Blit(imgscore, 0, 0);

		if (score > best_score)
			best_score = score;
	}

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		gameover = false;
		RestorePosBall(true);
	}

	return UPDATE_CONTINUE;
}



