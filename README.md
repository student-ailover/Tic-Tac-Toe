# Tic Tac Toe
## Introduction
This is the first game that I've created. It is the original **Tic Tac Toe**, which is played by two players. In this, there is a grid of 3X3 cells which are all empty intially. Each player is assigned a symbol : either an 'X' or an 'O'. Each player takes their turn by clicking on an empty cell which fills it with his / her respective symbol. The player which manages to an entire row, column, or diagonal with his / her symbol wins. The sound effect of a crowd cheering is played thereafter.
## Player names
Before the game begins, the application looks for a "player names.sav" file in the working directory to assign the player names. If no such file is found, it creates a dialog window and prompts the user for the players' names. After the names are entered, a save file by the same name is created and the player names are stored in it. The user can also try to change the player names later by clicking on the 'Set Player Names' menu.
When the game begins, a pseudo-random number generator decides which player will play first.
## Creating resources
The victory sound effect and the application icons are stored as .wav and .ico formats, respectively. A resources.res file is created by the windows resource compiler which contains these two resources.<br>
`windres resources.rc -O coff reosurces.res`
## Compilation
The code is compiled using MinGw. <br>
`gcc main.c resources.res -o "tic tac toe.exe" -lgdi32 -lwinmm -mwindows`<br>
The gdi32 and winmm library defines the necessary functions for graphics operations and sound effect, respectively. -mwindows specifies the subsystem which the executable a purely GUI application and hence not have a console window.
