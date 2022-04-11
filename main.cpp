#include "main.hpp"

using namespace std;

#define QUIT_TIMES 1

#define CTRL_KEY(k) ((k) & 0x1f)

typedef struct _Product{
    string category;
    string product_name;
    int price{0};
} Product;

vector<Product> Products;

struct termios orig_raw;

string filename;
bool dirty{false};
static string newfile_num{"1"};

enum editorKey {
    BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP ,
    ARROW_DOWN,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN
};

// terminal

void die(const char *s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}

void disableRawmode(void) {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_raw) == -1) die("tcsetattr");
}

void enableRawmode(void) {
    if (tcgetattr(STDIN_FILENO, &orig_raw)) die("tcgetattr");
    atexit(disableRawmode);
    
    struct termios raw = orig_raw;
    cfmakeraw(&raw);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

int readkey(void) {
    long nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }

    if (c == '\x1b'){   // escape
        char seq[3];

        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
                if (seq[2] == '~') {
                    switch (seq[1]) {
                        case '1': return HOME_KEY;
                        case '4': return END_KEY;
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                        case '7': return HOME_KEY;
                        case '8': return END_KEY;
                        case '3': return DEL_KEY;
                    }
                  }
                } else {
                    switch (seq[1]) {
                        case 'A': return ARROW_UP;
                        case 'B': return ARROW_DOWN;
                        case 'C': return ARROW_RIGHT;
                        case 'D': return ARROW_LEFT;
                        case 'H': return HOME_KEY;
                        case 'F': return END_KEY;
                    }
                }
            } else if (seq[0] == '0') {
                switch (seq[1]) {
                    case 'H': return HOME_KEY;
                    case 'F': return END_KEY;
                }
            }

        return '\x1b';
    } else {
        return c;
    }
}

// Functions

long search_name(string name) {
    for (vector<Product>::iterator itr = Products.begin(); itr != Products.end(); ++itr)
        if ((*itr).product_name == name)
            return (itr-Products.begin());
    return -1;
}

bool is_number(const string& s)
{
    return !s.empty() && find_if(s.begin(), s.end(), [](unsigned char c) { return !isdigit(c); }) == s.end();
}

void add_product(void) {
    Product new_product;
    write(STDOUT_FILENO,"Enter Product: ",15);
    long nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }
    if (c == 27) {
        disableRawmode();
        return;
    }
    write(STDOUT_FILENO, &c, 1);
    disableRawmode();
    getline(cin, new_product.product_name);
    new_product.product_name = c + new_product.product_name;
    long idx = search_name(new_product.product_name);
    if (idx != -1) {
        cout << "Already exists. (num: " << idx << ")" << endl;
        return;
    }
    
    cout << "Enter price: ";
    scanf("%d",&new_product.price);
    cout << "Enter category: ";
    cin.ignore();
    getline(cin, new_product.category);
    Products.push_back(new_product);
    dirty = true;
    cout << new_product.product_name << " Added." << endl;
}

void delete_product(void) {
    string s;
    cout << "Which one to delete? ";
    getline(cin, s);
    long idx;
    
    if (is_number(s))
        idx = stoi(s) - 1;
    else
        idx = search_name(s);
    
    if (idx >= 0 && idx < Products.end() - Products.begin()) {
        Products.erase(Products.begin() + idx);
        dirty = true;
        cout << Products[idx].product_name << " Deleted." << endl;
    } else {
        cout << Products[idx].product_name << " Not found." << endl;
    }
}

void find_product(void) {
    string name;
    cout << "Enter the product name: ";
    getline(cin, name);
    vector<size_t> idx;
    
    for (vector<Product>::iterator itr = Products.begin(); itr != Products.end(); ++itr)
        if (((*itr).product_name).find(name) != string::npos)
            idx.push_back(itr-Products.begin());
    
    if (idx.empty())
        cout << name << " Not found." << endl;
    else {
        cout << '\n';
        for (size_t i : idx) {
            cout << i+1 << ". " << Products[i].product_name << " / Price: " << put_money(Products[i].price) << " / Category: " << Products[i].category << endl;
        }
    }
}

void modify_product(void) {
    string s;
    cout << "Enter the product name or number to modify: ";
    getline(cin, s);
    long idx;
    
    if (is_number(s))
        idx = stoi(s) - 1;
    else
        idx = search_name(s);
    
    if (idx >= 0 && idx < Products.end() - Products.begin()) {
        string m;
        cout << "\n### Press Enter not to modify ###\n" << endl;
        cout << "(Existing: " << Products[idx].product_name << ")" << endl;
        cout << "Enter name: ";
        getline(cin, m);
        if (!empty(m))
            Products[idx].product_name = m;
        
        cout << "(Existing: " << Products[idx].price << ")" << endl;
        cout << "Enter price: ";
        getline(cin, m);
        if (!empty(m))
            Products[idx].price = stoi(m);
        
        cout << "(Existing: " << Products[idx].category << ")" << endl;
        cout << "Enter category: ";
        getline(cin, m);
        if (!empty(m))
            Products[idx].category = m;
        dirty = true;
        cout << '\n' << idx+1 << ". " << Products[idx].product_name << " / Price: " << put_money(Products[idx].price) << " / Category: " << Products[idx].category << "  Modified." << endl;
    } else {
        cout << s << " Not found." << endl;
    }
}

void list_product(void) {
    cout << "\n" << right << setw(11) << "Product" << setw(35) << "Price" << setw(13) << "Category" << endl;
    for (vector<Product>::iterator itr = Products.begin(); itr != Products.end(); ++itr) {
        cout << left << setw(4) << (itr-Products.begin()+1) << setw(37) << (*itr).product_name << setw(10)<< put_money((*itr).price) << (*itr).category << endl;
    }
}

void category(void) {
    string s;
    bool ex {false};
    cout << "What category? ";
    getline(cin, s);
    
    cout << "\nCategory : " << s << "\n\n";
    for (vector<Product>::iterator itr = Products.begin(); itr != Products.end(); ++itr) {
        if ((*itr).category == s) {
            cout << left << setw(4) << (itr-Products.begin()+1) << setw(40) << (*itr).product_name << setw(10) << put_money((*itr).price) << endl;
            ex = true;
        }
    }
    if (ex == false) cout << "None." << endl;
}

// File I/O

void open_file(string filename) {
    ifstream fdata{filename};
    string line,word;
    Product f_product;
    
    if (fdata.is_open()) {
        Products.clear();
        while (getline(fdata, line)) {
            stringstream str(line);
            
            getline(str,word,',');
            f_product.product_name = word;
            getline(str,word,',');
            f_product.price = stoi(word);
            getline(str,word);
            f_product.category = word;
            
            Products.push_back(f_product);
        }
        dirty = false;
        cout << filename << " Opened." << endl;
        fdata.close();
    } else
        cout << filename << " Created." << endl;
}

void saveas_file(void) {
    cout << "Save as: ";
    cin >> filename;
    if (filename.empty()) {
        cout << "Save aborted." << endl;
        return;
    }
}

void save_file(void) {
    if (filename.empty()) {
        saveas_file();
    }
    
    ofstream fdata{filename};
    
    if (fdata.is_open()) {
        for (Product p: Products) {
            fdata << p.product_name << ',' << p.price << ',' << p.category << endl;
        }
        fdata.close();
        dirty = false;
        cout << "Saved to " << filename << endl;
        return;
    }
    cout << "Save failed" << endl;
}

void keyprocess(void) {
    static int quit_times {QUIT_TIMES};
    write(STDOUT_FILENO, "\x1b[1E", 4);
    cout << "\nCtrl + A : Add / D : Delete / M : Modify / F : Find / L : List / O : Open / S : Save / Q : Quit" << endl;
    write(STDOUT_FILENO, "\x1b[1E", 4);
    
    long c = readkey();
    
    switch (c) {
        case CTRL_KEY('a'):
//            disableRawmode();
            add_product();
            enableRawmode();
            break;
        case CTRL_KEY('d'):
            disableRawmode();
            delete_product();
            enableRawmode();
            break;
        case CTRL_KEY('f'):
            disableRawmode();
            find_product();
            enableRawmode();
            break;
        case CTRL_KEY('m'):
            disableRawmode();
            modify_product();
            enableRawmode();
            break;
        case CTRL_KEY('l'):
            disableRawmode();
            list_product();
            enableRawmode();
            break;
        case CTRL_KEY('c'):
            disableRawmode();
            category();
            enableRawmode();
            break;
        case CTRL_KEY('o'):
            disableRawmode();
            cout << "Enter file name: ";
            cin >> filename;
            open_file(filename);
            enableRawmode();
            break;
        case CTRL_KEY('s'):
            save_file();
            break;
        case CTRL_KEY('q'):
            if (dirty == true) {
                cout << "File has unsaved changes." << endl;
                while (quit_times > 0) {
                    write(STDOUT_FILENO, "\x1b[1E", 4);
                    cout << "Press Ctrl+Q " << quit_times << " more time to quit. Or Ctrl+S to save." << endl;
                    write(STDOUT_FILENO, "\x1b[1E", 4);
                    c = readkey();
                    if (c == CTRL_KEY('s')) {
                        save_file();
                        return;
                    }
                    else if (c != CTRL_KEY('q'))
                        return;
                    quit_times--;
                }
            }
            write(STDOUT_FILENO, "\x1b[1E", 4);
            cout << "Program terminated." << endl;
            exit(1);
        default:
            break;
    }
}

int main(int argc, char *argv[]) {
    cout.imbue(std::locale("ko_KR.UTF-8"));     // Korea
    enableRawmode();
    
    if (argc >= 2) {
        filename = argv[1];
    } else {
        filename = "Untitled-" + newfile_num + ".csv";
        newfile_num = to_string(stoi(newfile_num) + 1);
    }
    open_file(filename);
    
    while (1) {
        keyprocess();
    }
    return 0;
}
