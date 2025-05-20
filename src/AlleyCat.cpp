#include "AlleyCat.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <box2d/box2d.h>
#include <iostream>

using namespace std;
using namespace bagel;

namespace AlleyCatGame
{
	bool AlleyCat::prepareWindowAndTexture()
	{
		if (!SDL_Init(SDL_INIT_VIDEO)) {
			std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
			return false;
		}
		if (!SDL_CreateWindowAndRenderer("Alley Cat", WIN_WIDTH, WIN_HEIGHT, 0, &win, &ren)) {
			std::cerr << "SDL_CreateWindowAndRenderer Error: " << SDL_GetError() << "\n";
			return false;
		}

		// Load background texture
		bgTex = IMG_LoadTexture(ren, "res/cat_bg.png");
		if (!bgTex) {
			std::cerr << "IMG_LoadTexture Error: " << SDL_GetError() << "\n";
			return false;
		}
		// Load cat sprite sheet
		catTex = IMG_LoadTexture(ren, "res/cat.png");
		if (!catTex) {
			std::cerr << "IMG_LoadTexture(cat) Error: " << SDL_GetError() << "\n";
			return false;
		}
		return true;
	}

	void AlleyCat::prepareBoxWorld()
	{
		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity = {0.0f, 9.8f};
		boxWorld = b2CreateWorld(&worldDef);
	}

	void AlleyCat::createBackgroundEntity()
	{
		auto e = Entity::create();
		SDL_FRect full{0.0f, 0.0f, static_cast<float>(WIN_WIDTH), static_cast<float>(WIN_HEIGHT)};
		e.addAll(
			Transform{{0.0f, 0.0f}, 0.0f},
			Drawable{full, {full.w, full.h}}
		);
	}

	// void AlleyCat::createCatEntity()
	// {
	// 	// Create Box2D dynamic body
	// 	b2BodyDef bodyDef = b2DefaultBodyDef();
	// 	bodyDef.type = b2_dynamicBody;
	// 	bodyDef.position = {
	// 		(WIN_WIDTH / 2.0f) / BOX_SCALE,
	// 		(WIN_HEIGHT / 2.0f) / BOX_SCALE
	// 	};
	//
	// 	b2ShapeDef shapeDef = b2DefaultShapeDef();
	// 	shapeDef.density = 1.0f;
	// 	shapeDef.material.friction = 0.3f;
	// 	shapeDef.material.restitution = 0.0f;
	//
	// 	b2Polygon box = b2MakeBox(
	// 		25.0f / BOX_SCALE, 25.0f / BOX_SCALE); // half-extents = 50x50 box
	//
	// 	b2BodyId body = b2CreateBody(boxWorld, &bodyDef);
	// 	b2CreatePolygonShape(body, &shapeDef, &box);
	//
	// 	// Create ECS entity
	// 	Entity freddy = Entity::create();
	// 	freddy.addAll(
	// 		Transform{{WIN_WIDTH / 2.0f, WIN_HEIGHT / 2.0f}, 0},
	// 		Drawable{{}, {50, 50}},
	// 		Intent{},
	// 		Collider{body}
	// 	);
	//
	// 	// Link Box2D body back to ECS entity (optional)
	// 	b2Body_SetUserData(body, new ent_type{freddy.entity()});
	// }
	void AlleyCat::createCatEntity() {
		// Box2D body
		b2BodyDef bd = b2DefaultBodyDef();
		bd.type = b2_dynamicBody;
		bd.position = {WIN_WIDTH/2.f/BOX_SCALE, WIN_HEIGHT/2.f/BOX_SCALE};
		auto body = b2CreateBody(boxWorld, &bd);
		b2ShapeDef sd = b2DefaultShapeDef(); sd.density=1; sd.material.friction=0.3f;
		b2Polygon box = b2MakeBox(25.f/BOX_SCALE,25.f/BOX_SCALE);
		b2CreatePolygonShape(body, &sd, &box);

		// ECS entity
		Entity cat = Entity::create();
		// first frame of sprite sheet
		SDL_FRect part{6, 44, 26, 20};
		SDL_FPoint size{64, 64};
		SDL_FPoint pos{WIN_WIDTH/2.f - size.x/2.f, WIN_HEIGHT/2.f - size.y/2.f};
		cat.addAll(
			Transform{pos,0},
			Drawable{part,size},
			Intent{},
			Collider{body}
		);
		b2Body_SetUserData(body, new ent_type{cat.entity()});
	}


	void AlleyCat::createWalls()
	{
		b2BodyDef bodyDef = b2DefaultBodyDef();
		bodyDef.type = b2_staticBody;
		b2ShapeDef shapeDef = b2DefaultShapeDef();
		shapeDef.density = 1.0f;
		b2Polygon box;

		bodyDef.position = {WIN_WIDTH / 2.0f / BOX_SCALE, WIN_HEIGHT / BOX_SCALE - 10.0f};
		box = b2MakeBox(WIN_WIDTH / 2.0f / BOX_SCALE, 10.0f / BOX_SCALE);
		b2BodyId ground = b2CreateBody(boxWorld, &bodyDef);
		b2CreatePolygonShape(ground, &shapeDef, &box);

		box = b2MakeBox(5, WIN_HEIGHT/2.0f/BOX_SCALE);

		bodyDef.position = {-7.50f ,WIN_HEIGHT/2.0f/BOX_SCALE};
		b2BodyId left = b2CreateBody(boxWorld, &bodyDef);
		b2CreatePolygonShape(left, &shapeDef, &box);

		bodyDef.position = {WIN_WIDTH/BOX_SCALE + 2.5f, WIN_HEIGHT/2.0f/BOX_SCALE};
		b2BodyId right = b2CreateBody(boxWorld, &bodyDef);
		b2CreatePolygonShape(right, &shapeDef, &box);
	}

	void AlleyCat::boxSystem(float deltaTime)
	{
		static const Mask mask = MaskBuilder()
		.set<Transform>()
		.set<Collider>()
		.build();

		// Advance Box2D physics
		b2World_Step(boxWorld, deltaTime, 8);

		for (ent_type e{0}; e.id <= World::maxId().id; ++e.id) {
			if (World::mask(e).test(mask)) {
				const auto& col = World::getComponent<Collider>(e);
				auto& t = World::getComponent<Transform>(e);

				b2Transform boxTransform = b2Body_GetTransform(col.body);
				t.p.x = boxTransform.p.x * BOX_SCALE;
				t.p.y = boxTransform.p.y * BOX_SCALE;
				t.angle = b2Rot_GetAngle(boxTransform.q); // (optional rotation)
			}
		}
	}

	void AlleyCat::inputSystem()
	{
		static const Mask mask = MaskBuilder()
			.set<Intent>()
			.build();

		const bool* keys = SDL_GetKeyboardState(nullptr);

		for (ent_type e{0}; e.id <= World::maxId().id; ++e.id) {
			if (World::mask(e).test(mask)) {
				auto& intent = World::getComponent<Intent>(e);
				intent.left = keys[SDL_SCANCODE_LEFT];
				intent.right = keys[SDL_SCANCODE_RIGHT];
			}
		}
	}

	void AlleyCat::moveSystem(float deltaTime)
	{
		static const Mask mask = MaskBuilder()
	   .set<Intent>()
	   .set<Collider>()
	   .build();

		for (ent_type e{0}; e.id <= World::maxId().id; ++e.id) {
			if (World::mask(e).test(mask)) {
				const auto& intent = World::getComponent<Intent>(e);
				const auto& col = World::getComponent<Collider>(e);

				b2Vec2 velocity = b2Body_GetLinearVelocity(col.body);
				float speed = 7.0f;

				velocity.x = 0;
				if (intent.left) velocity.x = -speed;
				if (intent.right) velocity.x = speed;

				b2Body_SetLinearVelocity(col.body, velocity);
			}
		}
	}
	void AlleyCat::drawSystem() {
		// Draw background
		SDL_RenderClear(ren);
		SDL_RenderTexture(ren, bgTex, nullptr, nullptr);

		// Draw all physics-driven entities (cat, etc.)
		static const Mask entityMask = MaskBuilder()
			.set<Transform>()
			.set<Drawable>()
			.set<Collider>()
			.build();

		for (ent_type e{0}; e.id <= World::maxId().id; ++e.id) {
			if (!World::mask(e).test(entityMask)) continue;
			const auto& t = World::getComponent<Transform>(e);
			const auto& d = World::getComponent<Drawable>(e);

			SDL_FRect dst{ t.p.x, t.p.y, d.size.x, d.size.y };
			SDL_RenderTexture(ren, catTex, &d.part, &dst);
		}

		SDL_RenderPresent(ren);
	}


	bool AlleyCat::valid() const
	{
		return ren != nullptr;
	}

    AlleyCat::AlleyCat()
	{
		if (!prepareWindowAndTexture()) return;
    	SDL_srand(time(nullptr));

		prepareBoxWorld();
		createWalls();
		createBackgroundEntity();
		createCatEntity();
	}

	AlleyCat::~AlleyCat()
    {
    	if (b2World_IsValid(boxWorld))
    		b2DestroyWorld(boxWorld);
    	if (tex != nullptr)
    		SDL_DestroyTexture(tex);
    	if (ren != nullptr)
    		SDL_DestroyRenderer(ren);
    	if (win != nullptr)
    		SDL_DestroyWindow(win);
		if (bgTex)
			SDL_DestroyTexture(bgTex);

    	SDL_Quit();
    }

	void AlleyCat::run()
	{
		auto start = SDL_GetTicks();
		float deltaTime = 0.0f;
		bool quit = false;

		while (!quit)
		{

			SDL_Event e;
			while (SDL_PollEvent(&e))
			{
				if ((e.type == SDL_EVENT_QUIT) ||
				   ((e.type == SDL_EVENT_KEY_DOWN) && (e.key.scancode == SDL_SCANCODE_ESCAPE)))
					quit = true;
			}

			inputSystem();
			moveSystem(deltaTime);
			boxSystem(deltaTime);
			drawSystem();

			auto end = SDL_GetTicks();
			deltaTime = (end - start) / 1000.0f;

			if (end - start < GAME_FRAME)
				SDL_Delay(GAME_FRAME - (end - start));
			start = end;
		}
	}
}