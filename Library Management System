/*
    LIBRARY MANAGEMENT SYSTEM
    ---------------------------
    A console-based C++ program using classes (OOP) that lets you:
      1. Add a new book
      2. Display all books
      3. Search a book by Title or Author
      4. Issue a book to a member
      5. Return a book
      6. Exit

    Data is stored PERSISTENTLY in "books.txt" so records survive
    between runs of the program.
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

// ---------------------------------------------------
// 1. Book class - represents one book in the library
// ---------------------------------------------------
class Book {
public:
    int id;
    string title;
    string author;
    bool isIssued;      // false = available, true = currently issued
    string issuedTo;    // name of member who has it (empty if not issued)

    Book() : id(0), isIssued(false) {}

    Book(int id, string title, string author, bool isIssued, string issuedTo)
        : id(id), title(title), author(author), isIssued(isIssued), issuedTo(issuedTo) {}
};

const string FILENAME = "books.txt";

// ---------------------------------------------------
// 2. Load all books from file into memory
// ---------------------------------------------------
vector<Book> loadBooks() {
    vector<Book> books;
    ifstream file(FILENAME);

    if (!file) return books; // no file yet on first run

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string idStr, title, author, issuedStr, issuedTo;

        // Stored format: id|title|author|isIssued(0/1)|issuedTo
        getline(ss, idStr, '|');
        getline(ss, title, '|');
        getline(ss, author, '|');
        getline(ss, issuedStr, '|');
        getline(ss, issuedTo, '|');

        Book b(stoi(idStr), title, author, issuedStr == "1", issuedTo);
        books.push_back(b);
    }

    file.close();
    return books;
}

// ---------------------------------------------------
// 3. Save all books from memory back into the file
// ---------------------------------------------------
void saveBooks(const vector<Book>& books) {
    ofstream file(FILENAME); // overwrite with fresh data

    for (const auto& b : books) {
        file << b.id << "|" << b.title << "|" << b.author << "|"
             << (b.isIssued ? "1" : "0") << "|" << b.issuedTo << "\n";
    }

    file.close();
}

// ---------------------------------------------------
// 4. Add a new book
// ---------------------------------------------------
void addBook() {
    vector<Book> books = loadBooks();

    int id;
    string title, author;

    cout << "\n--- Add New Book ---\n";
    cout << "Enter Book ID: ";
    cin >> id;

    for (const auto& b : books) {
        if (b.id == id) {
            cout << "A book with this ID already exists!\n";
            return;
        }
    }

    cin.ignore();
    cout << "Enter Title: ";
    getline(cin, title);
    cout << "Enter Author: ";
    getline(cin, author);

    Book newBook(id, title, author, false, "");
    books.push_back(newBook);
    saveBooks(books);

    cout << "Book added successfully!\n";
}

// ---------------------------------------------------
// 5. Display all books
// ---------------------------------------------------
void displayBooks() {
    vector<Book> books = loadBooks();

    if (books.empty()) {
        cout << "\nNo books found in the library.\n";
        return;
    }

    cout << "\n--- All Books ---\n";
    cout << "ID\tTitle\t\tAuthor\t\tStatus\n";
    cout << "----------------------------------------------------------\n";
    for (const auto& b : books) {
        cout << b.id << "\t" << b.title << "\t\t" << b.author << "\t\t";
        if (b.isIssued)
            cout << "Issued to " << b.issuedTo << "\n";
        else
            cout << "Available\n";
    }
}

// ---------------------------------------------------
// 6. Search for a book by title OR author (partial match)
// ---------------------------------------------------
void searchBook() {
    cin.ignore();
    string keyword;
    cout << "\nEnter Title or Author to search: ";
    getline(cin, keyword);

    vector<Book> books = loadBooks();
    bool found = false;

    for (const auto& b : books) {
        // Simple case-sensitive substring search
        if (b.title.find(keyword) != string::npos ||
            b.author.find(keyword) != string::npos) {
            found = true;
            cout << "\nFound: [ID " << b.id << "] " << b.title
                 << " by " << b.author << " - "
                 << (b.isIssued ? ("Issued to " + b.issuedTo) : "Available") << "\n";
        }
    }

    if (!found) {
        cout << "No matching books found.\n";
    }
}

// ---------------------------------------------------
// 7. Issue a book to a member
// ---------------------------------------------------
void issueBook() {
    int id;
    cout << "\nEnter Book ID to issue: ";
    cin >> id;

    vector<Book> books = loadBooks();
    bool found = false;

    for (auto& b : books) {
        if (b.id == id) {
            found = true;

            if (b.isIssued) {
                cout << "This book is already issued to " << b.issuedTo << ".\n";
                return;
            }

            string memberName;
            cin.ignore();
            cout << "Enter Member Name: ";
            getline(cin, memberName);

            b.isIssued = true;
            b.issuedTo = memberName;

            saveBooks(books);
            cout << "Book issued successfully to " << memberName << "!\n";
            return;
        }
    }

    if (!found) cout << "No book found with ID " << id << ".\n";
}

// ---------------------------------------------------
// 8. Return a book
// ---------------------------------------------------
void returnBook() {
    int id;
    cout << "\nEnter Book ID to return: ";
    cin >> id;

    vector<Book> books = loadBooks();
    bool found = false;

    for (auto& b : books) {
        if (b.id == id) {
            found = true;

            if (!b.isIssued) {
                cout << "This book was not issued to anyone.\n";
                return;
            }

            cout << "Book returned by " << b.issuedTo << ".\n";
            b.isIssued = false;
            b.issuedTo = "";

            saveBooks(books);
            cout << "Book return recorded successfully!\n";
            return;
        }
    }

    if (!found) cout << "No book found with ID " << id << ".\n";
}

// ---------------------------------------------------
// 9. Main menu loop
// ---------------------------------------------------
int main() {
    int choice;

    do {
        cout << "\n========= LIBRARY MANAGEMENT SYSTEM =========\n";
        cout << "1. Add Book\n";
        cout << "2. Display All Books\n";
        cout << "3. Search Book (Title/Author)\n";
        cout << "4. Issue Book\n";
        cout << "5. Return Book\n";
        cout << "6. Exit\n";
        cout << "================================================\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addBook(); break;
            case 2: displayBooks(); break;
            case 3: searchBook(); break;
            case 4: issueBook(); break;
            case 5: returnBook(); break;
            case 6: cout << "Exiting program. Goodbye!\n"; break;
            default: cout << "Invalid choice. Please try again.\n";
        }

    } while (choice != 6);

    return 0;
}
