#include "main.hpp"

using namespace std;

typedef struct _Product{
    string category;
    string product_name;
    int price;
} Product;

vector<Product> Products;

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
//    cin >> new_product.price;
    scanf("%d",&new_product.price);
    cout << "Enter category: ";
    cin.ignore();
    getline(cin, new_product.category);
    Products.push_back(new_product);
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

int main() {
    cout.imbue(std::locale("ko_KR.UTF-8"));     // Korea
    char command;
    while (1) {
        cout << "\n(A : Add / D : Delete / F : Find / L : List / Q : Quit)" << endl;
        cout << "Enter the key: ";
//        scanf(" %c", &command);
        cin >> command;
        switch (command) {
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
            case 'Q':
            case 'q':
                cout << "Program terminated." << endl;
                exit(0);
            default:
                cout << "Error(Unvalid key entered)" << endl;
                string rem;
                getline(cin,rem);
                break;
        }
    }
    return 0;
}
