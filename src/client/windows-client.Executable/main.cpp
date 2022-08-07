// windows-client.Executable.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "windows-client.Executable.h"

#include "engine.hpp"

#include "application.hpp"

inline application* instance = new app();


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

inline BOOL LBUTTONDOWN = false;

class Controller
{
public:
    XINPUT_KEYSTROKE keystroke;
    XINPUT_STATE state;
    DWORD status;
    int id;

    Controller(int id=-1) {
        this->id = id;
        if(id >= 0) GetState();
    }
    Controller &GetState() {
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        status = XInputGetState(id, &state);
        return *this;
    }
    Controller& GetKeystroke() {
        ZeroMemory(&keystroke, sizeof(XINPUT_KEYSTROKE));
        status = XInputGetKeystroke(id, 0, &keystroke);
        return *this;
    }

    bool IsConnected() {
        return id >= 0 && status == ERROR_SUCCESS;
    }

    operator bool() {
        return IsConnected();
    }

    // TODO: support a time, so this should be threaded off
    void Vibrate(int left, int right) {
        XINPUT_VIBRATION vibration;
        ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
        vibration.wLeftMotorSpeed = left;
        vibration.wRightMotorSpeed = right;
        XInputSetState(id, &vibration);
    }
};

Controller controllers[2];

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSCLIENTEXECUTABLE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSCLIENTEXECUTABLE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // foo();

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSCLIENTEXECUTABLE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSCLIENTEXECUTABLE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   int width = 800;
   int height = 1200;

   // TODO: Add fullscreen, borderless fullscreen and variable resolutions
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, width, height, nullptr, nullptr, hInstance, nullptr);
   if (!hWnd)
   {
      return false;
   }

   RECT rcClient, rcWind;
   POINT ptDiff;
   GetClientRect(hWnd, &rcClient);
   GetWindowRect(hWnd, &rcWind);
   width -= (rcWind.right - rcWind.left) - rcClient.right;
   height -= (rcWind.bottom - rcWind.top) - rcClient.bottom;

   auto HDC = GetDC(hWnd);

   static PIXELFORMATDESCRIPTOR pfd;
   memset(&pfd, 0, sizeof(pfd));
   pfd.nSize = sizeof(pfd);
   pfd.nVersion = 1;
   pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
   pfd.iPixelType = PFD_TYPE_RGBA;
   pfd.cColorBits = 32;

   auto pf = ChoosePixelFormat(HDC, &pfd);
   if (pf == 0) {
       return false;
   }

   if (SetPixelFormat(HDC, pf, &pfd) == FALSE) {
       return false;
   }

   DescribePixelFormat(HDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

   auto _HGLRC = wglCreateContext(HDC);
   wglMakeCurrent(HDC, _HGLRC);

   // TODO: Move to the graphics abstraction
   GLenum err = glewInit();

   if (err != GLEW_OK) {
       return false;
   }
   //    exit(1); // or handle the error in a nicer way
   //if (!GLEW_VERSION_2_1)  // check that the machine supports the 2.1 API.
   //    exit(1); // or handle the error in a nicer wa

   //AddClipboardFormatListener(hWnd);

   ShowWindow(hWnd, nCmdShow);

   SwapBuffers(HDC);

   UpdateWindow(hWnd);

   char executablePath[MAX_PATH];
   GetModuleFileNameA(NULL, executablePath, MAX_PATH);

   auto assetPath = filesystem->dirname(executablePath) + "\\..\\..\\assets";
   assets->init((void *)assetPath.c_str());
   assets->set("shader", "shaders.es320");

   instance->dimensions(width, height)->on_startup();

   return instance->started = true;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int nNoOfDevices = 0;
    POINT p;

    static auto hdc = GetDC(hWnd);

    static bool sizing = false;

    static bool active = true;

    RAWINPUT rawinput;
    UINT szData = sizeof(rawinput), szHeader = sizeof(RAWINPUTHEADER);
    HRAWINPUT handle;

    switch (message)
    {
    case WM_ACTIVATEAPP:
        if (wParam) {
            active = true;
        }
        else {
            active = false;
        }
        break;

    case WM_CREATE:
        AllocConsole();

        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);

        UINT nDevices;
        PRAWINPUTDEVICELIST pRawInputDeviceList;
        GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST));

        pRawInputDeviceList = (PRAWINPUTDEVICELIST)malloc(sizeof(RAWINPUTDEVICELIST) * nDevices);

        nNoOfDevices = GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST));

        RID_DEVICE_INFO rdi;
        rdi.cbSize = sizeof(RID_DEVICE_INFO);

        for (int i = 0; i < nNoOfDevices; i++) {
            TCHAR device[256];
            GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICENAME, device, 0);

            UINT cbSize = rdi.cbSize;
            GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICEINFO, &rdi, &cbSize);

            //std::wcout << L"Device Name: " << device << std::endl;
            //std::cout << "rdi.dwType ............................ " << rdi.dwType << std::endl;

            if (0 && rdi.dwType == RIM_TYPEMOUSE)
            {
                std::cout << "rdi.mouse.dwId ........................ " << rdi.mouse.dwId << std::endl;
                std::cout << "rdi.mouse.dwNumberOfButtons ........... " << rdi.mouse.dwNumberOfButtons << std::endl;
                std::cout << "rdi.mouse.dwSampleRate ................ " << rdi.mouse.dwSampleRate << std::endl;
            }

            if (0 && rdi.dwType == RIM_TYPEKEYBOARD)
            {
                std::cout << "rdi.keyboard.dwKeyboardMode ........... " << rdi.keyboard.dwKeyboardMode << std::endl;
                std::cout << "rdi.keyboard.dwNumberOfFunctionKeys ... " << rdi.keyboard.dwNumberOfFunctionKeys << std::endl;
                std::cout << "rdi.keyboard.dwNumberOfIndicators ..... " << rdi.keyboard.dwNumberOfIndicators << std::endl;
                std::cout << "rdi.keyboard.dwNumberOfKeysTotal ...... " << rdi.keyboard.dwNumberOfKeysTotal << std::endl;
                std::cout << "rdi.keyboard.dwType ................... " << rdi.keyboard.dwType << std::endl;
                std::cout << "rdi.keyboard.dwSubType ................ " << rdi.keyboard.dwSubType << std::endl;
            }

            if (0 && rdi.dwType == RIM_TYPEHID)
            {
                std::cout << "rdi.hid.dwVendorId .................... " << rdi.hid.dwVendorId << std::endl;
                std::cout << "rdi.hid.dwProductId ................... " << rdi.hid.dwProductId << std::endl;
                std::cout << "rdi.hid.dwVersionNumber ............... " << rdi.hid.dwVersionNumber << std::endl;
                std::cout << "rdi.hid.usUsage ....................... " << rdi.hid.usUsage << std::endl;
                std::cout << "rdi.hid.usUsagePage ................... " << rdi.hid.usUsagePage << std::endl;
            }
        }

        free(pRawInputDeviceList);

        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms645565(v=vs.85).aspx
        RAWINPUTDEVICE rid;
        rid.usUsagePage = 0x01;
        rid.dwFlags = RIDEV_INPUTSINK; //RIDEV_NOLEGACY
        rid.hwndTarget = hWnd;

        rid.usUsage = 0x02; // Mouse
        RegisterRawInputDevices(&rid, 1, sizeof(rid));

        rid.usUsage = 0x06; // Keyboard
        RegisterRawInputDevices(&rid, 1, sizeof(rid));

        rid.usUsage = 0x04; // Joystick
        RegisterRawInputDevices(&rid, 1, sizeof(rid));

        rid.usUsage = 0x05; // Gamepad
        RegisterRawInputDevices(&rid, 1, sizeof(rid));

        // _HMENU = CreatePopupMenu();
        // AppendMenu(_HMENU, MF_STRING, ID_TRAY_EXIT_CONTEXT_MENU_ITEM, TEXT("Exit"));

        //instance->dimensions(LOWORD(lParam), HIWORD(lParam))->on_resize();

        controllers[0].id = 0;
        controllers[1].id = 1;
        controllers[2].id = 2;

        //SetTimer(hWnd, 1, 10, NULL);
        SetTimer(hWnd, 1, 1, NULL);

        break;

    case WM_INPUT:
        if (active == false) {
            break;
        }

        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms645546(v=vs.85).aspx

        if (controllers[0].GetKeystroke()) {
            if (controllers[0].keystroke.Flags == XINPUT_KEYSTROKE_KEYDOWN) {
                if (gui->raise({ platform::input::GAMEPAD, platform::input::DOWN, controllers[0].keystroke.VirtualKey ^ 0x5800, 0, 0.0f, { 0.0f, 0.0f, 0.0f } }, 0, 0) == false) {
                    input->raise({ platform::input::GAMEPAD, platform::input::DOWN, controllers[0].keystroke.VirtualKey ^ 0x5800, 1, 0.0f, { 0.0f, 0.0f, 0.0f } });
                }
            }
            if (controllers[0].keystroke.Flags == XINPUT_KEYSTROKE_REPEAT) {
                if (gui->raise({ platform::input::GAMEPAD, platform::input::HELD, controllers[0].keystroke.VirtualKey ^ 0x5800, 0, 0.0f, { 0.0f, 0.0f, 0.0f } }, 0, 0) == false) {
                    input->raise({ platform::input::GAMEPAD, platform::input::HELD, controllers[0].keystroke.VirtualKey ^ 0x5800, 1, 0.0f, { 0.0f, 0.0f, 0.0f } });
                }
            }
            if (controllers[0].keystroke.Flags == XINPUT_KEYSTROKE_KEYUP) {
                if (gui->raise({ platform::input::GAMEPAD, platform::input::UP, controllers[0].keystroke.VirtualKey ^ 0x5800, 0, 0.0f, { 0.0f, 0.0f, 0.0f } }, 0, 0) == false) {
                    input->raise({ platform::input::GAMEPAD, platform::input::UP, controllers[0].keystroke.VirtualKey ^ 0x5800, 1, 0.0f, { 0.0f, 0.0f, 0.0f } });
                }
            }
        }

        handle = reinterpret_cast<HRAWINPUT>(lParam);
        //RAWINPUT input;
        //UINT szData = sizeof(input), szHeader = sizeof(RAWINPUTHEADER);
        //HRAWINPUT handle = reinterpret_cast<HRAWINPUT>(lParam);

        GetRawInputData(handle, RID_INPUT, &rawinput, &szData, szHeader);

        if (0 && rawinput.header.dwType == RIM_TYPEHID) { // Using xinput instead && rawinput.data.hid.bRawData & VK_GAMEPAD_MENU) {
            std::cout << "Generic HID - ";
        }

        if (0 && rawinput.header.dwType == RIM_TYPEKEYBOARD)
        {
            std::cout << "Keyboard - ";
            std::cout << " MakeCode:" << rawinput.data.keyboard.MakeCode;
            std::cout << " Flags:" << rawinput.data.keyboard.Flags;
            std::cout << " Reserved:" << rawinput.data.keyboard.Reserved;
            std::cout << " ExtraInformation:" << rawinput.data.keyboard.ExtraInformation;
            std::cout << " Message:" << rawinput.data.keyboard.Message;
            std::cout << " VKey:" << rawinput.data.keyboard.VKey;
            std::cout << std::endl;
        }

        if (0 && rawinput.header.dwType == RIM_TYPEMOUSE)
        {
            std::cout << "Mouse - ";
            std::cout << " usFlags:" << rawinput.data.mouse.usFlags;
            std::cout << " ulButtons:" << rawinput.data.mouse.ulButtons;
            std::cout << " usButtonFlags:" << rawinput.data.mouse.usButtonFlags;
            std::cout << " usButtonData:" << rawinput.data.mouse.usButtonData;
            std::cout << " ulRawButtons:" << rawinput.data.mouse.ulRawButtons;
            std::cout << " lLastX:" << rawinput.data.mouse.lLastX;
            std::cout << " lLastY:" << rawinput.data.mouse.lLastY;
            std::cout << " ulExtraInformation:" << rawinput.data.mouse.ulExtraInformation;
            std::cout << std::endl;
        }

        if (rawinput.header.dwType == RIM_TYPEKEYBOARD && (rawinput.data.keyboard.Flags == 0 || rawinput.data.keyboard.Flags == 2))
        {
            gui->raise({ platform::input::KEY, platform::input::DOWN, rawinput.data.keyboard.VKey, 0, 0.0f, { 0.0f, 0.0f, 0.0f } }, 0, 0);
            input->raise({ platform::input::KEY, platform::input::DOWN, rawinput.data.keyboard.VKey, 1, 0.0f, { 0.0f, 0.0f, 0.0f } });
        }
        if (rawinput.header.dwType == RIM_TYPEKEYBOARD && (rawinput.data.keyboard.Flags == 1 || rawinput.data.keyboard.Flags == 3))
        {
            gui->raise({ platform::input::KEY, platform::input::UP, rawinput.data.keyboard.VKey, 0, 0.0f, { 0.0f, 0.0f, 0.0f } }, 0, 0);
            input->raise({ platform::input::KEY, platform::input::UP, rawinput.data.keyboard.VKey, 1, 0.0f, { 0.0f, 0.0f, 0.0f } });
        }

        if (rawinput.header.dwType == RIM_TYPEMOUSE && rawinput.data.mouse.ulButtons == 0)
        {
            GetCursorPos(&p);
            ScreenToClient(hWnd, &p);
            if (gui->raise({ platform::input::POINTER, platform::input::MOVE, 1, 0, 0.0f, { (float)p.x, (float)p.y, 0.0f } }, p.x, p.y) == false) {
                input->raise({ platform::input::POINTER, platform::input::MOVE, 0, 1, 0.0f, { (float)p.x, (float)p.y, 0.0f } });
            }
        }
        if (rawinput.header.dwType == RIM_TYPEMOUSE && rawinput.data.mouse.ulButtons != 0)
        {
            GetCursorPos(&p);
            ScreenToClient(hWnd, &p);

            switch (rawinput.data.mouse.usButtonFlags) {
            case(RI_MOUSE_LEFT_BUTTON_DOWN):
                if (gui->raise({ platform::input::POINTER, platform::input::DOWN, 1, 0, 0.0f, { (float)p.x, (float)p.y, 0.0f } }, p.x, p.y) == false) {
                    input->raise({ platform::input::POINTER, platform::input::DOWN, 1, 0, 0.0f, { (float)p.x, (float)p.y, 0.0f } });
                }
                break;
            case(RI_MOUSE_RIGHT_BUTTON_DOWN):
                if (gui->raise({ platform::input::POINTER, platform::input::DOWN, 2, 0, 0.0f, { (float)p.x, (float)p.y, 0.0f } }, p.x, p.y) == false) {
                    input->raise({ platform::input::POINTER, platform::input::DOWN, 2, 0, 0.0f, { (float)p.x, (float)p.y, 0.0f } });
                }
                break;
            case(RI_MOUSE_MIDDLE_BUTTON_DOWN):
                input->raise({ platform::input::POINTER, platform::input::DOWN, 3, 0, 0.0f, { (float)p.x, (float)p.y, 0.0f } });
                break;
            case(RI_MOUSE_LEFT_BUTTON_UP):
                input->raise({ platform::input::POINTER, platform::input::UP, 1, 0, 0.0f, { (float)p.x, (float)p.y, 0.0f } });
                break;
            case(RI_MOUSE_RIGHT_BUTTON_UP):
                input->raise({ platform::input::POINTER, platform::input::UP, 2, 0, 0.0f, { (float)p.x, (float)p.y, 0.0f } });
                break;
            case(RI_MOUSE_MIDDLE_BUTTON_UP):
                input->raise({ platform::input::POINTER, platform::input::UP, 3, 0, 0.0f, { (float)p.x, (float)p.y, 0.0f } });
                break;
            case(RI_MOUSE_WHEEL):
                //(float)rawinput.data.mouse.usButtonData == 0xFF88 ? 0.1f : -0.1f
                short travel = rawinput.data.mouse.usButtonData;
                input->raise({ platform::input::POINTER, platform::input::WHEEL, 3, 0, (float)travel, { 0.0, 0.0f, 0.0f } });
                break;
            };
        }
        if (rawinput.header.dwType == RIM_TYPEMOUSE && rawinput.data.mouse.usButtonFlags & 0x0400)
        {
            if (rawinput.data.mouse.usButtonData == 0xFF88) { // 65416
                //instance->on_zoom_in();
            }
            if (rawinput.data.mouse.usButtonData == 0x0078) { // 120
                //instance->on_zoom_out();
            }
        }

        break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_SIZING:
        sizing = true;
        break;

    case WM_SIZE:
        instance->dimensions(LOWORD(lParam), HIWORD(lParam))->on_resize();
        sizing = false;
        //PostMessage(hWnd, WM_PAINT, 0, 0);
        break;

    case WM_TIMER:
        for (int i = 0; i < 3; i++) {
            if (0 && controllers[i].GetState()) {
                float LX = controllers[i].state.Gamepad.sThumbLX;
                float LY = controllers[i].state.Gamepad.sThumbLY;

                //determine how far the controller is pushed
                float magnitude = sqrt(LX * LX + LY * LY);

                //determine the direction the controller is pushed
                float normalizedLX = LX / magnitude;
                float normalizedLY = LY / magnitude;

                float normalizedMagnitude = 0;

                //check if the controller is outside a circular dead zone
                if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
                {
                    //clip the magnitude at its expected maximum value
                    if (magnitude > 32767) magnitude = 32767;

                    //adjust magnitude relative to the end of the dead zone
                    magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

                    //optionally normalize the magnitude with respect to its expected range
                    //giving a magnitude value of 0.0 to 1.0
                    normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                }
                else //if the controller is in the deadzone zero out the magnitude
                {
                    magnitude = 0.0;
                    normalizedMagnitude = 0.0;
                }
            }

        }

        if (instance->started) {
            instance->on_interval();
            instance->on_draw();
        }

        SwapBuffers(hdc);
        break;

    //case WM_PAINT:
        //PAINTSTRUCT ps;
        //HDC hdc = BeginPaint(hWnd, &ps);
        //EndPaint(hWnd, &ps);

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
