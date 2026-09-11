#include <algorithm>
#include <chrono>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// This is an educational local app. std::hash is used to avoid saving a PIN in
// plain text, but a production bank must use a real password-hashing library
// (such as Argon2 or bcrypt), encryption, access controls, and a secure server.
std::string hashPin(const std::string& pin) {
    return std::to_string(std::hash<std::string>{}("bank-demo-v1:" + pin));
}

bool isValidPin(const std::string& pin) {
    return pin.size() == 4 && std::all_of(pin.begin(), pin.end(), [](unsigned char c) {
        return std::isdigit(c) != 0;
    });
}

bool isSafeName(const std::string& name) {
    return !name.empty() && name.find('|') == std::string::npos;
}

class Account {
public:
    Account() = default;

    Account(std::string accountNumber, std::string customerName,
            std::string pinHash, double balance = 0.0)
        : accountNumber_(std::move(accountNumber)), customerName_(std::move(customerName)),
          pinHash_(std::move(pinHash)), balance_(balance) {}

    const std::string& accountNumber() const { return accountNumber_; }
    const std::string& customerName() const { return customerName_; }
    double balance() const { return balance_; }

    bool matchesPin(const std::string& pin) const { return pinHash_ == hashPin(pin); }

    bool deposit(double amount) {
        if (amount <= 0.0) return false;
        balance_ += amount;
        return true;
    }

    bool withdraw(double amount) {
        if (amount <= 0.0 || amount > balance_) return false;
        balance_ -= amount;
        return true;
    }

    std::string toRecord() const {
        std::ostringstream record;
        record << accountNumber_ << '|' << customerName_ << '|' << pinHash_ << '|'
               << std::fixed << std::setprecision(2) << balance_;
        return record.str();
    }

    static bool fromRecord(const std::string& line, Account& account) {
        std::stringstream stream(line);
        std::string number, name, pinHash, balanceText;

        if (!std::getline(stream, number, '|') || !std::getline(stream, name, '|') ||
            !std::getline(stream, pinHash, '|') || !std::getline(stream, balanceText)) {
            return false;
        }

        try {
            const double balance = std::stod(balanceText);
            if (number.empty() || !isSafeName(name) || pinHash.empty() || balance < 0.0) return false;
            account = Account(number, name, pinHash, balance);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }

private:
    std::string accountNumber_;
    std::string customerName_;
    std::string pinHash_;
    double balance_ = 0.0;
};

class FileManager {
public:
    explicit FileManager(fs::path dataFolder = "data")
        : dataFolder_(std::move(dataFolder)), accountFile_(dataFolder_ / "accounts.txt"),
          transactionFile_(dataFolder_ / "transactions.txt") {
        fs::create_directories(dataFolder_);
    }

    std::map<std::string, Account> loadAccounts() const {
        std::map<std::string, Account> accounts;
        std::ifstream input(accountFile_);
        std::string line;

        while (std::getline(input, line)) {
            Account account;
            if (Account::fromRecord(line, account)) {
                accounts[account.accountNumber()] = account;
            }
        }
        return accounts;
    }

    bool saveAccounts(const std::map<std::string, Account>& accounts) const {
        const fs::path temporaryFile = accountFile_.string() + ".tmp";
        std::ofstream output(temporaryFile, std::ios::trunc);
        if (!output) return false;

        for (const auto& [number, account] : accounts) {
            output << account.toRecord() << '\n';
        }
        output.close();
        if (!output) return false;

        std::error_code error;
        fs::remove(accountFile_, error);
        fs::rename(temporaryFile, accountFile_, error);
        if (error) {
            fs::remove(temporaryFile, error);
            return false;
        }
        return true;
    }

    void logTransaction(const std::string& accountNumber, const std::string& type,
                        double amount, double resultingBalance) const {
        std::ofstream output(transactionFile_, std::ios::app);
        if (!output) return;

        const std::time_t now = std::time(nullptr);
        std::tm localTime{};
#ifdef _WIN32
        localtime_s(&localTime, &now);
#else
        localtime_r(&now, &localTime);
#endif
        output << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << " | "
               << accountNumber << " | " << type << " | " << std::fixed
               << std::setprecision(2) << amount << " | Balance: "
               << resultingBalance << '\n';
    }

private:
    fs::path dataFolder_;
    fs::path accountFile_;
    fs::path transactionFile_;
};

class BankSystem {
public:
    BankSystem() : accounts_(files_.loadAccounts()) {}

    bool createAccount(const std::string& name, const std::string& pin, std::string& accountNumber) {
        if (!isSafeName(name) || !isValidPin(pin)) return false;

        accountNumber = nextAccountNumber();
        accounts_.emplace(accountNumber, Account(accountNumber, name, hashPin(pin)));
        if (!files_.saveAccounts(accounts_)) {
            accounts_.erase(accountNumber);
            return false;
        }
        files_.logTransaction(accountNumber, "ACCOUNT_CREATED", 0.0, 0.0);
        return true;
    }

    Account* login(const std::string& accountNumber, const std::string& pin) {
        const auto found = accounts_.find(accountNumber);
        if (found == accounts_.end() || !found->second.matchesPin(pin)) return nullptr;
        return &found->second;
    }

    bool deposit(Account& account, double amount) {
        if (!account.deposit(amount) || !files_.saveAccounts(accounts_)) return false;
        files_.logTransaction(account.accountNumber(), "DEPOSIT", amount, account.balance());
        return true;
    }

    bool withdraw(Account& account, double amount) {
        if (amount <= 0.0 || amount > account.balance()) return false;
        if (!account.withdraw(amount) || !files_.saveAccounts(accounts_)) return false;
        files_.logTransaction(account.accountNumber(), "WITHDRAWAL", amount, account.balance());
        return true;
    }

private:
    std::string nextAccountNumber() const {
        unsigned int highest = 1000;
        for (const auto& [number, account] : accounts_) {
            if (number.size() > 1 && number[0] == 'C') {
                try { highest = std::max(highest, static_cast<unsigned int>(std::stoul(number.substr(1)))); }
                catch (const std::exception&) { /* Ignore unexpected legacy record. */ }
            }
        }
        return "C" + std::to_string(highest + 1);
    }

    FileManager files_;
    std::map<std::string, Account> accounts_;
};

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

bool readAmount(const std::string& prompt, double& amount) {
    const std::string text = readLine(prompt);
    try {
        std::size_t used = 0;
        amount = std::stod(text, &used);
        return used == text.size() && amount > 0.0;
    } catch (const std::exception&) {
        return false;
    }
}

void customerMenu(BankSystem& bank, Account& account) {
    while (true) {
        std::cout << "\n--- Customer Dashboard ---\n"
                  << "1. Deposit\n2. Withdraw\n3. Check balance\n4. Logout\n";
        const std::string choice = readLine("Choose an option: ");

        if (choice == "1") {
            double amount;
            if (!readAmount("Deposit amount: ", amount) || !bank.deposit(account, amount)) {
                std::cout << "Could not complete the deposit. Enter a positive amount and try again.\n";
            } else {
                std::cout << "Deposit successful. New balance: " << std::fixed << std::setprecision(2)
                          << account.balance() << "\n";
            }
        } else if (choice == "2") {
            double amount;
            if (!readAmount("Withdrawal amount: ", amount) || !bank.withdraw(account, amount)) {
                std::cout << "Withdrawal failed. Check the amount and your available balance.\n";
            } else {
                std::cout << "Withdrawal successful. New balance: " << std::fixed << std::setprecision(2)
                          << account.balance() << "\n";
            }
        } else if (choice == "3") {
            std::cout << "Current balance: " << std::fixed << std::setprecision(2) << account.balance() << "\n";
        } else if (choice == "4") {
            std::cout << "Logged out successfully.\n";
            return;
        } else {
            std::cout << "Invalid choice. Please select 1 to 4.\n";
        }
    }
}

int main() {
    try {
        BankSystem bank;
        std::cout << "=================================\n"
                  << "      BANK MANAGEMENT SYSTEM     \n"
                  << "=================================\n";

        while (true) {
            std::cout << "\n1. Create account\n2. Login\n3. Exit\n";
            const std::string choice = readLine("Choose an option: ");

            if (choice == "1") {
                const std::string name = readLine("Customer name: ");
                const std::string pin = readLine("Choose a 4-digit PIN: ");
                std::string number;
                if (bank.createAccount(name, pin, number)) {
                    std::cout << "Account created successfully. Your account number is: " << number << "\n";
                } else {
                    std::cout << "Account could not be created. Use a name without | and a 4-digit PIN.\n";
                }
            } else if (choice == "2") {
                const std::string number = readLine("Account number: ");
                const std::string pin = readLine("PIN: ");
                Account* account = bank.login(number, pin);
                if (account == nullptr) {
                    std::cout << "Invalid account number or PIN.\n";
                } else {
                    std::cout << "Welcome, " << account->customerName() << "!\n";
                    customerMenu(bank, *account);
                }
            } else if (choice == "3") {
                std::cout << "Thank you for using the Bank Management System.\n";
                return 0;
            } else {
                std::cout << "Invalid choice. Please select 1, 2, or 3.\n";
            }
        }
    } catch (const std::exception& error) {
        std::cerr << "Application error: " << error.what() << '\n';
        return 1;
    }
}
