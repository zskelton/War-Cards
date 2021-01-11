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
#define PWAR_SLOT 10
#define CWAR_SLOT 11

// Defines for Returns
#define PLAYER_WON 100
#define COMPUTER_WON 101
#define TIED_CARDS 102
#define NO_WIN 103

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
CardWindow cardwnd;
bool gameOn = false;
bool autoPlay = false;

// Function Identifier
void createGame();

// Dealing Callback
void CARDLIBPROC dealDeck(CardRegion& cardrgn, int iNumClicked)
{
    if (!gameOn) {
        // Distribute in Two
        cardrgn.MoveCard(cardwnd.CardRegionFromId(PREADY_SLOT), 26, true);
        cardrgn.MoveCard(cardwnd.CardRegionFromId(CREADY_SLOT), 26, true);
        // Set Game to On
        gameOn = true;
    }
}

// Get Score Label
TCHAR* setlabel(bool isPlayer, int score = 0)
{
    _Post_ _Notnull_ TCHAR* label = (TCHAR*)malloc(100 * sizeof(TCHAR));
    if (isPlayer) {
        _stprintf_s(label, 100, "Player: %i", (score));
    }
    else {
        _stprintf_s(label, 100, "Computer: %i", (score)); // FIXME: C6387 Error
    }
    return label;
}

// Returns 0 - Tie, 1 - Player, 2 - Computer as Winner
int getWinner(CardRegion* player_played, CardRegion* computer_played)
{
    // Compare
    CardStack player = player_played->GetCardStack();
    CardStack computer = computer_played->GetCardStack();
    Card pCard = player.Top();
    Card cCard = computer.Top();

    if (pCard.HiVal() > cCard.HiVal()) {
        return PLAYER_WON;
    }
    
    if (cCard.HiVal() > pCard.HiVal()) {
        return COMPUTER_WON;
    }

    return TIED_CARDS;
}

// Plays War on Ties
int playWar()
{
    // Set Variables
    int winner = TIED_CARDS;
    bool inWar = true;
    bool playerCanPlay = false;
    bool computerCanPlay = false;

    CardRegion* player_ready = cardwnd.CardRegionFromId(PREADY_SLOT);
    CardRegion* player_played = cardwnd.CardRegionFromId(PPLAYED_SLOT);
    CardRegion* player_earned = cardwnd.CardRegionFromId(PEARNED_SLOT);
    CardRegion* computer_ready = cardwnd.CardRegionFromId(CREADY_SLOT);
    CardRegion* computer_played = cardwnd.CardRegionFromId(CPLAYED_SLOT);
    CardRegion* computer_earned = cardwnd.CardRegionFromId(CEARNED_SLOT);
    CardRegion* player_war = cardwnd.CardRegionFromId(PWAR_SLOT);
    CardRegion* computer_war = cardwnd.CardRegionFromId(CWAR_SLOT);

    // Loop Until War Complete
    while (winner == TIED_CARDS) {
        // Reset Checks
        playerCanPlay = false;
        computerCanPlay = false;
        // Check Player Has Enough Cards and Play
        if (player_ready->NumCards() > 4) {
            playerCanPlay = true;
        }
        if (player_ready->NumCards() < 4 && player_earned->NumCards() > 4) {
            player_earned->MoveCard(player_ready, player_earned->NumCards(), true);
            player_ready->Shuffle();
            playerCanPlay = true;
        }
        if (playerCanPlay) {
            player_played->MoveCard(player_war, 1, true);
            player_ready->MoveCard(player_war, 3, true);
            player_ready->MoveCard(player_played, 1, true);
        }
        // Check Computer Has Enough Cards and Play
        if (computer_ready->NumCards() > 4) {
            computerCanPlay = true;
        }
        if (computer_ready->NumCards() < 4 && computer_earned->NumCards() > 4) {
            computer_earned->MoveCard(computer_ready, computer_earned->NumCards(), true);
            computer_ready->Shuffle();
            computerCanPlay = true;
        }
        if (computerCanPlay) {
            computer_played->MoveCard(computer_war, 1, true);
            computer_ready->MoveCard(computer_war, 3, true);
            computer_ready->MoveCard(computer_played, 1, true);
        }

        // If No one Can Play, give to Player by Default
        if (!computerCanPlay && !playerCanPlay) {
            winner = PLAYER_WON;
        }

        // Set New Winner and Exit if Not Tied
        winner = getWinner(player_played, computer_played);
    }

    // Send War Cards to Winner
    if (winner == PLAYER_WON) {
        player_war->MoveCard(player_earned, player_war->NumCards(), true);
        computer_war->MoveCard(player_earned, computer_war->NumCards(), true);
    }
    else {
        player_war->MoveCard(computer_earned, player_war->NumCards(), true);
        computer_war->MoveCard(computer_earned, computer_war->NumCards(), true);
    }

    // Return Final Winner
    return winner;
}

// Check For Win
int checkWin() {
    CardRegion* player_ready = cardwnd.CardRegionFromId(PREADY_SLOT);
    CardRegion* player_earned = cardwnd.CardRegionFromId(PEARNED_SLOT);
    CardRegion* computer_ready = cardwnd.CardRegionFromId(CREADY_SLOT);
    CardRegion* computer_earned = cardwnd.CardRegionFromId(CEARNED_SLOT);

    // Get Totals
    int cTotal = computer_ready->NumCards() + computer_earned->NumCards();
    int pTotal = player_ready->NumCards() + player_earned->NumCards();

    // Check for Winner
    if (cTotal <= 0) {
        return PLAYER_WON;
    }

    if (pTotal <= 0) {
        return COMPUTER_WON;
    }
    
    return NO_WIN;
}

// Handle Win Event
void handleWin(int winner) {
    _Post_ _Notnull_ TCHAR* result = (TCHAR*)malloc(100 * sizeof(TCHAR));
    if (winner == PLAYER_WON) {
        sprintf_s(result, 20, "You won!");
    }
    else {
        sprintf_s(result, 20, "You lost."); // FIXME: Same C6387 Error on initializing TCHAR*
    }
    MessageBox(cardwnd, result, _T("Game Over!"), MB_OK);
    autoPlay = false;
    cardwnd.DeleteAll();
    gameOn = false;
    createGame();
}

// React to User Click
void CARDLIBPROC playCard(CardRegion& cardrgn, int iNumClicked)
{
    // Pull Variables
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
        // Variables
        bool inWar = false;
        int winner;

        // Complete First Round
        player_ready->MoveCard(player_played, 1, true);
        computer_ready->MoveCard(computer_played, 1, true);
        winner = getWinner(player_played, computer_played);

        // Play War if Tied
        if (winner == TIED_CARDS) {
            winner = playWar();
        }

        // Move Played Card Based on Winner
        if (winner == PLAYER_WON) {
            player_played->MoveCard(player_earned, 1, true);
            computer_played->MoveCard(player_earned, 1, true);
        }
        else {
            player_played->MoveCard(computer_earned, 1, true);
            computer_played->MoveCard(computer_earned, 1, true);
        }

        // Show Scores
        lbl_computer->SetText(setlabel(false, (computer_ready->NumCards() + computer_earned->NumCards())));
        lbl_player->SetText(setlabel(true, (player_ready->NumCards() + player_earned->NumCards())));

        // Check Win
        int res = checkWin();
        if (res != NO_WIN) {
            handleWin(res);
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

    // Keep Playing, if Auto Play
    // FIXME: Freezes when clicking the menu, otherwise works.
    //if (autoPlay) {
    //    playCard(cardrgn, iNumClicked);
    //}
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
    CardRegion* player_war = cardwnd.CreateRegion(PWAR_SLOT, true, 110, 210, 0, 1);
    CardRegion* computer_war = cardwnd.CreateRegion(CWAR_SLOT, true, 210, 210, 0, 1);

    // Set Buttons
    cardwnd.CreateButton(PSCORE_LBL, (TCHAR*)"Player: 26", CB_STATIC | CB_ALIGN_LEFT, true, 20, 410, 50, 25);
    cardwnd.CreateButton(CSCORE_LBL, (TCHAR*)"Computer: 26", CB_STATIC | CB_ALIGN_RIGHT, true, 320, 410, 50, 25);

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
    start_deck->SetClickProc(dealDeck);
    // 2. React to User
    player_ready->SetClickProc(playCard);
    
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
                cardwnd.DeleteAll();
                gameOn = false;
                createGame();
                break;
            case IDM_AUTOPLAY:
                autoPlay = !autoPlay;
                if (autoPlay) {
                    CheckMenuItem(GetMenu(hWnd), IDM_AUTOPLAY, MF_CHECKED);
                }
                else {
                    CheckMenuItem(GetMenu(hWnd), IDM_AUTOPLAY, MF_UNCHECKED);
                }
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
