#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <ctime>
#include <fstream>

const int GRID_SIZE = 16;
const int BEGINNER_MINES = 20;
const int PRO_MINES = 40;
const int MASTER_MINES = 60;

struct Cell
{
    int value;
    bool isRevealed;
    bool isFlagged;
};

void SetClr(int tcl, int bcl)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (tcl + (bcl * 16)));
}

void getRowColbyLeftClick(int& rpos, int& cpos, bool& isRightClick)
{
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD Events;
    INPUT_RECORD InputRecord;
    SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
    do
    {
        ReadConsoleInput(hInput, &InputRecord, 1, &Events);
        if (InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
        {
            cpos = InputRecord.Event.MouseEvent.dwMousePosition.X;
            rpos = InputRecord.Event.MouseEvent.dwMousePosition.Y;
            isRightClick = false;
            break;
        }
        else if (InputRecord.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
        {
            cpos = InputRecord.Event.MouseEvent.dwMousePosition.X;
            rpos = InputRecord.Event.MouseEvent.dwMousePosition.Y;
            isRightClick = true;
            break;
        }
    } while (true);
}

void gotoRowCol(int rpos, int cpos)
{
    COORD scrn;
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    scrn.X = cpos;
    scrn.Y = rpos;
    SetConsoleCursorPosition(hOutput, scrn);
}

void printCellValue(int row, int col, int cellLength, int cellWidth, Cell cell, Cell** grid)
{
    int r = row * cellLength + cellLength / 2;
    int c = col * cellWidth + cellWidth / 2;
    gotoRowCol(r, c);
    if (cell.isRevealed)
    {
        if (cell.value == -1)
        {
            SetClr(12, 0);
            std::cout << "*";
            SetClr(7, 0);
        }
        else
        {
            int mineCount = 0;
            for (int i = -1; i <= 1; i++)
            {
                for (int j = -1; j <= 1; j++)
                {
                    int newRow = row + i;
                    int newCol = col + j;
                    if (newRow >= 0 && newRow < GRID_SIZE && newCol >= 0 && newCol < GRID_SIZE)
                    {
                        if (grid[newRow][newCol].value == -1)
                        {
                            mineCount++;
                        }
                    }
                }
            }
            if (mineCount > 0)
            {
                std::cout << mineCount;
            }
            else
            {
                std::cout << "0";
            }
        }
    }
    else
    {
        if (cell.isFlagged)
        {
            SetClr(9, 0);
            std::cout << char(-37);
            SetClr(7, 0);
        }
        else
        {
            std::cout << " ";
        }
    }
}

void printcellborder(int row, int col, int celllength, int cellwidth, Cell** grid)
{
    int r = row * celllength;
    int c = col * cellwidth;

    gotoRowCol(r, c);
    for (int width = 0; width < cellwidth; width++)
    {
        std::cout << char(-37);
    }
    for (int length = 0; length < celllength; length++)
    {
        gotoRowCol(r + length, c);
        std::cout << char(-37);
    }
    r += celllength;
    gotoRowCol(r, c);
    for (int width = 0; width <= cellwidth; width++)
    {
        std::cout << char(-37);
    }
    r -= celllength;
    c += cellwidth;
    for (int length = 0; length < celllength; length++)
    {
        gotoRowCol(r + length, c);
        std::cout << char(-37);
    }

    printCellValue(row, col, celllength, cellwidth, grid[row][col], grid);
}

void printgrid(const int& totalRows, const int& totalCols, Cell** grid)
{
    SetClr(7, 0);

    for (int r = 0; r < totalRows; r++)
    {
        for (int c = 0; c < totalCols; c++)
        {
            printcellborder(r, c, 4, 4, grid);
        }
    }
}

void getclick(int celllength, int cellwidth, int& row, int& col, bool& isRightClick, Cell** grid)
{
    getRowColbyLeftClick(row, col, isRightClick);
    row /= celllength;
    col /= cellwidth;
    if (isRightClick)
    {
        if (!grid[row][col].isRevealed)
        {
            grid[row][col].isFlagged = !grid[row][col].isFlagged;
            printgrid(GRID_SIZE, GRID_SIZE, grid);
        }
    }
    else
    {
        if (!grid[row][col].isFlagged)
        {
            if (grid[row][col].value == -1)
            {
                for (int r = 0; r < GRID_SIZE; r++)
                {
                    for (int c = 0; c < GRID_SIZE; c++)
                    {
                        grid[r][c].isRevealed = true;
                    }
                }
                system("cls");
                printgrid(GRID_SIZE, GRID_SIZE, grid);
                SetClr(12, 0);
                std::cout << "\n\n\nGAME OVER!!!\n\n\nPAESAE POORAE!!!\n\n\nCHAL SARAK!!!\nSARAK UDHAR!!!" << std::endl;
                _getch();
                exit(0);
            }
            else
            {
                grid[row][col].isRevealed = true;
                printgrid(GRID_SIZE, GRID_SIZE, grid);
            }
        }
    }
}

void initializeGrid(Cell** grid)
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            grid[i][j].value = 0;
            grid[i][j].isRevealed = false;
            grid[i][j].isFlagged = false;
        }
    }
    std::srand(std::time(nullptr));
    for (int i = 0; i < 40; i++)
    {
        int row = std::rand() % GRID_SIZE;
        int col = std::rand() % GRID_SIZE;
        if (grid[row][col].value != -1)
        {
            grid[row][col].value = -1;
        }
        else
            i--;
    }
}

bool isGameWon(Cell** grid)
{
    int revealedCount = 0;
    for (int r = 0; r < GRID_SIZE; r++)
    {
        for (int c = 0; c < GRID_SIZE; c++)
        {
            if (grid[r][c].isRevealed && grid[r][c].value != -1)
            {
                revealedCount++;
            }
        }
    }
    return revealedCount == (GRID_SIZE * GRID_SIZE - BEGINNER_MINES);
}

void printMenu()
{
    system("cls");
    SetClr(14, 0);
    std::cout << " SHURU KARAE YA NAHI?: " << std::endl;
    SetClr(10, 0);
    std::cout << "  +-------------------------+" << std::endl;
    std::cout << "  |  CHALO BHAI SHURU KARO  |" << std::endl;
    std::cout << "  +-------------------------+" << std::endl;
    std::cout << "  +-------------------------+" << std::endl;
    std::cout << "  |      CHAL BAE HAT       |" << std::endl;
    std::cout << "  +-------------------------+" << std::endl;
}

void printLevelMenu()
{
    system("cls");
    SetClr(14, 0);
    std::cout << " CHAL BHAI LEVEL BHI BATA DAE APNA: " << std::endl;
    SetClr(10, 0);
    std::cout << "  +------------------------+" << std::endl;
    std::cout << "  |         NALLA!         |" << std::endl;
    std::cout << "  +------------------------+" << std::endl;
    std::cout << "  |       BABAR AZAM       |" << std::endl;
    std::cout << "  +------------------------+" << std::endl;
    std::cout << "  |     SHANDAR MOBILE     |" << std::endl;
    std::cout << "  +------------------------+" << std::endl;
}

void printExit()
{
    system("cls");
    SetClr(12, 0);
    std::cout << "KAHAN SAE ATAE HAE YAE LOG!!!" << std::endl;
}

bool CheckUser(const std::string& username, const std::string& password)
{
    std::ifstream file("users.bin", std::ios::binary | std::ios::in);
    std::string storedUsername;
    std::string storedPassword;
    int gamesPlayed, wins, losses;

    while (file >> storedUsername >> storedPassword >> gamesPlayed >> wins >> losses)//games played,wins,losses not implemented 
    {
        if (storedUsername == username && storedPassword == password)
        {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

void registerUser(const std::string& username, const std::string& password)
{
    std::ofstream file("users.bin", std::ios::binary | std::ios::app);
    int gamesPlayed = 0;
    int wins = 0;
    int losses = 0;

    file << username << " " << password << " " << gamesPlayed << " " << wins << " " << losses << std::endl;
    file.close();
}

bool loginOrRegister()
{
    std::cout << "\nHAN BHAI NAYA NAYA AYA HAE YA POORANA BANDA??\n\nPRESS P OR p FOR POORANA BANDA\n\nPRESS F OR f FOR FRESHPIECE" << std::endl;
    char choice;
    std::cin >> choice;

    if (choice == 'P' || choice == 'p')
    {
        std::string username, password;
        std::cout << "APNA NAAM BATAYAE SAHAB G: ";
        std::cin >> username;
        std::cout << "APNA PASSWORD BATAYAE SAHAB G: ";
        std::cin >> password;
        return CheckUser(username, password);
    }
    else if (choice == 'F' || choice == 'f')
    {
        std::string username, password;
        std::cout << "NAAM BOL BAE!!: ";
        std::cin >> username;
        std::cout << "PASSWORD BOL BAE!!: ";
        std::cin >> password;
        registerUser(username, password);
        std::cout << "\nHANNNNNNNNNNN. AB TU KHAEL SAKTA..." << std::endl;
        return loginOrRegister(); 
    }
    else
    {
        std::cout << "BOL BAE!! TU FRESH MEAT YA POORANA BANDA?" << std::endl;
        return loginOrRegister(); 
    }
}

int main()
{
    if (!loginOrRegister())
    {
        std::cout << "\nJHOOT BOLTA MERAE SATH\n\nF HOGAYA AB\n" << std::endl;
        return 0;
    }

    Cell** grid = new Cell * [GRID_SIZE];
    for (int i = 0; i < GRID_SIZE; i++)
    {
        grid[i] = new Cell[GRID_SIZE];
    }

    int mines = BEGINNER_MINES;

    while (true)
    {
        printMenu();
        int menuRow, menuCol;
        bool isRightClick;
        getclick(5, 20, menuRow, menuCol, isRightClick, grid);

        if (menuRow == 0 && menuCol == 0)
        {
            printLevelMenu();
            int levelRow, levelCol;
            getclick(5, 20, levelRow, levelCol, isRightClick, grid);

            if (levelRow == 0 && levelCol == 0)
            {
                mines = BEGINNER_MINES;
            }
            else if (levelRow == 1 && levelCol == 0)
            {
                mines = PRO_MINES;
            }
            else if (levelRow == 2 && levelCol == 0)
            {
                mines = MASTER_MINES;
            }

            system("cls");
            break;
        }
        else if (menuRow == 1 && menuCol == 0)
        {
            printExit();
            return 0;
        }
    }

    initializeGrid(grid);

    while (true)
    {
        printgrid(GRID_SIZE, GRID_SIZE, grid);
        int row, col;
        bool isRightClick;
        getclick(4, 4, row, col, isRightClick, grid);

        if (isGameWon(grid))
        {
            system("cls");
            SetClr(10, 0);
            std::cout << "\n\n\nKya Kahnae!!!\n\n\nLevel Hae Apka!!!" << std::endl;
            _getch();
            break;
        }
    }

    for (int i = 0; i < GRID_SIZE; i++)
    {
        delete[] grid[i];
    }
    delete[] grid;

    _getch();
    return 0;
}
