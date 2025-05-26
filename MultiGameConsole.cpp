#include <iostream>
#include <string>
#include <ctime>
#include <conio.h>
#include <cctype>
#include <windows.h>
#include <vector>
#include <cstdlib>
#include <limits>

using namespace std;

// =================== Shared Utilities ===================

void clearScreen() {
    system("cls");
}

void delay(int ms) {
    Sleep(ms);
}

void waitForEnter() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// =================== MindCipher Game ===================

struct Question {
    string text;
    string options[4];
    char correct;
    string explanation;
};

int highScore = 0;
bool resumeAvailable = false;
int resumeLevel = 0;
int resumeIndex = 0;
int resumeCorrect = 0;
int resumeTimeLeft = 0;

void showMindCipherInstructions() {
    clearScreen();
    cout << "--- MindCipher Instructions ---\n";
    cout << "1. You have 60 seconds to answer 5 questions in a level.\n";
    cout << "2. Questions increase in difficulty with each level.\n";
    cout << "3. Type 'QUIT' to exit mid-game. You can resume from where you left off.\n";
    cout << "4. Your best score is saved.\n\n";
    system("pause");
}

void showMindCipherMainMenu() {
    clearScreen();
    cout << "\n==============================\n";
    cout << "     Welcome to MindCipher\n";
    cout << "==============================\n";
    cout << "1. New Game\n";
    cout << "2. Resume Game\n";
    cout << "3. Best Score\n";
    cout << "4. Game Instructions\n";
    cout << "5. Back to Launcher\n";
    cout << "Choose an option: ";
}

bool askMindCipherQuestion(const Question &q, bool &quitFlag, int &timeLeft) {
    string input = "";
    char ch;
    time_t start = time(NULL);

    while (true) {
        clearScreen();
        cout << "Time left: " << timeLeft << " seconds\n";
        cout << "\n" << q.text << "\n";
        for (int i = 0; i < 4; i++) {
            cout << char('A' + i) << ") " << q.options[i] << "  ";
        }
        cout << "\n\nType 'QUIT' to exit" << string(10, ' ') << "Answer: " << input;

        delay(500);
        time_t now = time(NULL);
        if ((now - start) >= 1) {
            timeLeft -= 1;
            start = now;
        }

        if (timeLeft <= 0) {
            cout << "\nTime's up!\n";
            return false;
        }

        if (_kbhit()) {
            ch = _getch();
            if (ch == '\r') break;
            if (ch == '\b' && input.length() > 0) {
                input.erase(input.length() - 1);
            } else if (isprint(ch)) {
                input += ch;
            }
        }
    }

    cout << "\n";

    if (input == "QUIT") {
        quitFlag = true;
        return false;
    }

    if (input.length() == 1 && toupper(input[0]) >= 'A' && toupper(input[0]) <= 'D') {
        if (toupper(input[0]) == q.correct) {
            cout << "Correct!\n";
            return true;
        } else {
            cout << "Wrong. Correct answer: " << q.correct << ". " << q.explanation << "\n";
            return false;
        }
    } else {
        cout << "Invalid input.\n";
        return false;
    }
}

void startMindCipherLevel(int level) {
    Question questions[3][5] = {
        {
            {"What is ROT13 of 'HELLO'?", {"URYYB", "IFMMP", "GDKKN", "OLLEH"}, 'A', "ROT13 shifts each letter by 13."},
            {"Decrypt: 'Uifsf jt b tfdsfu dpef'", {"There is a secret code", "There is a hidden path", "You win the game", "Find the clue"}, 'A', "Each letter shifted -1 from original."},
            {"What is the reverse of 'CIPHER'?", {"REHPIC", "RHECIP", "REHPCI", "RICHEP"}, 'A', "Reverse of CIPHER is REHPIC."},
            {"Which letter is 3rd after 'D'?", {"F", "E", "H", "G"}, 'D', "'D'+3 = 'G'"},
            {"Binary to Decimal: 1011", {"13", "11", "10", "12"}, 'B', "1*8+0*4+1*2+1*1 = 11."}
        },
        {
            {"If A=1, B=2,... Z=26, what is the sum of CODE?", {"27", "35", "40", "32"}, 'A', "C(3)+O(15)+D(4)+E(5)=27."},
            {"Which key type encrypts with one key and decrypts with another?", {"Private", "Public", "Hash", "Symmetric"}, 'B', "Public key encryption uses two keys."},
            {"Decrypt: 'KHOOR' using Caesar +3", {"HELLO", "WORLD", "APPLE", "CYCLE"}, 'A', "K->H, H->E, O->L, O->L, R->O"},
            {"What is the base64 of 'A'?", {"QQ==", "AA==", "AQ==", "BA=="}, 'A', "'A' is 65 in ASCII, which is QQ== in base64."},
            {"Hex 0x1A in decimal?", {"24", "25", "26", "27"}, 'C', "1A = 1*16 + 10 = 26."}
        },
        {
            {"Which cipher rearranges letters: 'RGAE' -> 'GEAR'?", {"Caesar", "Substitution", "Transposition", "ROT13"}, 'C', "It's a Transposition Cipher."},
            {"Decode Caesar -5: 'XLMW MW XLI TIVJSVQERGI'", {"This is the performance", "Test is the challenge", "Work is the performance", "Hard is the practice"}, 'A', "Shift -5 gives original message."},
            {"Binary 1101 + 101 = ?", {"10110", "10010", "10000", "11100"}, 'B', "13 + 5 = 18 ? 10010."},
            {"Find pattern: 2, 4, 8, 16, ?", {"24", "30", "32", "28"}, 'C', "Each is *2."},
            {"If 'ZEBRAS' keyword is used in Columnar Cipher, it's a type of?", {"Substitution", "Block", "Transposition", "ROT"}, 'C', "It's a transposition cipher method."}
        }
    };

    int correctCount = 0;
    int timeLeft = 60;
    bool quit = false;

    for (int i = 0; i < 5 && !quit && timeLeft > 0; i++) {
        if (askMindCipherQuestion(questions[level - 1][i], quit, timeLeft)) {
            correctCount++;
        } else if (quit) {
            resumeAvailable = true;
            resumeLevel = level;
            resumeIndex = i;
            resumeCorrect = correctCount;
            resumeTimeLeft = timeLeft;
            cout << "\nGame progress saved.\n";
            break;
        }
        delay(1000);
    }

    if (!quit) {
        cout << "\nLevel " << level << " Complete! You got " << correctCount << " out of 5.\n";
        if (correctCount > highScore) {
            highScore = correctCount;
            cout << "New High Score!\n";
        }
    }

    system("pause");
}

void resumeMindCipherGame() {
    if (!resumeAvailable) {
        cout << "No game to resume.\n";
        system("pause");
        return;
    }

    cout << "Resuming Level " << resumeLevel << "...\n";
    startMindCipherLevel(resumeLevel);
    resumeAvailable = false;
}

void chooseMindCipherLevelAndStart() {
    clearScreen();
    cout << "Select Level:\n";
    cout << "1. Beginner Cipher\n";
    cout << "2. Intermediate Cipher\n";
    cout << "3. Advanced Cipher\n";
    cout << "Enter your level choice (1-3): ";
    int lvl;
    cin >> lvl;
    if (lvl >= 1 && lvl <= 3) {
        startMindCipherLevel(lvl);
    } else {
        cout << "Invalid level.\n";
        system("pause");
    }
}

void mindCipherInterface() {
    while (true) {
        showMindCipherMainMenu();
        int choice;
        cin >> choice;

        if (choice == 1) {
            chooseMindCipherLevelAndStart();
        } else if (choice == 2) {
            resumeMindCipherGame();
        } else if (choice == 3) {
            cout << "\nBest Score: " << highScore << "\n";
            system("pause");
        } else if (choice == 4) {
            showMindCipherInstructions();
        } else if (choice == 5) {
            break; // back to launcher
        } else {
            cout << "Invalid option. Try again.\n";
            system("pause");
        }
    }
}

// =================== Puzzle Dungeon Game ===================

struct Puzzle {
    string question;
    int answer;
};

// Game state for resume
bool puzzleResumeAvailable = false;
int puzzleResumeIndex = 0;
int puzzleResumeScore = 0;
int puzzleResumeLives = 3;
int puzzleResumeTime = 60;

Puzzle puzzles[10] = {
    {"What is 12 + 23?", 35},
    {"What is 7 * 6?", 42},
    {"What is the square of 9?", 81},
    {"Solve: (50 / 5) + 8", 18},
    {"What is 100 - 45?", 55},
    {"What is 11 * 11?", 121},
    {"If x = 5, what is x^3?", 125},
    {"What is 6 * 7?", 42},
    {"What is 9 + 10?", 19},
    {"Solve: 5^2 + 2^3", 33}
};

void clearScreens() {
    system("cls");
}

void delays(int ms) {
    Sleep(ms);
}

void showPuzzleInstructions() {
    clearScreen();
    cout << "========== Instructions ==========\n";
    cout << "1. Solve 3 puzzles to escape.\n";
    cout << "2. 3 lives only. Wrong answer = lose 1 life.\n";
    cout << "3. 60 seconds total to play.\n";
    cout << "4. Type 'QUIT' to pause anytime and resume later.\n";
    cout << "5. Type your answer directly as a number.\n";
    cout << "===================================\n";
    cout << "Press Enter to go back...";
    cin.ignore();
    cin.get();
}

void puzzleGameOver(bool won, int score) {
    clearScreen();
    if (won) {
        cout << "?? You escaped the dungeon!\n";
        cout << "Puzzles solved: " << score << "\n";
    } else {
        cout << "?? Game Over! You're trapped forever.\n";
        cout << "Puzzles solved: " << score << "\n";
    }
    puzzleResumeAvailable = false;
    system("pause");
}

bool askPuzzle(Puzzle p, int &lives, int &score, int &timeLeft, int index) {
    string input;
    time_t start = time(NULL);

    while (true) {
        clearScreen();
        cout << "Time Left: " << timeLeft << "s | Lives: " << lives << " | Solved: " << score << "\n\n";
        cout << "Puzzle " << (index + 1) << ": " << p.question << "\n\n";
        cout << "[QUIT]                                [ANSWER]: " << input;

        delays(300);

        time_t now = time(NULL);
        if (now - start >= 1) {
            timeLeft--;
            start = now;
        }

        if (timeLeft <= 0) {
            return false;
        }

        if (_kbhit()) {
            char ch = _getch();
            if (ch == '\r') break;
            if (ch == '\b' && input.length() > 0) input.erase(input.length() - 1);
            else if (isprint(ch)) input += ch;
        }
    }

    if (input == "QUIT") {
        puzzleResumeAvailable = true;
        return false;
    }

    int userAns = atoi(input.c_str());
    if (userAns == p.answer) {
        cout << "\n? Correct!\n";
        score++;
    } else {
        cout << "\n? Wrong! Correct answer: " << p.answer << "\n";
        lives--;
    }

    delays(1000);
    return true;
}

void startPuzzleDungeon(bool resume = false) {
    clearScreens();
    cout << "?? Entering Puzzle Dungeon...\n";

    // Shuffle puzzles
    for (int i = 0; i < 10; i++) {
        int j = rand() % 10;
        Puzzle temp = puzzles[i];
        puzzles[i] = puzzles[j];
        puzzles[j] = temp;
    }

    int score = resume ? puzzleResumeScore : 0;
    int lives = resume ? puzzleResumeLives : 3;
    int totalToSolve = 3;
    int timeLeft = resume ? puzzleResumeTime : 60;
    int index = resume ? puzzleResumeIndex : 0;

    bool quit = false;
    for (; index < 10 && lives > 0 && score < totalToSolve && timeLeft > 0; index++) {
        if (!askPuzzle(puzzles[index], lives, score, timeLeft, index)) {
            if (puzzleResumeAvailable) {
                puzzleResumeIndex = index;
                puzzleResumeLives = lives;
                puzzleResumeScore = score;
                puzzleResumeTime = timeLeft;
                cout << "\nGame Paused. You can resume it later.\n";
                system("pause");
            }
            break;
        }
    }

    if (!puzzleResumeAvailable)
        puzzleGameOver(score >= totalToSolve, score);
}

void  puzzleDungeonInterface() {
    while (true) {
        clearScreen();
         cout << "\n==============================\n";
    cout << "         Welcome to Puzzle Dungeon\n";
         cout << "==============================\n";
        cout << "1. Start New Game\n";
        cout << "2. Resume Game\n";
        cout << "3. Instructions\n";
        cout << "4. Exit\n";
        cout << "Choose option: ";
        int opt;
        cin >> opt;

        if (opt == 1) {
            puzzleResumeAvailable = false;
            startPuzzleDungeon(false);
        } else if (opt == 2) {
            if (!puzzleResumeAvailable) {
                cout << "No game was paused!\n";
                system("pause");
            } else {
                startPuzzleDungeon(true);
            }
        } else if (opt == 3) {
            cin.ignore();
            showPuzzleInstructions();
        } else if (opt == 4) {
            break;
        } else {
            cout << "Invalid!\n";
            system("pause");
        }
    }
}

int mainOfPuzzle() {
    srand(time(0));
     puzzleDungeonInterface();
    return 0;
}

// =================== Main Game Launcher ===================

void showLauncherMenu() {
    clearScreen();
    cout << "===========================\n";
    cout << "         G2 CONSOLE\n";
    cout << "===========================\n";
    cout << "1. Play MindCipher\n";
    cout << "2. Play Puzzle Dungeon\n";
    cout << "3. Exit\n";
    cout << "Choose an option: ";
}

int main() {
    while (true) {
        showLauncherMenu();
        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                mindCipherInterface();
                break;
            case 2:
                puzzleDungeonInterface();
                break;
            case 3:
                cout << "Thanks for playing! Goodbye.\n";
                return 0;
            default:
                cout << "Invalid choice. Try again.\n";
                system("pause");
        }
    }
}

