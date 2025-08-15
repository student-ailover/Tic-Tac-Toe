#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <stdlib.h>

char symbols[3][3] = {
    {' ', ' ', ' '},
    {' ', ' ', ' '},
    {' ', ' ', ' '}
};
int symbolCount;
char nextSymbol, winnerSymbol;
int boxLen;
RECT rcArea;

char windowText[50];

char playerName[2][20] = {"Ayush", "Amol"};

BOOL fGameOver;

POINT lineStart, lineEnd;

void InitGame(HWND hWnd){

    fGameOver = FALSE;
    nextSymbol = rand() % 2? 'X' : 'O';

    const char *firstPlayer;
    firstPlayer = nextSymbol == 'X'? playerName[0] : playerName[1];
    strcpy(windowText, firstPlayer);
    strcat(windowText, " will be the first one to play");
    SetWindowText(hWnd, windowText);

    symbolCount = 0;
    memset(&lineStart, 0, sizeof(POINT));
    memset(&lineEnd, 0, sizeof(POINT));
    memset(symbols, ' ', 9);

}

void CheckIfGameIsOver(HWND hWnd){

    // rows
    for(int row = 0; row < 3; row++){
        if(symbols[row][0] == symbols[row][1] && symbols[row][1] == symbols[row][2]
         && symbols[row][0] != ' '){
            winnerSymbol = symbols[row][0];
            lineStart.x = rcArea.left;
            lineStart.y = rcArea.top + boxLen * row + boxLen / 2;
            lineEnd.x = rcArea.right;
            lineEnd.y = lineStart.y;
            fGameOver = TRUE;
        }
    }

    // columns
    for(int col = 0; col < 3; col++){
        if(symbols[0][col] == symbols[1][col] && symbols[1][col] == symbols[2][col]
         && symbols[0][col] != ' '){
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
        && symbols[0][0] != ' '){

        winnerSymbol = symbols[0][0];
        lineStart.x = rcArea.left;
        lineStart.y = rcArea.top;
        lineEnd.x = rcArea.right;
        lineEnd.y = rcArea.bottom;
        fGameOver = TRUE;
        
    }

    else if(symbols[0][2] == symbols[1][1] && symbols[1][1] == symbols[2][0]
        && symbols[0][2] != ' '){

        winnerSymbol = symbols[0][2];
        lineStart.x = rcArea.right;
        lineStart.y = rcArea.top;
        lineEnd.x = rcArea.left;
        lineEnd.y = rcArea.bottom;
        fGameOver = TRUE;
        
    }

    else if(!fGameOver && symbolCount == 9){
        winnerSymbol = ' ';
        fGameOver = TRUE;
    }

    if(!fGameOver) return;

    RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);

    const char *winnerName;

    switch(winnerSymbol){

        case 'X': winnerName = playerName[0];
            break;

        case 'O': winnerName = playerName[1];
            break;

        case ' ':
            SetWindowText(hWnd, "DRAW");
            return;

    }

    strcpy(windowText, winnerName);
    strcat(windowText, " is the winner");
    SetWindowText(hWnd, windowText);

    strcpy(windowText, "Congratulations, ");
    strcat(windowText, winnerName);
    strcat(windowText, "!");

    if(!PlaySound("victory.wav", NULL, SND_FILENAME | SND_ASYNC))
        printf("unable to play sound\n");
    
    MessageBox(hWnd, "You are the winner", windowText, MB_OK);

}

#include "window_proc.h"

int APIENTRY WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR cmdLine,
    int nCmdShow
)
{

    WNDCLASS wc = {0};
    wc.hbrBackground = CreateSolidBrush(RGB(255, 0, 0));
    wc.lpszClassName = "My Window Class";
    wc.lpfnWndProc = WindowProc;

    if(!RegisterClass(&wc))
    {
        MessageBox(NULL, "UNABLE TO REGISTER CLASS", NULL, MB_OK);
        return FALSE;
    }

    HWND hWnd = CreateWindow(
        "My Window Class",
        "Tic Tac Toe",
        WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_SIZEBOX,
        50, 50, 500, 400,
        NULL, NULL, NULL, NULL
    );
    if(!hWnd)
    {
        MessageBox(NULL, "UNABLE TO CREATE WINDOW", NULL, MB_OK);
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;

}