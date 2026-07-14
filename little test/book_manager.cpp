#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <ctime>

using namespace std;

struct Book {
    string id;
    string title;
    string author;
    string category;
    double price;
    int stock;
    string added_date;
};

static vector<Book> books;
static const char* DATA_FILE = "books.txt";

static string now_str() {
    time_t t = time(nullptr);
    tm* tm_ptr = localtime(&t);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", tm_ptr);
    return string(buf);
}

static string trim(const string& s) {
    size_t l = s.find_first_not_of(" \t\r\n");
    size_t r = s.find_last_not_of(" \t\r\n");
    return (l == string::npos) ? "" : s.substr(l, r - l + 1);
}

void load_data() {
    books.clear();
    ifstream in(DATA_FILE);
    string line;
    while (getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;
        stringstream ss(line);
        Book b;
        getline(ss, b.id, '|');
        getline(ss, b.title, '|');
        getline(ss, b.author, '|');
        getline(ss, b.category, '|');
        string ps, sts, ds;
        getline(ss, ps, '|');
        getline(ss, sts, '|');
        getline(ss, ds, '|');
        if (ps.empty() || sts.empty()) continue;
        b.price = stod(ps);
        b.stock = stoi(sts);
        b.added_date = ds;
        books.push_back(b);
    }
}

void save_data() {
    ofstream out(DATA_FILE);
    for (auto& b : books) {
        out << b.id << '|'
            << b.title << '|'
            << b.author << '|'
            << b.category << '|'
            << b.price << '|'
            << b.stock << '|'
            << b.added_date << '\n';
    }
}

void add_book() {
    Book b;
    cout << "\n=== \u6DFB\u52A0\u56FE\u4E66 ===\n";
    cout << "\u56FE\u4E66\u7F16\u53F7: "; cin >> b.id;
    cin.ignore();
    cout << "\u4E66\u540D: "; getline(cin, b.title);
    cout << "\u4F5C\u8005: "; getline(cin, b.author);
    cout << "\u7C7B\u578B: "; getline(cin, b.category);
    cout << "\u4EF7\u683C: "; cin >> b.price;
    cout << "\u5728\u5E93\u6570\u91CF: "; cin >> b.stock;
    b.added_date = now_str();
    books.push_back(b);
    save_data();
    cout << "\u56FE\u4E66\u201C" << b.title << "\u201D\u6DFB\u52A0\u6210\u529F\uFF01\n";
}

void list_books() {
    if (books.empty()) {
        cout << "\n\u6682\u65E0\u56FE\u4E66\u8BB0\u5F55\u3002\n";
        return;
    }
    cout << "\n" << left
         << setw(12) << "\u7F16\u53F7"
         << setw(28) << "\u4E66\u540D"
         << setw(16) << "\u4F5C\u8005"
         << setw(14) << "\u7C7B\u578B"
         << setw(8) << "\u4EF7\u683C"
         << setw(10) << "\u5E93\u5B58"
         << setw(14) << "\u4E0A\u67B6\u65F6\u95F4" << "\n";
    cout << string(102, '-') << "\n";
    for (auto& b : books) {
        cout << setw(12) << b.id
             << setw(28) << b.title
             << setw(16) << b.author
             << setw(14) << b.category
             << setw(8) << fixed << setprecision(2) << b.price
             << setw(10) << b.stock
             << setw(14) << b.added_date << "\n";
    }
}

void search_book() {
    string kw;
    cin.ignore();
    cout << "\n\u8F93\u5165\u4E66\u540D\u6216\u7F16\u53F7\u641C\u7D22: ";
    getline(cin, kw);
    bool found = false;
    for (auto& b : books) {
        if (b.id == kw || b.title.find(kw) != string::npos) {
            if (!found) {
                cout << "\n" << left
                     << setw(12) << "\u7F16\u53F7"
                     << setw(28) << "\u4E66\u540D"
                     << setw(16) << "\u4F5C\u8005"
                     << setw(14) << "\u7C7B\u578B"
                     << setw(8) << "\u4EF7\u683C"
                     << setw(10) << "\u5E93\u5B58"
                     << setw(14) << "\u4E0A\u67B6\u65F6\u95F4" << "\n";
                cout << string(102, '-') << "\n";
                found = true;
            }
            cout << setw(12) << b.id
                 << setw(28) << b.title
                 << setw(16) << b.author
                 << setw(14) << b.category
                 << setw(8) << fixed << setprecision(2) << b.price
                 << setw(10) << b.stock
                 << setw(14) << b.added_date << "\n";
        }
    }
    if (!found) cout << "\u672A\u627E\u5230\u5339\u914D\u7684\u56FE\u4E66\u3002\n";
}

void delete_book() {
    string id;
    cout << "\n\u8F93\u5165\u8981\u5220\u9664\u7684\u56FE\u4E66\u7F16\u53F7: ";
    cin >> id;
    auto it = remove_if(books.begin(), books.end(),
        [&](Book& b) { return b.id == id; });
    if (it != books.end()) {
        books.erase(it, books.end());
        save_data();
        cout << "\u56FE\u4E66\u5DF2\u5220\u9664\u3002\n";
    } else {
        cout << "\u672A\u627E\u5230\u7F16\u53F7\u4E3A " << id << " \u7684\u56FE\u4E66\u3002\n";
    }
}

void update_book() {
    string id;
    cout << "\n\u8F93\u5165\u8981\u4FEE\u6539\u7684\u56FE\u4E66\u7F16\u53F7: ";
    cin >> id;
    for (auto& b : books) {
        if (b.id == id) {
            cin.ignore();
            cout << "\u65B0\u4E66\u540D(\u5F53\u524D:" << b.title << "): ";
            getline(cin, b.title);
            cout << "\u65B0\u4F5C\u8005(\u5F53\u524D:" << b.author << "): ";
            getline(cin, b.author);
            cout << "\u65B0\u7C7B\u578B(\u5F53\u524D:" << b.category << "): ";
            getline(cin, b.category);
            cout << "\u65B0\u4EF7\u683C(\u5F53\u524D:" << b.price << "): ";
            cin >> b.price;
            cout << "\u65B0\u5E93\u5B58(\u5F53\u524D:" << b.stock << "): ";
            cin >> b.stock;
            save_data();
            cout << "\u56FE\u4E66\u4FEE\u6539\u6210\u529F\u3002\n";
            return;
        }
    }
    cout << "\u672A\u627E\u5230\u8BE5\u7F16\u53F7\u7684\u56FE\u4E66\u3002\n";
}

int main() {
    load_data();
    int choice;
    do {
        cout << "\n========================================\n";
        cout << "      \u56FE\u4E66\u4FE1\u606F\u7BA1\u7406\u7CFB\u7EDF\n";
        cout << "========================================\n";
        cout << "  1. \u6DFB\u52A0\u56FE\u4E66\n";
        cout << "  2. \u67E5\u770B\u6240\u6709\u56FE\u4E66\n";
        cout << "  3. \u641C\u7D22\u56FE\u4E66\n";
        cout << "  4. \u4FEE\u6539\u56FE\u4E66\u4FE1\u606F\n";
        cout << "  5. \u5220\u9664\u56FE\u4E66\n";
        cout << "  0. \u9000\u51FA\n";
        cout << "========================================\n";
        cout << "\u8BF7\u9009\u62E9: ";
        cin >> choice;
        switch (choice) {
            case 1: add_book(); break;
            case 2: list_books(); break;
            case 3: search_book(); break;
            case 4: update_book(); break;
            case 5: delete_book(); break;
            case 0: cout << "\u5DF2\u9000\u51FA\u7CFB\u7EDF\u3002\n"; break;
            default: cout << "\u65E0\u6548\u9009\u62E9\uFF0C\u8BF7\u91CD\u8F93\u3002\n";
        }
    } while (choice != 0);
    return 0;
}
