#include <iostream>
#include "vault.h"

using namespace std;

int main() {
    string masterPassword;

    cout << "Enter Master Password: ";
    cin >> masterPassword;

    if (!authenticate(masterPassword)) {
        cout << "Authentication Failed!\n";
        return 0;
    }

    int choice;
    while (true) {
        cout << "\n--- Bunnylocker Menu ---\n";
        cout << "1. Add Password\n";
        cout << "2. View All\n";
        cout << "3. Search\n";
        cout << "4. Delete\n";
        cout << "5. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addPassword(masterPassword); break;
            case 2: viewPasswords(masterPassword); break;
            case 3: searchPassword(masterPassword); break;
            case 4: deletePassword(masterPassword); break;
            case 5: return 0;
            default: cout << "Invalid option.\n";
        }
    }

    return 0;
}

