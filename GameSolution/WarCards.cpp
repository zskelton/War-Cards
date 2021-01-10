// WarCards.cpp : Defines the entry point for the application.
//
#include "framework.h"
#include "WarCards.h"
#include "../CardLib/CardLib.h"
#include <stdio.h>

#define MAX_LOADSTRING 100

// Defines for Slots
#define DECK_SLOT 1
#define PREADY_SLOT 2
#define PPLAYED_SLOT 3
#define PEARNED_SLOT 4
#define CREADY_SLOT 5
#define CPLAYED_SLOT 6
#define CEARNED_SLOT 7
#define PSCORE_LBL 8
#define CSCORE_LBL 9
#define WAR_LBL 10

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
CardWindow cardwnd;
bool gameOn = false;

// Dealing Callback
void CARDLIBPROC deal_deck(CardRegion& cardrgn, int iNumClicked)
{
    // Distribute in Two
    cardrgn.MoveCard(cardwnd.CardRegionFromId(PREADY_SLOT), 26, true);
    cardrgn.MoveCard(cardwnd.CardRegionFromId(CREADY_SLOT), 26, true);    
    // Set Game to On
    gameOn = true;
}

// React to User Click
void CARDLIBPROC play_card(CardRegion& cardrgn, int iNumClicked)
{
    // Pull Variables
    CardRegion* start_deck = cardwnd.CardRegionFromId(DECK_SLOT);
    CardRegion* player_ready = cardwnd.CardRegionFromId(PREADY_SLOT);
    CardRegion* player_played = cardwnd.CardRegionFromId(PPLAYED_SLOT);
    CardRegion* player_earned = cardwnd.CardRegionFromId(PEARNED_SLOT);
    CardRegion* computer_ready = cardwnd.CardRegionFromId(CREADY_SLOT);
    CardRegion* computer_played = cardwnd.CardRegionFromId(CPLAYED_SLOT);
    CardRegion* computer_earned = cardwnd.CardRegionFromId(CEARNED_SLOT);

    CardButton* lbl_player = cardwnd.CardButtonFromId(PSCORE_LBL);
    CardButton* lbl_computer = cardwnd.CardButtonFromId(CSCORE_LBL);

    if (gameOn)
    {
        // Move Player Card to Played
        player_ready->MoveCard(player_played, 1, true);

        // Move Computer Card to Played
        computer_ready->MoveCard(computer_played, 1, true);

        // Compare
        CardStack player = player_played->GetCardStack();
        CardStack computer = computer_played->GetCardStack();
        Card pCard = player.Top();
        Card cCard = computer.Top();
        CardRegion* winner;

        // TODO: MAKE WAR - FOR NOW, Solve by suit
        if (pCard.HiVal() >= cCard.HiVal()) {
            winner = player_earned;
        }
        else {
            winner = computer_earned;
        }

        // Move Both to $.Earned
        player_played->MoveCard(winner, 1, true);
        computer_played->MoveCard(winner, 1, true);

        // Show Score - Computer
        TCHAR cscore[100];
        sprintf_s(cscore, 100, "Computer: %i", (computer_ready->NumCards() + computer_earned->NumCards()));
        lbl_computer->SetText(cscore);

        // Show Score - Player
        TCHAR pscore[100];
        sprintf_s(pscore, 100, "Computer: %i", (player_ready->NumCards() + player_earned->NumCards()));
        lbl_player->SetText(pscore);

        // Check Win
        if ((player_ready->NumCards() + player_earned->NumCards()) == 52)
        {
            gameOn = false;
            MessageBox(cardwnd, _T("Winner!"), _T("Winner"), MB_OK);
        }

        // Check Empty Deck for Player
        if (player_ready->NumCards() == 0) {
            player_earned->Shuffle();
            player_earned->MoveCard(player_ready, player_earned->NumCards(), true);
        }

        // Check Empty Deck for Computer
        if (computer_ready->NumCards() == 0) {
            computer_earned->Shuffle();
            computer_earned->MoveCard(computer_ready, computer_earned->NumCards(), true);
        }
    }
}

// Create Game Method
void createGame()
{   
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

    // Set Buttons
    cardwnd.CreateButton(PSCORE_LBL, (TCHAR*)"Player: 0", CB_STATIC | CB_ALIGN_LEFT, true, 20, 410, 50, 25);
    cardwnd.CreateButton(CSCORE_LBL, (TCHAR*)"Computer: 0", CB_STATIC | CB_ALIGN_RIGHT, true, 320, 410, 50, 25);
    cardwnd.CreateButton(WAR_LBL, (TCHAR*)"", CB_STATIC | CB_ALIGN_CENTER, true, 160, 410, 50, 25);

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

    // Set Events
    // 1. Deal
    start_deck->SetClickProc(deal_deck);
    // 2. React to User
    player_ready->SetClickProc(play_card);
    
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
