# Bank Management Application (C++)

A menu-driven C++ Bank Management Application that demonstrates object-oriented programming and file handling.

## Features

- Create an account with a customer name and 4-digit PIN
- Log in using the account number and PIN
- Deposit money
- Withdraw money, with an insufficient-funds check
- Check the current balance
- Persist accounts in `data/accounts.txt`
- Log successful transactions in `data/transactions.txt`

## OOP design

| Class | Responsibility |
| --- | --- |
| `Account` | Holds a customer's account data and validates deposits and withdrawals. |
| `FileManager` | Reads and writes account records and transaction logs. |
| `BankSystem` | Coordinates account creation, login, and banking operations. |

## Run the program

### Option 1: with a C++ compiler

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o bank_management
./bank_management
```

On Windows, run `bank_management.exe` instead of `./bank_management`.

### Option 2: with CMake

```bash
cmake -S . -B build
cmake --build build
```

Then run the program created inside the `build` folder.

## Files created when the app runs

- `data/accounts.txt` — account number, name, PIN hash, and balance
- `data/transactions.txt` — transaction audit log

These files are excluded from Git on purpose: they may contain real customer data.

## Security note

This project is suitable for an academic demonstration. It does **not** provide production banking security. It avoids saving the PIN as plain text, but `std::hash` is not a password-hashing algorithm. A real application needs Argon2/bcrypt, a protected database, encryption, secure sessions, authorization, input monitoring, backups, and server-side controls.

## Upload to GitHub

1. Create a new repository at GitHub.com.
2. Choose **Add file** → **Upload files**.
3. Upload `main.cpp`, `CMakeLists.txt`, `README.md`, and `.gitignore` from this folder.
4. Click **Commit changes**.

Do not upload the `data` folder if it contains real records.
