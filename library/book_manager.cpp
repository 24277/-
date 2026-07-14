/*
 * book_manager.cpp —— 图书信息管理系统
 * 
 * 功能：添加、查看、搜索、修改、删除图书
 * 数据持久化：以 '|' 分隔的文本文件 books.txt
 * 字段：图书编号 | 书名 | 作者 | 类型 | 价格 | 在库数量 | 上架时间
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <ctime>

using namespace std;

// Book 结构体 —— 存储单本图书的全部信息
struct Book {
    string id;          // 图书编号（唯一标识）
    string title;       // 书名
    string author;      // 作者
    string category;    // 类型（如：文学、科技、历史）
    double price;       // 价格
    int stock;          // 在库数量
    string added_date;  // 上架时间（YYYY-MM-DD）
};

static vector<Book> books;        // 内存中的图书列表
static const char* DATA_FILE = "books.txt";  // 数据文件路径

// now_str —— 获取当前日期字符串（YYYY-MM-DD 格式）
static string now_str() {
    time_t t = time(nullptr);
    tm* tm_ptr = localtime(&t);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", tm_ptr);
    return string(buf);
}

// trim —— 去除字符串首尾空白字符
static string trim(const string& s) {
    size_t l = s.find_first_not_of(" \t\r\n");
    size_t r = s.find_last_not_of(" \t\r\n");
    return (l == string::npos) ? "" : s.substr(l, r - l + 1);
}

// load_data —— 从 books.txt 读取数据到内存
// 每行格式：编号|书名|作者|类型|价格|数量|上架日期
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

// save_data —— 将内存数据写回 books.txt
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

// add_book —— 交互式添加新图书
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

// list_books —— 表格形式列出所有图书
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

// search_book —— 按书名或编号搜索图书
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

// delete_book —— 按编号删除图书
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

// update_book —— 交互式修改指定编号的图书信息
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

// main —— 主菜单循环，加载数据后进入操作选择
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
