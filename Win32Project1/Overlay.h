#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <Dwmapi.h>

// defines the callback function
typedef VOID(CALLBACK * Callback)();

struct Vector2
{
	INT x;
	INT y;

	Vector2(INT x, INT y)
	{
		this->x = x;
		this->y = y;
	}
};

// defines the option class
// used to store an option for a page
class Option
{
public:
	// stores the text of the option
	LPCWSTR lpText;
	// stores the colour of the option
	D3DCOLOR d3dColor;
	// stores the key associated with the option
	DWORD KeyCode;
	// dictates if the option is visable or not
	BOOL Visable;
};

// defines the page class
// used to store a page for a menu
class Page
{
private:
	// points to the previous and next page
	LIST_ENTRY Entry;
private:
	// defines the function used to add an option to the page
	Option* AddOption(LPCWSTR lpText, D3DCOLOR d3dColor, DWORD KeyCode);
};

// defines the menu class
// used to store an menu for the overlay
class Menu
{
public:
	// points to the previous and next menus
	LIST_ENTRY Entry;
	// defines the class constructor
	Menu(LPCWSTR lpTitle, D3DCOLOR d3dColor, Vector2 vPosition);
	// defines the function used to add a page to the menu
	Page* AddPage();
private:
	// stores the title of the overlay
	LPCWSTR lpTitle;
	// stores the menu position
	Vector2 vPosition;
	// stores the menu colour
	D3DCOLOR d3dColor;
	// dictates if the menu is visable or not
	BOOL Visable;
	// a linked list of pages on the menu
	LIST_ENTRY PageList;
	// contains the page the menu is currently on
	INT PageIndex;
	// contains the amount of pages in the menu
	INT PageAmount;
};

// defines the overlay class
// can be used to create an overlay window for a given window
class Overlay
{
private:
	// stores the window handle
	HWND hWindow;
	// stores the target window
	HWND hTarget;
	// stores the width of the window
	DWORD Width;
	// stores the height of the window
	DWORD Height;
	// stores the callback for the window message loop
	// Callback pCallback;
	// stores a linked list of menus for the overlay
	// LIST_ENTRY MenuList;
	// stores the instances the owns the window
	// HINSTANCE hInstance;
	// stores the title of the window
	// LPCWSTR lpTitle;
	// stores the direct object associated with the window
	IDirect3D9Ex* DirectObject;
	// stores the device object associated with the window
	IDirect3DDevice9Ex* DeviceObject;
	// stores the direct font associated with the window
	ID3DXFont* DirectFont;
	// defines the function used to register the overlay class with windows
	BOOL RegisterOverlay(HINSTANCE hInstance, LPCWSTR lpTitle);
	// defines the function used to create the overlay window
	BOOL CreateOverlay(HINSTANCE hInstance, LPCWSTR lpTitle);
	// defines the function used to unregister the overlay class with windows
	// BOOL UnregisterOverlay(HINSTANCE hInstance, LPCWSTR lpTitle);
	// defines the constructor
	// Overlay();
	// defines the function used to initialise directx
	BOOL InitDirectX();
	// defines the function that validates the windows exist
	BOOL ValidateWindows();
	// defines the function used to fix the overlay to the target window
	// BOOL FixTarget();
public:
	// stores the render callback for the main window loop
	// Callback pRender;
	// stores the update callback for the main window loop
	// Callback pUpdate;
	// defines the static function used to create an overlay
	static Overlay* CreateW(HINSTANCE hInstance, LPCWSTR lpTitle, HWND hTarget);
	// defines the window message handler
	static LRESULT CALLBACK MessageHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	// defines the function used to show the window
	BOOL Show();
	// defines the function used to enter the window's main loop
	DWORD Start(Callback pUpdate);
	// defines the function to start drawing on the overlay
	BOOL StartDrawing();
	// defines the function to stop drawing on the overlay
	BOOL StopDrawing();
	// defines the function used to draw text on the overlay
	BOOL DrawTextA(LPCSTR lpText, INT x, INT y, D3DCOLOR dColor);
	// defines the function used to draw text on the overlay
	BOOL DrawTextW(LPCWSTR lpText, INT x, INT y, D3DCOLOR dColor);
	// defines the function used to obtain the width of the overlay
	DWORD GetWidth();
	// defines the function used to obtain the height of the overlay
	DWORD GetHeight();
	// defines the function used to add a menu to the overlay
	// Menu* AddMenu(LPCWSTR lpTitle, D3DCOLOR d3dColor, Vector2 vPosition);
};