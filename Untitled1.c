#include <iostream>
#include <string>
#include <list>
#include <queue>
#include <memory>
#include <limits>
#include <map>
#include <iomanip>
#include <windows.h>
#include <vector>

using namespace std;

// ======================================================
//                  CONSOLE THEME
// ======================================================

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

const WORD COLOR_DEFAULT = 7;
const WORD COLOR_TITLE   = 14;
const WORD COLOR_VIP     = 14;
const WORD COLOR_REGULAR = 7;
const WORD COLOR_ERROR   = 12;
const WORD COLOR_SUCCESS = 10;
const WORD COLOR_RESET   = 7;

// Forward declaration
void setColor(WORD color);

// ======================================================
//              GET CONSOLE WIDTH
// ======================================================

int getConsoleWidth()
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(hConsole, &info);
    return info.srWindow.Right - info.srWindow.Left + 1;
}

// ======================================================
//                CENTER TEXT
// ======================================================

void printCentered(const string &text = "")
{
    int consoleWidth = getConsoleWidth();
    int textLength = static_cast<int>(text.length());
    int spaces = (consoleWidth - textLength) / 2;
    if (spaces < 0) spaces = 0;

    cout << string(spaces, ' ') << text << endl;
}

void printCenteredNoNewLine(const string &text)
{
    int consoleWidth = getConsoleWidth();
    int textLength = static_cast<int>(text.length());
    int spaces = (consoleWidth - textLength) / 2;
    if (spaces < 0) spaces = 0;

    cout << string(spaces, ' ') << text;
}

void printCenteredPrompt(const string &prompt)
{
    setColor(COLOR_TITLE);
    printCenteredNoNewLine(prompt);
    setColor(COLOR_DEFAULT);
}

// ======================================================
//         CONSOLE THEME - Dark Elegant Palette
// ======================================================

void setConsoleTheme()
{
    CONSOLE_SCREEN_BUFFER_INFOEX info;
    info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);

    if (GetConsoleScreenBufferInfoEx(hConsole, &info))
    {
        // Background - Dark Void (#141616)
        info.ColorTable[0] = RGB(20, 22, 22);

        // Normal text - Soft Equilibrium (brighter for readability)
        info.ColorTable[7] = RGB(180, 175, 170);

        // Success - Soft elegant green
        info.ColorTable[10] = RGB(90, 180, 120);

        // Error - Falu Red (#7F1D1A)
        info.ColorTable[12] = RGB(127, 29, 26);

        // Title / VIP / Royal - Warm Artillery tone
        info.ColorTable[14] = RGB(180, 165, 140);

        SetConsoleScreenBufferInfoEx(hConsole, &info);
    }

    CONSOLE_SCREEN_BUFFER_INFO screenInfo;
    GetConsoleScreenBufferInfo(hConsole, &screenInfo);

    DWORD cellCount = screenInfo.dwSize.X * screenInfo.dwSize.Y;
    COORD home = {0, 0};
    DWORD written;

    FillConsoleOutputAttribute(hConsole, 0, cellCount, home, &written);
    FillConsoleOutputCharacter(hConsole, ' ', cellCount, home, &written);
    SetConsoleCursorPosition(hConsole, home);
}

void setColor(WORD color)
{
    SetConsoleTextAttribute(hConsole, color);
}

// ======================================================
//                    ROOM CLASS
// ======================================================

class Room
{
private:
    int num;
    double price;
    bool available;
    int bookedByCustomerId;
    string bookedByCustomerName;
    string paymentMethod;
    bool isPaid;
    string category;
    string bedType;
    int nights;
    string phoneNumber;

public:
    Room(int n, double p, const string &cat = "Regular")
        : num(n), price(p), available(true),
          bookedByCustomerId(-1), bookedByCustomerName(""),
          paymentMethod(""), isPaid(false),
          category(cat), bedType(""), nights(0), phoneNumber("") {}

    bool book(int customerId, const string &customerName)
    {
        if (available)
        {
            available = false;
            bookedByCustomerId = customerId;
            bookedByCustomerName = customerName;
            paymentMethod = "";
            isPaid = false;
            return true;
        }
        return false;
    }

    void cancel()
    {
        available = true;
        bookedByCustomerId = -1;
        bookedByCustomerName = "";
        paymentMethod = "";
        isPaid = false;
        bedType = "";
        nights = 0;
        phoneNumber = "";
    }

    void setPaymentInfo(const string &method, bool paid)
    {
        paymentMethod = method;
        isPaid = paid;
    }

    void setBookingDetails(const string &bed, int nightsCount, const string &phone)
    {
        bedType = bed;
        nights = nightsCount;
        phoneNumber = phone;
    }

    bool isAvailable() const { return available; }

    bool isBookedBy(int customerId) const
    {
        return !available && bookedByCustomerId == customerId;
    }

    double getPrice() const { return price; }
    int getNumber() const { return num; }
    int getBookedCustomerId() const { return bookedByCustomerId; }
    string getBookedCustomerName() const { return bookedByCustomerName; }
    string getPaymentMethod() const { return paymentMethod; }
    bool getIsPaid() const { return isPaid; }
    string getCategory() const { return category; }
    string getBedType() const { return bedType; }
    int getNights() const { return nights; }
    string getPhoneNumber() const { return phoneNumber; }

    double getTotalPrice() const
    {
        return price * nights;
    }
};

// ======================================================
//                   PERSON CLASS
// ======================================================

class Person
{
protected:
    string name;
    int id;

public:
    virtual ~Person() {}

    void setData(const string &n, int i)
    {
        name = n;
        id = i;
    }

    void showData() const
    {
        setColor(COLOR_DEFAULT);
        printCentered("Name: " + name);
        printCentered("ID: " + to_string(id));
    }

    string getName() const { return name; }
    int getId() const { return id; }
};

// ======================================================
//                FORWARD DECLARATION
// ======================================================

class HotelSystem;

// ======================================================
//                  PAYMENT CLASS
// ======================================================

class Payment
{
public:
    virtual void pay(double amount) = 0;
    virtual ~Payment() {}
};

class CashPayment : public Payment
{
public:
    void pay(double amount) override
    {
        setColor(COLOR_SUCCESS);
        printCentered("Booking done.");
        setColor(COLOR_DEFAULT);
        printCentered("Please pay " + to_string(static_cast<int>(amount)) + " EGP in cash.");
    }
};

class CardPayment : public Payment
{
public:
    void pay(double amount) override
    {
        setColor(COLOR_SUCCESS);
        printCentered("Paid " + to_string(static_cast<int>(amount)) + " EGP by card.");
        setColor(COLOR_DEFAULT);
    }
};

// ======================================================
//                  CUSTOMER CLASS
// ======================================================

class Customer : public Person
{
public:
    bool bookRoom(Room &r, HotelSystem &h, const string &bedType, int nights, const string &phoneNumber);
    bool cancelRoom(Room &r, HotelSystem &h, Customer &assignedCustomerOut, bool &nextCustomerAssigned);
};

// ======================================================
//                WAITING ENTRY
// ======================================================

struct WaitingEntry
{
    Customer customer;
    int roomNumber;
    string bedType;
    int nights;
    string phoneNumber;
};

// ======================================================
//                  MANAGER CLASS
// ======================================================

class Manager : public Person
{
public:
    void showRooms(HotelSystem &h) const;
    void showWaitingList(HotelSystem &h) const;
};

// ======================================================
//                  HOTEL SYSTEM
// ======================================================

class HotelSystem
{
private:
    list<Room> rooms;
    queue<WaitingEntry> waitingList;

    void printCategoryTable(const string &categoryFilter, WORD color, const string &label) const
    {
        string line = "+------------+--------------+----------------+";

        setColor(COLOR_TITLE);
        printCentered(label);
        printCentered(line);
        setColor(COLOR_DEFAULT);

        string header = "| Room No.   | Price/Night  | Status         |";
        printCentered(header);

        setColor(color);
        printCentered(line);
        setColor(COLOR_DEFAULT);

        bool anyRoom = false;

        for (const Room &r : rooms)
        {
            if (r.getCategory() == categoryFilter)
            {
                anyRoom = true;

                string roomNumber = to_string(r.getNumber());
                string price = to_string(static_cast<int>(r.getPrice()));
                string status = r.isAvailable() ? "Available" : "Booked";

                string row = "| " + roomNumber +
                             string(11 - static_cast<int>(roomNumber.length()), ' ') +
                             "| " + price +
                             string(13 - static_cast<int>(price.length()), ' ') +
                             "| " + status +
                             string(15 - static_cast<int>(status.length()), ' ') + "|";

                setColor(color);
                printCentered(row);
                setColor(COLOR_DEFAULT);
            }
        }

        if (!anyRoom)
        {
            setColor(COLOR_ERROR);
            printCentered("| No rooms in this category.               |");
            setColor(COLOR_DEFAULT);
        }

        setColor(color);
        printCentered(line);
        setColor(COLOR_DEFAULT);
    }

public:
    void addRoom(const Room &r)
    {
        rooms.push_back(r);
    }

    void addToWaitingList(const Customer &c, int roomNumber,
                          const string &bedType = "", int nights = 0,
                          const string &phoneNumber = "")
    {
        waitingList.push({c, roomNumber, bedType, nights, phoneNumber});
        setColor(COLOR_TITLE);
        printCentered("Added to waiting list for Room " + to_string(roomNumber) + ".");
        setColor(COLOR_DEFAULT);
    }

    void showRooms() const
    {
        setColor(COLOR_TITLE);
        printCentered("================================================");
        printCentered("HOTEL ROOMS");
        printCentered("================================================");
        setColor(COLOR_DEFAULT);

        printCategoryTable("Royal", COLOR_TITLE, "ROYAL SUITE");
        cout << endl;
        printCategoryTable("VIP", COLOR_VIP, "VIP ROOMS");
        cout << endl;
        printCategoryTable("Regular", COLOR_REGULAR, "REGULAR ROOMS");
    }

    void showWaitingList() const
    {
        setColor(COLOR_TITLE);
        printCentered("===== WAITING LIST =====");
        setColor(COLOR_DEFAULT);

        if (waitingList.empty())
        {
            setColor(COLOR_SUCCESS);
            printCentered("Waiting list is empty.");
            setColor(COLOR_DEFAULT);
            return;
        }

        queue<WaitingEntry> temp = waitingList;
        map<int, int> countPerRoom;
        int position = 1;

        while (!temp.empty())
        {
            const WaitingEntry &entry = temp.front();

            setColor(COLOR_TITLE);
            printCentered("Position: " + to_string(position));
            setColor(COLOR_DEFAULT);
            entry.customer.showData();
            printCentered("Waiting for Room: " + to_string(entry.roomNumber));
            printCentered("-----------------");

            countPerRoom[entry.roomNumber]++;
            temp.pop();
            position++;
        }

        setColor(COLOR_TITLE);
        printCentered("----- Waiting Count By Room -----");
        setColor(COLOR_DEFAULT);
        for (const auto &entry : countPerRoom)
        {
            printCentered("Room " + to_string(entry.first) + ": " +
                          to_string(entry.second) + " waiting");
        }
    }

    bool serveWaitingCustomer(Room &r, Customer &assignedCustomerOut)
    {
        if (waitingList.empty()) return false;

        queue<WaitingEntry> remaining;
        bool found = false;
        WaitingEntry matchedEntry;

        while (!waitingList.empty())
        {
            WaitingEntry entry = waitingList.front();
            waitingList.pop();

            if (!found && entry.roomNumber == r.getNumber())
            {
                matchedEntry = entry;
                found = true;
            }
            else
            {
                remaining.push(entry);
            }
        }

        waitingList = remaining;

        if (!found) return false;

        Customer &nextCustomer = matchedEntry.customer;

        if (r.book(nextCustomer.getId(), nextCustomer.getName()))
        {
            r.setBookingDetails(matchedEntry.bedType, matchedEntry.nights, matchedEntry.phoneNumber);
            assignedCustomerOut = nextCustomer;

            setColor(COLOR_SUCCESS);
            printCentered("Room " + to_string(r.getNumber()) +
                          " is now given to the next waiting customer.");
            setColor(COLOR_DEFAULT);
            nextCustomer.showData();
            return true;
        }
        return false;
    }

    bool hasWaitingCustomers() const
    {
        return !waitingList.empty();
    }

    void printManagerRoomGroup(const string &category) const
    {
        vector<Room> categoryRooms;

        for (const Room &r : rooms)
        {
            if (r.getCategory() == category)
                categoryRooms.push_back(r);
        }

        if (categoryRooms.empty()) return;

        setColor(COLOR_TITLE);
        if (category == "Royal")
            printCentered("ROYAL SUITE");
        else if (category == "VIP")
            printCentered("VIP ROOMS");
        else
            printCentered("REGULAR ROOMS");
        setColor(COLOR_DEFAULT);
        cout << endl;

        const int boxWidth = 26;
        const int gap = 3;

        int totalWidth = static_cast<int>(categoryRooms.size()) * boxWidth +
                         (static_cast<int>(categoryRooms.size()) - 1) * gap;

        int consoleWidth = getConsoleWidth();
        int leftSpaces = (consoleWidth - totalWidth) / 2;
        if (leftSpaces < 0) leftSpaces = 0;

        cout << string(leftSpaces, ' ');
        for (size_t i = 0; i < categoryRooms.size(); i++)
        {
            setColor(COLOR_TITLE);
            cout << "+" << string(boxWidth - 2, '-') << "+";
            if (i < categoryRooms.size() - 1) cout << string(gap, ' ');
        }

        cout << endl << string(leftSpaces, ' ');
        for (size_t i = 0; i < categoryRooms.size(); i++)
        {
            string text = "Room " + to_string(categoryRooms[i].getNumber());
            int padding = boxWidth - 2 - static_cast<int>(text.length());
            if (padding < 0) padding = 0;

            setColor(COLOR_TITLE);
            cout << "|" << string(padding / 2, ' ') << text
                 << string(padding - padding / 2, ' ') << "|";
            if (i < categoryRooms.size() - 1) cout << string(gap, ' ');
        }

        cout << endl << string(leftSpaces, ' ');
        for (size_t i = 0; i < categoryRooms.size(); i++)
        {
            string text = categoryRooms[i].getCategory();
            int padding = boxWidth - 2 - static_cast<int>(text.length());
            if (padding < 0) padding = 0;

            setColor(COLOR_DEFAULT);
            cout << "|" << string(padding / 2, ' ') << text
                 << string(padding - padding / 2, ' ') << "|";
            if (i < categoryRooms.size() - 1) cout << string(gap, ' ');
        }

        cout << endl << string(leftSpaces, ' ');
        for (size_t i = 0; i < categoryRooms.size(); i++)
        {
            string text = to_string(static_cast<int>(categoryRooms[i].getPrice())) + " EGP/n";
            int padding = boxWidth - 2 - static_cast<int>(text.length());
            if (padding < 0) padding = 0;

            setColor(COLOR_DEFAULT);
            cout << "|" << string(padding / 2, ' ') << text
                 << string(padding - padding / 2, ' ') << "|";
            if (i < categoryRooms.size() - 1) cout << string(gap, ' ');
        }

        cout << endl << string(leftSpaces, ' ');
        for (size_t i = 0; i < categoryRooms.size(); i++)
        {
            string text = categoryRooms[i].isAvailable() ? "AVAILABLE" : "BOOKED";
            int padding = boxWidth - 2 - static_cast<int>(text.length());
            if (padding < 0) padding = 0;

            if (categoryRooms[i].isAvailable())
                setColor(COLOR_SUCCESS);
            else
                setColor(COLOR_ERROR);

            cout << "|" << string(padding / 2, ' ') << text
                 << string(padding - padding / 2, ' ') << "|";
            setColor(COLOR_DEFAULT);

            if (i < categoryRooms.size() - 1) cout << string(gap, ' ');
        }

        cout << endl << string(leftSpaces, ' ');
        for (size_t i = 0; i < categoryRooms.size(); i++)
        {
            setColor(COLOR_TITLE);
            cout << "+" << string(boxWidth - 2, '-') << "+";
            if (i < categoryRooms.size() - 1) cout << string(gap, ' ');
        }

        setColor(COLOR_DEFAULT);
        cout << endl << endl;
    }

    void showRoomsDetailed() const
    {
        setColor(COLOR_TITLE);
        printCentered("==================================================");
        printCentered("MANAGER DASHBOARD");
        printCentered("==================================================");
        setColor(COLOR_DEFAULT);
        cout << endl;

        printManagerRoomGroup("Royal");
        printManagerRoomGroup("VIP");
        printManagerRoomGroup("Regular");

        bool hasBookedRooms = false;

        for (const Room &r : rooms)
        {
            if (!r.isAvailable())
            {
                if (!hasBookedRooms)
                {
                    setColor(COLOR_TITLE);
                    printCentered("BOOKED ROOMS DETAILS");
                    setColor(COLOR_DEFAULT);
                    printCentered("--------------------------------------------------");
                    hasBookedRooms = true;
                }

                setColor(COLOR_TITLE);
                printCentered("Room " + to_string(r.getNumber()) + " - " + r.getCategory());
                setColor(COLOR_DEFAULT);
                printCentered("Customer: " + r.getBookedCustomerName());
                printCentered("Customer ID: " + to_string(r.getBookedCustomerId()));
                printCentered("Bed: " + (r.getBedType().empty() ? "Not set" : r.getBedType()));
                printCentered("Nights: " + to_string(r.getNights()));
                printCentered("Phone: " + (r.getPhoneNumber().empty() ? "Not set" : r.getPhoneNumber()));
                printCentered("Total: " + to_string(static_cast<int>(r.getTotalPrice())) + " EGP");

                if (r.getPaymentMethod().empty())
                {
                    printCentered("Payment: Not selected");
                    setColor(COLOR_ERROR);
                    printCentered("Status: UNPAID");
                    setColor(COLOR_DEFAULT);
                }
                else
                {
                    printCentered("Payment: " + r.getPaymentMethod());
                    if (r.getIsPaid())
                    {
                        setColor(COLOR_SUCCESS);
                        printCentered("Status: PAID");
                        setColor(COLOR_DEFAULT);
                    }
                    else
                    {
                        setColor(COLOR_ERROR);
                        printCentered("Status: WAITING FOR CASH");
                        setColor(COLOR_DEFAULT);
                    }
                }

                printCentered("--------------------------------------------------");
            }
        }

        cout << endl;
        setColor(COLOR_TITLE);
        printCentered("==================================================");
        printCentered("WAITING LIST");
        printCentered("==================================================");
        setColor(COLOR_DEFAULT);

        if (waitingList.empty())
        {
            setColor(COLOR_SUCCESS);
            printCentered("No one is waiting.");
            setColor(COLOR_DEFAULT);
        }
        else
        {
            queue<WaitingEntry> temp = waitingList;
            int position = 1;

            while (!temp.empty())
            {
                const WaitingEntry &entry = temp.front();

                setColor(COLOR_TITLE);
                printCentered("[ Customer " + to_string(position) + " ]");
                setColor(COLOR_DEFAULT);

                printCentered("Name: " + entry.customer.getName());
                printCentered("ID: " + to_string(entry.customer.getId()));
                printCentered("Waiting for Room: " + to_string(entry.roomNumber));
                printCentered("Bed: " + (entry.bedType.empty() ? "Not set" : entry.bedType));
                printCentered("Nights: " + to_string(entry.nights));
                printCentered("Phone: " + (entry.phoneNumber.empty() ? "Not set" : entry.phoneNumber));
                printCentered("----------------------------------------------");

                temp.pop();
                position++;
            }

            setColor(COLOR_TITLE);
            printCentered("Total Waiting: " + to_string(waitingList.size()));
            setColor(COLOR_DEFAULT);
        }

        cout << endl;
        setColor(COLOR_TITLE);
        printCentered("==================================================");
        printCentered("END OF MANAGER VIEW");
        printCentered("==================================================");
        setColor(COLOR_DEFAULT);
    }

    Room* findRoom(int roomNumber)
    {
        for (Room &r : rooms)
        {
            if (r.getNumber() == roomNumber)
                return &r;
        }
        return nullptr;
    }
};

// ======================================================
//              CUSTOMER BOOK ROOM
// ======================================================

bool Customer::bookRoom(Room &r, HotelSystem &h, const string &bedType,
                        int nights, const string &phoneNumber)
{
    if (r.book(id, name))
    {
        r.setBookingDetails(bedType, nights, phoneNumber);

        setColor(COLOR_SUCCESS);
        printCentered("Room booked successfully.");
        setColor(COLOR_DEFAULT);
        return true;
    }

    setColor(COLOR_ERROR);
    printCentered("Room is not available.");
    setColor(COLOR_DEFAULT);

    h.addToWaitingList(*this, r.getNumber(), bedType, nights, phoneNumber);
    return false;
}

// ======================================================
//              CUSTOMER CANCEL ROOM
// ======================================================

bool Customer::cancelRoom(Room &r, HotelSystem &h,
                          Customer &assignedCustomerOut, bool &nextCustomerAssigned)
{
    nextCustomerAssigned = false;

    if (r.isAvailable())
    {
        setColor(COLOR_ERROR);
        printCentered("Room is already free.");
        setColor(COLOR_DEFAULT);
        return false;
    }

    if (!r.isBookedBy(id))
    {
        setColor(COLOR_ERROR);
        printCentered("You cannot cancel this room.");
        printCentered("It is booked by someone else.");
        setColor(COLOR_DEFAULT);
        return false;
    }

    r.cancel();

    setColor(COLOR_SUCCESS);
    printCentered("Room cancelled successfully.");
    setColor(COLOR_DEFAULT);

    if (h.hasWaitingCustomers())
    {
        if (h.serveWaitingCustomer(r, assignedCustomerOut))
        {
            nextCustomerAssigned = true;
        }
    }

    return true;
}

// ======================================================
//                 MANAGER FUNCTIONS
// ======================================================

void Manager::showRooms(HotelSystem &h) const
{
    h.showRoomsDetailed();
}

void Manager::showWaitingList(HotelSystem &h) const
{
    h.showWaitingList();
}

// ======================================================
//                  PAYMENT FUNCTION
// ======================================================

bool handlePayment(double amount, string &outMethod, bool &outPaid)
{
    setColor(COLOR_TITLE);
    printCentered("Choose payment method:");
    setColor(COLOR_DEFAULT);
    printCentered("1. Cash");
    printCentered("2. Card");
    printCenteredPrompt("Choice: ");

    int choice;
    cin >> choice;

    unique_ptr<Payment> payment;

    if (choice == 1)
    {
        payment = make_unique<CashPayment>();
        payment->pay(amount);
        outMethod = "Cash";
        outPaid = false;
        return true;
    }
    else if (choice == 2)
    {
        string cardNumber, expiryDate, cvv, cardHolderName;

        setColor(COLOR_TITLE);
        printCentered("===== Card Details =====");
        setColor(COLOR_DEFAULT);

        printCenteredPrompt("Card Holder Name: ");
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, cardHolderName);

        printCenteredPrompt("Card Number: ");
        getline(cin, cardNumber);

        printCenteredPrompt("Expiry (MM/YY): ");
        getline(cin, expiryDate);

        printCenteredPrompt("CVV: ");
        getline(cin, cvv);

        setColor(COLOR_SUCCESS);
        printCentered("Card details saved.");
        setColor(COLOR_DEFAULT);

        payment = make_unique<CardPayment>();
        payment->pay(amount);

        outMethod = "Card";
        outPaid = true;
        return true;
    }
    else
    {
        setColor(COLOR_ERROR);
        printCentered("Invalid choice. Payment skipped.");
        setColor(COLOR_DEFAULT);
        outMethod = "";
        outPaid = false;
        return false;
    }
}

// ======================================================
//                    READ INTEGER
// ======================================================

int readInt(const string &prompt)
{
    int value;
    printCenteredPrompt(prompt);

    while (!(cin >> value))
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        setColor(COLOR_ERROR);
        printCenteredPrompt("Invalid input, enter a number: ");
        setColor(COLOR_DEFAULT);
    }
    return value;
}

// ======================================================
//                       MAIN
// ======================================================

int main()
{
    setConsoleTheme();
    setColor(COLOR_DEFAULT);
    system("cls");

    HotelSystem hotel;

    // ========== ROYAL SUITE ==========
    hotel.addRoom(Room(301, 7500, "Royal"));
    hotel.addRoom(Room(302, 8200, "Royal"));

    // ========== VIP ROOMS ==========
    hotel.addRoom(Room(201, 2800, "VIP"));
    hotel.addRoom(Room(202, 3200, "VIP"));

    // ========== REGULAR ROOMS ==========
    hotel.addRoom(Room(101, 950, "Regular"));
    hotel.addRoom(Room(102, 1100, "Regular"));
    hotel.addRoom(Room(103, 1350, "Regular"));

    Manager manager;
    manager.setData("Hotel Manager", 100);

    int choice;

    do
    {
        setColor(COLOR_TITLE);
        printCentered("================================================");
        printCentered("HOTEL SYSTEM MENU");
        printCentered("================================================");
        setColor(COLOR_DEFAULT);

        printCentered("1. Show Rooms");
        printCentered("2. Book Room");
        printCentered("3. Cancel Room");
        printCentered("4. Show Waiting List");
        printCentered("5. Manager View");
        printCentered("6. Exit");

        setColor(COLOR_TITLE);
        printCentered("================================================");
        setColor(COLOR_DEFAULT);

        choice = readInt("Enter choice: ");

        switch (choice)
        {
        case 1:
            hotel.showRooms();
            break;

        case 2:
        {
            string name;
            printCenteredPrompt("Enter customer name: ");
            cin >> name;

            int id = readInt("Enter customer ID: ");
            int roomNumber = readInt("Enter room number: ");

            Customer customer;
            customer.setData(name, id);

            Room* room = hotel.findRoom(roomNumber);

            if (room == nullptr)
            {
                setColor(COLOR_ERROR);
                printCentered("Room not found.");
                setColor(COLOR_DEFAULT);
                break;
            }

            string bedType;
            printCenteredPrompt("Bed type (Single/Double): ");
            cin >> bedType;

            int nights = readInt("Number of nights: ");

            string phoneNumber;
            printCenteredPrompt("Phone number: ");
            cin >> phoneNumber;

            bool booked = customer.bookRoom(*room, hotel, bedType, nights, phoneNumber);

            if (booked)
            {
                string method;
                bool paid = false;

                double total = room->getPrice() * nights;

                setColor(COLOR_TITLE);
                printCentered("Total amount: " + to_string(static_cast<int>(total)) + " EGP");
                setColor(COLOR_DEFAULT);

                if (handlePayment(total, method, paid))
                {
                    room->setPaymentInfo(method, paid);
                }
            }
            break;
        }

        case 3:
        {
            string name;
            printCenteredPrompt("Enter customer name: ");
            cin >> name;

            int id = readInt("Enter customer ID: ");
            int roomNumber = readInt("Enter room number: ");

            Customer customer;
            customer.setData(name, id);

            Room* room = hotel.findRoom(roomNumber);

            if (room != nullptr)
            {
                Customer assignedCustomer;
                bool nextAssigned = false;

                bool cancelled = customer.cancelRoom(*room, hotel, assignedCustomer, nextAssigned);

                if (cancelled && nextAssigned)
                {
                    setColor(COLOR_SUCCESS);
                    printCentered("Room " + to_string(room->getNumber()) +
                                  " is now given to the next waiting customer:");
                    setColor(COLOR_DEFAULT);
                    assignedCustomer.showData();

                    printCentered("This customer must pay now.");

                    string method;
                    bool paid = false;

                    double total = room->getPrice() * room->getNights();
                    setColor(COLOR_TITLE);
                    printCentered("Total amount: " + to_string(static_cast<int>(total)) + " EGP");
                    setColor(COLOR_DEFAULT);

                    if (handlePayment(total, method, paid))
                    {
                        room->setPaymentInfo(method, paid);
                    }
                }
            }
            else
            {
                setColor(COLOR_ERROR);
                printCentered("Room not found.");
                setColor(COLOR_DEFAULT);
            }
            break;
        }

        case 4:
            manager.showWaitingList(hotel);
            break;

        case 5:
            manager.showRooms(hotel);
            break;

        case 6:
            setColor(COLOR_SUCCESS);
            printCentered("Thank you for using Hotel System.");
            setColor(COLOR_DEFAULT);
            break;

        default:
            setColor(COLOR_ERROR);
            printCentered("Invalid choice. Try again.");
            setColor(COLOR_DEFAULT);
        }

    } while (choice != 6);

    setColor(COLOR_RESET);
    return 0;
}
