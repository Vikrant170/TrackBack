#include <iostream>
#include <string>
#include <vector>
#include <limits>

// -------------------------
// User and Derived Classes
// -------------------------
class User {
protected:
    int userId;
    std::string name;
    std::string email;
    std::string password;  // In a real system, never store plain passwords!
public:
    User(int id, const std::string &n, const std::string &em, const std::string &pass)
        : userId(id), name(n), email(em), password(pass) {}
    virtual ~User() {}
    
    std::string getEmail() const { return email; }
    std::string getName() const { return name; }
    int getUserId() const { return userId; }
    
    bool verifyPassword(const std::string &pass) const { return password == pass; }
    virtual std::string getRole() const { return "User"; }
};

class Student : public User {
private:
    std::string department;
public:
    Student(int id, const std::string &n, const std::string &em, const std::string &pass, const std::string &dept)
        : User(id, n, em, pass), department(dept) {}
    std::string getDepartment() const { return department; }
    std::string getRole() const override { return "Student"; }
};

class Admin : public User {
public:
    Admin(int id, const std::string &n, const std::string &em, const std::string &pass)
        : User(id, n, em, pass) {}
    std::string getRole() const override { return "Admin"; }
};

// -------------------------
// Item and Derived Classes
// -------------------------
class Item {
protected:
    int itemId;
    std::string description;
    std::string category;
    std::string location;
    std::string date;   // For simplicity, date is stored as string (e.g., "2025-03-05")
    std::string status; // "Pending", "Approved", "Claimed"
public:
    Item(int id, const std::string &desc, const std::string &cat, const std::string &loc, const std::string &dt)
        : itemId(id), description(desc), category(cat), location(loc), date(dt), status("Pending") {}
    virtual ~Item() {}
    
    virtual void display() const {
        std::cout << "Item ID: " << itemId << "\n"
                  << "Description: " << description << "\n"
                  << "Category: " << category << "\n"
                  << "Location: " << location << "\n"
                  << "Date: " << date << "\n"
                  << "Status: " << status << "\n";
    }
    
    int getItemId() const { return itemId; }
    std::string getStatus() const { return status; }
    void setStatus(const std::string &newStatus) { status = newStatus; }
    std::string getCategory() const { return category; }
    std::string getLocation() const { return location; }
    std::string getDescription() const { return description; }
};

class LostItem : public Item {
private:
    std::string lostDate;
    std::string additionalInfo;
public:
    LostItem(int id, const std::string &desc, const std::string &cat, const std::string &loc,
             const std::string &dt, const std::string &lDate, const std::string &addInfo)
        : Item(id, desc, cat, loc, dt), lostDate(lDate), additionalInfo(addInfo) {}
    
    void display() const override {
        std::cout << "----- Lost Item -----\n";
        Item::display();
        std::cout << "Lost Date: " << lostDate << "\n"
                  << "Additional Info: " << additionalInfo << "\n";
    }
};

class FoundItem : public Item {
private:
    std::string foundDate;
    std::string additionalInfo;
public:
    FoundItem(int id, const std::string &desc, const std::string &cat, const std::string &loc,
              const std::string &dt, const std::string &fDate, const std::string &addInfo)
        : Item(id, desc, cat, loc, dt), foundDate(fDate), additionalInfo(addInfo) {}
    
    void display() const override {
        std::cout << "----- Found Item -----\n";
        Item::display();
        std::cout << "Found Date: " << foundDate << "\n"
                  << "Additional Info: " << additionalInfo << "\n";
    }
};

// -------------------------
// Notification Class
// -------------------------
class Notification {
private:
    int notificationId;
    std::string message;
public:
    Notification(int id, const std::string &msg) : notificationId(id), message(msg) {}
    
    void send(const User &user) const {
        std::cout << "Notification for " << user.getEmail() << ": " << message << "\n";
    }
};

// -------------------------
// LostAndFoundSystem Class
// -------------------------
class LostAndFoundSystem {
private:
    std::vector<User *> users;
    std::vector<Item *> items;
    int nextUserId;
    int nextItemId;
    
    // Utility function to clear input buffer (for getline after std::cin >> choice)
    void clearInput() {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    
public:
    LostAndFoundSystem() : nextUserId(1), nextItemId(1) {
        // Create a default admin
        users.push_back(new Admin(nextUserId++, "System Admin", "admin@college.edu", "adminpass"));
    }
    
    ~LostAndFoundSystem() {
        for (auto u : users) {
            delete u;
        }
        for (auto i : items) {
            delete i;
        }
    }
    
    void registerStudent() {
        std::string name, email, password, department;
        clearInput();
        std::cout << "Enter name: ";
        std::getline(std::cin, name);
        std::cout << "Enter email: ";
        std::getline(std::cin, email);
        std::cout << "Enter password: ";
        std::getline(std::cin, password);
        std::cout << "Enter department: ";
        std::getline(std::cin, department);
        
        users.push_back(new Student(nextUserId++, name, email, password, department));
        std::cout << "Student registered successfully!\n";
    }
    
    User* login() {
        std::string email, password;
        clearInput();
        std::cout << "Enter email: ";
        std::getline(std::cin, email);
        std::cout << "Enter password: ";
        std::getline(std::cin, password);
        
        for (auto user : users) {
            if (user->getEmail() == email && user->verifyPassword(password)) {
                std::cout << "Login successful! Welcome, " << user->getName()
                          << " (" << user->getRole() << ")\n";
                return user;
            }
        }
        std::cout << "Invalid email or password.\n";
        return nullptr;
    }
    
    void addLostItem() {
        std::string description, category, location, date, lostDate, additionalInfo;
        clearInput();
        std::cout << "Enter description: ";
        std::getline(std::cin, description);
        std::cout << "Enter category: ";
        std::getline(std::cin, category);
        std::cout << "Enter location: ";
        std::getline(std::cin, location);
        std::cout << "Enter report date (YYYY-MM-DD): ";
        std::getline(std::cin, date);
        std::cout << "Enter lost date (YYYY-MM-DD): ";
        std::getline(std::cin, lostDate);
        std::cout << "Enter additional info: ";
        std::getline(std::cin, additionalInfo);
        
        items.push_back(new LostItem(nextItemId++, description, category, location, date, lostDate, additionalInfo));
        std::cout << "Lost item reported successfully. Awaiting admin approval.\n";
    }
    
    void addFoundItem() {
        std::string description, category, location, date, foundDate, additionalInfo;
        clearInput();
        std::cout << "Enter description: ";
        std::getline(std::cin, description);
        std::cout << "Enter category: ";
        std::getline(std::cin, category);
        std::cout << "Enter location: ";
        std::getline(std::cin, location);
        std::cout << "Enter report date (YYYY-MM-DD): ";
        std::getline(std::cin, date);
        std::cout << "Enter found date (YYYY-MM-DD): ";
        std::getline(std::cin, foundDate);
        std::cout << "Enter additional info: ";
        std::getline(std::cin, additionalInfo);
        
        items.push_back(new FoundItem(nextItemId++, description, category, location, date, foundDate, additionalInfo));
        std::cout << "Found item reported successfully. Awaiting admin approval.\n";
    }
    
    void searchItems() {
        std::string keyword;
        clearInput();
        std::cout << "Enter keyword to search (description, category, or location): ";
        std::getline(std::cin, keyword);
        bool foundAny = false;
        for (auto item : items) {
            // A simple substring search (case-sensitive for simplicity)
            if (item->getDescription().find(keyword) != std::string::npos ||
                item->getCategory().find(keyword) != std::string::npos ||
                item->getLocation().find(keyword) != std::string::npos) {
                item->display();
                std::cout << "---------------------\n";
                foundAny = true;
            }
        }
        if (!foundAny) {
            std::cout << "No items found matching the keyword.\n";
        }
    }
    
    void adminApproveItem() {
        bool pendingFound = false;
        std::cout << "Listing pending items for approval:\n";
        for (auto item : items) {
            if (item->getStatus() == "Pending") {
                item->display();
                std::cout << "---------------------\n";
                pendingFound = true;
            }
        }
        if (!pendingFound) {
            std::cout << "No pending items for approval.\n";
            return;
        }
        
        int id;
        std::cout << "Enter the item ID to approve: ";
        std::cin >> id;
        
        bool approved = false;
        for (auto item : items) {
            if (item->getItemId() == id && item->getStatus() == "Pending") {
                item->setStatus("Approved");
                std::cout << "Item approved successfully.\n";
                // In a real application, you would now trigger a notification to the reporting user.
                approved = true;
                break;
            }
        }
        if (!approved) {
            std::cout << "Item not found or already approved.\n";
        }
    }
    
    void showAllItems() {
        if (items.empty()) {
            std::cout << "No items reported yet.\n";
            return;
        }
        for (auto item : items) {
            item->display();
            std::cout << "---------------------\n";
        }
    }
};

// -------------------------
// Main Function
// -------------------------
int main() {
    LostAndFoundSystem system;
    int choice;
    
    while (true) {
        std::cout << "\n=== College Lost and Found System ===\n";
        std::cout << "1. Register (Student)\n";
        std::cout << "2. Login\n";
        std::cout << "3. Exit\n";
        std::cout << "Enter choice: ";
        std::cin >> choice;
        
        if (choice == 1) {
            system.registerStudent();
        }
        else if (choice == 2) {
            User* user = system.login();
            if (user) {
                if (user->getRole() == "Student") {
                    int studentChoice;
                    do {
                        std::cout << "\n--- Student Menu ---\n";
                        std::cout << "1. Report Lost Item\n";
                        std::cout << "2. Report Found Item\n";
                        std::cout << "3. Search Items\n";
                        std::cout << "4. Logout\n";
                        std::cout << "Enter choice: ";
                        std::cin >> studentChoice;
                        
                        if (studentChoice == 1) {
                            system.addLostItem();
                        }
                        else if (studentChoice == 2) {
                            system.addFoundItem();
                        }
                        else if (studentChoice == 3) {
                            system.searchItems();
                        }
                    } while (studentChoice != 4);
                }
                else if (user->getRole() == "Admin") {
                    int adminChoice;
                    do {
                        std::cout << "\n--- Admin Menu ---\n";
                        std::cout << "1. Approve Items\n";
                        std::cout << "2. View All Items\n";
                        std::cout << "3. Logout\n";
                        std::cout << "Enter choice: ";
                        std::cin >> adminChoice;
                        
                        if (adminChoice == 1) {
                            system.adminApproveItem();
                        }
                        else if (adminChoice == 2) {
                            system.showAllItems();
                        }
                    } while (adminChoice != 3);
                }
            }
        }
        else if (choice == 3) {
            std::cout << "Exiting system. Goodbye!\n";
            break;
        }
        else {
            std::cout << "Invalid choice. Please try again.\n";
        }
    }
    
    return 0;
}
