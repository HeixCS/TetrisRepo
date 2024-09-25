#include <iostream>
#include <thread>
using namespace std;
#include <Windows.h>
#include <chrono>

wstring tetrominio[7];
int nFieldWidth = 12;
int nFieldHeight = 18;

int nScreenWidth = 80; //X size
int nScreenHeight = 30; //Y Size
unsigned char* pField = nullptr;

int Rotate(int px, int py, int r)
{
    switch (r % 4)
    {
    case 0:
        return py * 4 + px; // 0 degrees
    case 1:
        return 12 + py - (px * 4); // 90 degrees
    case 2:
        return 15 - (py * 4) - px; // 180 degrees
    case 3:
        return 3 - py + (px * 4); // 270 degrees
    }
    return 0;
}
bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY) {
    
    for (int px = 0; px < 4; px++) {
        for (int py = 0; py < 4; py++) {
            int pi = Rotate(px, py, nRotation);

            int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

            if (nPosX + px >= 0 && nPosX + px < nFieldWidth) {
                if (nPosY + py >= 0 && nPosY + py < nFieldHeight) {
                    if (tetrominio[nTetromino][pi] == L'X' && pField[fi] != 0) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

int main()
{
    // Create assets
    tetrominio[0].append(L"..X.");
    tetrominio[0].append(L"..X.");
    tetrominio[0].append(L"..X.");
    tetrominio[0].append(L"..X.");

    tetrominio[1].append(L"..X.");
    tetrominio[1].append(L".XX.");
    tetrominio[1].append(L".X..");
    tetrominio[1].append(L"....");

    tetrominio[2].append(L".X..");
    tetrominio[2].append(L".XX.");
    tetrominio[2].append(L"..X.");
    tetrominio[2].append(L"....");

    tetrominio[3].append(L"....");
    tetrominio[3].append(L".XX.");
    tetrominio[3].append(L".XX.");
    tetrominio[3].append(L"....");

    tetrominio[4].append(L"..X.");
    tetrominio[4].append(L".XX.");
    tetrominio[4].append(L"..X.");
    tetrominio[4].append(L"....");

    tetrominio[5].append(L"....");
    tetrominio[5].append(L".XX.");
    tetrominio[5].append(L"..X.");
    tetrominio[5].append(L"..X.");

    tetrominio[6].append(L"....");
    tetrominio[6].append(L".XX.");
    tetrominio[6].append(L".X..");
    tetrominio[6].append(L".X..");

    // Initialize Field
    pField = new unsigned char[nFieldWidth * nFieldHeight];
    for (int x = 0; x < nFieldWidth; x++)
    {
        for (int y = 0; y < nFieldHeight; y++)
        {
            pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;
        }
    }

    // Makes screen
    wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
    for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';

    // Makes frame buffer to display
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    //Game Logic
    bool bGameOver = false;

    int nCurrentPiece = 1;
    int nCurrent= 0;
    int nCurrentX = nFieldWidth / 2;
    int nCurrentY = 0;
    int nCurrentRotation = 0;

    // Writes frame buffer to display
    WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
    bool bKey[4];
    bool bRotateHold = false;

    int nSpeed = 20;
    int nSpeedCounter = 0;
    bool bForceDown = false;
    int n = 1;
    while (!bGameOver) {
        //Game timing
        std::this_thread::sleep_for(50ms);
        nSpeedCounter++;
        bForceDown = (nSpeedCounter == nSpeed);


        //Input
        for (int k = 0; k < 4; k++) {
            bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;
        }
        //Game Logic
        if (bKey[0]) {
            if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)
                ) {
                nCurrentX = nCurrentX + 1;
            }
        }
        if (bKey[1]) {
            if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)
                ) {
                nCurrentX = nCurrentX - 1;
            }
        }
        if (bKey[2]) {
            if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY+1)
                ) {
                nCurrentY = nCurrentY + 1;
            }
        }
        if (bKey[3]) {
            if (!bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation+1, nCurrentX, nCurrentY + 1)
                ) {
                nCurrentRotation++;
                bRotateHold = true;
            }
            else {
                bRotateHold = false;
            }
            
        }
        
        if (bForceDown) {
            if ( DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1) ) {
                nCurrentY+=1;
            }
            else {
                //Lock piece into field
                //Have we any lines?
                for (int px = 0; px < 4; px++) {
                    for (int py = 0; py < 4; py++) {
                        if (tetrominio[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X') {
                            pField[(nCurrentY + py - 1) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;
                        }
                    }
                }
                //Choose next piece
                nCurrentX = nFieldWidth / 2;
                nCurrentY = 0;
                nCurrentRotation = 0;
                nCurrentPiece = rand() % 7;

                // If piece does not fit
                bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY + 1);

            }
            nSpeedCounter = 0;
        }
        //Render Output

        

        // Draw Field
        for (int x = 0; x < nFieldWidth; x++) {
            for (int y = 0; y < nFieldHeight; y++) {

                screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];

            }
        }
        //Draw current piece
        for (int px = 0; px < 4; px++) {
            for (int py = 0; py < 4; py++) {
                if (tetrominio[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X') {
                    screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;
                }
            }
        }

        // Displays Frame
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

    }
    return 0;
}