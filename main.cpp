#include <bits/stdc++.h>
#include "sqlite3.h"
using namespace std;

sqlite3* openDatabase(){
    sqlite3* db;
    int rc = sqlite3_open("lost_and_found.db", &db);
    if(rc){
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return nullptr;
    }
    
    const char* createUsersTable = "CREATE TABLE IF NOT EXISTS Users ("
                                "userId INTEGER PRIMARY KEY, "
                                "name TEXT NOT NULL, "
                                "email TEXT UNIQUE NOT NULL, "
                                "password TEXT NOT NULL, "
                                "role TEXT NOT NULL, "
                                "scholarId INTEGER, "
                                "department TEXT, "
                                "yearJoined INTEGER);";
    
    const char* createItemsTable = "CREATE TABLE IF NOT EXISTS Items ("
                                "itemId INTEGER PRIMARY KEY, "
                                "userId INTEGER, "
                                "category TEXT NOT NULL, "
                                "location TEXT NOT NULL, "
                                "date TEXT NOT NULL, "
                                "status TEXT NOT NULL, "
                                "description TEXT NOT NULL, "
                                "lostOrFound TEXT NOT NULL, "
                                "eventDate TEXT, "
                                "additionalInfo TEXT, "
                                "FOREIGN KEY(userId) REFERENCES Users(userId));";

    const char* createCountersTable = "CREATE TABLE IF NOT EXISTS Counters ("
                                    "counterType TEXT PRIMARY KEY, "
                                    "key TEXT, "
                                    "value INTEGER);";

    sqlite3_exec(db, createCountersTable, nullptr, nullptr, nullptr);
    sqlite3_exec(db, createUsersTable, nullptr, nullptr, nullptr);
    sqlite3_exec(db, createItemsTable, nullptr, nullptr, nullptr);
    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
    
    return db;
}

class User {
protected:
    int userId;
    string name;
    string email;
    string password;

public:
    User(int id, const string &n, const string &em, const string &pass)
        : userId(id), name(n), email(em), password(pass) {}
    
    string getEmail() const { return email; }
    string getName() const { return name; }
    int getUserId() const { return userId; }
    
    bool verifyPassword(const string &pass) const { return password == pass; }
    virtual string getRole() const { return "User"; }
    virtual void saveToDatabase(sqlite3* db) {
        string sql = "INSERT INTO Users (userId, name, email, password, role) VALUES (" +
                    to_string(userId) + ", '" + name + "', '" + email + "', '" + password + "', 'User');";
        sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
    }
};

class Student : public User {
private:
    int scholarId;
    string department;
public:
    Student(int id, const string &n, const string &em, const string &pass, const int &sch, const string &dept)
        : User(id, n, em, pass), scholarId(sch), department(dept) {}
    int getScholarID() const { return scholarId; }
    string getDepartment() const { return department; }
    string getRole() const override { return "Student"; }
    
    void saveToDatabase(sqlite3* db) override {
        string sql = "INSERT INTO Users (userId, name, email, password, role, scholarId, department) VALUES (" +
                    to_string(userId) + ", '" + name + "', '" + email + "', '" + password + "', 'Student', " +
                    to_string(scholarId) + ", '" + department + "');";
        sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
    }
};

class Professor : public User {
private:
    string department;
public:
    Professor(int id, const string &n, const string &em, const string &pass, const string& dept)
        : User(id, n, em, pass), department(dept) {}
    string getDepartment() const { return department; }
    string getRole() const override { return "Professor"; }
    
    void saveToDatabase(sqlite3* db) override {
        string sql = "INSERT INTO Users (userId, name, email, password, role, department) VALUES (" +
                    to_string(userId) + ", '" + name + "', '" + email + "', '" + password + "', 'Professor', '" +
                    department + "');";
        sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
    }
};

class Admin : public User {
public:
    Admin(int id, const string &n, const string &em, const string &pass)
        : User(id, n, em, pass) {}
    string getRole() const override { return "Admin"; }
    
    void saveToDatabase(sqlite3* db) override {
        string sql = "INSERT INTO Users (userId, name, email, password, role) VALUES (" +
                    to_string(userId) + ", '" + name + "', '" + email + "', '" + password + "', 'Admin');";
        sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
    }
};

class LostAndFoundSystem;

class Item {
protected:
    int itemId;
    int userId;
    string category;
    string location;
    string date;
    string status; // "Pending", "Approved", "Claimed"
    string description;
public:
    Item(int id, int uid, const string &cat, const string &loc, const string& dt, const string &desc)
        : itemId(id), userId(uid), category(cat), location(loc), date(dt), status("Pending"), description(desc) {}
    virtual ~Item() {}
    
    virtual void display() const {
        cout << "Item ID: " << itemId << "\n"
             << "Category: " << category << "\n"
             << "Description: " << description << "\n"
             << "Date: " << date << "\n"
             << "Location: " << location << "\n"
             << "Status: " << status << "\n";
    }
    
    int getItemId() const { return itemId; }
    int getUserId() const { return userId; }
    string getStatus() const { return status; }
    void setStatus(const string &newStatus) { status = newStatus; }
    string getCategory() const { return category; }
    string getDate() const { return date; }
    string getLocation() const { return location; }
    string getDescription() const { return description; }
    virtual string getType() const = 0;
    virtual string getEventDate() const = 0;
    virtual string getAdditionalInfo() const = 0;

    virtual void saveToDatabase(sqlite3* db) {
        string sql = "INSERT INTO Items (itemId, userId, category, location, date, status, description, lostOrFound, eventDate, additionalInfo) VALUES (" +
                     to_string(itemId) + ", " + to_string(userId) + ", '" + category + "', '" + location + "', '" + date + "', '" +
                     status + "', '" + description + "', '" + getType() + "', '" + getEventDate() + "', '" + getAdditionalInfo() + "');";
        sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
    }
    
    void updateStatusInDatabase(sqlite3* db) {
        string sql = "UPDATE Items SET status = '" + status + "' WHERE itemId = " + to_string(itemId) + ";";
        sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
    }
    
    void deleteFromDatabase(sqlite3* db) {
        string sql = "DELETE FROM Items WHERE itemId = " + to_string(itemId) + ";";
        sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
    }

    friend void approveItem(LostAndFoundSystem &system, int itemId);
};

class LostItem : public Item {
private:
    string lostDate;
    string additionalInfo;
public:
    LostItem(int id, int uid, const string &cat, const string &loc, const string& dt, const string &desc,
             const string &lDate, const string &addInfo)
        : Item(id, uid, cat, loc, dt, desc), lostDate(lDate), additionalInfo(addInfo) {}
    
    void display() const override {
        cout << "\n----- Lost Item -----\n";
        Item::display();
        cout << "Lost Date: " << lostDate << "\n" << "Additional Info: " << additionalInfo << "\n";
    }
    
    string getType() const override { return "Lost"; }
    string getEventDate() const override { return lostDate; }
    string getAdditionalInfo() const override { return additionalInfo; }
};

class FoundItem : public Item {
private:
    string foundDate;
    string additionalInfo;
public:
    FoundItem(int id, int uid, const string &cat, const string &loc, const string& dt, const string &desc,
              const string &fDate, const string &addInfo)
        : Item(id, uid, cat, loc, dt, desc), foundDate(fDate), additionalInfo(addInfo) {}
    
    void display() const override {
        cout << "\n----- Found Item -----\n";
        Item::display();
        cout << "Found Date: " << foundDate << "\n" << "Additional Info: " << additionalInfo << "\n";
    }
    
    string getType() const override { return "Found"; }
    string getEventDate() const override { return foundDate; }
    string getAdditionalInfo() const override { return additionalInfo; }
};

class LostAndFoundSystem {
private:
    vector<User *> users;
    vector<Item *> items;
    sqlite3* db;
    
    void clearInput() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    
    void loadUsersFromDatabase() {
        sqlite3_stmt* stmt;
        const char* sql = "SELECT * FROM Users;";
        
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int userId = sqlite3_column_int(stmt, 0);
                string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                string email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                string password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
                string role = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
                
                if (role == "Student") {
                    int scholarId = sqlite3_column_int(stmt, 5);
                    string department = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
                    users.push_back(new Student(userId, name, email, password, scholarId, department));
                }
                else if (role == "Professor") {
                    string department = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
                    users.push_back(new Professor(userId, name, email, password, department));
                }
                else if (role == "Admin"){
                    users.push_back(new Admin(userId, name, email, password));
                }
            }
            sqlite3_finalize(stmt);
        }
    }
    
    void loadItemsFromDatabase() {
        sqlite3_stmt* stmt;
        const char* sql = "SELECT * FROM Items;";
        
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int itemId = sqlite3_column_int(stmt, 0);
                int userId = sqlite3_column_int(stmt, 1);
                string category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                string location = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
                string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
                string status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
                string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
                string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
                string eventDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
                string additionalInfo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
                
                if (type == "Lost") {
                    items.push_back(new LostItem(itemId, userId, category, location, date, description, eventDate, additionalInfo));
                } else {
                    items.push_back(new FoundItem(itemId, userId, category, location, date, description, eventDate, additionalInfo));
                }
                
                // Set the status after creation
                items.back()->setStatus(status);
            }
            sqlite3_finalize(stmt);
        }
    }

    int getCounterValue(const string& counterType, const string& key) {
        sqlite3_stmt* stmt;
        string sql = "SELECT value FROM Counters WHERE counterType = ? AND key = ?;";
        int value = 0;
        
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, counterType.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, key.c_str(), -1, SQLITE_STATIC);
            
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                value = sqlite3_column_int(stmt, 0);
            }
            sqlite3_finalize(stmt);
        }
        return value;
    }
    
    void updateCounter(const string& counterType, const string& key, int value) {
        string sql = "INSERT OR REPLACE INTO Counters (counterType, key, value) VALUES (?, ?, ?);";
        sqlite3_stmt* stmt;
        
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, counterType.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, key.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 3, value);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
    }
    
public:
    unordered_map<string, int> deptcodes;

    LostAndFoundSystem() : db(openDatabase()) {
        deptcodes["Mathematics"] = 01;
        deptcodes["Chemistry"] = 03;
        deptcodes["Humanities"] = 04;
        deptcodes["Physics"] = 02;
        deptcodes["Management"] = 05;
        deptcodes["CE"] = 11;
        deptcodes["CSE"] = 12;
        deptcodes["EE"] = 13;
        deptcodes["ECE"] = 14;
        deptcodes["EIE"] = 15;
        deptcodes["ME"] = 16;
        if (db) {
            loadUsersFromDatabase();
            loadItemsFromDatabase();
            
            // Create default admin if no users exist
            if (users.empty()) {
                users.push_back(new Admin(1, "System Admin", "admin@college.edu", "adminpass"));
                users.back()->saveToDatabase(db);
            }
        } else {
            cerr << "Failed to initialize database. Some functionality may be limited." << endl;
            // Create a default admin in memory if database fails
            users.push_back(new Admin(1, "System Admin", "admin@college.edu", "adminpass"));
        }
    }
    
    ~LostAndFoundSystem() {
        for (auto u : users) {
            delete u;
        }
        for (auto i : items) {
            delete i;
        }
        if (db) sqlite3_close(db);
    }

    void registerStudent() {
        string name, email, password, department;
        int scholarId, yearJoined;
        clearInput();
        cout << "Enter name: ";
        getline(cin, name);
        cout << "Enter email: ";
        getline(cin, email);
        cout << "Enter password: ";
        getline(cin, password);
        cout << "Enter department: ";
        getline(cin, department);
        cout << "Enter scholar ID: ";
        cin >> scholarId;

        int fcode = (scholarId/10000)%10;
        int yearCode = scholarId/100000;
        int deptCode = deptcodes[department];
        int schId = scholarId%1000;

        int studId = fcode * 10000000 + yearCode * 100000 + deptCode * 1000 + schId;
        
        Student* student = new Student(studId, name, email, password, scholarId, department);
        users.push_back(student);
        if (db) student->saveToDatabase(db);
        cout << "Student registered successfully with ID: " << studId << endl;
    }

    void registerProfessor() {
        string name, email, password, department;
        int yearJoined;
        clearInput();
        cout << "Enter name: ";
        getline(cin, name);
        cout << "Enter email: ";
        getline(cin, email);
        cout << "Enter password: ";
        getline(cin, password);
        cout << "Enter department: ";
        getline(cin, department);
        cout << "Enter year joined: ";
        cin >> yearJoined;

        int professorCount = getCounterValue("ProfessorCount", department) + 1;
        updateCounter("ProfessorCount", department, professorCount);

        int yearCode = yearJoined % 100;
        int deptCode = deptcodes[department];
        int profId = 40000000 + yearCode * 100000 + deptCode * 1000 + professorCount;
        
        Professor* professor = new Professor(profId, name, email, password, department);
        users.push_back(professor);
        if (db) professor->saveToDatabase(db);
        cout << "Professor registered successfully with ID: " << profId << endl;
    }
    
    User* login() {
        string email = ""; 
        string password;
        int userid = 0;
        clearInput();
        char c;
        do {
            cout << "Enter email(e)/userid(u)?";
            cin >> c;
            if (c == 'e') {
                cout << "Enter your email ID : ";
                cin >> email;
            }
            else if (c == 'u') {
                cout << "Enter your user ID : ";
                cin >> userid;
            }
            else cout << "Invalid Choice!";
        } while (c != 'u' && c != 'e');
        cout << "Enter password: ";
        clearInput();
        getline(cin, password);
        
        for (auto user : users) {
            if ((user->getEmail() == email || user->getUserId() == userid) && user->verifyPassword(password)) {
                cout << "\n Login successful! Welcome to TrackBack, " << user->getName() << " (" << user->getRole() << ")\n";
                return user;
            }
        }
        cout << "Invalid email or password.\n";
        return nullptr;
    }
    
    void addLostItem(int userId) {
        string description, category, location, date, lostDate, additionalInfo;
        clearInput();
        cout << "Enter category: ";
        getline(cin, category);
        cout << "Enter description: ";
        getline(cin, description);
        cout << "Enter location: ";
        getline(cin, location);
        cout << "Enter report date (DD-MM-YY): ";
        getline(cin, date);
        cout << "Enter lost date (DD-MM-YY): ";
        getline(cin, lostDate);
        cout << "Enter additional info: ";
        getline(cin, additionalInfo);

        int lcount = getCounterValue("LostItemCount", lostDate) + 1;
        updateCounter("LostItemCount", lostDate, lcount);
        string ldate = lostDate;
        ldate.erase(remove(ldate.begin(), ldate.end(), '-'), ldate.end());
        int lostId = 100000000 + stoi(ldate) * 100 + lcount;
        
        LostItem* item = new LostItem(lostId, userId, category, location, date, description, lostDate, additionalInfo);
        items.push_back(item);
        if (db) item->saveToDatabase(db);
        cout << "Lost item reported successfully with Item ID " << lostId << ". Awaiting admin approval..\n";
    }

    
    void addFoundItem(int userId) {
        string description, category, location, date, foundDate, additionalInfo;
        clearInput();
        cout << "Enter category: ";
        getline(cin, category);
        cout << "Enter description: ";
        getline(cin, description);
        cout << "Enter location: ";
        getline(cin, location);
        cout << "Enter report date (DD-MM-YY): ";
        getline(cin, date);
        cout << "Enter found date (DD-MM-YY): ";
        getline(cin, foundDate);
        cout << "Enter additional info: ";
        getline(cin, additionalInfo);

        int fcount = getCounterValue("FoundItemCount", foundDate) + 1;
        updateCounter("FoundItemCount", foundDate, fcount);

        string fdate = foundDate;
        fdate.erase(remove(fdate.begin(), fdate.end(), '-'), fdate.end());
        int foundId = 200000000 + stoi(fdate) * 100 + fcount;
        
        FoundItem* item = new FoundItem(foundId, userId, category, location, date, description, foundDate, additionalInfo);
        items.push_back(item);
        if (db) item->saveToDatabase(db);
        cout << "Found item reported successfully with Item ID " << foundId << ". Awaiting admin approval..\n";
    }
    
    void searchItems() {
        string keyword;
        clearInput();
        cout << "Enter keyword to search (category, description, or location): ";
        cin >> keyword;
        transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
        bool foundAny = false;
        
        for (auto item : items) {
            if (item->getStatus() != "Approved") continue;
            
            string des = item->getDescription(), cat = item->getCategory(), loc = item->getLocation();
            transform(des.begin(), des.end(), des.begin(), ::tolower);
            transform(cat.begin(), cat.end(), cat.begin(), ::tolower);
            transform(loc.begin(), loc.end(), loc.begin(), ::tolower);
            
            if (des.find(keyword) != string::npos || cat.find(keyword) != string::npos || loc.find(keyword) != string::npos) {
                item->display();
                cout << "---------------------\n";
                foundAny = true;
            }
        }
        if (!foundAny) cout << "No approved items found matching the keyword.\n";
    }
    
    void adminApproveItem() {
        bool pendingFound = false;
        cout << "\n Listing pending items for approval:\n";
        for (auto item : items) {
            if (item->getStatus() == "Pending") {
                item->display();
                cout << "---------------------\n";
                pendingFound = true;
            }
        }
        if (!pendingFound) {
            cout << "No pending items for approval.\n";
            return;
        }
        
        int id;
        cout << "\n Enter the item ID to approve: ";
        cin >> id;
        approveItem(*this, id);
    }
    
    void showAllItems() {
        if (items.empty()) {
            cout << "No items reported yet.\n";
            return;
        }
        for (auto item : items) {
            item->display();
            cout << "---------------------\n";
        }
    }
    
    void deleteUserItem(int userId) {
        vector<Item*> userItems;
        for (auto item : items) {
            if (item->getUserId() == userId) {
                userItems.push_back(item);
            }
        }
        
        if (userItems.empty()) {
            cout << "You haven't reported any items.\n";
            return;
        }
        
        cout << "Your reported items:\n";
        for (auto item : userItems) {
            item->display();
            cout << "---------------------\n";
        }
        
        int itemId;
        cout << "Enter the item ID you want to delete: ";
        cin >> itemId;
        
        for (auto it = items.begin(); it != items.end(); ) {
            if ((*it)->getItemId() == itemId && (*it)->getUserId() == userId) {
                if (db) (*it)->deleteFromDatabase(db);
                delete *it;
                it = items.erase(it);
                cout << "Item deleted successfully.\n";
                return;
            } else {
                ++it;
            }
        }
        
        cout << "Item not found or you don't have permission to delete it.\n";
    }

    friend void approveItem(LostAndFoundSystem &system, int itemId);
};

void approveItem(LostAndFoundSystem &system, int itemId) {
    for (auto item : system.items) {
        if (item->getItemId() == itemId && item->getStatus() == "Pending") {
            item->setStatus("Approved");
            if (system.db) item->updateStatusInDatabase(system.db);
            cout << "Item approved successfully.\n";
            return;
        }
    }
    cout << "Item not found or already approved.\n";
}

int main() {
    LostAndFoundSystem system;
    int choice;
    int t = 1;
    int u = 1;
    
    while (t == 1) {
        cout << "\n=== College Lost and Found System ===\n";
        cout << "1. Register (Student)\n";
        cout << "2. Register (Professor)\n";
        cout << "3. Login\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        
        if (choice == 1) system.registerStudent();
        else if (choice == 2) system.registerProfessor();
        else if (choice == 3) {
            User* user = system.login();
            if (user) {
                if (user->getRole() == "Student" || user->getRole() == "Professor") {
                    int schoice;
                    do {
                        cout << "\n--- Menu ---\n";
                        cout << "1. Report Lost Item\n";
                        cout << "2. Report Found Item\n";
                        cout << "3. Search Items\n";
                        cout << "4. Delete My Item\n";
                        cout << "5. Logout\n";
                        cout << "Enter choice: ";
                        cin >> schoice;
                        
                        if (schoice == 1) system.addLostItem(user->getUserId());
                        else if (schoice == 2) system.addFoundItem(user->getUserId());
                        else if (schoice == 3) system.searchItems();
                        else if (schoice == 4) system.deleteUserItem(user->getUserId());
                        if(schoice!=5) {
                            cout << "Press 1 to continue to items menu: ";
                            cin >> u;
                        }
                    } while (schoice != 5 && u == 1);
                }
                else if (user->getRole() == "Admin") {
                    int adminChoice;
                    do {
                        cout << "\n--- Admin Menu ---\n";
                        cout << "1. Approve Items\n";
                        cout << "2. View All Items\n";
                        cout << "3. Logout\n";
                        cout << "Enter choice: ";
                        cin >> adminChoice;
                        
                        if (adminChoice == 1) system.adminApproveItem();
                        else if (adminChoice == 2) system.showAllItems();
                        if(adminChoice!=3) {
                            cout << "Press 1 to continue to admin menu: ";
                            cin >> u;
                        }
                    } while (adminChoice != 3 && u == 1);
                }
            }
        }
        else if (choice == 4) {
            cout << "Exiting system. Goodbye!\n";
            break;
        }
        else cout << "Invalid choice. Please try again.\n";
        cout << "Press 1 to continue to login page: " << endl;
        cin >> t;
    }
    return 0;
}