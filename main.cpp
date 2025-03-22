#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <limits>
using namespace std;

// User and Derived Classes
class User{
protected:
    int userId;
    string name;
    string email;
    string password;

public:
    User(int id, const string &n, const string &em, const string &pass)
        : userId(id), name(n), email(em), password(pass) {}
    // virtual ~User() {}
    
    string getEmail() const{ return email; }
    string getName() const{ return name; }
    int getUserId() const{ return userId; }
    
    bool verifyPassword(const string &pass) const{ return password == pass; }
    virtual string getRole() const{ return "User"; }
};

class Student : public User{
private:
    int scholarId;
    string department;
public:
    Student(int id, const string &n, const string &em, const string &pass, const int &sch, const string &dept)
        : User(id, n, em, pass), scholarId(sch), department(dept){}
    int getScholarID() const{ return scholarId;}
    string getDepartment() const{ return department; }
    string getRole() const override{ return "Student"; }
};

class Professor: public User{
private:
    string department;
public:
    Professor(int id, const string &n, const string &em, const string &pass, const string& dept)
        : User(id, n, em, pass), department(dept){}
    string getDepartment() const{ return department; }
    string getRole() const override{ return "Professor"; }
};

class Admin : public User{
public:
    Admin(int id, const string &n, const string &em, const string &pass)
        : User(id, n, em, pass){}
    string getRole() const override{ return "Admin"; }
};

// Item and Derived Classes
class Item{
protected:
    int itemId;
    string category;
    string location;
    string date;
    string status; // "Pending", "Approved", "Claimed"
    string description;
public:
    Item(int id, const string &cat, const string &loc, const string& dt, const string &desc)
        : itemId(id), category(cat), location(loc), date(dt), status("Pending"), description(desc){}
    virtual ~Item(){}
    
    virtual void display() const{
        cout << "Item ID: " << itemId << "\n"
             << "Category: " << category << "\n"
             << "Description: " << description << "\n"
             << "Date: " << date << "\n"
             << "Location: " << location << "\n"
             << "Status: " << status << "\n";
    }
    
    int getItemId() const{ return itemId; }
    string getStatus() const{ return status; }
    void setStatus(const string &newStatus){ status = newStatus; }
    string getCategory() const{ return category; }
    string getDate() const{ return date; }
    string getLocation() const{ return location; }
    string getDescription() const{ return description; }

    friend void approveItem(LostAndFoundSystem &system, int itemId);
};

class LostItem : public Item{
private:
    string lostDate;
    string additionalInfo;
public:
    LostItem(int id, const string &cat, const string &loc, const string& dt, const string &desc,
             const string &lDate, const string &addInfo)
        : Item(id, cat, loc, dt, desc), lostDate(lDate), additionalInfo(addInfo){}
    
    void display() const override{
        cout << "----- Lost Item -----\n";
        Item::display();
        cout << "Lost Date: " << lostDate << "\n" << "Additional Info: " << additionalInfo << "\n";
    }
};

class FoundItem : public Item{
private:
    string foundDate;
    string additionalInfo;
public:
    FoundItem(int id, const string &cat, const string &loc, const string& dt, const string &desc,
              const string &fDate, const string &addInfo)
        : Item(id, cat, loc, dt, desc), foundDate(fDate), additionalInfo(addInfo) {}
    
    void display() const override{
        cout << "----- Found Item -----\n";
        Item::display();
        cout << "Found Date: " << foundDate << "\n" << "Additional Info: " << additionalInfo << "\n";
    }
};

// Notification Class
class Notification {
private:
    int notificationId;
    string message;
public:
    Notification(int id, const string &msg) : notificationId(id), message(msg) {}
    
    void send(const User &user) const{
        cout << "Notification for " << user.getName() << ": " << message << "\n";
    }
};

// LostAndFoundSystem Class
class LostAndFoundSystem {
private:
    vector<User *> users;
    vector<Item *> items;
    int nextUserId;
    int nextItemId;
    
    // Utility function to clear input buffer (for getline after cin >> choice)
    void clearInput(){
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    
public:
    LostAndFoundSystem() : nextUserId(1), nextItemId(1){
        // Create a default admin
        users.push_back(new Admin(nextUserId++, "System Admin", "admin@college.edu", "adminpass"));
    }
    
    ~LostAndFoundSystem(){
        for(auto u : users){
            delete u;
        }
        for(auto i : items){
            delete i;
        }
    }
    
    void registerStudent(){
        string name, email, password, department;
        int scholarId;
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
        
        users.push_back(new Student(nextUserId++, name, email, password, scholarId, department));
        cout << "Student registered successfully!\n";
    }

    void registerProfessor(){
        string name, email, password, department;
        clearInput();
        cout << "Enter name: ";
        getline(cin, name);
        cout << "Enter email: ";
        getline(cin, email);
        cout << "Enter password: ";
        getline(cin, password);
        cout << "Enter department: ";
        getline(cin, department);
        
        users.push_back(new Professor(nextUserId++, name, email, password, department));
        cout << "Professor registered successfully!\n";
    }
    
    User* login(){
        string email = ""; 
        string password;
        int userid = 0;
        clearInput();
        char c;
        do{
            cout << "Enter email(e)/userid(u)?";
            cin >> c;
            if(c == 'e'){
                cout << "Enter your email ID : ";
                cin >> email;
            }
            else if(c == 'u'){
                cout << "Enter your user ID : ";
                cin >> userid;
            }
            else cout << "Invalid Choice!";
        } while( c!='u' || c!='e');
        cout << "Enter password: ";
        getline(cin, password);
        
        for(auto user : users){
            if((user->getEmail() == email || user->getUserId() == userid) && user->verifyPassword(password)){
                cout << "Login successful! Welcome to TrackBack, " << user->getName() << " (" << user->getRole() << ")\n";
                return user;
            }
        }
        cout << "Invalid email or password.\n";
        return nullptr;
    }
    
    void addLostItem(){
        string description, category, location, date, lostDate, additionalInfo;
        clearInput();
        cout << "Enter category: ";
        getline(cin, category);
        cout << "Enter description: ";
        getline(cin, description);
        cout << "Enter location: ";
        getline(cin, location);
        cout << "Enter report date (DD-MM-YYYY): ";
        getline(cin, date);
        cout << "Enter lost date (DD-MM-YYYY): ";
        getline(cin, lostDate);
        cout << "Enter additional info: ";
        getline(cin, additionalInfo);
        
        items.push_back(new LostItem(nextItemId++, category, location, date, description, lostDate, additionalInfo));
        cout << "Lost item reported successfully. Awaiting admin approval..\n";
    }
    
    void addFoundItem(){
        string description, category, location, date, foundDate, additionalInfo;
        clearInput();
        cout << "Enter category: ";
        getline(cin, category);
        cout << "Enter description: ";
        getline(cin, description);
        cout << "Enter location: ";
        getline(cin, location);
        cout << "Enter report date (DD-MM-YYYY): ";
        getline(cin, date);
        cout << "Enter found date (DD-MM-YYYY): ";
        getline(cin, foundDate);
        cout << "Enter additional info: ";
        getline(cin, additionalInfo);
        
        items.push_back(new FoundItem(nextItemId++, category, location, date, description, foundDate, additionalInfo));
        cout << "Found item reported successfully. Awaiting admin approval..\n";
    }
    
    void searchItems(){
        string keyword;
        clearInput();
        cout << "Enter keyword to search (category, description, or location): ";
        cin >> keyword;
        transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
        bool foundAny = false;
        for(auto item : items){
            string des = item->getDescription(), cat = item->getCategory(), loc = item->getLocation();
            transform(des.begin(), des.end(), des.begin(), ::tolower);
            transform(cat.begin(), cat.end(), cat.begin(), ::tolower);
            transform(loc.begin(), loc.end(), loc.begin(), ::tolower);
            if(des.find(keyword) != string::npos || cat.find(keyword) != string::npos || loc.find(keyword) != string::npos){
                item->display();
                cout << "---------------------\n";
                foundAny = true;
            }
        }
        if(!foundAny) cout << "No items found matching the keyword.\n";
    }
    
    void adminApproveItem(){
        bool pendingFound = false;
        cout << "Listing pending items for approval:\n";
        for(auto item : items){
            if (item->getStatus() == "Pending") {
                item->display();
                cout << "---------------------\n";
                pendingFound = true;
            }
        }
        if(!pendingFound){
            cout << "No pending items for approval.\n";
            return;
        }
        
        int id;
        cout << "Enter the item ID to approve: ";
        cin >> id;
        approveItem(*this, id);
    }
    
    void showAllItems(){
        if(items.empty()){
            cout << "No items reported yet.\n";
            return;
        }
        for(auto item : items){
            item->display();
            cout << "---------------------\n";
        }
    }

    friend void approveItem(LostAndFoundSystem &system, int itemId);
};

void approveItem(LostAndFoundSystem &system, int itemId){
    for(auto item : system.items){
        if(item->getItemId() == itemId && item->getStatus() == "Pending"){
            item->setStatus("Approved");
            cout << "Item approved successfully.\n";
            return;
        }
    }
    cout << "Item not found or already approved.\n";
}
// Main Function
int main(){
    LostAndFoundSystem system;
    int choice;
    
    while(true){
        cout << "\n=== College Lost and Found System ===\n";
        cout << "1. Register (Student)\n";
        cout << "2. Register (Professor)\n";
        cout << "3. Login\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        
        if(choice == 1) system.registerStudent();
        else if(choice == 2) system.registerProfessor();
        else if(choice == 3){
            User* user = system.login();
            if(user){
                if(user->getRole() == "Student" || user->getRole() == "Professor"){
                    int schoice;
                    do{
                        cout << "\n--- Menu ---\n";
                        cout << "1. Report Lost Item\n";
                        cout << "2. Report Found Item\n";
                        cout << "3. Search Items\n";
                        cout << "4. Logout\n";
                        cout << "Enter choice: ";
                        cin >> schoice;
                        
                        if (schoice == 1)  system.addLostItem();
                        else if (schoice == 2) system.addFoundItem();
                        else if (schoice == 3) system.searchItems();
                    }while(schoice != 4);
                }
                else if (user->getRole() == "Admin"){
                    int adminChoice;
                    do{
                        cout << "\n--- Admin Menu ---\n";
                        cout << "1. Approve Items\n";
                        cout << "2. View All Items\n";
                        cout << "3. Logout\n";
                        cout << "Enter choice: ";
                        cin >> adminChoice;
                        
                        if (adminChoice == 1) system.adminApproveItem();
                        else if (adminChoice == 2) system.showAllItems();
                    } while(adminChoice != 3);
                }
            }
        }
        else if(choice == 4){
            cout << "Exiting system. Goodbye!\n";
            break;
        }
        else cout << "Invalid choice. Please try again.\n";
    } 
    return 0;
}