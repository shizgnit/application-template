// windows-client.Executable.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "windows-client.Executable.h"

#include "engine.hpp"

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

   int width = 100;
   int height = 100;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

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
       return 0;
   }

   if (SetPixelFormat(HDC, pf, &pfd) == FALSE) {
       return 0;
   }

   DescribePixelFormat(HDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

   auto _HGLRC = wglCreateContext(HDC);
   wglMakeCurrent(HDC, _HGLRC);

   GLenum err = glewInit();
   //if (err != GLEW_OK)
   //    exit(1); // or handle the error in a nicer way
   //if (!GLEW_VERSION_2_1)  // check that the machine supports the 2.1 API.
   //    exit(1); // or handle the error in a nicer wa

   //AddClipboardFormatListener(hWnd);

   ShowWindow(hWnd, nCmdShow);

   //gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, 0.1f, 1000.0f);
   //gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, -1.0f, 1.0f);

   SwapBuffers(HDC);

   UpdateWindow(hWnd);

   static char path[] = "C:\\Projects\\application-template\\src\\client\\android-client.Packaging\\assets\\";
   assets->init((void*)path);
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

    RAWINPUT input;
    UINT szData = sizeof(input), szHeader = sizeof(RAWINPUTHEADER);
    HRAWINPUT handle;

    switch (message)
    {
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

        break;

    case WM_INPUT:
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms645546(v=vs.85).aspx

        handle = reinterpret_cast<HRAWINPUT>(lParam);
        //RAWINPUT input;
        //UINT szData = sizeof(input), szHeader = sizeof(RAWINPUTHEADER);
        //HRAWINPUT handle = reinterpret_cast<HRAWINPUT>(lParam);

        GetRawInputData(handle, RID_INPUT, &input, &szData, szHeader);

        if (0 && input.header.dwType == RIM_TYPEKEYBOARD)
        {
            std::cout << "Keyboard - ";
            std::cout << " MakeCode:" << input.data.keyboard.MakeCode;
            std::cout << " Flags:" << input.data.keyboard.Flags;
            std::cout << " Reserved:" << input.data.keyboard.Reserved;
            std::cout << " ExtraInformation:" << input.data.keyboard.ExtraInformation;
            std::cout << " Message:" << input.data.keyboard.Message;
            std::cout << " VKey:" << input.data.keyboard.VKey;
            std::cout << std::endl;
        }

        if (0 && input.header.dwType == RIM_TYPEMOUSE)
        {
            std::cout << "Mouse - ";
            std::cout << " usFlags:" << input.data.mouse.usFlags;
            std::cout << " ulButtons:" << input.data.mouse.ulButtons;
            std::cout << " usButtonFlags:" << input.data.mouse.usButtonFlags;
            std::cout << " usButtonData:" << input.data.mouse.usButtonData;
            std::cout << " ulRawButtons:" << input.data.mouse.ulRawButtons;
            std::cout << " lLastX:" << input.data.mouse.lLastX;
            std::cout << " lLastY:" << input.data.mouse.lLastY;
            std::cout << " ulExtraInformation:" << input.data.mouse.ulExtraInformation;
            std::cout << std::endl;
        }

        if (input.header.dwType == RIM_TYPEKEYBOARD && (input.data.keyboard.Flags == 0 || input.data.keyboard.Flags == 2))
        {
            //platform::api::input->key_down(input.data.keyboard.VKey);
        }
        if (input.header.dwType == RIM_TYPEKEYBOARD && (input.data.keyboard.Flags == 1 || input.data.keyboard.Flags == 3))
        {
            //platform::api::input->key_up(input.data.keyboard.VKey);
        }

        if (input.header.dwType == RIM_TYPEMOUSE && input.data.mouse.ulButtons == 0)
        {
            GetCursorPos(&p);
            ScreenToClient(hWnd, &p);
            //platform::api::input->mouse_move(p.x, p.y);
        }
        if (input.header.dwType == RIM_TYPEMOUSE && input.data.mouse.usButtonFlags & 0x0001)
        {
            GetCursorPos(&p);
            ScreenToClient(hWnd, &p);
            //platform::api::input->touch_press((float)p.x, (float)p.y);
            //LBUTTONDOWN = true;
        }
        if (input.header.dwType == RIM_TYPEMOUSE && input.data.mouse.usButtonFlags & 0x0002)
        {
            //LBUTTONDOWN = false;
        }
        if (input.header.dwType == RIM_TYPEMOUSE && input.data.mouse.usButtonFlags & 0x0400)
        {
            if (input.data.mouse.usButtonData == 0xFF88) { // 65416
                //platform::api::input->touch_zoom_in();
            }
            if (input.data.mouse.usButtonData == 0x0078) { // 120
                //platform::api::input->touch_zoom_out();
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

    case WM_PAINT:
        {
            if(instance->started)
                instance->on_draw();
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_SIZE:
        instance->dimensions(LOWORD(lParam), HIWORD(lParam))->on_resize();
        PostMessage(hWnd, WM_PAINT, 0, 0);
        break;

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
