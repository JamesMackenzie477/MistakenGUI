#include "Overlay.h"

// handles any messages sent to the window
LRESULT CALLBACK Overlay::MessageHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// checks the recieved message
	switch (uMsg)
	{
	case WM_DESTROY:
		// if the message is to destory the window
		// closes the window
		PostQuitMessage(0);
		// breaks from the switch
		break;
	case WM_PAINT:
		// if windows wants to repaint the window
		// stores the overlay window rect
		RECT rWindow;
		// gets the rect of the overlay window
		if (GetWindowRect(hwnd, &rWindow))
		{
			// creates the area to extend the frame
			MARGINS mTarget = { 0, 0, rWindow.right - rWindow.left, rWindow.bottom - rWindow.top };
			// extends the window frame into the client (makes it transparent)
			DwmExtendFrameIntoClientArea(hwnd, &mTarget);
		}
		// breaks from the switch
		break;
	}
	// lets windows handle the message
	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

// draws text on the overlay
BOOL Overlay::DrawTextA(LPCSTR lpText, INT x, INT y, D3DCOLOR dColor)
{
	// creates the text position
	RECT TextPosition;
	// sets the shadow left offset
	TextPosition.left = x + 1;
	// sets the shadow top offset
	TextPosition.top = y + 1;
	// draws the text shadow on the window
	if (DirectFont->DrawTextA(NULL, lpText, -1, &TextPosition, DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0)))
	{
		// sets the text left offset
		TextPosition.left = x;
		// sets the text top offset
		TextPosition.top = y;
		// draws the text
		if (DirectFont->DrawTextA(NULL, lpText, -1, &TextPosition, DT_NOCLIP, dColor))
		{
			// the text has been drawn successfully
			return TRUE;
		}
	}
	// the text has failed
	return FALSE;
}

// draws text on the overlay
BOOL Overlay::DrawTextW(LPCWSTR lpText, INT x, INT y, D3DCOLOR dColor)
{
	// creates the text position
	RECT TextPosition;
	// sets the shadow left offset
	TextPosition.left = x + 1;
	// sets the shadow top offset
	TextPosition.top = y + 1;
	// draws the text shadow on the window
	if (DirectFont->DrawTextW(NULL, lpText, -1, &TextPosition, DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0)))
	{
		// sets the text left offset
		TextPosition.left = x;
		// sets the text top offset
		TextPosition.top = y;
		// draws the text
		if (DirectFont->DrawTextW(NULL, lpText, -1, &TextPosition, DT_NOCLIP, dColor))
		{
			// the text has been drawn successfully
			return TRUE;
		}
	}
	// the text has failed
	return FALSE;
}

// initializes directx on the overlay
BOOL Overlay::InitDirectX()
{
	// creates a directx instance
	if (Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectObject) == D3D_OK)
	{
		// creates the device parameters
		D3DPRESENT_PARAMETERS DeviceParams;
		// zeros out the structure
		ZeroMemory(&DeviceParams, sizeof(D3DPRESENT_PARAMETERS));
		// sets the state to windowed
		DeviceParams.Windowed = TRUE;
		// discards any buffers after use
		DeviceParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
		// sets the windows for the device to output to
		DeviceParams.hDeviceWindow = hWindow;
		// sets the back buffer format
		DeviceParams.BackBufferFormat = D3DFMT_A8R8G8B8;
		// enables the depth stencil
		DeviceParams.EnableAutoDepthStencil = TRUE;
		// sets the depth stencil format
		DeviceParams.AutoDepthStencilFormat = D3DFMT_D16;
		// creates the device object
		if (DirectObject->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &DeviceParams, NULL, &DeviceObject) == D3D_OK)
		{
			// creates a font to be used within the window
			if (D3DXCreateFontW(DeviceObject, 14, 0, 0, 0, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial", &DirectFont) == D3D_OK)
			{
				// directx initialization was successful
				return TRUE;
			}
		}
	}
	// directx initialization failed
	return FALSE;
}

// starts the message loop for the overlay
DWORD Overlay::Start(Callback pUpdate)
{
	// sets the target window to non topmost
	if (SetWindowPos(hTarget, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE))
	{
		// shows the window
		if (!ShowWindow(hWindow, SW_SHOW))
		{
			// stores our message
			MSG RecievedMessage;
			// enters the overlay's main loop
			while (RecievedMessage.message != WM_QUIT)
			{
				// deals with any pending messages
				if (PeekMessageW(&RecievedMessage, NULL, 0, 0, PM_REMOVE))
				{
					// translates the message
					TranslateMessage(&RecievedMessage);
					// deals with the message
					DispatchMessage(&RecievedMessage);
				}
				else
				{
					// validates that the target window exists
					// if (!IsWindow(hTarget)) break;
					// fixes the overlay to the target
					SetWindowPos(hWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					// calls the set callback
					pUpdate();
				}
			}
			// returns the message parameter
			return RecievedMessage.wParam;
		}
	}
}

// creates a new overlay object for the target window
Overlay* Overlay::CreateW(HINSTANCE hInstance, LPCWSTR lpTitle, HWND hTarget)
{
	// validates the parameters
	if (lpTitle && IsWindow(hTarget))
	{
		// constructs a new overlay object
		Overlay* pOverlay = new Overlay();
		// sets the target window
		pOverlay->hTarget = hTarget;
		// this struct holds information for the window class
		WNDCLASSEX WindowClass;
		// zeros out the structure
		ZeroMemory(&WindowClass, sizeof(WNDCLASSEX));
		// sets the window class size
		WindowClass.cbSize = sizeof(WNDCLASSEX);
		// sets the style of the window
		WindowClass.style = CS_HREDRAW | CS_VREDRAW;
		// sets the messge callback function
		WindowClass.lpfnWndProc = MessageHandler;
		// sets the instance this window is linked to
		WindowClass.hInstance = hInstance;
		// sets the window defualt cursor
		WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		// sets the window background
		WindowClass.hbrBackground = (HBRUSH)RGB(0, 0, 0);
		// sets the class title
		WindowClass.lpszClassName = lpTitle;
		// registers the overlay class
		if (RegisterClassExW(&WindowClass))
		{
			// stores the target window rect
			RECT rTarget;
			// gets the rect of the target window
			if (GetWindowRect(hTarget, &rTarget))
			{
				// stores the width of the window
				pOverlay->Width = rTarget.right - rTarget.left;
				// stores the height of the window
				pOverlay->Height = rTarget.bottom - rTarget.top;
				// creates the overlay window
				pOverlay->hWindow = CreateWindowExW(WS_EX_TRANSPARENT | WS_EX_LAYERED, lpTitle, lpTitle, WS_EX_TOPMOST | WS_POPUP, rTarget.left, rTarget.top, pOverlay->Width, pOverlay->Height, NULL, NULL, hInstance, NULL);
				// if the window has been created
				if (pOverlay->hWindow)
				{
					// sets the transparency color
					if (SetLayeredWindowAttributes(pOverlay->hWindow, RGB(0, 0, 0), NULL, LWA_COLORKEY))
					{
						// sets the opacity of the window
						if (SetLayeredWindowAttributes(pOverlay->hWindow, NULL, 255, LWA_ALPHA))
						{
							// initializes directx on the window
							if (pOverlay->InitDirectX())
							{
								// unregisters the overlay class
								// UnregisterClassW(lpTitle, hInstance);
								// returns the class object
								return pOverlay;
							}
						}
					}
				}
			}
			// unregisters the overlay class
			// UnregisterClassW(lpTitle, hInstance);
		}
		// destroys the overlay object
		delete pOverlay;
	}
	// else we return null
	return NULL;
}

// returns the width of the overlay
DWORD Overlay::GetWidth()
{
	// returns the width attribute
	return Width;
}

// returns the height of the overlay
DWORD Overlay::GetHeight()
{
	// returns the height attribute
	return Height;
}

// adds a menu to the overlay
/*Menu* Overlay::AddMenu(LPCWSTR lpTitle, D3DCOLOR d3dColor, Vector2 vPosition)
{
	// constructs a new menu
	Menu* pMenu = new Menu(lpTitle, d3dColor, vPosition);
	// adds the menu to the linked list of menus
	pMenu->Entry.Flink = MenuList.Flink;
	pMenu->Entry.Blink = &MenuList;
	MenuList.Flink->Blink = &pMenu->Entry;
	MenuList.Flink = &pMenu->Entry;
}*/
// add menu system
// size and move window with target window
// add destructor
// set up in constructor, then validate attributes in ::Create()
// overide Render and Update
// multiple threads

























// validates the main window and the target window exist
BOOL Overlay::ValidateWindows()
{
	// returns the result
	return IsWindow(hWindow) && IsWindow(hTarget);
}

// registers the overlay class
BOOL Overlay::RegisterOverlay(HINSTANCE hInstance, LPCWSTR lpTitle)
{
	// this struct holds information for the window class
	WNDCLASSEX WindowClass;
	// zeros out the structure
	ZeroMemory(&WindowClass, sizeof(WNDCLASSEX));
	// sets the window class size
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	// sets the style of the window
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	// sets the messge callback function
	WindowClass.lpfnWndProc = MessageHandler;
	// sets the instance this window is linked to
	WindowClass.hInstance = hInstance;
	// sets the window defualt cursor
	WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	// sets the window background
	WindowClass.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	// sets the class title
	WindowClass.lpszClassName = lpTitle;
	// registers the window class and returns true
	if (RegisterClassExW(&WindowClass)) return TRUE;
	// returns false if the registration failed
	else return FALSE;
}

// creates the overlay window
BOOL Overlay::CreateOverlay(HINSTANCE hInstance, LPCWSTR lpTitle)
{
	// stores the target window rect
	RECT rTarget;
	// gets the rect of the target window
	if (GetWindowRect(hTarget, &rTarget))
	{
		// creates the overlay window
		hWindow = CreateWindowExW(WS_EX_TRANSPARENT | WS_EX_LAYERED, lpTitle, lpTitle, WS_EX_TOPMOST | WS_POPUP, rTarget.left, rTarget.top, rTarget.right - rTarget.left, rTarget.bottom - rTarget.top, NULL, NULL, hInstance, NULL);
		// if the window has been created
		if (hWindow)
		{
			// sets the transparency color
			if (SetLayeredWindowAttributes(hWindow, RGB(0, 0, 0), NULL, LWA_COLORKEY))
			{
				// sets the opacity of the window
				if (SetLayeredWindowAttributes(hWindow, NULL, 255, LWA_ALPHA))
				{
					// window creation succeeded
					return TRUE;
				}
			}
		}
	}
	// window creation failed
	return FALSE;
}

// notifies the overlay to start drawing
BOOL Overlay::StartDrawing()
{
	// clears the current buffer
	if (DeviceObject->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0) == D3D_OK)
	{
		// creates a scene
		if (DeviceObject->BeginScene() == D3D_OK)
		{
			// drawing has started
			return TRUE;
		}
	}
	// drawing has failed
	return FALSE;
}

// notifies the overlay to stop drawing
BOOL Overlay::StopDrawing()
{
	// clears the current buffer
	if (DeviceObject->EndScene() == D3D_OK)
	{
		// creates a scene
		if (DeviceObject->PresentEx(0, 0, 0, 0, 0) == D3D_OK)
		{
			// drawing has started
			return TRUE;
		}
	}
	// drawing has failed
	return FALSE;
}

// shows the window
BOOL Overlay::Show()
{
	// the window was already visable so we return false
	if (ShowWindow(hWindow, SW_SHOW)) return FALSE;
	// the window was not visable so we return true
	else return TRUE;
}