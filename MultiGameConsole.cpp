#include <iostream>
#include <string>
#include <ctime>
#include <conio.h>
#include <cctype>
#include <windows.h>
#include <vector>
#include <cstdlib>
#include <limits>
#include <cstring>
#include <fstream>

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


// Hangman Game

#define MAX 100
const int MAX_WRONG = 6;
string words[] = {"program", "science", "variable", "function", "computer"};

// Clear screen helper
void clearScreens() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Stack for wrong guesses
struct Stack {
    char items[MAX];
    int top;

    void init() { top = -1; }
    bool isEmpty() { return top == -1; }
    bool isFull() { return top == MAX - 1; }
    void push(char ch) { if (!isFull()) items[++top] = ch; }
    char pop() { return (!isEmpty()) ? items[top--] : '\0'; }
    char peek() { return (!isEmpty()) ? items[top] : '\0'; }
    void display() { for (int i = 0; i <= top; i++) cout << items[i] << " "; }
    bool contains(char ch) {
        for (int i = 0; i <= top; i++)
            if (items[i] == ch) return true;
        return false;
    }
};

// Queue for correct guesses
struct Queue {
    char items[MAX];
    int front, rear;

    void init() { front = rear = -1; }
    bool isEmpty() { return front == -1; }
    bool isFull() { return (rear + 1) % MAX == front; }
    void enqueue(char ch) {
        if (isFull()) return;
        if (isEmpty()) front = rear = 0;
        else rear = (rear + 1) % MAX;
        items[rear] = ch;
    }
    bool contains(char ch) {
        if (isEmpty()) return false;
        int i = front;
        while (true) {
            if (items[i] == ch) return true;
            if (i == rear) break;
            i = (i + 1) % MAX;
        }
        return false;
    }
    void loadFromString(const string& s) {
        for (int i = 0; i < s.length(); i++) enqueue(s[i]);
    }
    void display() {
        if (isEmpty()) return;
        int i = front;
        while (true) {
            cout << items[i] << " ";
            if (i == rear) break;
            i = (i + 1) % MAX;
        }
    }
};

// Draw hangman
void drawHangman(int wrong) {
    string hangman[] = {
        "\n\n\n\n\n\n",
        "   \n   \n   \n   \n   \n___|___\n",
        "   _______\n   |\n   |\n   |\n   |\n___|___\n",
        "   _______\n   |     |\n   |     O\n   |\n   |\n___|___\n",
        "   _______\n   |     |\n   |     O\n   |     |\n   |\n___|___\n",
        "   _______\n   |     |\n   |     O\n   |    /|\\\n   |\n___|___\n",
        "   _______\n   |     |\n   |     O\n   |    /|\\\n   |    / \\\n___|___\n"
    };
    cout << hangman[wrong] << endl;
}

// Show word progress
void showProgress(string word, Queue q) {
    for (int i = 0; i < word.length(); ++i) {
        if (q.contains(word[i])) cout << word[i];
        else cout << "_";
        cout << " ";
    }
    cout << "\n";
}

// Check if word is complete
bool isWordGuessed(string word, Queue q) {
    for (int i = 0; i < word.length(); ++i)
        if (!q.contains(word[i])) return false;
    return true;
}

// Show instructions
void showInstructions() {
    cout << "\n?? INSTRUCTIONS:\n";
    cout << "---------------------------\n";
    cout << "1. Guess the hidden word letter-by-letter.\n";
    cout << "2. You lose after 6 wrong guesses (full hangman).\n";
    cout << "3. Type 'QUIT' anytime to pause and save the game.\n";
    cout << "4. Stack stores wrong guesses. Queue stores correct ones.\n";
    cout << "---------------------------\n\n";
}

// Save game to file
void saveGame(string word, Queue& correct, Stack& wrong, int wrongCount) {
    ofstream file("resume_hangman.txt");
    if (!file) {
        cout << "Error saving game!\n";
        return;
    }
    file << word << "\n";
    // Save correct letters
    for (int i = 0; i < word.length(); i++) {
        if (correct.contains(word[i]))
            file << word[i];
    }
    file << "\n";
    // Save wrong letters
    for (int i = 0; i <= wrong.top; i++)
        file << wrong.items[i];
    file << "\n" << wrongCount << "\n";
    file.close();
    cout << "\n?? Game paused and saved.\n";
}

// Load game from file
bool loadGame(string& word, Queue& correct, Stack& wrong, int& wrongCount) {
    ifstream file("resume_hangman.txt");
    if (!file) return false;

    string correctStr, wrongStr;
    file >> word >> correctStr >> wrongStr >> wrongCount;

    correct.init();
    correct.loadFromString(correctStr);

    wrong.init();
    for (int i = 0; i < wrongStr.length(); i++) wrong.push(wrongStr[i]);

    file.close();
    return true;
}

// Remove resume file
void clearResumes() {
    remove("resume_hangman.txt");
}

// Play game
void playGame(bool resume = false) {
    clearScreen();  // Clear before showing game interface

    string word;
    Queue correct;
    Stack wrong;
    correct.init();
    wrong.init();
    int wrongCount = 0;

    if (resume) {
        if (!loadGame(word, correct, wrong, wrongCount)) {
            cout << "? No paused game found.\n";
            cout << "\nPress Enter to return to menu...";
            cin.ignore();
            cin.get();
            return;
        } else {
            cout << "?? Resuming previous game...\n";
        }
    } else {
        srand((unsigned)time(0));
        word = words[rand() % 5];
    }

    while (wrongCount < MAX_WRONG) {
        drawHangman(wrongCount);
        cout << "Word: ";
        showProgress(word, correct);
        cout << "Wrong letters: ";
        wrong.display();
        cout << "\n";

        cout << "[Type 'QUIT' to pause and save]\n";
        cout << "? QUIT      ? Guess a letter: ";
        string input;
        cin >> input;

        if (input == "QUIT" || input == "quit") {
            saveGame(word, correct, wrong, wrongCount);
            cout << "\nPress Enter to return to menu...";
            cin.ignore();
            cin.get();
            clearScreen();  // Clear game interface before returning to menu
            return;
        }

        char ch = tolower(input[0]);
        if (!isalpha(ch)) {
            cout << "? Invalid input.\n";
            continue;
        }

        if (correct.contains(ch) || wrong.contains(ch)) {
            cout << "?? Already guessed.\n";
            continue;
        }

        if (word.find(ch) != string::npos) {
            correct.enqueue(ch);
            cout << "? Correct!\n";
        } else {
            wrong.push(ch);
            wrongCount++;
            cout << "? Wrong!\n";
        }

        if (isWordGuessed(word, correct)) {
            cout << "\n?? You guessed the word: " << word << "\n";
            clearResumes();  // remove resume if game complete
            cout << "\nPress Enter to return to menu...";
            cin.ignore();
            cin.get();
            clearScreen();  // Clear game interface before returning to menu
            return;
        }
    }

    drawHangman(wrongCount);
    cout << "\n?? Game Over! The word was: " << word << "\n";
    clearResumes();  // remove resume if failed
    cout << "\nPress Enter to return to menu...";
    cin.ignore();
    cin.get();
    clearScreen();  // Clear game interface before returning to menu
}

// Main menu
void Hangman() {
    int choice;
    do {
        clearScreen();  // Clear before showing menu
        cout << "\n==============================\n";
        cout << "           Hangman\n";
        cout << "==============================\n";
        cout << "1. New Game\n";
        cout << "2. Resume Game\n";
        cout << "3. Instructions\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            playGame(false);
            break;
        case 2:
            playGame(true);
            break;
        case 3:
            clearScreen();
            showInstructions();
            cout << "\nPress Enter to return to menu...";
            cin.ignore();
            cin.get();
            break;
        case 4:
            cout << "?? Exiting... Thanks for playing!\n";
            break;
        default:
            cout << "? Invalid option. Try again.\n";
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
            break;
        }
    } while (choice != 4);
}

int mains() {
    Hangman();
    return 0;
}

// =================== Main Game Launcher ===================

void showLauncherMenu() {
    clearScreen();
    cout << "===========================\n";
    cout << "         G2 CONSOLE\n";
    cout << "===========================\n";
    cout << "1. Play MindCipher\n";
    cout << "2. Play Hangman\n";
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
                Hangman();
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





