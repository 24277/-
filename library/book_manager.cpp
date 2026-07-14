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
#include <cmath>
#include <map>

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

// Customer 结构体 —— 存储顾客信息
struct Customer {
    string name;        // 顾客名
    double balance;     // 账户余额
};

static vector<Book> books;              // 内存中的图书列表
static map<string, Customer> customers; // 顾客名 -> 顾客
static const char* DATA_FILE = "books.txt";      // 数据文件路径
static const char* CUSTOMER_FILE = "customers.txt"; // 顾客数据文件路径

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

// load_customers —— 从 customers.txt 读取顾客数据
// 每行格式：顾客名|余额
void load_customers() {
    customers.clear();
    ifstream in(CUSTOMER_FILE);
    string line;
    while (getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;
        stringstream ss(line);
        Customer c;
        getline(ss, c.name, '|');
        string bs;
        getline(ss, bs, '|');
        if (c.name.empty() || bs.empty()) continue;
        c.balance = stod(bs);
        customers[c.name] = c;
    }
}

// save_customers —— 将顾客数据写回 customers.txt
void save_customers() {
    ofstream out(CUSTOMER_FILE);
    for (auto& kv : customers) {
        out << kv.second.name << '|' << kv.second.balance << '\n';
    }
}

// is_duplicate —— 判断图书是否与现有图书完全重复（书名、作者、价格、上架日期完全一致）
// 若重复则累加库存并返回 true
static bool try_merge(Book& b) {
    for (auto& exist : books) {
        if (exist.title == b.title && exist.author == b.author
            && fabs(exist.price - b.price) < 0.001 && exist.added_date == b.added_date) {
            exist.stock += b.stock;
            cout << "\u8BE5\u56FE\u4E66\u4E0E\u7F16\u53F7 " << exist.id
                 << " \u5B8C\u5168\u91CD\u590D\uFF0C\u5DF2\u5408\u5E76\uFF0C\u5F53\u524D\u5E93\u5B58: " << exist.stock << "\n";
            return true;
        }
    }
    return false;
}

// add_book —— 交互式添加新图书
void add_book() {
    Book b;
    cout << "\n=== \u6DFB\u52A0\u56FE\u4E66 ===\n";
    cout << "\u56FE\u4E66\u7F16\u53F7: "; getline(cin, b.id);
    cout << "\u4E66\u540D: "; getline(cin, b.title);
    cout << "\u4F5C\u8005: "; getline(cin, b.author);
    cout << "\u7C7B\u578B: "; getline(cin, b.category);
    cout << "\u4EF7\u683C: "; { string _t; getline(cin, _t); b.price = stod(_t); }
    cout << "\u5728\u5E93\u6570\u91CF: "; { string _t; getline(cin, _t); b.stock = stoi(_t); }
    b.added_date = now_str();
    if (try_merge(b)) {
        save_data();
        return;
    }
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
    getline(cin, id);
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
    getline(cin, id);
    for (auto& b : books) {
        if (b.id == id) {
            cout << "\u65B0\u4E66\u540D(\u5F53\u524D:" << b.title << "): ";
            getline(cin, b.title);
            cout << "\u65B0\u4F5C\u8005(\u5F53\u524D:" << b.author << "): ";
            getline(cin, b.author);
            cout << "\u65B0\u7C7B\u578B(\u5F53\u524D:" << b.category << "): ";
            getline(cin, b.category);
            cout << "\u65B0\u4EF7\u683C(\u5F53\u524D:" << b.price << "): ";
            { string _t; getline(cin, _t); b.price = stod(_t); }
            cout << "\u65B0\u5E93\u5B58(\u5F53\u524D:" << b.stock << "): ";
            { string _t; getline(cin, _t); b.stock = stoi(_t); }
            save_data();
            cout << "\u56FE\u4E66\u4FEE\u6539\u6210\u529F\u3002\n";
            return;
        }
    }
    cout << "\u672A\u627E\u5230\u8BE5\u7F16\u53F7\u7684\u56FE\u4E66\u3002\n";
}

// register_customer —— 注册新顾客
void register_customer() {
    Customer c;
    cout << "\n=== \u6CE8\u518C\u987E\u5BA2 ===\n";
    cout << "\u987E\u5BA2\u540D: "; getline(cin, c.name);
    if (customers.count(c.name)) {
        cout << "\u8BE5\u987E\u5BA2\u540D\u5DF2\u5B58\u5728\u3002\n";
        return;
    }
    c.balance = 0;
    customers[c.name] = c;
    save_customers();
    cout << "\u987E\u5BA2\u201C" << c.name << "\u201D\u6CE8\u518C\u6210\u529F\uFF01\n";
}

// recharge —— 顾客充值
void recharge() {
    string name, amount_s;
    cout << "\n=== \u5145\u503C ===\n";
    cout << "\u987E\u5BA2\u540D: "; getline(cin, name);
    if (!customers.count(name)) {
        cout << "\u987E\u5BA2\u201C" << name << "\u201D\u4E0D\u5B58\u5728\uFF0C\u8BF7\u5148\u6CE8\u518C\u3002\n";
        return;
    }
    cout << "\u5145\u503C\u91D1\u989D: "; getline(cin, amount_s);
    double amount = stod(amount_s);
    customers[name].balance += amount;
    save_customers();
    cout << "\u5145\u503C\u6210\u529F\uFF01\u5F53\u524D\u4F59\u989D: " << fixed << setprecision(2) << customers[name].balance << "\n";
}

// purchase_book —— 顾客购买图书
void purchase_book() {
    load_data();
    string name, bid, qty_s;
    cout << "\n=== \u8D2D\u4E70\u56FE\u4E66 ===\n";
    cout << "\u987E\u5BA2\u540D: "; getline(cin, name);
    if (!customers.count(name)) {
        cout << "\u987E\u5BA2\u201C" << name << "\u201D\u4E0D\u5B58\u5728\uFF0C\u8BF7\u5148\u6CE8\u518C\u3002\n";
        return;
    }
    cout << "\u56FE\u4E66\u7F16\u53F7: "; getline(cin, bid);
    int idx = -1;
    for (size_t i = 0; i < books.size(); i++) {
        if (books[i].id == bid) { idx = (int)i; break; }
    }
    if (idx == -1) {
        cout << "\u672A\u627E\u5230\u7F16\u53F7\u4E3A " << bid << " \u7684\u56FE\u4E66\u3002\n";
        return;
    }
    Book& b = books[idx];
    cout << "\u8D2D\u4E70\u6570\u91CF: "; getline(cin, qty_s);
    int qty = stoi(qty_s);
    if (qty <= 0) { cout << "\u6570\u91CF\u65E0\u6548\u3002\n"; return; }
    if (b.stock < qty) {
        cout << "\u5E93\u5B58\u4E0D\u8DB3\uFF0C\u5F53\u524D\u5E93\u5B58: " << b.stock << "\n";
        return;
    }
    double cost = b.price * qty;
    if (customers[name].balance < cost) {
        cout << "\u4F59\u989D\u4E0D\u8DB3\uFF01\u5F53\u524D\u4F59\u989D: " << fixed << setprecision(2) << customers[name].balance
             << "\uFF0C\u6240\u9700: " << cost << "\n";
        return;
    }
    customers[name].balance -= cost;
    b.stock -= qty;
    save_data();
    save_customers();
    cout << "\u8D2D\u4E70\u6210\u529F\uFF01\u201C" << b.title << "\u201D x" << qty
         << "\uFF0C\u82B1\u8D39: " << cost << "\uFF0C\u4F59\u989D: " << customers[name].balance << "\n";
}

// query_books_customer —— 顾客查询图书（仅查看，支持按书名/编号搜索）
void query_books_customer() {
    load_data();
    if (books.empty()) {
        cout << "\n\u6682\u65E0\u56FE\u4E66\u8BB0\u5F55\u3002\n";
        return;
    }
    string kw;
    cout << "\n\u8F93\u5165\u4E66\u540D\u6216\u7F16\u53F7\u67E5\u8BE2\uFF08\u76F4\u63A5\u56DE\u8F66\u67E5\u770B\u5168\u90E8\uFF09: ";
    getline(cin, kw);
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
        if (kw.empty() || b.id.find(kw) != string::npos || b.title.find(kw) != string::npos) {
            cout << setw(12) << b.id
                 << setw(28) << b.title
                 << setw(16) << b.author
                 << setw(14) << b.category
                 << setw(8) << fixed << setprecision(2) << b.price
                 << setw(10) << b.stock
                 << setw(14) << b.added_date << "\n";
        }
    }
}

// customer_menu —— 顾客子菜单
void customer_menu() {
    string line;
    do {
        cout << "\n========================================\n";
        cout << "      \u987E\u5BA2\u670D\u52A1\u7CFB\u7EDF\n";
        cout << "========================================\n";
        cout << "  1. \u6CE8\u518C\u987E\u5BA2\n";
        cout << "  2. \u5145\u503C\n";
        cout << "  3. \u67E5\u8BE2\u56FE\u4E66\n";
        cout << "  4. \u8D2D\u4E70\u56FE\u4E66\n";
        cout << "  0. \u8FD4\u56DE\u4E3B\u83DC\u5355\n";
        cout << "========================================\n";
        cout << "\u8BF7\u9009\u62E9: ";
        getline(cin, line);
        if (line.size() != 1 || line[0] < '0' || line[0] > '4') {
            cout << "\u65E0\u6548\u9009\u62E9\uFF0C\u8BF7\u91CD\u8F93\u3002\n";
            continue;
        }
        int c = line[0] - '0';
        if (c == 0) break;
        switch (c) {
            case 1: register_customer(); break;
            case 2: recharge(); break;
            case 3: query_books_customer(); break;
            case 4: purchase_book(); break;
        }
    } while (true);
}

// main —— 主菜单循环，加载数据后进入操作选择
int main() {
    load_data();
    load_customers();
    string line;
    do {
        cout << "\n========================================\n";
        cout << "      \u56FE\u4E66\u4FE1\u606F\u7BA1\u7406\u7CFB\u7EDF\n";
        cout << "========================================\n";
        cout << "  1. \u6DFB\u52A0\u56FE\u4E66\n";
        cout << "  2. \u67E5\u770B\u6240\u6709\u56FE\u4E66\n";
        cout << "  3. \u641C\u7D22\u56FE\u4E66\n";
        cout << "  4. \u4FEE\u6539\u56FE\u4E66\u4FE1\u606F\n";
        cout << "  5. \u5220\u9664\u56FE\u4E66\n";
        cout << "  6. \u987E\u5BA2\u670D\u52A1\n";
        cout << "  0. \u9000\u51FA\n";
        cout << "========================================\n";
        cout << "\u8BF7\u9009\u62E9: ";
        getline(cin, line);
        if (line.empty()) { getline(cin, line); }
        if (line.size() != 1 || line[0] < '0' || line[0] > '6') {
            cout << "\u65E0\u6548\u9009\u62E9\uFF0C\u8BF7\u91CD\u8F93\u3002\n";
            continue;
        }
        int choice = line[0] - '0';
        if (choice == 0) {
            cout << "\u5DF2\u9000\u51FA\u7CFB\u7EDF\u3002\n";
            break;
        }
        switch (choice) {
            case 1: add_book(); break;
            case 2: list_books(); break;
            case 3: search_book(); break;
            case 4: update_book(); break;
            case 5: delete_book(); break;
            case 6: customer_menu(); break;
        }
    } while (true);
    return 0;
}
