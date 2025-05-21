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


	void AlleyCat::createPlatforms() {
		b2ShapeDef sd = b2DefaultShapeDef();
		sd.density              = 1.0f;
		sd.material.friction    = 1.0f;

		constexpr float Wpx = 150.f, Hpx = 1.f;
		float halfW = (Wpx * 0.5f) / BOX_SCALE;
		float halfH = (Hpx * 0.5f) / BOX_SCALE;

		for (auto& c : platformCenters) {
			float bodyCenterYPx = c.y + halfH * BOX_SCALE;
			b2BodyDef bd = b2DefaultBodyDef();
			bd.type     = b2_staticBody;
			bd.position = { c.x / BOX_SCALE, bodyCenterYPx / BOX_SCALE };
			b2BodyId body = b2CreateBody(boxWorld, &bd);
			b2Polygon plat = b2MakeBox(halfW, halfH);
			b2ShapeId shapeId = b2CreatePolygonShape(body, &sd, &plat);
			platformShapes.push_back(shapeId);
		}
	}

	void AlleyCat::createCatEntity() {
		// Box2D body
		b2BodyDef bd = b2DefaultBodyDef();
		bd.type = b2_dynamicBody;
		bd.position = {10.0f/BOX_SCALE, WIN_HEIGHT / BOX_SCALE - 1.0f};
		auto body = b2CreateBody(boxWorld, &bd);
		b2ShapeDef sd = b2DefaultShapeDef(); sd.density=1; sd.material.friction=0.3f;
		SDL_FPoint size{64, 64};
		float halfW = (size.x * 0.5f) / BOX_SCALE;
		float halfH = (size.y * 0.5f) / BOX_SCALE;

		b2Polygon box = b2MakeBox(halfW, halfH);
		b2CreatePolygonShape(body, &sd, &box);

		Entity cat = Entity::create();
		SDL_FRect part{6, 44, 26, 20};

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

		bodyDef.position = {WIN_WIDTH / 2.0f / BOX_SCALE, WIN_HEIGHT / BOX_SCALE - 1.0f};
		box = b2MakeBox(WIN_WIDTH / 2.0f / BOX_SCALE, 10.0f / BOX_SCALE);
		b2BodyId ground = b2CreateBody(boxWorld, &bodyDef);
		b2CreatePolygonShape(ground, &shapeDef, &box);

		box = b2MakeBox(5, WIN_HEIGHT/2.0f/BOX_SCALE);

		bodyDef.position = {-5.5f ,WIN_HEIGHT/2.0f/BOX_SCALE};
		b2BodyId left = b2CreateBody(boxWorld, &bodyDef);
		b2CreatePolygonShape(left, &shapeDef, &box);

		bodyDef.position = {WIN_WIDTH/BOX_SCALE + 4.5f, WIN_HEIGHT/2.0f/BOX_SCALE};
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
				intent.up    = keys[SDL_SCANCODE_UP];
			}
		}
	}

	void AlleyCat::moveSystem()
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
				constexpr float speed     = DESIRED_PX_SPEED / BOX_SCALE;
				constexpr float jumpSpeed = std::sqrt(2*9.8f*(DESIRED_PX_JUMP/BOX_SCALE));
				constexpr float groundThreshold = 0.01f;

				velocity.x = 0;
				if (intent.left) velocity.x = -speed;
				if (intent.right) velocity.x = speed;
				if (intent.up && std::fabs(velocity.y) < groundThreshold) {
					velocity.y = -jumpSpeed;
					b2Body_SetLinearVelocity(col.body, velocity);// negative = up against gravity
				}

				b2Body_SetLinearVelocity(col.body, velocity);
			}
		}
	}
// void AlleyCat::platformsSystem()
// {
//
//     float catY = 0.f;
// 	std::cout<< "catY: " << catY << std::endl;
//     static const Mask catMask = MaskBuilder()
//         .set<Intent>().set<Collider>().set<Transform>().set<Drawable>().build();
//     for (ent_type e{0}; e.id <= World::maxId().id; ++e.id) {
//
//         if (!World::mask(e).test(catMask)) continue;
//     	std::cout << "there is entity in platformSystem loop with id:" << e.id <<std::endl;
//         catY = World::getComponent<Transform>(e).p.y;
//         break;
//     }
//     for (size_t i = 0; i < NumDefs; ++i) {
//         auto& slot = spawnedPlatforms[i];
//         const auto& def = platformDefs[i];
//
//         bool shouldExist = (catY < def.y);
//     	std::cout << "catY: " << catY << " def.y: " << def.y <<"platform should exists? : "<<shouldExist  << std::endl;
//         bool doesExist   = (slot.ent.id != 0);
//
//         if (shouldExist && !doesExist) {
//         	std::cout<<"creating platform" << std::endl;
//             float halfW = (def.w * .5f) / BOX_SCALE;
//         	std::cout<<"halfW: " << halfW << std::endl;
//             float halfH = (def.h * .5f) / BOX_SCALE;
//         	std::cout<<"halfH: " << halfH << std::endl;
//             float cy    = def.y + halfH * BOX_SCALE;
//         	std::cout<<"cy: " << cy << std::endl;
//
//             b2BodyDef bd = b2DefaultBodyDef();
//             bd.type     = b2_staticBody;
//             bd.position = { def.x/BOX_SCALE, cy/BOX_SCALE };
//             slot.body   = b2CreateBody(boxWorld, &bd);
// 			std::cout<<"created platform" << std::endl;
//             b2ShapeDef sd = b2DefaultShapeDef();
//             sd.density           = 1.0f;
//             sd.material.friction = 1.0f;
//             b2Polygon plat = b2MakeBox(halfW, halfH);
//             b2CreatePolygonShape(slot.body, &sd, &plat);
//         	std::cout<<"created polygon" << std::endl;
//
//             auto ent = Entity::create();
//             ent.addAll(
//               Transform{{def.x-def.w*.5f,def.y-def.h*.5f},0.f},
//               Platform{def.w,def.h}
//             );
//
//         	std::cout<<"created platform entity" << std::endl;
//             // link body→entity so we can find it later
//             b2Body_SetUserData(slot.body, new ent_type{ent.entity()});
//
//         }
//         else if (!shouldExist && doesExist) {
//             b2BodyId b = slot.body;
//             auto *ePtr = static_cast<ent_type*>(b2Body_GetUserData(b));
//             if (ePtr) {
//                 World::destroyEntity(*ePtr);
//                 delete ePtr;
//             }
//         	b2DestroyBody(b);
//             slot = {};
//         }
//     }
// }

	void AlleyCat::drawSystem() {
		// Draw background
		SDL_RenderClear(ren);
		SDL_RenderTexture(ren, bgTex, nullptr, nullptr);

		static const Mask entityMask = MaskBuilder()
			.set<Transform>()
			.set<Drawable>()
			.set<Collider>()
			.build();

		for (ent_type e{0}; e.id <= World::maxId().id; ++e.id) {
			if (!World::mask(e).test(entityMask)) continue;
			const auto& t = World::getComponent<Transform>(e);
			const auto& d = World::getComponent<Drawable>(e);

			SDL_FRect dst{
				t.p.x ,
				t.p.y,
				d.size.x,
				d.size.y
			};
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
		//createBackgroundEntity();
		createPlatforms();
		createCatEntity();
	}

	AlleyCat::~AlleyCat()
    {
    	if (b2World_IsValid(boxWorld))
    		b2DestroyWorld(boxWorld);
    	if (tex)
    		SDL_DestroyTexture(tex);
    	if (ren)
    		SDL_DestroyRenderer(ren);
    	if (win)
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

			moveSystem();
			boxSystem(deltaTime);
			//platformsSystem();
			drawSystem();

			auto end = SDL_GetTicks();
			deltaTime = (end - start) / 1000.0f;

			if (end - start < GAME_FRAME)
				SDL_Delay(GAME_FRAME - (end - start));
			start = end;
		}
	}
}