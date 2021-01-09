// WarCards.cpp : Defines the entry point for the application.
//
#include "framework.h"
#include "WarCards.h"
#include "../CardLib/CardLib.h"

#define MAX_LOADSTRING 100

// Defines for Slots
#define DECK_SLOT 1
#define PREADY_SLOT 2
#define PPLAYED_SLOT 3
#define PEARNED_SLOT 4
#define CREADY_SLOT 5
#define CPLAYED_SLOT 6
#define CEARNED_SLOT 7

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
CardWindow cardwnd;

// Dealing Callback
void CARDLIBPROC deal_deck(CardRegion& cardrgn, int iNumClicked)
{
    CardRegion* player_ready = cardwnd.CardRegionFromId(PREADY_SLOT);
    CardRegion* computer_ready = cardwnd.CardRegionFromId(CREADY_SLOT);
    
    cardrgn.MoveCard(player_ready, 26, true);
    cardrgn.MoveCard(computer_ready, 26, true);

    //CardStack deck = cardrgn.GetCardStack();
    
}

// Create Game Method
void createGame()
{    
    // Layout:
    //          Deck (down)
    //  P1 (d) Battle l/r (d)  AI (down)
    //
    //  P1.earned (Up)     AI.earned (Up)

    // 1. Start -> Click Deck
    //    - then Split Deck to Two
    // 2. AI Lays down Card
    //    - then Player Clicks Card to Laydown
    // 3. Highest Card Goes to {victor}.earned
    //    - Check Win Condition
    //       * Player or AI Cards + Earned == 0
    //          + Win Graphics
    //    - Check Reset Deck Condition
    //       * Player/AI Cards == 0 and Earned > 0
    //          + Shuffle Player/AI Cards :TODO: Set as Option
    //          + Move Earned to Cards
    //    - Goto #2
    //// TODO: Map out war.

    // Initialize Stacks
    CardStack deck;
    CardStack playerReady;
    CardStack playerEarned;
    CardStack playerPlayed;
    CardStack computerReady;
    CardStack computerEarned;
    CardStack computerPlayed;

    // Initialize Slots for Stacks
    CardRegion* start_deck = cardwnd.CreateRegion(DECK_SLOT,    true, 160,  10, 0, 0);
    CardRegion* player_ready = cardwnd.CreateRegion(PREADY_SLOT,  true,  10, 110, 0, 0);
    CardRegion* player_played = cardwnd.CreateRegion(PPLAYED_SLOT, true, 110, 110, 0, 0);
    CardRegion* player_earned = cardwnd.CreateRegion(PEARNED_SLOT, true,  10, 210, 0, 0);
    CardRegion* computer_ready = cardwnd.CreateRegion(CREADY_SLOT,  true, 310, 110, 0, 0);
    CardRegion* computer_played = cardwnd.CreateRegion(CPLAYED_SLOT, true, 210, 110, 0, 0);
    CardRegion* computer_earned = cardwnd.CreateRegion(CEARNED_SLOT, true, 310, 210, 0, 0);

    // Set Stack Properties
    start_deck->SetFaceDirection(CS_FACE_DOWN, 0);
    player_ready->SetFaceDirection(CS_FACE_DOWN, 0);
    player_played->SetFaceDirection(CS_FACE_UP, 0);
    player_earned->SetFaceDirection(CS_FACE_DOWN, 0);
    computer_ready->SetFaceDirection(CS_FACE_DOWN, 0);
    computer_played->SetFaceDirection(CS_FACE_UP, 0);
    computer_earned->SetFaceDirection(CS_FACE_DOWN, 0);

    // Fill Deck with Cards and Shuffle
    deck.NewDeck();
    deck.Shuffle();

    // Place Deck on Start
    start_deck->SetCardStack(deck);

    
    start_deck->SetClickProc(deal_deck);
    
    // Draw
    cardwnd.Update();
    cardwnd.Redraw();
}


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
    LoadStringW(hInstance, IDC_WARCARDS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WARCARDS));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WARCARDS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WARCARDS);
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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
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
    int wWidth, wHeight;

    switch (message)
    {
    case WM_CREATE:
        {
            cardwnd.Create(hWnd, WS_EX_CLIENTEDGE, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0);
            createGame();
        }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_NEWGAME:
                createGame();
                break;
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
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_SIZE:
        {
            wWidth = LOWORD(lParam);
            wHeight = HIWORD(lParam);

            MoveWindow(cardwnd, 0, 0, wWidth, wHeight, TRUE);
        }
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
