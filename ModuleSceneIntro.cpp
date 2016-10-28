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

	sensoredball_lost = sensoredball_enter_RT = sensoredball_end_RT = reboted = sensored_cylinder = sensoredball_enter_left = sensoredball_points_left = sensoredball_end_left = sensoredball_enterext_left = lefttube_active = false;

	CATEGORY_MAIN_PINBALL = -1;
	CATEGORY_NOTMAIN_PINBALL = -2;
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
	reboters.add(App->physics->CreateCircle(249,120,14,CATEGORY_MAIN_PINBALL,2.0f,false,false));
	reboters.add(App->physics->CreateCircle(216, 151, 14, CATEGORY_MAIN_PINBALL,2.0f, false, false));
	reboters.add(App->physics->CreateCircle(173, 118, 14, CATEGORY_MAIN_PINBALL,2.0f, false, false));
	imgreboter = App->textures->Load("pinball/reboter.png");

	item = reboters.getFirst();

	fx_reboter = App->audio->LoadFx("pinball/Audio/Reboter.wav");

	while (item != nullptr)
	{
		item->data->listener = this;
		item = item->next;
	}

	//Sensors
	sensorball_lost = App->physics->CreateRectangleSensor(157, 600, 350, 50);

	sensorball_enter_left = App->physics->CreateRectangleSensor(50,164,11,15,90);
	sensorball_enterext_left = App->physics->CreateRectangleSensor(50, 137, 11, 5, 28);
	sensorball_points_left = App->physics->CreateRectangleSensor(190,50,10,5,90);
	sensorball_end_left = App->physics->CreateRectangleSensor(48, 465, 10, 10);

	sensor_cylinder = App->physics->CreateRectangleSensor(320, 165, 10, 10);
	sensorball_enter_RT = App->physics->CreateRectangleSensor(263, 205,28, 12, 37);
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
	App->renderer->Blit(lefttube_hotel_entry, 12, 28);
	App->renderer->Blit(lefttube_hotel, 12, 28);
	//Left tube exit
	App->renderer->Blit(lefttube_below_exit, 12, 28);

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
		float32 angle;

		angle = App->player->flipperleft->body->GetAngle();
		App->renderer->Blit(App->player->imgflipperleft, 93, 529,NULL,1.0f,RADTODEG * angle - 17,10,10);

		angle = App->player->flipperright->body->GetAngle();
		App->renderer->Blit(App->player->imgflipperright, 171, 530,NULL,1.0f, RADTODEG *angle +18,45,10);
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

	//Left tube entry up
	App->renderer->Blit(lefttube_above_exit, 12, 28);

	{//Right tube up
		App->renderer->Blit(righttube_up, 222, 104);
	}

	// sensors -------------------
	if (sensoredball_lost)
	{
		App->player->RestorePosBall();
		App->physics->ChangeFilter(cylinder->body, CATEGORY_NOTMAIN_PINBALL);
		sensoredball_lost = false;
	}

	if (sensored_cylinder)
	{
		App->physics->ChangeFilter(cylinder->body, CATEGORY_MAIN_PINBALL);
		sensored_cylinder = false;
	}
	if (sensoredball_enter_RT)
	{
		if (!lefttube_active)
		{
			App->player->RightTubeBallEnter();
			App->audio->PlayFx(fx_righttube);
			App->player->score += 5000;
		}
		sensoredball_enter_RT = false;
	}

	if (sensoredball_end_RT)
	{
		App->player->RightTubeBallExit();
		sensoredball_end_RT = false;
	}

	if (sensoredball_enter_left)
	{
		App->player->LeftTubeBallEnter();
		lefttube_active = true;
		sensoredball_enter_left = false;
	}

	if (sensoredball_enterext_left)
	{
		App->player->LeftTubeBallEnterExt();
		sensoredball_enterext_left = false;
	}

	if (sensoredball_points_left)
	{
		if (lefttube_active)
		{
			App->audio->PlayFx(fx_lefttube);
			App->player->score += 5000;
		}
		sensoredball_points_left = false;
	}

	if (sensoredball_end_left)
	{
		App->player->LeftTubeBallExit();
		lefttube_active = false;
		sensoredball_end_left = false;
	}

	if (reboted)
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

	if (bodyB == sensor_cylinder)
		sensored_cylinder = true;

	if (bodyB == sensorball_enter_left)
		sensoredball_enter_left = true;

	if (bodyB == sensorball_enterext_left)
		sensoredball_enterext_left = true;

	if (bodyB == sensorball_points_left)
		sensoredball_points_left = true;

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

	int pinball_part1[158] = {
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
		92, 228,
		89, 229,
		88, 236,
		92, 240,
		100, 250,
		103, 257,
		102, 261,
		99, 262,
		95, 262,
		81, 243,
		73, 231,
		67, 218,
		63, 208,
		60, 179,
		59, 169,
		59, 155,
		42, 150,
		41, 155,
		41, 169,
		41, 180,
		42, 202,
		43, 219,
		46, 237,
		49, 249,
		51, 254,
		56, 256,
		61, 261,
		65, 267,
		71, 271,
		69, 274,
		68, 280,
		66, 284,
		60, 284,
		49, 265,
		48, 259,
		45, 250
	};
	pinballtable.add(App->physics->CreateChain(0, 0, pinball_part1, 158, CATEGORY_MAIN_PINBALL));

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

	int paintball_part4[28] = {
		279, 296,
		290, 276,
		301, 254,
		308, 234,
		314, 217,
		319, 199,
		324, 170,
		327, 175,
		322, 201,
		318, 219,
		312, 235,
		303, 257,
		294, 277,
		283, 296
	};
	cylinder = App->physics->CreateChain(0, 0, paintball_part4, 28, CATEGORY_NOTMAIN_PINBALL);
	pinballtable.add(cylinder);
}


void ModuleSceneIntro::SetLeftTubeChain()
{
		int lefttube_coord[154] = {
			59, 155,
			40, 150,
			45, 113,
			53, 98,
			61, 86,
			68, 76,
			77, 66,
			90, 56,
			107, 46,
			125, 41,
			147, 37,
			168, 38,
			191, 41,
			214, 52,
			235, 70,
			247, 86,
			254, 95,
			263, 118,
			266, 138,
			265, 159,
			261, 180,
			253, 201,
			236, 225,
			219, 243,
			200, 258,
			179, 273,
			147, 292,
			133, 303,
			111, 321,
			98, 338,
			90, 349,
			82, 361,
			76, 375,
			71, 390,
			68, 401,
			65, 413,
			63, 428,
			62, 444,
			61, 465,
			61, 473,
			57, 480,
			50, 482,
			43, 480,
			41, 476,
			41, 447,
			45, 415,
			52, 384,
			62, 360,
			75, 337,
			93, 312,
			124, 287,
			148, 270,
			176, 253,
			201, 236,
			220, 217,
			238, 190,
			245, 169,
			248, 155,
			249, 137,
			245, 124,
			239, 107,
			229, 93,
			215, 78,
			194, 65,
			182, 57,
			176, 55,
			160, 54,
			147, 54,
			132, 55,
			116, 61,
			103, 69,
			89, 81,
			82, 92,
			75, 104,
			69, 118,
			65, 127,
			60, 143
	};
	lefttube = App->physics->CreateChain(0, 0, lefttube_coord, 154, CATEGORY_NOTMAIN_PINBALL);

	int lefttube_intermediate_coord[24] = {
		60, 183,
		41, 187,
		40, 163,
		41, 141,
		44, 126,
		49, 111,
		56, 100,
		73, 102,
		67, 118,
		62, 131,
		59, 145,
		58, 165
	};
	lefttube_intermediate = App->physics->CreateChain(0, 0, lefttube_intermediate_coord, 20, CATEGORY_NOTMAIN_PINBALL);

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
