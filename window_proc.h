HPEN hPen;
HPEN hGameOverPen;
HFONT hFont;

LRESULT CALLBACK WindowProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
){

    switch (uMsg)
    {
    case WM_CREATE:{

        srand(time(0));
        InitGame(hWnd);
        hPen = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
        hGameOverPen = CreatePen(PS_SOLID, 15, RGB(0, 0, 0));

        return 0;
    }

    case WM_SIZE:{

        if(hFont) DeleteObject(hFont);

        boxLen = (LOWORD(lParam) / 3 < HIWORD(lParam) / 3) ? (LOWORD(lParam) / 3) : (HIWORD(lParam) / 3);
        rcArea.left = (LOWORD(lParam) - boxLen * 3) / 2;
        rcArea.top = (HIWORD(lParam) - boxLen * 3) / 2;
        rcArea.right = rcArea.left + boxLen * 3;
        rcArea.bottom = rcArea.top + boxLen * 3;

        hFont = CreateFont(boxLen - 10,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
                CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH,TEXT("Impact"));

        SelectObject(GetDC(hWnd), hFont);

        RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);

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
        
        if(symbols[row][col] != ' ') return 0;

        symbols[row][col] = nextSymbol;
        symbolCount++;
        
        CheckIfGameIsOver(hWnd);
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

        FillRect(hDC, &rcLeft, hBackgroundBrush);
        FillRect(hDC, &rcRight, hBackgroundBrush);

        DeleteObject(hBackgroundBrush);

        return 1;
    }

    case WM_PAINT:{

        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(hWnd, &ps);

        // draw the boxes

        HPEN hOriginalPen = SelectObject(hDC, hPen);
        HFONT hOriginalFont = SelectObject(hDC, hFont);
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
        DeleteObject(hFont);
        DestroyWindow(hWnd);
    }

    case WM_DESTROY:
        PostQuitMessage(TRUE);
        return 0;
    
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

}
