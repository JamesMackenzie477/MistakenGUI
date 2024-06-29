#include "Mistaken.h"
#include <string>

using namespace std;

// stores the overlay window object
Overlay* gOverlay;

// cryengine globals
SSystemGlobalEnvironment* gEnv;
IGameFramework* gGameFramework;
IActorSystem* gActorSystem;
IVehicleSystem* gVehicleSystem;
IItemSystem* gItemSystem;
IActor* gPlayerActor;
IPhysVars* pPhysVars;

// stores the enabled features
BOOLEAN ActorEspEnabled = TRUE;
BOOLEAN VehicleEspEnabled = TRUE;
BOOLEAN ItemEspEnabled = FALSE; // buggy
BOOLEAN SuperJump = FALSE;
BOOLEAN FlashMode = FALSE;
BOOLEAN ClientInvincibility = FALSE;
BOOLEAN FlyMode = FALSE;
BOOLEAN MoonGravity = FALSE;
BOOLEAN DisableCollision = FALSE;

// prints a message to miscreated's standard output
void Announcement(const char* message)
{
	// gets the static class to pass to the function call
	__int64 StaticClass = *(__int64*)((__int64)GetModuleHandleA("CryGameSDK.dll") + 0x920E68);
	// defines the function type and argument types
	typedef __int64(__fastcall * _GetClass)(__int64);
	// sets the function address
	_GetClass GetClass = (_GetClass)((__int64)GetModuleHandleA("CryGameSDK.dll") + 0x1F8C40);
	// calls the function
	__int64 ClassAddress = GetClass(StaticClass);
	// defines the function type and argument types
	typedef void(__fastcall * _Print)(__int64, unsigned int, const char*);
	// sets the function address
	_Print Print = (_Print)((__int64)GetModuleHandleA("CryGameSDK.dll") + 0x2A4970);
	// calls the function
	Print(ClassAddress, TRUE, message);
}

// this function allows recursive iteration of items
VOID IterItems(IItemEntry* pItemEntry)
{
	// while the item is not the last
	while (!pItemEntry->IsEnd && pItemEntry)
	{
		// recursively iterates
		IterItems(pItemEntry->pChildItem);
		// if the item is not owned by a player
		if (!pItemEntry->pItem->pEntity->ParentId)
		{
			// gets the items entity
			IEntity* pEntity = pItemEntry->pItem->pEntity;
			// stores the screen position of the entity
			D3DXVECTOR3* ScreenPosition = new D3DXVECTOR3();
			// translates the position to the screen
			if (gEnv->pRenderer->ProjectToScreen(pEntity->GetWorldPos(), ScreenPosition, gOverlay->GetWidth(), gOverlay->GetHeight()))
			{
				// calculates the distance from the main player
				INT Distance = D3DXVec3Length(&(*gPlayerActor->pEntity->GetWorldPos() - *pEntity->GetWorldPos()));
				// prints the item on the screen
				gOverlay->DrawTextA((pEntity->GetClass()->GetName() + (string)" (" + to_string(Distance) + (string)"m)").c_str(), ScreenPosition->x, ScreenPosition->y, D3DCOLOR_ARGB(255, 153, 255, 51));
			}
		}
		// get the next item
		pItemEntry = pItemEntry->pNextItem;
	}
}

// draws all scene entities on the overlay
VOID DrawEntities()
{
	// get an entity iterator
	IEntityIt* pEntityIterator = gEnv->pEntitySystem->GetEntityIterator();
	// iterates through entities
	while (!pEntityIterator->IsEnd())
	{
		// gets the next entity
		IEntity* pEntity = pEntityIterator->Next();
		// if the entity exists
		if (pEntity)
		{
			// gets the item linked to the id of the entity
			IItem* pItem = gItemSystem->GetItem(pEntity->GetId());
			// if the item exists
			if (pItem)
			{
				// if the item is not owned by a player
				if (!pItem->pEntity->ParentId)
				{
					// stores the screen position of the entity
					D3DXVECTOR3* ScreenPosition = new D3DXVECTOR3();
					// translates the position to the screen
					if (gEnv->pRenderer->ProjectToScreen(pEntity->GetWorldPos(), ScreenPosition, gOverlay->GetWidth(), gOverlay->GetHeight()))
					{
						// calculates the distance from the main player
						INT Distance = D3DXVec3Length(&(*gPlayerActor->pEntity->GetWorldPos() - *pEntity->GetWorldPos()));
						// prints the item on the screen
						gOverlay->DrawTextA((pEntity->GetClass()->GetName() + (string)" (" + to_string(Distance) + (string)"m)").c_str(), ScreenPosition->x, ScreenPosition->y, D3DCOLOR_ARGB(255, 153, 255, 51));
					}
				}
			}
		}
	}
}

// draws all vehicles on the overlay
VOID DrawVehicles()
{
	// gets a vehicle iterator
	IVehicleIterator* pVehicleIterator = gVehicleSystem->CreateVehicleIterator();
	// iterates through vehicles
	while (IVehicle* pVehicle = pVehicleIterator->Next())
	{
		// stores the screen position of the entity
		D3DXVECTOR3* ScreenPosition = new D3DXVECTOR3();
		// translates the position to the screen
		if (gEnv->pRenderer->ProjectToScreen(pVehicle->pEntity->GetWorldPos(), ScreenPosition, gOverlay->GetWidth(), gOverlay->GetHeight()))
		{
			// calculates the distance from the main player
			INT Distance = D3DXVec3Length(&(*gPlayerActor->pEntity->GetWorldPos() - *pVehicle->pEntity->GetWorldPos()));
			// prints the vehicle on the screen
			gOverlay->DrawTextA((pVehicle->pEntity->GetClass()->GetName() + (string)" (" + to_string(Distance) + (string)"m)").c_str(), ScreenPosition->x, ScreenPosition->y, D3DCOLOR_ARGB(255, 51, 153, 255));
		}
	}
}

// draws all items on the overlay
VOID DrawItems()
{
	// gets the first entry in the map
	IItemEntry* pItemEntry = gItemSystem->pItemMap->pFirstItem;
	// recursively iterates embedded map and prints items
	IterItems(pItemEntry);
}

// draws actors on the overlay
VOID DrawActors()
{
	// gets an actor iterator
	IActorIterator* pActorIterator = gActorSystem->CreateActorIterator();
	// iterates through actors
	while (IActor* pActor = pActorIterator->Next())
	{
		// if the actor is not the main player
		if (pActor != gPlayerActor)
		{
			// if the actor is alive
			if (!pActor->IsDead())
			{
				// stores the screen position of the entity
				D3DXVECTOR3* ScreenPosition = new D3DXVECTOR3();
				// translates the position to the screen
				if (gEnv->pRenderer->ProjectToScreen(pActor->pEntity->GetWorldPos(), ScreenPosition, gOverlay->GetWidth(), gOverlay->GetHeight()))
				{
					// calculates the distance from the main player
					INT Distance = D3DXVec3Length(&(*gPlayerActor->pEntity->GetWorldPos() - *pActor->pEntity->GetWorldPos()));
					// prints the player on the screen
					gOverlay->DrawTextA((pActor->pEntity->GetName() + (string)" (" + to_string(Distance) + (string)"m)").c_str(), ScreenPosition->x, ScreenPosition->y, D3DCOLOR_ARGB(255, 255, 0, 0));
					// prints the player health on the screen
					gOverlay->DrawTextA(("Health: " + to_string((INT)pActor->GetHealth()) + "%").c_str(), ScreenPosition->x, ScreenPosition->y + 15, D3DCOLOR_ARGB(255, 255, 0, 0));
				}
			}
		}
	}
}

// draws the ui
VOID DrawUI()
{
	// draws the title
	gOverlay->DrawTextW(PROGRAM_TITLE, 10, 10, D3DCOLOR_ARGB(255, 0, 0, 255));
	// draws the actors
	if (ActorEspEnabled)
	{
		// notifies the user
		gOverlay->DrawTextW(L"Player ESP [ON]", 10, 30, D3DCOLOR_ARGB(255, 0, 255, 0));
		// draws the actors
		DrawActors();
	}
	else
	{
		// notifies the user
		gOverlay->DrawTextW(L"Player ESP [OFF]", 10, 30, D3DCOLOR_ARGB(255, 255, 0, 0));
	}
	// draws the vehicles
	if (VehicleEspEnabled)
	{
		// notifies the user
		gOverlay->DrawTextW(L"Vehicle ESP [ON]", 10, 50, D3DCOLOR_ARGB(255, 0, 255, 0));
		// draws the vehicles
		DrawVehicles();
	}
	else
	{
		// notifies the user
		gOverlay->DrawTextW(L"Vehicle ESP [OFF]", 10, 50, D3DCOLOR_ARGB(255, 255, 0, 0));
	}
	// draws the items
	if (ItemEspEnabled)
	{
		// notifies the user
		gOverlay->DrawTextW(L"Item ESP [ON]", 10, 70, D3DCOLOR_ARGB(255, 0, 255, 0));
		// draws the items
		DrawEntities();
	}
	else
	{
		// notifies the user
		gOverlay->DrawTextW(L"Item ESP [OFF]", 10, 70, D3DCOLOR_ARGB(255, 255, 0, 0));
	}
	// if the player has enabled superjump
	if (SuperJump)
	{
		// notifies the user
		gOverlay->DrawTextW(L"Super Jump [ON]", 10, 90, D3DCOLOR_ARGB(255, 0, 255, 0));
	}
	else
	{
		// notifies the user
		gOverlay->DrawTextW(L"Super Jump [OFF]", 10, 90, D3DCOLOR_ARGB(255, 255, 0, 0));
	}
	// if the player has enabled flash mode
	if (FlashMode)
	{
		// notifies the user
		gOverlay->DrawTextW(L"Flash Mode [ON]", 10, 110, D3DCOLOR_ARGB(255, 0, 255, 0));
	}
	else
	{
		// notifies the user
		gOverlay->DrawTextW(L"Flash Mode [OFF]", 10, 110, D3DCOLOR_ARGB(255, 255, 0, 0));
	}
	// if the player has enabled no client deaths
	if (ClientInvincibility)
	{
		// notifies the user
		gOverlay->DrawTextW(L"Client Invincibility [ON]", 10, 130, D3DCOLOR_ARGB(255, 0, 255, 0));
	}
	else
	{
		// notifies the user
		gOverlay->DrawTextW(L"Client Invincibility [OFF]", 10, 130, D3DCOLOR_ARGB(255, 255, 0, 0));
	}
	// if the player has enabled fly mode
	if (FlyMode)
	{
		// notifies the user
		gOverlay->DrawTextW(L"Fly Mode [ON]", 10, 150, D3DCOLOR_ARGB(255, 0, 255, 0));
	}
	else
	{
		// notifies the user
		gOverlay->DrawTextW(L"Fly Mode [OFF]", 10, 150, D3DCOLOR_ARGB(255, 255, 0, 0));
	}
	// if the player has enabled moon gravity
	if (MoonGravity)
	{
		// notifies the user
		gOverlay->DrawTextW(L"Moon Gravity [ON]", 10, 170, D3DCOLOR_ARGB(255, 0, 255, 0));
	}
	else
	{
		// notifies the user
		gOverlay->DrawTextW(L"Moon Gravity [OFF]", 10, 170, D3DCOLOR_ARGB(255, 255, 0, 0));
	}
	// if the player has enabled no collision
	if (DisableCollision)
	{
		// notifies the user
		gOverlay->DrawTextW(L"Disable Collsion [ON]", 10, 190, D3DCOLOR_ARGB(255, 0, 255, 0));
	}
	else
	{
		// notifies the user
		gOverlay->DrawTextW(L"Disable Collsion [OFF]", 10, 190, D3DCOLOR_ARGB(255, 255, 0, 0));
	}
}

// clears the back buffer and pushes the new buffer
VOID Render()
{
	// starts drawing on the overlay
	gOverlay->StartDrawing();
	// draws the user interface
	DrawUI();
	// stops drawing on the overlay
	gOverlay->StopDrawing();
}

// processes the player input
VOID ProcessInput()
{
	// toggles hack options
	if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000) ActorEspEnabled = !ActorEspEnabled;
	if (GetAsyncKeyState(VK_NUMPAD2) & 0x8000) VehicleEspEnabled = !VehicleEspEnabled;
	if (GetAsyncKeyState(VK_NUMPAD3) & 0x8000) ItemEspEnabled = !ItemEspEnabled;
	if (GetAsyncKeyState(VK_NUMPAD4) & 0x8000) SuperJump = !SuperJump;
	if (GetAsyncKeyState(VK_NUMPAD5) & 0x8000) FlashMode = !FlashMode;
	if (GetAsyncKeyState(VK_NUMPAD6) & 0x8000) ClientInvincibility = !ClientInvincibility;
	if (GetAsyncKeyState(VK_NUMPAD7) & 0x8000) FlyMode = !FlyMode;
	if (GetAsyncKeyState(VK_NUMPAD8) & 0x8000) MoonGravity = !MoonGravity;
	if (GetAsyncKeyState(VK_NUMPAD9) & 0x8000) DisableCollision = !DisableCollision;
	// if fly mode is enabled
	if (FlyMode)
	{
		// sets the gravity based on player key conbinations
		if (GetAsyncKeyState(VK_MULTIPLY) & 0x8000)
		{
			// going up
			pPhysVars->Gravity = 13;
		}
		else if (GetAsyncKeyState(VK_SUBTRACT) & 0x8000)
		{
			// low gravity (no fall damage)
			pPhysVars->Gravity = -1.7;
		}
		else if (GetAsyncKeyState(VK_DIVIDE) & 0x8000)
		{
			// no gravity
			pPhysVars->Gravity = 0.0;
		}
		else
		{
			// if moon gravity is enabled
			if (MoonGravity)
			{
				// moon gravity
				pPhysVars->Gravity = -1.7;
			}
			else
			{
				// resets the gravity
				pPhysVars->Gravity = -13;
			}
		}
	}
	else
	{
		// if moon gravity is enabled
		if (MoonGravity)
		{
			// moon gravity
			pPhysVars->Gravity = -1.7;
		}
		else
		{
			// resets the gravity
			pPhysVars->Gravity = -13;
		}
	}
	// if super jump is enabled
	if (SuperJump)
	{
		// enables super jump
		gPlayerActor->m_params.jumpHeight = 999999;
	}
	else
	{
		// disables super jump
		gPlayerActor->m_params.jumpHeight = 1;
	}
	// if flash mode is enabled
	if (FlashMode)
	{
		// enables super speed
		gPlayerActor->m_params.speedMultiplier = 999999;
	}
	else
	{
		// disabled super speed
		gPlayerActor->m_params.speedMultiplier = 1;
	}
	// if client invincibility is enabled
	if (ClientInvincibility)
	{
		// makes the player invincible
		gPlayerActor->Invincible(TRUE);
	}
	// if no collision is enabled
	if (DisableCollision)
	{
		// disables collision
		gPlayerActor->GetAnimatedCharacter()->RequestPhysicalColliderMode(1);
	}
	else
	{
		// enables collision
		gPlayerActor->GetAnimatedCharacter()->RequestPhysicalColliderMode(0);
	}
}

// the callback function for the overlay
VOID CALLBACK Update()
{
	// gets the player actor
	gPlayerActor = gGameFramework->GetClientActor();
	// if the player actor exists (if we're in game)
	if (gPlayerActor)
	{
		// processes any user input
		ProcessInput();
		// renders to the overlay
		Render();
	}
}

// the main dll thread
DWORD WINAPI Mistaken(LPVOID hInstance)
{
	// gets all of the global variables for use within the program later
	// gets and sets the global environment class
	if (gEnv = *(SSystemGlobalEnvironment**)((__int64)GetModuleHandleW(SDK_DLL_NAME) + GENV_OFFSET))
	{
		// gets the game framework
		if (gGameFramework = gEnv->pGame->GetIGameFramework())
		{
			// gets the actor system
			if (gActorSystem = gGameFramework->GetIActorSystem())
			{
				// gets the vehicle system
				if (gVehicleSystem = gGameFramework->GetIVehicleSystem())
				{
					// gets the item system
					if (gItemSystem = gGameFramework->GetIItemSystem())
					{
						// gets the physical variables
						if (pPhysVars = gEnv->pPhysicalWorld->GetPhysVars())
						{
							// finds the process window
							if (HWND hTarget = FindWindowW(NULL, TARGET_WINDOW))
							{
								// creates the overlay window
								if (gOverlay = Overlay::CreateW((HINSTANCE)hInstance, PROGRAM_TITLE, hTarget))
								{
									// enters the main loop
									return gOverlay->Start(Update);
								}
							}
						}
					}
				}
			}
		}
	}
	// returns to the kernel
	return 0xDEADBEEF;
}

// the dll main entry point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	// if the dll is being attached to a process
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// creates a thread to run our dll
		CreateThread(NULL, NULL, Mistaken, hinstDLL, NULL, NULL);
	}
	// returns to the kernel
	return 0xDEADBEEF;
}