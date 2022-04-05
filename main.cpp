#include "main.hpp"

using namespace std;

#define QUIT_TIMES 1

typedef struct _Product{
    string category;
    string product_name;
    int price;
} Product;

vector<Product> Products;

string filename;
bool dirty{false};
static string newfile_num{"1"};

// Functions

long search_name(string name) {
    for (vector<Product>::iterator itr = Products.begin(); itr != Products.end(); ++itr)
        if ((*itr).product_name == name)
            return (itr-Products.begin());
    return -1;
}

void add_product(void) {
    Product new_product;
    cout << "Enter product: ";
    cin.ignore();
    getline(cin, new_product.product_name);
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
    string name;
    cout << "Which one to delete? ";
    cin.ignore();
    getline(cin, name);
    long idx {search_name(name)};
    if (idx != -1) {
        Products.erase(Products.begin() + idx);
        dirty = true;
        cout << name << " Deleted." << endl;
    } else {
        cout << name << " Not found." << endl;
    }
}

void find_product(void) {
    string name;
    cout << "Enter the product name: ";
    cin.ignore();
    getline(cin, name);
    long idx {search_name(name)};
    if (idx != -1) {
        cout << idx+1 << ". " << name << " / Price: " << put_money(Products[idx].price) << " / Category: " << Products[idx].category << endl;
    } else {
        cout << name << " Not found." << endl;
    }
}

void list_product(void) {
    cout << right << setw(10) << "Product" << setw(35) << "Price" << setw(13) << "Category" << endl;
    for (vector<Product>::iterator itr = Products.begin(); itr != Products.end(); ++itr) {
        cout << left << (itr-Products.begin()+1) << ". " << setw(37) << (*itr).product_name << setw(10)<< put_money((*itr).price) << (*itr).category << endl;
    }
}

void modify_product(void) {
    string name;
    cout << "Enter the product to modify: ";
    cin.ignore();
    getline(cin, name);
    long idx {search_name(name)};
    if (idx != -1) {
        string m;
        cout << "\n### Press Enter not to modify ###\n" << endl;
        cout << "(Existing: " << name << ")" << endl;
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
        cout << name << " Not found." << endl;
    }
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
    string command;

    cout << "\n(A : Add / D : Delete / M : Modify / F : Find / L : List / O : Open / S : Save / Q : Quit)" << endl;
    cout << "Enter the key: ";
    cin >> command;
    switch (command[0]) {
        case 'A':
        case 'a':
            add_product();
            break;
        case 'D':
        case 'd':
            delete_product();
            break;
        case 'F':
        case 'f':
            find_product();
            break;
        case 'L':
        case 'l':
            list_product();
            break;
        case 'M':
        case 'm':
            modify_product();
            break;
        case 'O':
        case 'o':
            cout << "Enter file name: ";
            cin >> filename;
            open_file(filename);
            break;
        case 'S':
        case 's':
            save_file();
            break;
        case 'Q':
        case 'q':
            if (dirty == true) {
                cout << "File has unsaved changes." << endl;
                while (quit_times > 0) {
                    cout << "Enter Q " << quit_times << " more time to quit: ";
                    cin >> command;
                    if (command[0] != 'q')
                        return;
                    quit_times--;
                }
            }
            cout << "Program terminated." << endl;
            exit(1);
        default:
            cout << "Error(Unvalid key entered)" << endl;
            break;
    }
}

int main(int argc, char *argv[]) {
    cout.imbue(std::locale("ko_KR.UTF-8"));     // Korea
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
