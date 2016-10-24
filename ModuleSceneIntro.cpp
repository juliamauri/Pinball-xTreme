#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModulePlayer.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	backgound_shape = backgound_border = imgthrower = imgthrowercomplement = NULL;

	sensoredball_lost = false;

	CATEGORY_MAIN_PINBALL = -1;
	CATEGORY_NOTMAIN_PAINBALL = -2;
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

	//Sensors
	sensorball_lost = App->physics->CreateRectangleSensor(157, 590, 105, 50);



	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	App->textures->Unload(backgound_border);
	App->textures->Unload(backgound_shape);

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	// All draw functions ------------------------------------------------------
	
	//BackGround
	App->renderer->Blit(backgound_shape, 0, 0);

	if (App->player->ball != nullptr)
	{//ball
		int x, y;
		App->player->ball->GetPosition(x, y);
		App->renderer->Blit(App->player->imgball, x, y);
	}

	{//trowercomplement
		int x, y;
		throwercomplement->GetPosition(x, y);
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_IDLE)
			App->renderer->Blit(imgthrowercomplement, x, y + 2);
	}

	//BackGround Mark
	App->renderer->Blit(backgound_border, 0, 0);

	{//trower
		int x, y;
		thrower->GetPosition(x, y);
		App->renderer->Blit(imgthrower, x, y - 3);
	}

	// sensors -------------------
	if (sensoredball_lost == true)
	{
		App->player->RestorePosBall();
		sensoredball_lost = false;
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	if (bodyB == sensorball_lost)
		sensoredball_lost = true;
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
