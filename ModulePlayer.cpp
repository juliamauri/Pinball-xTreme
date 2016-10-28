#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModulePlayer.h"
#include "ModuleInput.h"
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
	
	flipperleft = App->physics->CreateRectangle(122,538,49,6,App->scene_intro->CATEGORY_MAIN_PINBALL);
	flipperleft->body->SetType(b2_dynamicBody);
	flipperleft_wheel = App->physics->CreateCircle(100,538,2, App->scene_intro->CATEGORY_NOTMAIN_PINBALL, false, false);
	imgflipperleft = App->textures->Load("pinball/flipperleft.png");

	
	revoluteJointDef_left.bodyA = flipperleft->body;
	revoluteJointDef_left.bodyB = flipperleft_wheel->body;
	revoluteJointDef_left.referenceAngle = 0 * DEGTORAD;
	revoluteJointDef_left.enableLimit = true;
	revoluteJointDef_left.lowerAngle = -15 * DEGTORAD;
	revoluteJointDef_left.upperAngle = 30 * DEGTORAD;
	revoluteJointDef_left.localAnchorA.Set(PIXEL_TO_METERS(-24), 0);
	revoluteJointDef_left.localAnchorB.Set(0, 0);
	b2RevoluteJoint* joint_left = (b2RevoluteJoint*)App->physics->world->CreateJoint(&revoluteJointDef_left);

	flipperright = App->physics->CreateRectangle(197, 538, 49, 6, App->scene_intro->CATEGORY_MAIN_PINBALL);
	flipperright->body->SetType(b2_dynamicBody);
	flipperright_wheel = App->physics->CreateCircle(218, 538, 2, App->scene_intro->CATEGORY_NOTMAIN_PINBALL, false, false);
	imgflipperright = App->textures->Load("pinball/flipperright.png");

	revoluteJointDef_right.bodyA = flipperright->body;
	revoluteJointDef_right.bodyB = flipperright_wheel->body;
	revoluteJointDef_right.referenceAngle = 0 * DEGTORAD;
	revoluteJointDef_right.enableLimit = true;
	revoluteJointDef_right.lowerAngle = -30 * DEGTORAD;
	revoluteJointDef_right.upperAngle = 15 * DEGTORAD;
	revoluteJointDef_right.localAnchorA.Set(PIXEL_TO_METERS(24), 0);
	revoluteJointDef_right.localAnchorB.Set(0, 0);
	b2RevoluteJoint* joint_right = (b2RevoluteJoint*)App->physics->world->CreateJoint(&revoluteJointDef_right);
	

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

void ModulePlayer::LeftTubeBallEnter()
{
	App->physics->ChangeFilter(ball->body, App->scene_intro->CATEGORY_MAIN_PINBALL);

	App->physics->ChangeFilter(App->scene_intro->lefttube->body, App->scene_intro->CATEGORY_MAIN_PINBALL);

	App->physics->ChangeFilter(App->scene_intro->righttube->body, App->scene_intro->CATEGORY_MAIN_PINBALL);

	b2Vec2 vel = ball->body->GetLinearVelocity();
	ball->body->SetLinearVelocity(b2Vec2(vel.x, vel.y*2));
}

void ModulePlayer::LeftTubeBallEnterExt()
{

	App->physics->ChangeFilter(App->scene_intro->lefttube->body, App->scene_intro->CATEGORY_NOTMAIN_PINBALL);

	App->physics->ChangeFilter(App->scene_intro->lefttube_intermediate->body, App->scene_intro->CATEGORY_MAIN_PINBALL);
}

void ModulePlayer::LeftTubeBallExit()
{
	App->physics->ChangeFilter(ball->body, App->scene_intro->CATEGORY_NOTMAIN_PINBALL);

	App->physics->ChangeFilter(App->scene_intro->righttube->body, App->scene_intro->CATEGORY_NOTMAIN_PINBALL);

	App->physics->ChangeFilter(App->scene_intro->lefttube_intermediate->body, App->scene_intro->CATEGORY_NOTMAIN_PINBALL);

	b2Vec2 vel = ball->body->GetLinearVelocity();
	ball->body->SetLinearVelocity(b2Vec2(vel.x, vel.y / 4));
}

void ModulePlayer::RightTubeBallEnter()
{
	App->physics->ChangeFilter(ball->body, App->scene_intro->CATEGORY_MAIN_PINBALL);
	App->physics->ChangeFilter(App->scene_intro->lefttube->body, App->scene_intro->CATEGORY_MAIN_PINBALL);

	b2Vec2 vel = ball->body->GetLinearVelocity();

	for (uint timer = SDL_GetTicks(); SDL_GetTicks() - timer < 1000; timer)
		ball->body->SetTransform(b2Vec2(PIXEL_TO_METERS(280), PIXEL_TO_METERS(200)), 0);

	ball->body->SetLinearVelocity(b2Vec2(vel.x, vel.y*-1));
}

void ModulePlayer::RightTubeBallExit()
{
	App->physics->ChangeFilter(ball->body, App->scene_intro->CATEGORY_NOTMAIN_PINBALL);
	App->physics->ChangeFilter(App->scene_intro->lefttube->body, App->scene_intro->CATEGORY_NOTMAIN_PINBALL);

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
		b2Vec2 force = b2Vec2(0, -200);
		flipperleft->body->ApplyForceToCenter(force, 1);
		revoluteJointDef_left.lowerAngle = 30 * DEGTORAD;

		App->audio->PlayFx(fx_flipper);
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		b2Vec2 force = b2Vec2(0, -200);
		flipperright->body->ApplyForceToCenter(force, 1);
		revoluteJointDef_right.lowerAngle = 30 * DEGTORAD;

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
		App->physics->ChangeFilter(App->scene_intro->cylinder->body,App->scene_intro->CATEGORY_NOTMAIN_PINBALL );
	}

	return UPDATE_CONTINUE;
}



