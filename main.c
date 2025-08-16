#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <time.h>
#include <stdlib.h>

BOOL AskPlayerNames();
void InitGame();
void CheckIfGameIsOver(HINSTANCE);
BOOL AskPlayerNames();

char symbols[3][3];
int symbolCount;
char nextSymbol, winnerSymbol;
int boxLen;
RECT rcArea;

char windowText[50];
char playerName[2][20];
BOOL fGameOver;

POINT lineStart, lineEnd;
HWND hMainWnd;

#define IDM_SET_PLAYER_NAMES 1
#include "resources.h"
#include "window_proc.h"

void InitGame(){

    fGameOver = FALSE;
    nextSymbol = rand() % 2? 'X' : 'O';

    const char *firstPlayer;
    firstPlayer = nextSymbol == 'X'? playerName[0] : playerName[1];
    strcpy(windowText, firstPlayer);
    strcat(windowText, " will be the first one to play");
    SetWindowText(hMainWnd, windowText);

    symbolCount = 0;
    memset(&lineStart, 0, sizeof(POINT));
    memset(&lineEnd, 0, sizeof(POINT));
    memset(symbols, '\0', 9);

}

void CheckIfGameIsOver(HINSTANCE hInst){

    // check for rows
    for(int row = 0; row < 3; row++){
        if(symbols[row][0] == symbols[row][1] && symbols[row][1] == symbols[row][2]
         && symbols[row][0] != '\0'){
            winnerSymbol = symbols[row][0];
            lineStart.x = rcArea.left;
            lineStart.y = rcArea.top + boxLen * row + boxLen / 2;
            lineEnd.x = rcArea.right;
            lineEnd.y = lineStart.y;
            fGameOver = TRUE;
        }
    }

    // check for columns
    for(int col = 0; col < 3; col++){
        if(symbols[0][col] == symbols[1][col] && symbols[1][col] == symbols[2][col]
         && symbols[0][col] != '\0'){
            winnerSymbol = symbols[0][col];
            lineStart.x = rcArea.left + boxLen * col + boxLen / 2;
            lineStart.y = rcArea.top;
            lineEnd.x = lineStart.x;
            lineEnd.y = rcArea.bottom;
            fGameOver = TRUE;
        }
    }

    // diagonals
    if(symbols[0][0] == symbols[1][1] && symbols[1][1] == symbols[2][2]
        && symbols[0][0] != '\0'){

        winnerSymbol = symbols[0][0];
        lineStart.x = rcArea.left;
        lineStart.y = rcArea.top;
        lineEnd.x = rcArea.right;
        lineEnd.y = rcArea.bottom;
        fGameOver = TRUE;
        
    }

    else if(symbols[0][2] == symbols[1][1] && symbols[1][1] == symbols[2][0]
        && symbols[0][2] != '\0'){

        winnerSymbol = symbols[0][2];
        lineStart.x = rcArea.right;
        lineStart.y = rcArea.top;
        lineEnd.x = rcArea.left;
        lineEnd.y = rcArea.bottom;
        fGameOver = TRUE;
        
    }

    // if no winner is found and all the symbols are filled then Game is 'Draw'
    else if(!fGameOver && symbolCount == 9){
        winnerSymbol = '\0';
        fGameOver = TRUE;
    }

    if(!fGameOver) return;

    RedrawWindow(hMainWnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);

    const char *winnerName;

    switch(winnerSymbol){

        case 'X': winnerName = playerName[0];
            break;

        case 'O': winnerName = playerName[1];
            break;

        case '\0':
            SetWindowText(hMainWnd, "DRAW");
            return;

    }

    strcpy(windowText, winnerName);
    strcat(windowText, " is the winner");
    SetWindowText(hMainWnd, windowText);

    strcpy(windowText, "Congratulations, ");
    strcat(windowText, winnerName);
    strcat(windowText, "!");

    if(!PlaySound(MAKEINTRESOURCE(IDS_VICTORY), hInst, SND_RESOURCE | SND_ASYNC))
        printf("unable to play sound\n");
    
    MessageBox(hMainWnd, "You are the winner", windowText, MB_OK);

}

BOOL AskPlayerNames(){

    // hide and disable main window
    EnableWindow(hMainWnd, FALSE);
    ShowWindow(hMainWnd, SW_HIDE);

    // retrieve the size of the screen, create a dialog window size is (1/5)th the screen's size,
    // and is centred on the screen
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HWND hDlg = CreateWindow(
        "My Dialog Class", "Enter the player names", WS_SYSMENU | WS_BORDER | WS_VISIBLE,
        screenWidth / 2.5, screenHeight / 2.5, screenWidth / 5, screenHeight / 5,
        NULL, NULL, NULL, NULL
    );
    if(!hDlg)
    {
        MessageBox(NULL, "UNABLE TO CREATE DIALOG WINDOW", NULL, MB_OK);
        return FALSE;
    }

    return TRUE;

}

int APIENTRY WinMain(
    HINSTANCE hInst,
    HINSTANCE hPrevInstance,
    LPSTR cmdLine,
    int nCmdShow
)
{

    HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDC_APP));
    if(!hIcon)
    {
        MessageBox(NULL, "UNABLE TO LOAD ICON", NULL, MB_OK);
        return FALSE;
    }

    WNDCLASS wc = {0};

    // define and register main window's class
    wc.hbrBackground = CreateSolidBrush(RGB(255, 0, 0));
    wc.lpszClassName = "My Window Class";
    wc.lpfnWndProc = MainWndProc;
    wc.hIcon = hIcon;

    if(!RegisterClass(&wc))
    {
        MessageBox(NULL, "UNABLE TO REGISTER CLASS", NULL, MB_OK);
        return FALSE;
    }

    // define and register dialog window's class
    wc.hbrBackground = NULL;
    wc.lpszClassName = "My Dialog Class";
    wc.lpfnWndProc = DlgProc;
    wc.hIcon = hIcon;

    if(!RegisterClass(&wc))
    {
        MessageBox(NULL, "UNABLE TO REGISTER DIALOG CLAS", NULL, MB_OK);
        return FALSE;
    }

    // create the main window and its menu
    HMENU hMenu = CreateMenu();
    AppendMenu(hMenu, MF_STRING, IDM_SET_PLAYER_NAMES, "Set Player Names");
    hMainWnd = CreateWindow(
        "My Window Class",
        NULL,
        WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_SIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, hMenu, hInst, NULL
    );
    if(!hMainWnd)
    {
        MessageBox(NULL, "UNABLE TO CREATE WINDOW", NULL, MB_OK);
        return FALSE;
    }

    // try to open the save and find the player names
    FILE *fp = fopen("player names.sav", "r");

    // if unable to do so, ask the user directly through a dialog window
    if(!fp)
    {
        if(!AskPlayerNames()) return FALSE;
    }
    else{

        fscanf(fp, "%s %s", playerName[0], playerName[1]);
        fclose(fp);

        // make the main window visible
        ShowWindow(hMainWnd, SW_NORMAL);

    }

    InitGame();

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;

}