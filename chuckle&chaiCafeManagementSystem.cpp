
#include <iostream>
#include <string>
#include <unistd.h>
#include <termios.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <limits>
using namespace std;

// --- Data structures ---
struct Node {
    int data;
    string foodname;
    int quantity;
    float price;
    Node* next;
};
Node* head = nullptr;
int player_coins = 0;

// --- Utility functions ---
void msleep(int ms) { usleep(ms * 1000); }
char getch_linux() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &newt);
    char ch = getchar();
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldt);
    return ch;
}
void cls() { system("clear"); }
void br(int n) { while (n--) cout << "\n"; }
void pre(int t) { while (t--) cout << "\t"; }
string read_password_masked() {
    string pwd;
    while (true) {
        char ch = getch_linux();
        if (ch == '\n' || ch == '\r') { cout << "\n"; break; }
        if (ch == 8 || ch == 127) {
            if (!pwd.empty()) {
                pwd.pop_back();
                cout << "\b \b";
            }
        } else {
            pwd.push_back(ch);
            cout << "*";
        }
    }
    return pwd;
}

// --- Linked list operations ---
void insertEnd(int id, const string& name, int qty, float price) {
    Node* n = new Node{ id, name, qty, price, nullptr };
    if (!head) head = n;
    else {
        Node* cur = head;
        while (cur->next) cur = cur->next;
        cur->next = n;
    }
}
Node* findNode(int id) {
    for (Node* cur = head; cur; cur = cur->next)
        if (cur->data == id) return cur;
    return nullptr;
}
void updateFood(int id, int newqty) {
    Node* n = findNode(id);
    if (n) n->quantity = newqty;
}
void deleteFood(int id) {
    if (!head) return;
    if (head->data == id) {
        Node* tmp = head;
        head = head->next;
        delete tmp;
        return;
    }
    Node* cur = head;
    while (cur->next && cur->next->data != id) cur = cur->next;
    if (cur->next) {
        Node* tmp = cur->next;
        cur->next = tmp->next;
        delete tmp;
    }
}
int countItems() {
    int cnt = 0;
    for (Node* cur = head; cur; cur = cur->next) cnt++;
    return cnt;
}
void foodList() {
    cout << "-------------------------------------------\n";
    cout << "ID | Name         | Price   | In Stock\n";
    cout << "-------------------------------------------\n";
    for (Node* cur = head; cur; cur = cur->next) {
        printf("%2d | %-12s | %7.2f | %4d\n",
               cur->data, cur->foodname.c_str(), cur->price, cur->quantity);
    }
    cout << "-------------------------------------------\n";
}

// --- Tic-tac-toe game ---
char squareBoard[10];
void resetBoard() { for (int i = 1; i <= 9; i++) squareBoard[i] = '0' + i; }
void showBoard() {
    cls();
    cout << "\n Tic Tac Toe\n\n";
    cout << " " << squareBoard[1] << " | " << squareBoard[2] << " | " << squareBoard[3] << "\n";
    cout << "---+---+---\n";
    cout << " " << squareBoard[4] << " | " << squareBoard[5] << " | " << squareBoard[6] << "\n";
    cout << "---+---+---\n";
    cout << " " << squareBoard[7] << " | " << squareBoard[8] << " | " << squareBoard[9] << "\n\n";
}
int checkWin() {
    int wins[8][3] = {
        {1,2,3},{4,5,6},{7,8,9},
        {1,4,7},{2,5,8},{3,6,9},
        {1,5,9},{3,5,7}
    };
    for (auto& w: wins)
        if (squareBoard[w[0]] == squareBoard[w[1]] &&
            squareBoard[w[1]] == squareBoard[w[2]])
            return 1;
    for (int i = 1; i <= 9; i++)
        if (squareBoard[i] == '0' + i) return -1;
    return 0;
}
int playGame() {
    resetBoard();
    int player = 1, status, choice;
    char mark;
    do {
        showBoard();
        cout << "Player " << player << ", enter choice: ";
        cin >> choice;
        mark = (player % 2 ? 'X' : 'O');
        if (choice >= 1 && choice <= 9 && squareBoard[choice] == '0' + choice)
            squareBoard[choice] = mark;
        else {
            cout << "Invalid move\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            player--;
        }
        status = checkWin();
        player++;
    } while (status == -1);
    showBoard();
    return (status == 1 ? 100 : 20);
}

// --- Backup visual ---
void backuploader() {
    cout << "\nBacking up data:\n";
    for (int i = 0; i <= 20; i++) {
        int perc = i * 5;
        cout << "\r[" << string(i, '=') << string(20 - i, ' ') << "] " << perc << "%";
        cout.flush();
        msleep(100);
    }
    cout << "\nBackup done.\n";
}

// --- Main menu UI ---
void showMainMenu() {
    cls();
    cout << "=== CHUCKLES & CHAI CAFE MANAGEMENT SYSTEM ===\n";
    pre(1); cout << "1. Item List & Order\n";
    pre(1); cout << "2. Manager Panel\n";
    pre(1); cout << "3. Play Game (Earn Coins)\n";
    pre(1); cout << "4. Exit\n";
    br(1);
}

int main() {
    int cashCount = 0, orderCount = 0;
    int cardNo[100];
    float cardMoney[100];
    int totalOrder[100], orderQty[100];
    float totalCash = 0.0f;

    // Seed items
    insertEnd(1, "Cake", 8, 475);
    insertEnd(2, "Coffee", 46, 125);
    insertEnd(3, "IceCream", 46, 130);
    insertEnd(4, "Fries", 34, 220);
    insertEnd(5, "Pizza", 13, 250);

main_loop:
    showMainMenu();
    cout << "Coins: " << player_coins << "\n";
    pre(2);
    int choice;
    if (!(cin >> choice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        goto main_loop;
    }

    if (choice == 1) {
        cls();
        foodList();
        cout << "Select item ID (0 to return): ";
        int sid; cin >> sid;
        if (sid == 0) goto main_loop;
        Node* sel = findNode(sid);
        if (!sel) {
            cout << "Invalid ID!\n";
            msleep(1000);
            goto main_loop;
        }
        cout << "Quantity: ";
        int qty; cin >> qty;
        if (qty <= 0 || qty > sel->quantity) {
            cout << "Invalid or insufficient stock.\n";
            msleep(1000);
            goto main_loop;
        }

        float cost = sel->price * qty;
        if (player_coins > 0) {
            cout << "Use coins? (" << player_coins << " available) Y/N: ";
            char yc; cin >> yc;
            if (yc=='Y' || yc=='y') {
                int use = min(player_coins, (int)cost);
                cout << "Use how many (max " << use << "): ";
                int uc; cin >> uc;
                if (uc > 0 && uc <= use) {
                    player_coins -= uc;
                    cost -= uc;
                }
            }
        }

        cout << "Final cost: $" << cost << "\nPay via: 1=Cash, 2=Card: ";
        int pay; cin >> pay;
        if (pay == 1) {
            totalCash += cost;
        } else if (pay == 2) {
            cout << "Enter card number: ";
            cin >> cardNo[++cashCount];
            cardMoney[cashCount] = cost;
            totalCash += cost;
        } else {
            cout << "Invalid option!\n";
            msleep(1000);
            goto main_loop;
        }

        orderCount++;
        totalOrder[orderCount] = sid;
        orderQty[orderCount] = qty;
        updateFood(sid, sel->quantity - qty);

        cout << "Order placed. Paid $" << cost << "\n";
        msleep(1000);
        goto main_loop;
    }
    else if (choice == 2) {
        while (true) {
            cls();
            pre(2);
            cout << "Enter manager password or '1' to go back: ";
            string pwd = read_password_masked();
            if (pwd == "1") goto main_loop;
            if (pwd == "12221") break;
            cout << "Wrong password.\nPress any key to retry...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getch_linux();
        }

        while (true) {
            cls();
            cout << "=== Manager Panel ===\n";
            cout << "1=Total Cash\n2=Card History\n3=Add Food\n4=Delete Food\n";
            cout << "5=List Items\n6=Count Items\n7=Backup\n8=View Orders\n0=Logout\n";
            cout << "Choice: ";
            int m;
            if (!(cin >> m)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (m == 0) break;

            switch (m) {
                case 1: cout << "Total cash today: $" << totalCash << "\n"; break;
                case 2:
                    if (cashCount == 0) cout << "No card payments.\n";
                    else {
                        for (int i = 1; i <= cashCount; i++)
                            cout << "Card " << cardNo[i] << ": $" << cardMoney[i] << "\n";
                    }
                    break;
                case 3: {
                    int id,q;
                    float p;
                    string nm;
                    cout << "New ID: "; cin >> id;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Name: "; getline(cin, nm);
                    cout << "Qty: "; cin >> q;
                    cout << "Price: "; cin >> p;
                    insertEnd(id, nm, q, p);
                    cout << "Added.\n";
                } break;
                case 4:
                    cout << "ID to delete: ";
                    cin >> choice;
                    deleteFood(choice);
                    cout << "Deleted if existed.\n";
                    break;
                case 5: foodList(); break;
                case 6: cout << "Total items: " << countItems() << "\n"; break;
                case 7: {
                    time_t now = time(0);
                    char buf[64];
                    strftime(buf, sizeof(buf), "backup_%Y%m%d_%H%M%S.txt", localtime(&now));
                    FILE* f = fopen(buf, "w");
                    backuploader();
                    if (f) {
                        fprintf(f, "Cash: %.2f\nCoins: %d\n", totalCash, player_coins);
                        fclose(f);
                        cout << "Saved to " << buf << "\n";
                    } else cout << "Failed to save.\n";
                } break;
                case 8:
                    cout << "Orders history:\nID | Qty | Remaining\n";
                    for (int i = 1; i <= orderCount; i++) {
                        Node* n = findNode(totalOrder[i]);
                        if (n)
                            cout << n->data << " | " << orderQty[i] << " | " << n->quantity << "\n";
                    }
                    break;
                default:
                    cout << "Invalid selection.\n";
            }
            cout << "Press Enter to continue..."; cin.get();
        }
        goto main_loop;
    }
    else if (choice == 3) {
        int earned = playGame();
        player_coins += earned;
        cout << "You earned " << earned << " coins. Total: " << player_coins << "\n";
        msleep(1200);
        goto main_loop;
    }
    else if (choice == 4) {
        cls();
        cout << "Thank you! Goodbye.\n";
        msleep(800);
        return 0;
    }
    else goto main_loop;
}

//---------------------------------------------------------
//          How to compile & run the project  
//---------------------------------------------------------

// $ sudo apt update
// $ sudo apt install build-essential g++

// $ chmod +x project_name

// $ g++ -std=c++11 -Wall -o project_name project_name.cpp

// ./project_name

//---------------------------------------------------------