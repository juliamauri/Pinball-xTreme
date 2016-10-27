#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModulePlayer.h"
#include "ModuleWindow.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	backgound_shape = backgound_border = imgthrower = imgthrowercomplement = righttube_up = righttube_down = imgreboter = NULL;

	sensoredball_lost = sensoredball_enter_RT = sensoredball_end_RT = reboted = false;

	CATEGORY_MAIN_PINBALL = -1;
	CATEGORY_NOTMAIN_PINBALL = -2;
	CATEGORY_TRANSPARENCE = 3;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;
	
	//Background
	backgound_shape = App->textures->Load("pinball/backgroundCombined.png");
	backgound_border = App->textures->Load("pinball/backgroundBorder.png");

	SetMainPinballChain();

	//Thrower
	imgthrower = App->textures->Load("pinball/thrower.png");
	thrower = App->physics->CreateRectangle(319, 537, 16, 61, CATEGORY_MAIN_PINBALL);
	
	imgthrowercomplement = App->textures->Load("pinball/throwercomplement.png");
	throwercomplement = App->physics->CreateRectangle(319, 554, 14, 98, CATEGORY_MAIN_PINBALL);
	throwercomplement->listener = this;

	//Left tube
	lefttube_hotel_entry = App->textures->Load("pinball/lefttube_hotel_entry.png");
	lefttube_hotel = App->textures->Load("pinball/lefttube_hotel.png");
	lefttube_below_exit = App->textures->Load("pinball/lefttube_below_exit.png");
	lefttube_above_exit = App->textures->Load("pinball/lefttube_above_exit.png");
	SetLeftTubeChain();

	fx_lefttube = App->audio->LoadFx("pinball/Audio/Left_Tube.wav");

	//Right tube
	righttube_up = App->textures->Load("pinball/rightuppipe.png");
	righttube_down = App->textures->Load("pinball/rightdownpipe.png");
	
	SetRightTubeChain();

	fx_righttube = App->audio->LoadFx("pinball/Audio/Righ_tube.wav");

	//Reboter
	p2List_item<PhysBody*>* item;
	reboters.add(App->physics->CreateCircle(250,120,14,CATEGORY_MAIN_PINBALL,2.0f,false,false));
	reboters.add(App->physics->CreateCircle(216, 158, 14, CATEGORY_MAIN_PINBALL,2.0f, false, false));
	reboters.add(App->physics->CreateCircle(170, 118, 14, CATEGORY_MAIN_PINBALL,2.0f, false, false));
	imgreboter = App->textures->Load("pinball/reboter.png");

	item = reboters.getFirst();

	fx_reboter = App->audio->LoadFx("pinball/Audio/Reboter.wav");

	while (item != nullptr)
	{
		item->data->listener = this;
		item = item->next;
	}

	//Sensors
	sensorball_lost = App->physics->CreateRectangleSensor(157, 600, 105, 50);

	sensorball_enter_left = App->physics->CreateRectangleSensor(75,247,32,15,-50);
	sensorball_end_left = App->physics->CreateRectangleSensor(48, 444, 18, 16, 0);
	
	sensorball_enter_RT = App->physics->CreateRectangleSensor(263, 203, 28, 12, 37);
	sensorball_end_RT = App->physics->CreateRectangleSensor(266, 465, 18, 10);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	App->textures->Unload(backgound_border);
	App->textures->Unload(backgound_shape);

	App->textures->Unload(imgthrower);
	App->textures->Unload(imgthrowercomplement);

	App->textures->Unload(lefttube_hotel_entry);
	App->textures->Unload(lefttube_hotel);
	App->textures->Unload(lefttube_below_exit);
	App->textures->Unload(lefttube_above_exit);

	App->textures->Unload(righttube_up);
	App->textures->Unload(righttube_down);

	App->textures->Unload(imgreboter);

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	// All draw functions ------------------------------------------------------
	
	//BackGround
	App->renderer->Blit(backgound_shape, 0, 0);

	//Left tube entry
	App->renderer->Blit(lefttube_hotel_entry, 12, 25);
	App->renderer->Blit(lefttube_hotel, 12, 25);
	//Left tube exit
	App->renderer->Blit(lefttube_below_exit, 12, 25);
	App->renderer->Blit(lefttube_above_exit, 12, 25);

	//Right tube down
		App->renderer->Blit(righttube_down, 222, 104);
	
	if (App->player->ball != nullptr)
	{//ball
		int x, y;
		App->player->ball->GetPosition(x, y);
		App->renderer->Blit(App->player->imgball, x, y);
	}

	//flippers
	{
		App->renderer->Blit(App->player->imgflipperleft, 93, 529);

		App->renderer->Blit(App->player->imgflipperright, 171, 530);
	}

	//Reboters
	{
		p2List_item<PhysBody*>* reboter = reboters.getFirst();

		while (reboter != nullptr)
		{
			int x, y;
			reboter->data->GetPosition(x,y);
			App->renderer->Blit(imgreboter, x, y);
			reboter = reboter->next;
		}
	}

	{//trowercomplement
		if (App->player->trower == true)
		{
			int x, y;
			App->scene_intro->throwercomplement->GetPosition(x, y);
			App->renderer->Blit(App->scene_intro->imgthrowercomplement, x, y + 2 - (int)App->player->veloy*1.5f);
		}
		else
		{
			int x, y;
			throwercomplement->GetPosition(x, y);
			if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_IDLE)
				App->renderer->Blit(imgthrowercomplement, x, y + 2);
		}
	}

	//BackGround Mark
	App->renderer->Blit(backgound_border, 0, 0);

	{//trower
		int x, y;
		thrower->GetPosition(x, y);
		App->renderer->Blit(imgthrower, x, y - 3);
	}

	{//Right tube up
		App->renderer->Blit(righttube_up, 222, 104);
	}

	// sensors -------------------
	if (sensoredball_lost == true)
	{
		App->player->RestorePosBall();
		sensoredball_lost = false;
	}

	/*while (sensoredball_enter_left == true)
	{
		CATEGORY_TRANSPARENCE;
	
	
	else
		App->player->LeftTubeBallEnter();
	}
	if (sensoredball_enter_left2 == true)
	{
		App->player->Reset();
	}
	*/
	if (sensoredball_end_left == true)
	{
		App->player->Reset();
	}

	if (sensoredball_enter_RT == true)
	{
		App->player->RightTubeBallEnter();
		App->audio->PlayFx(fx_righttube);
		sensoredball_enter_RT = false;
	}

	if (sensoredball_end_RT == true)
	{
		App->player->RightTubeBallExit();
		sensoredball_end_RT = false;
	}

	if (sensoredball_enter_left == true)
	{
		App->audio->PlayFx(fx_lefttube);
		sensoredball_enter_left = false;
	}

	if (reboted == true)
	{
		App->player->score += 1000;
		App->audio->PlayFx(fx_reboter);
		reboted = false;
	}
	
	//Window Title
	p2SString title("BS: %i - Score: %i", App->player->best_score, App->player->score);

	App->window->SetTitle(title.GetString());

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	if (bodyB == sensorball_lost)
		sensoredball_lost = true;

	if (bodyB == sensorball_enter_left)
		sensoredball_enter_left = true;

	if (bodyB == sensorball_enter_left2)
	{
		sensoredball_enter_left2 == true;
	}
	if (bodyB == sensorball_end_left)
		sensoredball_end_left = true;

	if (bodyB == sensorball_enter_RT)
		sensoredball_enter_RT = true;

	if (bodyB == sensorball_end_RT)
		sensoredball_end_RT = true;

	p2List_item<PhysBody*>* item = reboters.getFirst();

	while (item != nullptr)
	{
		if (bodyB == item->data)
		{
			reboted = true;
		}
		item = item->next;
	}

	if (bodyB == throwercomplement)
		canthrow = true;
	else
		canthrow = false;

}

void ModuleSceneIntro::SetMainPinballChain()
{
	int pinball[148] = {
		153, 19,
		172, 18,
		190, 18,
		211, 21,
		227, 25,
		249, 32,
		265, 42,
		281, 55,
		295, 71,
		307, 89,
		313, 103,
		318, 117,
		322, 133,
		325, 150,
		326, 166,
		326, 184,
		326, 564,
		311, 564,
		311, 301,
		307, 301,
		305, 306,
		305, 397,
		302, 401,
		298, 402,
		294, 402,
		291, 402,
		287, 398,
		287, 308,
		285, 306,
		282, 306,
		267, 340,
		266, 346,
		266, 388,
		267, 394,
		298, 424,
		300, 428,
		302, 430,
		302, 580,
		282, 580,
		282, 537,
		279, 533,
		273, 533,
		208, 565,
		208, 586,
		107, 586,
		107, 564,
		43, 534,
		39, 534,
		34, 537,
		34, 577,
		16, 577,
		16, 431,
		19, 425,
		49, 398,
		53, 391,
		53, 358,
		50, 337,
		37, 297,
		31, 276,
		25, 248,
		23, 226,
		21, 212,
		21, 171,
		23, 146,
		31, 117,
		41, 95,
		53, 77,
		68, 59,
		83, 47,
		97, 38,
		110, 31,
		124, 26,
		138, 22,
		149, 20
	};
	pinballtable.add(App->physics->CreateChain(0, 0, pinball, 148, CATEGORY_MAIN_PINBALL));

	int pinball_part1[120] = {
		47, 258,
		46, 250,
		43, 240,
		40, 224,
		38, 198,
		38, 161,
		41, 144,
		45, 128,
		52, 110,
		63, 93,
		74, 79,
		90, 66,
		106, 55,
		122, 48,
		138, 43,
		146, 42,
		151, 42,
		165, 56,
		169, 62,
		169, 91,
		179, 110,
		179, 164,
		175, 169,
		117, 213,
		113, 213,
		107, 204,
		102, 193,
		101, 184,
		101, 177,
		104, 169,
		111, 161,
		113, 157,
		113, 151,
		108, 145,
		102, 143,
		95, 146,
		88, 153,
		83, 162,
		80, 169,
		79, 176,
		79, 189,
		80, 197,
		83, 207,
		90, 222,
		93, 226,
		93, 230,
		90, 231,
		54, 258,
		53, 261,
		50, 261,
		50, 261,
		50, 261,
		90, 231,
		90, 231,
		54, 258,
		53, 261,
		50, 261,
		50, 261,
		50, 261,
		50, 261
	};
	pinballtable.add(App->physics->CreateChain(0, 0, pinball_part1, 100, CATEGORY_MAIN_PINBALL));

	int pinball_part2[82] = {
		196, 206,
		192, 221,
		191, 225,
		194, 227,
		197, 227,
		202, 221,
		208, 219,
		213, 219,
		219, 222,
		224, 229,
		224, 236,
		221, 242,
		221, 249,
		225, 251,
		253, 182,
		259, 188,
		264, 193,
		270, 200,
		257, 262,
		259, 266,
		263, 266,
		278, 249,
		288, 236,
		289, 232,
		288, 228,
		281, 216,
		273, 201,
		261, 184,
		249, 170,
		239, 161,
		226, 153,
		214, 148,
		213, 145,
		210, 145,
		209, 148,
		209, 151,
		207, 158,
		205, 167,
		203, 176,
		200, 189,
		197, 201
	};
	pinballtable.add(App->physics->CreateChain(0, 0, pinball_part2, 82, CATEGORY_MAIN_PINBALL));

	int pinball_part3[49] = {
		263, 63,
		279, 79,
		293, 98,
		301, 114,
		305, 127,
		308, 142,
		309, 153,
		309, 166,
		308, 177,
		305, 190,
		302, 198,
		298, 205,
		295, 206,
		292, 204,
		287, 192,
		276, 172,
		269, 161,
		259, 149,
		247, 135,
		245, 130,
		246, 86,
		247, 65,
		249, 62,
		252, 58
	};
	pinballtable.add(App->physics->CreateChain(0, 0, pinball_part3, 49, CATEGORY_MAIN_PINBALL));

	//Barra per on passa la bola al llançar-la
	/*
	int pinball_part4[26] = {
	280, 294,
	292, 267,
	302, 241,
	309, 222,
	313, 207,
	317, 208,
	315, 220,
	312, 231,
	306, 248,
	302, 258,
	295, 272,
	287, 289,
	283, 296
	};
	pinballtable.add(App->physics->CreateChain(0, 0, pinball_part4, 26, true));
	*/

	int pinball_part5[24] = {
		34, 455,
		34, 501,
		35, 505,
		94, 533,
		97, 531,
		96, 529,
		92, 527,
		38, 503,
		38, 448,
		37, 446,
		35, 447,
		34, 449
	};
	pinballtable.add(App->physics->CreateChain(0, 0, pinball_part5, 24, CATEGORY_MAIN_PINBALL));

	int pinball_part6[20] = {
		278, 456,
		278, 502,
		221, 529,
		221, 532,
		224, 532,
		282, 505,
		282, 448,
		281, 446,
		279, 447,
		278, 449
	};
	pinballtable.add(App->physics->CreateChain(0, 0, pinball_part6, 20, CATEGORY_MAIN_PINBALL));

	int pinball_part7[26] = {
		186, 77,
		186, 90,
		188, 93,
		191, 94,
		195, 94,
		198, 91,
		199, 88,
		199, 67,
		197, 63,
		194, 61,
		191, 61,
		188, 63,
		186, 66
	};
	pinballtable.add(App->physics->CreateChain(0, 0, pinball_part7, 26, CATEGORY_MAIN_PINBALL));

	int pinball_part8[26] = {
		215, 69,
		215, 85,
		217, 89,
		220, 91,
		224, 91,
		227, 89,
		229, 86,
		229, 66,
		228, 62,
		225, 58,
		220, 58,
		217, 60,
		215, 64
	};
	pinballtable.add(App->physics->CreateChain(0, 0, pinball_part8, 26, CATEGORY_MAIN_PINBALL));
}

void ModuleSceneIntro::SetLeftTubeChain()
{
	int lefttube_coord[118] = {
		50, 252,
		43, 215,
		40, 179,
		41, 143,
		48, 111,
		66, 79,
		88, 56,
		115, 41,
		138, 36,
		161, 33,
		190, 38,
		219, 53,
		245, 78,
		258, 107,
		263, 133,
		261, 163,
		253, 193,
		231, 223,
		208, 249,
		176, 269,
		145, 289,
		120, 308,
		102, 328,
		88, 347,
		77, 372,
		67, 402,
		64, 430,
		62, 457,
		40, 459,
		41, 431,
		43, 403,
		49, 375,
		60, 347,
		74, 324,
		96, 303,
		120, 280,
		160, 252,
		189, 233,
		207, 219,
		226, 200,
		237, 178,
		242, 151,
		242, 125,
		234, 100,
		223, 82,
		205, 69,
		185, 57,
		165, 52,
		142, 54,
		119, 62,
		98, 74,
		81, 89,
		70, 112,
		67, 131,
		64, 148,
		66, 172,
		69, 189,
		74, 212,
		82, 230
	};
	lefttube = App->physics->CreateChain(0, 0, lefttube_coord, 118, CATEGORY_NOTMAIN_PINBALL);
}
void ModuleSceneIntro::SetRightTubeChain()
{
	int righttube_coord[112] = {
		269, 137,
		268, 146,
		281, 149,
		282, 145,
		283, 139,
		283, 129,
		280, 120,
		273, 112,
		265, 108,
		258, 105,
		249, 105,
		243, 107,
		238, 110,
		232, 115,
		227, 121,
		224, 133,
		223, 139,
		225, 146,
		228, 151,
		232, 158,
		283, 213,
		294, 230,
		300, 240,
		300, 253,
		300, 259,
		298, 264,
		268, 323,
		262, 343,
		259, 355,
		258, 369,
		257, 471,
		264, 474,
		270, 474,
		274, 472,
		274, 372,
		275, 364,
		277, 349,
		285, 325,
		304, 291,
		313, 272,
		315, 265,
		315, 251,
		315, 230,
		302, 211,
		287, 192,
		244, 150,
		239, 144,
		238, 139,
		238, 134,
		241, 126,
		245, 122,
		250, 119,
		256, 119,
		263, 122,
		267, 127,
		269, 133
	};
	righttube = App->physics->CreateChain(0, 0, righttube_coord, 112, CATEGORY_NOTMAIN_PINBALL);
}
