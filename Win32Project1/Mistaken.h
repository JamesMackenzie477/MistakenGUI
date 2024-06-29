#include "Overlay.h"

// defines the program title
#define PROGRAM_TITLE L"Mistaken Remastered v0.0.1"
// defines the name of the target window
#define TARGET_WINDOW L"Miscreated"
// defines the global environment pointer offset
#define GENV_OFFSET 0x9310A0
// defines the name of the sdk dll
#define SDK_DLL_NAME L"CryGameSDK.dll"

// defines our vector3
// used for location within the game
struct Vector3
{
	float x;
	float y;
	float z;
};

// a class for the entity class
// can be used to get the name of an entity class
class IEntityClass
{
public:
	// returns the name of the class
	const char* GetName()
	{
		typedef const char*(__thiscall *tFunc)(IEntityClass*);
		tFunc vGetName = *(tFunc*)(*(DWORD64*)this + 0x108);
		return vGetName(this);
	}
};

// a class for an entity within the game
// can be used to query an entity's name or position
class IEntity
{
public:
	char __PADDING1__[0x58]; // 0x0
	D3DXVECTOR3 Position; // 0x58
	char __PADDING2__[0xC8]; // 0x64
	DWORD32 ParentId; // 0x12C

	// sets the entity's world position and rotation
	__int64 GetWorldTM()
	{
		typedef __int64(__fastcall * Function)(IEntity*);
		Function GetWorldTM = *(Function*)(*(DWORD64*)this + 0x1E0);
		return GetWorldTM(this);
	}

	// sets the entity's world position and rotation
	void SetWorldTM(__int64 pWorldTM)
	{
		typedef void(__fastcall *Function)(IEntity*, __int64);
		Function SetWorldTM = *(Function*)(*(DWORD64*)this + 0x1D0);
		SetWorldTM(this, pWorldTM);
	}

	// returns the entity's name
	const char* GetName()
	{
		typedef const char*(__thiscall *tFunc)(IEntity*);
		tFunc vGetName = *(tFunc*)(*(DWORD64*)this + 0x598);
		return vGetName(this);
	}

	// returns the entity id
	DWORD32 GetId()
	{
		typedef DWORD32(__thiscall *tFunc)(IEntity*);
		tFunc vGetId = *(tFunc*)(*(DWORD64*)this + 0x40);
		return vGetId(this);
	}

	// returns a pointer to the entity class
	IEntityClass* GetClass()
	{
		typedef IEntityClass*(__thiscall *tFunc)(IEntity*);
		tFunc vGetClass = *(tFunc*)(*(DWORD64*)this + 0xB0);
		return vGetClass(this);
	}

	// returns true if the entity is active
	bool IsActive()
	{
		typedef bool(__thiscall *tFunc)(IEntity*);
		tFunc vIsActive = *(tFunc*)(*(DWORD64*)this + 0x2A0);
		return vIsActive(this);
	}

	// returns the entity's world position as a vector3
	D3DXVECTOR3* GetWorldPos()
	{
		// Vector3* WorldPosition = new Vector3();
		// typedef bool(__fastcall *tFunc)(IEntity*, Vector3*);
		// tFunc vGetWorldPos = *(tFunc*)(*(DWORD64*)this + 0x278);
		// vGetWorldPos(this, WorldPosition);
		// return WorldPosition;
		// the actual function call breaks the main image
		// don't know why, but this mimics that function
		// stores the world position
		D3DXVECTOR3* WorldPosition = new D3DXVECTOR3();
		// gets the x position of the entity
		WorldPosition->x = *(float*)((DWORD64)this + 0x8C);
		// gets the y position of the entity
		WorldPosition->y = *(float*)((DWORD64)this + 0x9C);
		// gets the z position of the entity
		WorldPosition->z = *(float*)((DWORD64)this + 0xAC);
		// returns a pointer to the position vector
		return WorldPosition;
	}

	// sets the entity's world position as a vector3
	VOID SetWorldPos(D3DXVECTOR3* Position)
	{
		// sets the x position of the entity
		*(float*)((DWORD64)this + 0x8C) = Position->x;
		// sets the y position of the entity
		*(float*)((DWORD64)this + 0x9C) = Position->y;
		// sets the z position of the entity
		*(float*)((DWORD64)this + 0xAC) = Position->z;
	}

	// returns the entity owner entity id
	DWORD32 GetOwnerId()
	{
		typedef DWORD32(__thiscall *tFunc)(IEntity*);
		tFunc vGetOwnerId = *(tFunc*)(*(DWORD64*)this + 0x508);
		return vGetOwnerId(this);
	}
};

// a class for an item within the game
// can be used to obtain the entity linked with an item
class IItem
{
public:
	char __PADDING1__[0x40]; // 0x0
	IEntity* pEntity; // 0x40

	// returns the items owner entity id
	DWORD32 GetOwnerId()
	{
		typedef DWORD32(__thiscall *tFunc)(IItem*);
		tFunc vGetOwnerId = *(tFunc*)(*(DWORD64*)this + 0x1A8);
		return vGetOwnerId(this);
	}
};

class IActorStats
{
public:
	char __PADDING1__[0x1C]; // 0x0
	FLOAT inFiring; // 0x1C
	FLOAT maxAirSpeed;
	BOOLEAN isGrabbed;
	BOOLEAN isRagDoll;
	BOOLEAN isInBlendRagdoll;
};

class IActorParams
{
public:
	char __PADDING1__[0x38]; // 0x0
	FLOAT internalSpeedMult; // 0x38
	FLOAT speedMultiplier; // 0x3C
	char __PADDING2__[0x44]; // 0x40
	FLOAT jumpHeight; // 0x84
};

class IAnimatedCharacter
{
public:
	// requests a new collider mode
	void RequestPhysicalColliderMode(int Mode)
	{
		// defines the function type and argument types
		typedef void(__fastcall * _RequestPhysicalColliderMode)(IAnimatedCharacter*, int, int, const char*);
		// sets the function address
		_RequestPhysicalColliderMode RequestPhysicalColliderMode = *(_RequestPhysicalColliderMode*)(*(__int64*)this + 0x1F8);
		// calls the function
		RequestPhysicalColliderMode(this, Mode, 1, "Actor::HandleEvent");
	}
};

// a class for a game actor
// can be used to get information on the actor
class IActor
{
public:
	char __PADDING1__[0x38]; // 0x0
	DWORD64 EntityId; // 0x38
	IEntity* pEntity; // 0x40
	char __PADDING2__[0xC80]; // 0x48
	IActorParams m_params; // 0xCC8

	// teleports the actor to the target actor
	void TeleportToActor(IActor* pActor)
	{
		// sets the worldtm of our linked entity to the worldtm of the target's entity
		this->pEntity->SetWorldTM(pActor->pEntity->GetWorldTM());
	}

	// returns the actor params class associated with this actor
	IActorParams* GetActorParams()
	{
		// returns the actor params
		return (IActorParams*)((DWORD64)this + 0xCC8);
	}

	// returns the actor stats class associated with this actor
	IActorStats* GetActorStats()
	{
		typedef IActorStats*(__thiscall *tFunc)(IActor*);
		tFunc vGetActorStats = *(tFunc*)(*(DWORD64*)this + 0x710);
		return vGetActorStats(this);
	}

	// returns the animated chartacter for this actor
	IAnimatedCharacter* GetAnimatedCharacter()
	{
		return *(IAnimatedCharacter**)((__int64)this + 0xE40);
	}

	// ------------------------- INVINCIBILITY WIP -------------------------

	// test function
	FLOAT _Debug_GetHealth()
	{
		// gets the actor health
		return *(DWORD*)((DWORD64)this + 0x94) ^ *(DWORD*)((DWORD64)this + 0x98);
	}

	// toggles invincibility on and off
	VOID Invincible(BOOLEAN toggle)
	{
		DWORD64 FuncAddr;
		// stores the old protection
		DWORD OldProtect;
		// disables the netkill function
		// gets the function address
		FuncAddr = *(DWORD64*)(*(DWORD64*)this + 0x5E0);
		// edits the memory protection
		if (VirtualProtect((PVOID)FuncAddr, 1, PAGE_EXECUTE_READWRITE, &OldProtect))
		{
			// if the player wants to be invincible
			if (toggle)
			{
				// writes the shellcode ret to the address
				*(BYTE*)FuncAddr = 0xC3;
			}
		}
		// disables the sethealth function
		// gets the function address
		FuncAddr = *(DWORD64*)(*(DWORD64*)this + 0x110);
		// edits the memory protection
		if (VirtualProtect((PVOID)FuncAddr, 1, PAGE_EXECUTE_READWRITE, &OldProtect))
		{
			// if the player wants to be invincible
			if (toggle)
			{
				// writes the shellcode ret to the address
				*(BYTE*)FuncAddr = 0xC3;
			}
		}
		// disables the ragdollize function
		// gets the function address
		FuncAddr = *(DWORD64*)(*(DWORD64*)this + 0x688);
		// edits the memory protection
		if (VirtualProtect((PVOID)FuncAddr, 1, PAGE_EXECUTE_READWRITE, &OldProtect))
		{
			// if the player wants to be invincible
			if (toggle)
			{
				// writes the shellcode ret to the address
				*(BYTE*)FuncAddr = 0xC3;
			}
		}
		// disables the onfall function
		// gets the function address
		FuncAddr = ((DWORD64)GetModuleHandleA("CryGameSDK.dll") + 0x859D0);
		// edits the memory protection
		if (VirtualProtect((PVOID)FuncAddr, 1, PAGE_EXECUTE_READWRITE, &OldProtect))
		{
			// if the player wants to be invincible
			if (toggle)
			{
				// writes the shellcode ret to the address
				*(BYTE*)FuncAddr = 0xC3;
			}
		}
		// disables the unconcious function
		// gets the function address
		FuncAddr = ((DWORD64)GetModuleHandleA("CryGameSDK.dll") + 0x68A050);
		// edits the memory protection
		if (VirtualProtect((PVOID)FuncAddr, 1, PAGE_EXECUTE_READWRITE, &OldProtect))
		{
			// if the player wants to be invincible
			if (toggle)
			{
				// writes the shellcode ret to the address
				*(BYTE*)FuncAddr = 0xC3;
			}
		}
	}

	// stands the actor up
	VOID StandUp()
	{
		typedef VOID(__thiscall *tFunc)(IActor*);
		tFunc vStandUp = *(tFunc*)(*(DWORD64*)this + 0x758);
		return vStandUp(this);
	}

	// ---------------------------------------------------------------------

	// returns a pointer to the linked entity
	IEntity* GetLinkedEntity()
	{
		typedef IEntity*(__thiscall *tFunc)(IActor*);
		tFunc vGetLinkedEntity = *(tFunc*)(*(DWORD64*)this + 0x348);
		return vGetLinkedEntity(this);
	}

	// returns a pointer to the players current item
	IItem* GetCurrentItem()
	{
		typedef IItem*(__thiscall *tFunc)(IActor*, bool);
		tFunc vGetCurrentItem = *(tFunc*)(*(DWORD64*)this + 0x318);
		return vGetCurrentItem(this, false);
	}

	// returns the actors health as a float
	float GetHealth()
	{
		typedef float(__thiscall *tFunc)(IActor*);
		tFunc vGetHealth = *(tFunc*)(*(DWORD64*)this + 0x118);
		return vGetHealth(this);
	}

	// sets the actors health as a float
	void SetHealth(float health)
	{
		typedef void(__fastcall *tFunc)(IActor*, float);
		tFunc vSetHealth = *(tFunc*)(*(DWORD64*)this + 0x110);
		return vSetHealth(this, health);
	}

	// sets the actors max health as a float
	void SetMaxHealth(float health)
	{
		typedef void(__fastcall *tFunc)(IActor*, float);
		tFunc vSetMaxHealth = *(tFunc*)(*(DWORD64*)this + 0x148);
		return vSetMaxHealth(this, health);
	}

	// returns true if the actor is dead
	bool IsDead()
	{
		typedef bool(__thiscall *tFunc)(IActor*);
		tFunc vIsDead = *(tFunc*)(*(DWORD64*)this + 0x1B8);
		return vIsDead(this);
	}

	// returns true if the actor has fallen
	bool IsFallen()
	{
		typedef bool(__thiscall *tFunc)(IActor*);
		tFunc vIsFallen = *(tFunc*)(*(DWORD64*)this + 0x170);
		return vIsFallen(this);
	}

	// returns true if the actor is a player
	bool IsPlayer()
	{
		typedef bool(__thiscall *tFunc)(IActor*);
		tFunc vIsPlayer = *(tFunc*)(*(DWORD64*)this + 0x3D0);
		return vIsPlayer(this);
	}

	// returns the entity class name as a constant char pointer
	const char* GetEntityClassName()
	{
		typedef const char*(__thiscall *tFunc)(IActor*);
		tFunc vGetEntityClassName = *(tFunc*)(*(DWORD64*)this + 0x428);
		return vGetEntityClassName(this);
	}
};

// a class for the entity iterator
// can be used to iterate all entities in the scene
class IEntityIt
{
public:
	// true if this is the end of the iterator
	bool IsEnd()
	{
		typedef bool(__thiscall *tFunc)(IEntityIt*);
		tFunc vIsEnd = *(tFunc*)(*(DWORD64*)this + 0x18);
		return vIsEnd(this);
	}

	// returns the next entity pointer in the iterator
	IEntity* Next()
	{
		typedef IEntity*(__thiscall *tFunc)(IEntityIt*);
		tFunc vNext = *(tFunc*)(*(DWORD64*)this + 0x20);
		return vNext(this);
	}
};

// a class for the actor iterator
// can be used to iterate all actors in the scene
class IActorIterator
{
public:
	// returns the next actor pointer in the iterator
	IActor* Next()
	{
		typedef IActor*(__thiscall *tFunc)(IActorIterator*);
		tFunc vNext = *(tFunc*)(*(DWORD64*)this + 0x78);
		return vNext(this);
	}
};

// a class for the game actor system
// can be used to obtain a map of scene actors
class IActorSystem
{
public:
	// returns a pointer to an iterator for all of the actors in the scene
	IActorIterator* CreateActorIterator()
	{
		IActorIterator* ActorIterator;
		typedef IActorIterator*(__fastcall *tFunc)(IActorSystem*, IActorIterator**);
		tFunc vCreateActorIterator = *(tFunc*)(*(DWORD64*)this + 0x168);
		vCreateActorIterator(this, &ActorIterator);
		return ActorIterator;
	}

	// returns the count of actors in the scene
	int GetActorCount()
	{
		typedef int(__thiscall *tFunc)(IActorSystem*);
		tFunc vGetActorCount = *(tFunc*)(*(DWORD64*)this + 0x120);
		return vGetActorCount(this);
	}
};

// defines an item entry
// used for iteration of the item map
class IItemEntry
{
public:
	IItemEntry* pChildItem; // 0x0
	IItemEntry* pPreviousItem; // 0x8
	IItemEntry* pNextItem; // 0x10
	char __PADDING2__[0x1]; // 0x18
	BOOLEAN IsEnd; // 0x19
	char __PADDING3__[0x6]; // 0x1A
	DWORD32 ItemId; // 0x20
	char __PADDING4__[0x4]; // 0x24
	IItem* pItem; // 0x28
};

// the item map definition
// used to iterate game items
class IItemMap
{
public:
	char __PADDING1__[0x8]; // 0x0
	IItemEntry* pFirstItem; // 0x8
};

// a class for a game vehicle
// can be used to get information on the vehicle
class IVehicle
{
public:
	char __PADDING1__[0x38]; // 0x0
	DWORD64 EntityId; // 0x38
	IEntity* pEntity; // 0x40
};

// a class for the game item system
// can be used to obtain a map of scene items
class IItemSystem
{
public:
	char __PADDING1__[0xC8]; // 0x0
	IItemMap* pItemMap; // 0xC8
	DWORD64 ItemCount; // 0xD0

	// returns a pointer to the item of the given id
	IItem* GetItem(DWORD32 ItemId)
	{
		typedef IItem*(__fastcall *tFunc)(IItemSystem*, DWORD32);
		tFunc vGetItem = *(tFunc*)(*(DWORD64*)this + 0x1B0);
		return vGetItem(this, ItemId);
	}
};

// a class for the vehicle iterator
// can be used to iterate all vehicles in the scene
class IVehicleIterator
{
public:
	// returns the next vehicle pointer in the iterator
	IVehicle* Next()
	{
		typedef IVehicle*(__thiscall *tFunc)(IVehicleIterator*);
		tFunc vNext = *(tFunc*)(*(DWORD64*)this + 0x98);
		return vNext(this);
	}
};

// a class for the game vehicle system
// can be used to obtain a map of scene vehicles
class IVehicleSystem
{
public:
	// returns a pointer to an iterator for all of the vehicles in the scene
	IVehicleIterator* CreateVehicleIterator()
	{
		IVehicleIterator* VehicleIterator;
		typedef IVehicleIterator*(__fastcall *tFunc)(IVehicleSystem*, IVehicleIterator**);
		tFunc vCreateVehicleIterator = *(tFunc*)(*(DWORD64*)this + 0x2D0);
		vCreateVehicleIterator(this, &VehicleIterator);
		return VehicleIterator;
	}
};

// a class for a level in the game
// can be used to obtain the level path
class ILevel
{
public:
	// returns the level path as a constant char
	const char* GetPath()
	{
		typedef const char*(__thiscall *tFunc)(ILevel*);
		tFunc vGetPath = *(tFunc*)(*(DWORD64*)this + 0x18);
		return vGetPath(this);
	}
};

// a class for the level system
// can be used to load levels
class ILevelSystem
{
public:
	// loads a level by the given name
	ILevel* GetCurrentLevel()
	{
		typedef ILevel*(__thiscall *tFunc)(ILevelSystem*);
		tFunc vLoadLevel = *(tFunc*)(*(DWORD64*)this + 0x80);
		return vLoadLevel(this);
	}

	// loads a level by the given name
	bool LoadLevel(const char* LevelName)
	{
		typedef bool(__thiscall *tFunc)(ILevelSystem*, const char*);
		tFunc vLoadLevel = *(tFunc*)(*(DWORD64*)this + 0x88);
		return vLoadLevel(this, LevelName);
	}
};

// a class for the game framework
// can be used to request objects from within the scene
class IGameFramework
{
public:
	// returns a pointer to the level system
	ILevelSystem* GetILevelSystem()
	{
		typedef ILevelSystem*(__thiscall *tFunc)(IGameFramework*);
		tFunc vGetILevelSystem = *(tFunc*)(*(DWORD64*)this + 0x2A8);
		return vGetILevelSystem(this);
	}

	// returns a pointer to the actor system
	IActorSystem* GetIActorSystem()
	{
		typedef IActorSystem*(__thiscall *tFunc)(IGameFramework*);
		tFunc vGetIActorSystem = *(tFunc*)(*(DWORD64*)this + 0x2D8);
		return vGetIActorSystem(this);
	}

	// returns a pointer to the item system
	IItemSystem* GetIItemSystem()
	{
		typedef IItemSystem*(__fastcall *tFunc)(IGameFramework*);
		tFunc vGetItemSystem = *(tFunc*)(*(DWORD64*)this + 0x2F8);
		return vGetItemSystem(this);
	}

	// returns a pointer to the vehicle system
	IVehicleSystem* GetIVehicleSystem()
	{
		typedef IVehicleSystem*(__fastcall *tFunc)(IGameFramework*);
		tFunc vGetIVehicleSystem = *(tFunc*)(*(DWORD64*)this + 0x368);
		return vGetIVehicleSystem(this);
	}

	// returns a pointer to the client actor
	IActor* GetClientActor()
	{
		typedef IActor*(__thiscall *tFunc)(IGameFramework*);
		tFunc vGetClientActor = *(tFunc*)(*(DWORD64*)this + 0x4D0);
		return vGetClientActor(this);
	}
};

// a class for the scene camera
// can be used to obtain the main origin
class ICamera
{
public:
	// returns the camera world position as a vector3
	D3DXVECTOR3* GetWorldPos()
	{
		// stores the world position
		D3DXVECTOR3* WorldPosition = new D3DXVECTOR3();
		// gets the x position of the entity
		WorldPosition->x = *(float*)((DWORD64)this + 0x14);
		// gets the y position of the entity
		WorldPosition->y = *(float*)((DWORD64)this + 0x24);
		// gets the z position of the entity
		WorldPosition->z = *(float*)((DWORD64)this + 0x34);
		// returns a pointer to the position vector
		return WorldPosition;
	}

	// sets the camera's world position as a vector3
	VOID SetWorldPos(D3DXVECTOR3* Position)
	{
		// sets the x position of the entity
		*(float*)((DWORD64)this + 0x14) = Position->x;
		// sets the y position of the entity
		*(float*)((DWORD64)this + 0x24) = Position->y;
		// sets the z position of the entity
		*(float*)((DWORD64)this + 0x34) = Position->z;
	}
};

// a class for the 3d engine
// can be used to obtain the scene camera
class I3DEngine
{
public:
	// returns a pointer to the scenes rendering camera
	ICamera* GetRenderingCamera()
	{
		typedef ICamera*(__thiscall *tFunc)(I3DEngine*);
		tFunc vGetRenderingCamera = *(tFunc*)(*(DWORD64*)this + 0x78);
		return vGetRenderingCamera(this);
	}
};

// defines the game class
// used to request the game framework class
class IGame
{
public:
	// returns a pointer to the game framework
	IGameFramework* GetIGameFramework()
	{
		typedef IGameFramework*(__thiscall *tFunc)(IGame*);
		tFunc vGetIGameFramework = *(tFunc*)(*(DWORD64*)this + 0x180);
		return vGetIGameFramework(this);
	}
};

// a class for the entity system
// can be used to request an entity iterator
class IEntitySystem
{
public:
	// returns a pointer to an iterator for all of the entities in the scene
	IEntityIt* GetEntityIterator()
	{
		typedef IEntityIt*(__thiscall *tFunc)(IEntitySystem*);
		tFunc vGetEntityIterator = *(tFunc*)(*(DWORD64*)this + 0x98);
		return vGetEntityIterator(this);
	}
};

// a class for the game renderer
// can be used to convert world positions to screen co-ordinates
class IRenderer
{
public:
	// returns the screen position of a world location
	bool ProjectToScreen(D3DXVECTOR3* inPos, D3DXVECTOR3* outPos, LONG Width, LONG Height)
	{
		// I don't know if ida's being a prick, but apparently this is how many arguments the function takes...
		typedef bool(__fastcall *tFunc)(IRenderer*, __int64 a2, __int64 a3, __int64 a4, int a5, int a6, float WorldPositionX, float WorldPositionY, float WorldPositionZ, int a10, int a11, int a12, float *ScreenPositionX, float *ScreenPositionY, float *ScreenPositionZ, int a16, __int64 ThreadThingy, __int64 a18, __int64 a19, int a20, int a21, int a22);
		tFunc vProjectToScreen = *(tFunc*)(*(DWORD64*)this + 0x5B8);
		if (vProjectToScreen(this, NULL, NULL, NULL, NULL, NULL, inPos->x, inPos->y, inPos->z, NULL, NULL, NULL, &outPos->x, &outPos->y, &outPos->z, NULL, NULL, NULL, NULL, NULL, NULL, NULL))
		{
			if (outPos->z < 1.0f)
			{
				outPos->x *= (float)Width * 0.01f;
				outPos->y *= (float)Height * 0.01f;
				outPos->z *= 1.0f;
				return true;
			}
		}
		return false;
	}
};

// a structure of all the physical variables
struct IPhysVars
{
public:
	char __PADDING1__[0x68]; // 0x0
	BOOL FlyMode; // 0x68
	char __PADDING2__[0x54]; // 0x6C
	FLOAT Gravity; // 0xC0
};

// a class for the physical game world
// can be used to configure physical variables
class IPhysicalWorld
{
public:
	// returns a pointer to the physical vars structure
	IPhysVars* GetPhysVars()
	{
		typedef IPhysVars*(__thiscall *tFunc)(IPhysicalWorld*);
		tFunc vGetPhysVars = *(tFunc*)(*(DWORD64*)this + 0x78);
		return vGetPhysVars(this);
	}
};

class ICVar
{
public:
	// sets the cvar value
	VOID Set(int value)
	{
		typedef VOID(__thiscall *tFunc)(ICVar*, int);
		tFunc vSet = *(tFunc*)(*(DWORD64*)this + 0x38);
		return vSet(this, value);
	}

	// sets the cvar value
	VOID Set(float value)
	{
		typedef VOID(__thiscall *tFunc)(ICVar*, float);
		tFunc vSet = *(tFunc*)(*(DWORD64*)this + 0x40);
		return vSet(this, value);
	}

	// sets the cvar value
	VOID Set(const char* value)
	{
		typedef VOID(__thiscall *tFunc)(ICVar*, const char*);
		tFunc vSet = *(tFunc*)(*(DWORD64*)this + 0x48);
		return vSet(this, value);
	}
};

class IConsole
{
public:
	// executes a console command
	VOID ExecuteString(const char * command, bool bSilentMode = false, bool bDeferExecution = false)
	{
		typedef VOID(__fastcall *tFunc)(IConsole*, const char *, bool, bool);
		tFunc vExecuteString = *(tFunc*)(*(DWORD64*)this + 0x118);
		return vExecuteString(this, command, bSilentMode, bDeferExecution);
	}

	// retrieves the requested cvar
	ICVar* GetCVar(const char * name)
	{
		typedef ICVar*(__fastcall *tFunc)(IConsole*, const char *);
		tFunc vGetCVar = *(tFunc*)(*(DWORD64*)this + 0xB8);
		return vGetCVar(this, name);
	}
};

// defines the cryengine global environment class
// we can parse this class to get what information we need from the game
class SSystemGlobalEnvironment
{
public:
	char __PADDING1__[0x18]; // 0x0
	I3DEngine* p3DEngine; // 0x18
	char __PADDING2__[0x30]; // 0x20
	IPhysicalWorld* pPhysicalWorld; // 0x50
	char __PADDING3__[0x78]; // 0x58
	IGame* pGame; // 0xD0
	char __PADDING4__[0x20]; // 0xD8
	IEntitySystem* pEntitySystem; // 0xF8
	char __PADDING5__[0x8]; // 0x100
	IConsole* pConsole; // 0x108
	char __PADDING6__[0x78]; // 0x110
	IRenderer* pRenderer; // 0x188
};

class CGodMode
{
public:
	char __PADDING1__[0x4]; // 0x0
	BOOL m_godMode; // 0x4

	// gets the god mode static instance
	static CGodMode* GetInstance()
	{
		typedef CGodMode*(__thiscall *tFunc)();
		tFunc vGetInstance = (tFunc)((DWORD64)GetModuleHandleA("CryGameSDK.dll") + 0xC0B40);
		return vGetInstance();
	}

	// returns true if god mode is active
	bool IsGodModeActive()
	{
		typedef bool(__thiscall *tFunc)(CGodMode*);
		tFunc vIsGodModeActive = (tFunc)((DWORD64)GetModuleHandleA("CryGameSDK.dll") + 0xC9110);
		return vIsGodModeActive(this);
	}
};

// SSystemGlobalEnvironment - CryGameSDK - String "    %02d) %s" https://github.com/CRYTEK/CRYENGINE/blob/048c8901647b124eb3e1d4f300cd033893ad00e7/Code/GameSDK/GameDll/GameStartup.cpp#L384

// pRenderer->ProjectToScreen() - CryGameSDK - String "BlendOutTime" https://github.com/CRYTEK/CRYENGINE/blob/26524289c15a660965a447dcb22628643917c820/Code/GameSDK/GameDll/Effects/GameEffects/ExplosionGameEffect.cpp#L510

// pEnv->IGame->GetIGameFramework()->GetIActorSystem() - CryGameSDK - String "player" 3rd down https://github.com/CRYTEK/CRYENGINE/blob/26524289c15a660965a447dcb22628643917c820/Code/GameSDK/GameDll/GameStateRecorder.cpp#L1444
// pEnv->IGame->GetIGameFramework()->GetClientActor()

// pActor->GetHealth() - CryGameSDK - String "%8.2f health" https://github.com/CRYTEK/CRYENGINE/blob/26524289c15a660965a447dcb22628643917c820/Code/GameSDK/GameDll/DebugGun.cpp#L180
// pActor->GetCurrentItem() - CryGameSDK - String "OnUsed" https://github.com/CRYTEK/CRYENGINE/blob/2eab71629ab910e782246c5434522302d60b1c6d/Code/GameSDK/GameDll/PlayerEntityInteraction.cpp#L236
// pActor->IsPlayer() - CryGameSDK - String "Actor '%s' has no inventory, when trying to pickup '%s'!" https://github.com/CRYTEK/CRYENGINE/blob/2eab71629ab910e782246c5434522302d60b1c6d/Code/GameSDK/GameDll/Accessory.cpp#L54

// pActor->Entity Attribute Offset - CryGameSDK - String "Actor '%s' has no inventory, when trying to pickup '%s'!" https://github.com/CRYTEK/CRYENGINE/blob/2eab71629ab910e782246c5434522302d60b1c6d/Code/GameSDK/GameDll/Accessory.cpp#L56
// pEntity->GetClass() - CryGameSDK - String "Actor '%s' has no inventory, when trying to pickup '%s'!" https://github.com/CRYTEK/CRYENGINE/blob/2eab71629ab910e782246c5434522302d60b1c6d/Code/GameSDK/GameDll/Accessory.cpp#L56

// pEntityClass->GetName() - CryGameSDK - String "ProceduralObject" https://github.com/CRYTEK/CRYENGINE/blob/26524289c15a660965a447dcb22628643917c820/Code/GameSDK/GameDll/WorldBuilder.cpp#L62

// pItem->pEntity - CryGameSDK - String "Item: %s" https://github.com/CRYTEK/CRYENGINE/blob/ba1b181d5559ea23d84467c9a1f684214dcbdf52/Code/GameSDK/GameDll/Actor.cpp#L5240

// pItemSystem->GetItem - CryGameSDK - String "AnimatedCharacter" https://github.com/CRYTEK/CRYENGINE/blob/ba1b181d5559ea23d84467c9a1f684214dcbdf52/Code/GameSDK/GameDll/Actor.cpp#L375
// pItemSystem->AddItem - CryGameSDK - String "WeaponMelee" https://github.com/CRYTEK/CRYENGINE/blob/ba1b181d5559ea23d84467c9a1f684214dcbdf52/Code/GameSDK/GameDll/Item.cpp#L277

// TODO
// attempt vehicle iterator and see if it works
// fix the eax register!
// find the offset of the item owner id