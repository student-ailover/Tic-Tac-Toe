#ifndef WINDOW_PROC_H
#define WINDOW_PROC_H

static BOOL fPlayerNamesSelected;
static HWND hPlayerEdit[2];
static HWND hSaveButton;
static WNDPROC hEditDefaultProc;
static WNDPROC hButtonDefaultProc;

LRESULT CALLBACK MainWindowProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ButtonProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK EditProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK EditProc(
    HWND hEdit,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{

    switch (uMsg)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_TAB:
            if(hEdit == hPlayerEdit[0])
                SetFocus(hPlayerEdit[1]);
            else if(hEdit == hPlayerEdit[1]) SetFocus(hSaveButton);
            return 0;
        
        }
        break;
    }

    return hEditDefaultProc(hEdit, uMsg, wParam, lParam);

}

LRESULT CALLBACK ButtonProc(
    HWND hButton,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{

    switch (uMsg)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_TAB:
            SetFocus(hPlayerEdit[0]);
            return 0;

        // when the user presses Enter while the button has keyboard focuses,
        // it programmatically generate a 'Button Click' message
        case VK_RETURN:
            printf("enter\n");
            SendMessage(hButton, BM_CLICK, (WPARAM) 0, (LPARAM) 0);
            return 0;
        
        default:
            break;
        }
        break;
    
    default:
        break;
    }

    return hButtonDefaultProc(hButton, uMsg, wParam, lParam);

}

LRESULT CALLBACK DlgProc(
    HWND hDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{

    static HFONT hDlgFont;

    switch (uMsg)
    {

    case WM_CREATE:{

        RECT rcClient;
        GetClientRect(hDlg, &rcClient);

        // create a font for the dialog
        hDlgFont = CreateFont(
            rcClient.bottom / 5 - 5,
            0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
                CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH,TEXT("Times New Roman")
        );

        // create each edit control, set it's text font, and subclass its proc
        for(int i = 0; i < 2; i++){
            hPlayerEdit[i] = CreateWindow(
                "EDIT",
                playerName[i],
                WS_CHILD | WS_VISIBLE | ES_UPPERCASE | WS_BORDER,
                (rcClient.right / 5) * 2, (rcClient.bottom / 5) * (1 + i * 2),
                (rcClient.right / 5) * 2, (rcClient.bottom / 5),
                hDlg, NULL, NULL, NULL
            );
            SendMessage(hPlayerEdit[i], WM_SETFONT, (WPARAM) hDlgFont, (LPARAM) 0);
            int nameLen = strlen(playerName[i]);
            SendMessage(hPlayerEdit[i], EM_SETSEL, (WPARAM) nameLen, (LPARAM) nameLen);
            hEditDefaultProc = (WNDPROC) SetWindowLong(hPlayerEdit[i], GWL_WNDPROC, (LONG) EditProc);
        }
        
        // if unable to create the edit controls
        if(!hPlayerEdit[0] || !hPlayerEdit[1])
        {
            MessageBox(NULL, "UNABLE TO CREATE EDIT CONTROLS", NULL, MB_OK);
            return -1;
        }

        // create a save button
        hSaveButton = CreateWindow(
            "BUTTON",
            "Save",
            WS_CHILD | WS_VISIBLE,
            (rcClient.right - (rcClient.right / 5) * 2) / 2, (rcClient.bottom / 5) * 4,
            (rcClient.right / 5) * 2, (rcClient.bottom / 5),
            hDlg, NULL, NULL, NULL
        );
        
        if(!hSaveButton)
        {
            MessageBox(NULL, "UNABLE TO CREATE SAVE BUTTON", NULL, MB_OK);
            return -1;
        }

        // subclass its procedure
        hButtonDefaultProc = (WNDPROC) SetWindowLong(hSaveButton, GWL_WNDPROC, (LONG) ButtonProc);

        // set keyboard focus to first edit control
        SetFocus(hPlayerEdit[0]);

        return 0;
    }

    case WM_LBUTTONDOWN:{
        SetFocus(hDlg);
        return 0;
    }

    case WM_PAINT:{

        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(hDlg, &ps);

        RECT rcDlg;
        GetClientRect(hDlg, &rcDlg);

        HFONT hOriginalFont = SelectObject(hDC, hDlgFont);

        RECT rcText;
        rcText.left = 0;
        rcText.right = (rcDlg.right / 5) * 2;
        rcText.top = (rcDlg.bottom / 5) * 1;
        rcText.bottom = (rcDlg.bottom / 5) * 2;
        DrawText(hDC, "Player 1:", -1, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        rcText.top += (rcDlg.bottom / 5) * 2;
        rcText.bottom += (rcDlg.bottom / 5) * 2;
        DrawText(hDC, "Player 2:", -1, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hDC, hOriginalFont);
        EndPaint(hDlg, &ps);

        return 0;
    }

    case WM_COMMAND:{

        // change made here
        if(HIWORD(wParam) != BN_CLICKED) return 0;

        // when save button is clicked
        // if either edit control is empty then ignore the click
        else if(!GetWindowTextLength(hPlayerEdit[0]) || !GetWindowTextLength(hPlayerEdit[1]))
            return 0;

        fPlayerNamesSelected = TRUE;

        // retrieve the player names from their respective edit controls and save in the
        // "player names.sav" file
        GetWindowText(hPlayerEdit[0], playerName[0], sizeof(playerName[0]));
        GetWindowText(hPlayerEdit[1], playerName[1], sizeof(playerName[0]));
        printf("%s\n", playerName[0]);

        FILE *fp = fopen("player names.sav", "w");
        if(!fp)
        {
            MessageBox(NULL, "UNABLE TO SAVE PLAYER NAMES", NULL, MB_OK);
            return 0;
        }
        fprintf(fp, "%s %s", playerName[0], playerName[1]);
        fclose(fp);
        
        // set the proper title for the main window
        char mainWndTitle[50];
        strcpy(mainWndTitle, "Now, it is ");
        strcat(mainWndTitle, playerName[nextSymbol == 'X' ? 0 : 1]);
        strcat(mainWndTitle, "'s turn");
        SetWindowText(hMainWnd, mainWndTitle);

        // enable and show the main window, and then destroy the dialog
        EnableWindow(hMainWnd, TRUE);
        ShowWindow(hMainWnd, SW_NORMAL);
        DestroyWindow(hDlg);

        return 0;
    }

    case WM_CLOSE:{

        DestroyWindow(hDlg);
        PostQuitMessage(TRUE);

        return 0;
    }

    case WM_DESTROY:{

        if(!fPlayerNamesSelected) return 0;
        DeleteObject(hDlgFont);
        return 0;

    }
    
    default:
        return DefWindowProc(hDlg, uMsg, wParam, lParam);
    }

}

LRESULT CALLBACK MainWndProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
){

    static HINSTANCE hInst;
    static HPEN hBorderPen, hGameOverPen;
    static HFONT hSymbolFont;

    switch (uMsg)
    {
    case WM_CREATE:{

        hInst = ((CREATESTRUCT*) lParam)->hInstance;

        srand(time(0));
        InitGame(hWnd);

        hBorderPen = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
        hGameOverPen = CreatePen(PS_SOLID, 15, RGB(0, 0, 0));

        return 0;
    }

    // after the main window gets resized, destroy the existsing font
    // and create a new, bigger font
    case WM_SIZE:{

        if(hSymbolFont) DeleteObject(hSymbolFont);

        boxLen = (LOWORD(lParam) / 3 < HIWORD(lParam) / 3) ? (LOWORD(lParam) / 3) : (HIWORD(lParam) / 3);
        rcArea.left = (LOWORD(lParam) - boxLen * 3) / 2;
        rcArea.top = (HIWORD(lParam) - boxLen * 3) / 2;
        rcArea.right = rcArea.left + boxLen * 3;
        rcArea.bottom = rcArea.top + boxLen * 3;

        if(hSymbolFont) DeleteObject(hSymbolFont);
        hSymbolFont = CreateFont(boxLen - 10,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
                CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH,TEXT("Impact"));

        SelectObject(GetDC(hWnd), hSymbolFont);

        RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);

        return 0;
    }

    case WM_COMMAND:{

        printf("here\n");
        switch(LOWORD(wParam)){

            case IDM_SET_PLAYER_NAMES:
                AskPlayerNames();
                break;

        }

        return 0;
    }

    case WM_LBUTTONDOWN:{

        if(fGameOver){
            InitGame(hWnd);
            RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
            return 0;    
        }

        // stop playing the current sound
        PlaySound(NULL, NULL, SND_ASYNC);

        POINT cursorPos;
        cursorPos.x = LOWORD(lParam);
        cursorPos.y = HIWORD(lParam);

        // if cursor is outside the boxes
        if(!PtInRect(&rcArea, cursorPos)) return 0;

        int row = (cursorPos.y - rcArea.top) / boxLen;
        int col = (cursorPos.x - rcArea.left) / boxLen;
        
        if(symbols[row][col] != '\0') return 0;

        symbols[row][col] = nextSymbol;
        symbolCount++;
        
        CheckIfGameIsOver(hInst);
        RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

        if(!fGameOver){
            nextSymbol = nextSymbol == 'X'? 'O' : 'X';
            const char *nextPlayerName = nextSymbol == 'X' ? playerName[0] : playerName[1];
            strcpy(windowText, "Now, it is ");
            strcat(windowText, nextPlayerName);
            strcat(windowText, "'s turn");
            SetWindowText(hWnd, windowText);
        }

        return 0;
    }

    // send just before WM_PAINT
    case WM_ERASEBKGND:{

        HDC hDC = (HDC) wParam;
        HBRUSH hBackgroundBrush = CreateSolidBrush(RGB(255, 0, 0));

        RECT rcClient;
        GetClientRect(hWnd, &rcClient);

        RECT rcLeft;
        rcLeft.left = rcClient.left;
        rcLeft.top = rcClient.top;
        rcLeft.right = rcArea.left + 1;        
        rcLeft.bottom = rcClient.bottom;

        RECT rcRight;
        rcRight.left = rcArea.right;
        rcRight.top = rcClient.top;
        rcRight.right = rcClient.right;
        rcRight.bottom = rcClient.bottom;

        RECT rcTop;
        rcTop.left = rcArea.left;
        rcTop.right = rcArea.right;
        rcTop.top = rcClient.top;
        rcTop.bottom = rcArea.top;

        RECT rcBottom;
        rcBottom.left = rcArea.left;
        rcBottom.right = rcArea.right;
        rcBottom.top = rcArea.top;
        rcBottom.bottom = rcClient.bottom;

        FillRect(hDC, &rcLeft, hBackgroundBrush);
        FillRect(hDC, &rcRight, hBackgroundBrush);
        FillRect(hDC, &rcTop, hBackgroundBrush);
        FillRect(hDC, &rcBottom, hBackgroundBrush);

        DeleteObject(hBackgroundBrush);

        return 1;
    }

    // send invalidate region of client is to be painted
    case WM_PAINT:{

        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(hWnd, &ps);

        // draw the boxes

        HPEN hOriginalPen = SelectObject(hDC, hBorderPen);
        HFONT hOriginalFont = SelectObject(hDC, hSymbolFont);
        SetBkColor(hDC, RGB(255, 255, 255));
        SetTextColor(hDC, RGB(0, 0, 0));

        for(int row = 0; row < 3; row++){
            for(int col = 0; col < 3; col++){

                char text[2] = {0};
                RECT rcBox;
                rcBox.left = rcArea.left + col * boxLen;
                rcBox.right = rcBox.left + boxLen;
                rcBox.top = rcArea.top + row * boxLen;
                rcBox.bottom = rcBox.top + boxLen;

                // draw the rectange
                Rectangle(hDC, rcBox.left, rcBox.top, rcBox.right, rcBox.bottom);

                // draw the symbol
                text[0] = symbols[row][col];
                DrawText(hDC, text, -1, &rcBox, DT_VCENTER | DT_SINGLELINE | DT_CENTER);

            }
        }

        // draw the line
        if(fGameOver)
        {
            SelectObject(hDC, hGameOverPen);
            MoveToEx(hDC, lineStart.x, lineStart.y, (LPPOINT) NULL);
            LineTo(hDC, lineEnd.x, lineEnd.y);
        }

        SelectObject(hDC, hOriginalPen);
        SelectObject(hDC, hOriginalFont);
        EndPaint(hWnd, &ps);

        return 0;

    }

    case WM_CLOSE:{
        DestroyWindow(hWnd);
        return 0;
    }

    // send after the window is destroy, post a WM_QUIT which will end the message loop
    case WM_DESTROY:
        DeleteObject(hSymbolFont);
        DeleteObject(hBorderPen);
        DeleteObject(hGameOverPen);
        PostQuitMessage(TRUE);
        return 0;
    
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

}

#endif
