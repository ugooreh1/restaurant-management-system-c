/*
    ELE118 C Programming Project
    Smart Restaurant Ordering and Table Management System

    Description:
    This program simulates a restaurant system with two user areas: Customer and Worker.
    Customers are guided through a structured process of being seated, creating an order,
    adding items, reviewing their order, and finalising payment.

    Workers can monitor and manage the system by viewing tables, tracking active orders,
    updating order statuses, and viewing restaurant performance statistics.

    Key Features:
    - Structured user flow for ease of use
    - Defensive programming for safe input handling
    - Order and table management using arrays of structures
    - Realistic billing system with discounts and service charges
    - Receipt timestamp using the time.h library

    Academic Integrity:
    The module leader will submit C programs to a plagiarism checker. Any suspicion
    of academic misconduct will be investigated and may lead to penalties.

    Author: Ugochukwu Oreh
    Date: 4th May, 2026.
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_MENU_ITEMS 12
#define MAX_TABLES 8
#define MAX_ORDERS 30
#define MAX_ORDER_ITEMS 10
#define NAME_LENGTH 50
#define CATEGORY_LENGTH 30
#define STATUS_LENGTH 20

/* Structure used to store details for each menu item. */
typedef struct {
    int itemID;
    char name[NAME_LENGTH];
    char category[CATEGORY_LENGTH];
    float price;
    bool available;
} MenuItem;

/* Structure used to store details for each restaurant table. */
typedef struct {
    int tableNumber;
    int capacity;
    bool occupied;
    int assignedGroupSize;
    float currentBill;
    int activeOrderCount;
} Table;

/* Structure used to store details for each customer order. */
typedef struct {
    int orderID;
    int tableNumber;
    char customerName[NAME_LENGTH];
    int itemIDs[MAX_ORDER_ITEMS];
    int quantities[MAX_ORDER_ITEMS];
    int itemCount;
    float subtotal;
    float discount;
    float serviceCharge;
    float finalTotal;
    bool studentDiscountApplied;
    bool isActive;
    char status[STATUS_LENGTH];
} Order;

/* Function prototypes */
int getValidInt(const char prompt[], int min, int max);
int getYesNoChoice(const char prompt[]);
void getTextInput(const char prompt[], char text[], int length);
void pauseScreen(void);
void printCurrentDateTime(void);

void initialiseMenu(MenuItem menu[], int size);
void initialiseTables(Table tables[], int size);
void initialiseOrders(Order orders[], int size);

void customerHome(const char userName[], MenuItem menu[], int menuSize, Table tables[], int tableSize, Order orders[], int orderSize);
void workerHome(const char userName[], MenuItem menu[], int menuSize, Table tables[], int tableSize, Order orders[], int orderSize);

int startCustomerVisit(const char userName[], MenuItem menu[], int menuSize, Table tables[], int tableSize, Order orders[], int orderSize);
void addItemsToOrderByID(Order orders[], int orderSize, const MenuItem menu[], int menuSize, Table tables[], int tableSize, int orderID);
void reviewOrderByID(const Order orders[], int orderSize, const MenuItem menu[], int menuSize, int orderID);
void finaliseBillByID(Order orders[], int orderSize, Table tables[], int tableSize, const MenuItem menu[], int menuSize, int orderID);

void displayMenu(const MenuItem menu[], int size);
void displayTables(const Table tables[], int size);
void displayActiveOrders(const Order orders[], int orderSize);
void displayRestaurantSummary(const Table tables[], int tableCount, const Order orders[], int orderCount);
void updateOrderStatus(Order orders[], int orderSize, Table tables[], int tableSize);

int findFreeTableForGroup(const Table tables[], int tableSize, int groupSize);
int findOrderIndexByID(const Order orders[], int orderSize, int orderID);
int findMenuIndexByID(const MenuItem menu[], int menuSize, int itemID);
int findTableIndexByNumber(const Table tables[], int tableSize, int tableNumber);
int findEmptyOrderSlot(const Order orders[], int orderSize);

float calculateOrderSubtotal(const Order *order, const MenuItem menu[], int menuSize);
float calculateDiscount(float subtotal, bool studentDiscountApplied);
float calculateServiceCharge(float discountedSubtotal, bool applyServiceCharge);
float calculateFinalTotal(float subtotal, float discount, float serviceCharge);

/*
    Main Function:
    Entry point of the program.
    Initialises all main arrays, asks the user for their name,
    and allows them to access either the customer area or worker area.
*/
int main()
{
    MenuItem menu[MAX_MENU_ITEMS];
    Table tables[MAX_TABLES];
    Order orders[MAX_ORDERS];

    char userName[NAME_LENGTH];
    int running = 1;
    int roleChoice;

    initialiseMenu(menu, MAX_MENU_ITEMS);
    initialiseTables(tables, MAX_TABLES);
    initialiseOrders(orders, MAX_ORDERS);

    printf("=============================================\n");
    printf("      Welcome to Smart Restaurant System\n");
    printf("=============================================\n");
    printf("This system will guide you step by step.\n\n");

    getTextInput("Please enter your name: ", userName, NAME_LENGTH);

    /* Main access loop allows the user to switch between customer and worker areas. */
    while (running == 1)
    {
        printf("\n=============================================\n");
        printf("              MAIN ACCESS MENU\n");
        printf("=============================================\n");
        printf("Hello %s. What would you like to access?\n\n", userName);
        printf("1. Customer Area\n");
        printf("2. Worker Area\n");
        printf("3. Exit System\n");

        roleChoice = getValidInt("Enter your choice: ", 1, 3);

        switch (roleChoice)
        {
            case 1:
                customerHome(userName, menu, MAX_MENU_ITEMS, tables, MAX_TABLES, orders, MAX_ORDERS);
                break;

            case 2:
                workerHome(userName, menu, MAX_MENU_ITEMS, tables, MAX_TABLES, orders, MAX_ORDERS);
                break;

            case 3:
                running = 0;
                break;
        }
    }

    printf("\nThank you for using the system. Goodbye, %s!\n", userName);

    return 0;
}

/*
    Function: customerHome
    Purpose:
    Controls the customer side of the system.
    The customer is guided through starting a visit, adding items,
    reviewing their order, and finalising the bill.
*/
void customerHome(const char userName[], MenuItem menu[], int menuSize, Table tables[], int tableSize, Order orders[], int orderSize)
{
    int running = 1;
    int choice;
    int currentOrderID = 0;

    while (running == 1)
    {
        printf("\n=============================================\n");
        printf("              CUSTOMER HOME\n");
        printf("=============================================\n");

        if (currentOrderID == 0)
        {
            printf("You do not currently have an active order.\n\n");
            printf("1. Start a new restaurant visit\n");
            printf("2. View the menu only\n");
            printf("3. Return to main access menu\n");

            choice = getValidInt("What would you like to do? ", 1, 3);

            switch (choice)
            {
                case 1:
                    currentOrderID = startCustomerVisit(userName, menu, menuSize, tables, tableSize, orders, orderSize);
                    pauseScreen();
                    break;

                case 2:
                    displayMenu(menu, menuSize);
                    pauseScreen();
                    break;

                case 3:
                    running = 0;
                    break;
            }
        }
        else
        {
            printf("Your active Order ID is: %d\n\n", currentOrderID);
            printf("1. Add more items to my order\n");
            printf("2. Review my order\n");
            printf("3. Finalise and pay bill\n");
            printf("4. View menu again\n");
            printf("5. Return to main access menu\n");

            choice = getValidInt("What would you like to do next? ", 1, 5);

            switch (choice)
            {
                case 1:
                    addItemsToOrderByID(orders, orderSize, menu, menuSize, tables, tableSize, currentOrderID);
                    pauseScreen();
                    break;

                case 2:
                    reviewOrderByID(orders, orderSize, menu, menuSize, currentOrderID);
                    pauseScreen();
                    break;

                case 3:
                    finaliseBillByID(orders, orderSize, tables, tableSize, menu, menuSize, currentOrderID);
                    currentOrderID = 0;
                    pauseScreen();
                    break;

                case 4:
                    displayMenu(menu, menuSize);
                    pauseScreen();
                    break;

                case 5:
                    running = 0;
                    break;
            }
        }
    }
}

/*
    Function: workerHome
    Purpose:
    Provides management features for restaurant workers.
    Workers can view tables, view active orders, update order status,
    and view summary statistics.
*/
void workerHome(const char userName[], MenuItem menu[], int menuSize, Table tables[], int tableSize, Order orders[], int orderSize)
{
    int running = 1;
    int choice;

    while (running == 1)
    {
        printf("\n=============================================\n");
        printf("               WORKER HOME\n");
        printf("=============================================\n");
        printf("Hello %s. Choose a management option below.\n\n", userName);
        printf("1. View menu\n");
        printf("2. View table status\n");
        printf("3. View active orders\n");
        printf("4. Update order status\n");
        printf("5. View restaurant summary\n");
        printf("6. Return to main access menu\n");

        choice = getValidInt("Choose an option: ", 1, 6);

        switch (choice)
        {
            case 1:
                displayMenu(menu, menuSize);
                pauseScreen();
                break;

            case 2:
                displayTables(tables, tableSize);
                pauseScreen();
                break;

            case 3:
                displayActiveOrders(orders, orderSize);
                pauseScreen();
                break;

            case 4:
                updateOrderStatus(orders, orderSize, tables, tableSize);
                pauseScreen();
                break;

            case 5:
                displayRestaurantSummary(tables, tableSize, orders, orderSize);
                pauseScreen();
                break;

            case 6:
                running = 0;
                break;
        }
    }
}

/*
    Function: startCustomerVisit
    Purpose:
    Starts a customer visit by assigning a suitable table and creating an order.
    It returns the new order ID so the customer can continue managing that order.
*/
int startCustomerVisit(const char userName[], MenuItem menu[], int menuSize, Table tables[], int tableSize, Order orders[], int orderSize)
{
    int groupSize;
    int tableIndex;
    int orderIndex;
    int orderID = 0;
    int addNow;

    printf("\n=============================================\n");
    printf("            START NEW VISIT\n");
    printf("=============================================\n");
    printf("First, the system will seat your group at a suitable table.\n");

    groupSize = getValidInt("How many people are in your group? ", 1, 8);

    /* Find the first available table that can fit the group. */
    tableIndex = findFreeTableForGroup(tables, tableSize, groupSize);

    if (tableIndex == -1)
    {
        printf("\nSorry, no suitable table is currently available for a group of %d.\n", groupSize);
    }
    else
    {
        /* Find an unused order slot before creating a new order. */
        orderIndex = findEmptyOrderSlot(orders, orderSize);

        if (orderIndex == -1)
        {
            printf("\nSorry, the system cannot create more orders at the moment.\n");
        }
        else
        {
            tables[tableIndex].occupied = true;
            tables[tableIndex].assignedGroupSize = groupSize;
            tables[tableIndex].currentBill = 0.0;
            tables[tableIndex].activeOrderCount = 1;

            orders[orderIndex].orderID = orderIndex + 1;
            orders[orderIndex].tableNumber = tables[tableIndex].tableNumber;
            strcpy(orders[orderIndex].customerName, userName);
            orders[orderIndex].itemCount = 0;
            orders[orderIndex].subtotal = 0.0;
            orders[orderIndex].discount = 0.0;
            orders[orderIndex].serviceCharge = 0.0;
            orders[orderIndex].finalTotal = 0.0;
            orders[orderIndex].studentDiscountApplied = false;
            orders[orderIndex].isActive = true;
            strcpy(orders[orderIndex].status, "Preparing");

            orderID = orders[orderIndex].orderID;

            printf("\nGreat! You have been seated at Table %d.\n", tables[tableIndex].tableNumber);
            printf("Your Order ID is %d. Please remember this number.\n", orderID);

            addNow = getYesNoChoice("\nWould you like to add food items now? Enter 1 for Yes or 0 for No: ");

            if (addNow == 1)
            {
                addItemsToOrderByID(orders, orderSize, menu, menuSize, tables, tableSize, orderID);
                reviewOrderByID(orders, orderSize, menu, menuSize, orderID);
            }
            else
            {
                printf("\nNo problem. You can add items later from Customer Home.\n");
            }
        }
    }

    return orderID;
}

/*
    Function: addItemsToOrderByID
    Purpose:
    Adds menu items to an existing active order.
    The function checks availability and updates quantity if the item already exists.
*/
void addItemsToOrderByID(Order orders[], int orderSize, const MenuItem menu[], int menuSize, Table tables[], int tableSize, int orderID)
{
    int orderIndex;
    int tableIndex;
    int addMore = 1;
    int itemID;
    int itemIndex;
    int existingItemIndex;
    int quantity;
    int i;

    orderIndex = findOrderIndexByID(orders, orderSize, orderID);

    if (orderIndex == -1 || orders[orderIndex].isActive == false)
    {
        printf("\nThis order could not be found or is no longer active.\n");
    }
    else
    {
        printf("\n=============================================\n");
        printf("              ADD ITEMS\n");
        printf("=============================================\n");
        printf("You can add up to %d different menu items to one order.\n", MAX_ORDER_ITEMS);
        printf("Unavailable items cannot be ordered.\n");

        while (addMore == 1 && orders[orderIndex].itemCount < MAX_ORDER_ITEMS)
        {
            displayMenu(menu, menuSize);

            itemID = getValidInt("\nEnter the item ID you want to add: ", 101, 112);
            itemIndex = findMenuIndexByID(menu, menuSize, itemID);

            if (itemIndex == -1)
            {
                printf("That item ID does not exist.\n");
            }
            else if (menu[itemIndex].available == false)
            {
                printf("Sorry, %s is currently unavailable.\n", menu[itemIndex].name);
            }
            else
            {
                quantity = getValidInt("Enter quantity: ", 1, 20);
                existingItemIndex = -1;

                /* Update quantity instead of duplicating the same item in the order. */
                for (i = 0; i < orders[orderIndex].itemCount && existingItemIndex == -1; i++)
                {
                    if (orders[orderIndex].itemIDs[i] == itemID)
                    {
                        existingItemIndex = i;
                    }
                }

                if (existingItemIndex != -1)
                {
                    orders[orderIndex].quantities[existingItemIndex] += quantity;

                    printf("\nUpdated %s quantity to %d.\n",
                           menu[itemIndex].name,
                           orders[orderIndex].quantities[existingItemIndex]);
                }
                else
                {
                    orders[orderIndex].itemIDs[orders[orderIndex].itemCount] = itemID;
                    orders[orderIndex].quantities[orders[orderIndex].itemCount] = quantity;
                    orders[orderIndex].itemCount++;

                    printf("\nAdded %d x %s to your order.\n", quantity, menu[itemIndex].name);
                }

                /* Update the live subtotal displayed against the table. */
                orders[orderIndex].subtotal = calculateOrderSubtotal(&orders[orderIndex], menu, menuSize);
                tableIndex = findTableIndexByNumber(tables, tableSize, orders[orderIndex].tableNumber);

                if (tableIndex != -1)
                {
                    tables[tableIndex].currentBill = orders[orderIndex].subtotal;
                }
            }

            if (orders[orderIndex].itemCount < MAX_ORDER_ITEMS)
            {
                addMore = getYesNoChoice("\nAdd another item? Enter 1 for Yes or 0 for No: ");
            }
            else
            {
                printf("\nMaximum number of different items reached for this order.\n");
            }
        }
    }
}

/*
    Function: reviewOrderByID
    Purpose:
    Displays the current order, including items, quantities, line totals,
    and the current subtotal.
*/
void reviewOrderByID(const Order orders[], int orderSize, const MenuItem menu[], int menuSize, int orderID)
{
    int orderIndex;
    int menuIndex;
    int i;
    float lineTotal;

    orderIndex = findOrderIndexByID(orders, orderSize, orderID);

    if (orderIndex == -1)
    {
        printf("\nOrder not found.\n");
    }
    else
    {
        printf("\n=============================================\n");
        printf("              ORDER REVIEW\n");
        printf("=============================================\n");
        printf("Customer: %s\n", orders[orderIndex].customerName);
        printf("Order ID: %d\n", orders[orderIndex].orderID);
        printf("Table: %d\n", orders[orderIndex].tableNumber);
        printf("Status: %s\n", orders[orderIndex].status);

        if (orders[orderIndex].itemCount == 0)
        {
            printf("\nNo items have been added yet.\n");
        }
        else
        {
            printf("\n%-25s %-10s %-10s\n", "Item", "Qty", "Line Total");
            printf("---------------------------------------------\n");

            for (i = 0; i < orders[orderIndex].itemCount; i++)
            {
                menuIndex = findMenuIndexByID(menu, menuSize, orders[orderIndex].itemIDs[i]);

                if (menuIndex != -1)
                {
                    lineTotal = menu[menuIndex].price * orders[orderIndex].quantities[i];

                    printf("%-25s %-10d GBP %.2f\n",
                           menu[menuIndex].name,
                           orders[orderIndex].quantities[i],
                           lineTotal);
                }
            }

            printf("---------------------------------------------\n");
            printf("Current subtotal: GBP %.2f\n", calculateOrderSubtotal(&orders[orderIndex], menu, menuSize));
        }
    }
}

/*
    Function: finaliseBillByID
    Purpose:
    Completes an order and prints a final bill.
    The function calculates subtotal, discount, service charge, and final total.
*/
void finaliseBillByID(Order orders[], int orderSize, Table tables[], int tableSize, const MenuItem menu[], int menuSize, int orderID)
{
    int orderIndex;
    int tableIndex;
    int applyServiceCharge;

    orderIndex = findOrderIndexByID(orders, orderSize, orderID);

    if (orderIndex == -1 || orders[orderIndex].isActive == false)
    {
        printf("\nThis order could not be found or has already been finalised.\n");
    }
    else if (orders[orderIndex].itemCount == 0)
    {
        /* Prevents an empty order from being finalised. */
        printf("\nYou cannot finalise an empty order. Please add at least one item first.\n");
    }
    else
    {
        orders[orderIndex].subtotal = calculateOrderSubtotal(&orders[orderIndex], menu, menuSize);

        orders[orderIndex].studentDiscountApplied =
            getYesNoChoice("Apply student discount? Enter 1 for Yes or 0 for No: ");

        orders[orderIndex].discount =
            calculateDiscount(orders[orderIndex].subtotal, orders[orderIndex].studentDiscountApplied);

        applyServiceCharge =
            getYesNoChoice("Apply 12.5 percent service charge? Enter 1 for Yes or 0 for No: ");

        orders[orderIndex].serviceCharge =
            calculateServiceCharge(orders[orderIndex].subtotal - orders[orderIndex].discount, applyServiceCharge);

        orders[orderIndex].finalTotal =
            calculateFinalTotal(orders[orderIndex].subtotal,
                                orders[orderIndex].discount,
                                orders[orderIndex].serviceCharge);

        strcpy(orders[orderIndex].status, "Completed");
        orders[orderIndex].isActive = false;

        /* Reset the table after payment is completed. */
        tableIndex = findTableIndexByNumber(tables, tableSize, orders[orderIndex].tableNumber);

        if (tableIndex != -1)
        {
            tables[tableIndex].currentBill = 0.0;
            tables[tableIndex].activeOrderCount = 0;
            tables[tableIndex].occupied = false;
            tables[tableIndex].assignedGroupSize = 0;
        }

        printf("\n=============================================\n");
        printf("                 FINAL BILL\n");
        printf("=============================================\n");
        printCurrentDateTime();
        printf("Customer Name: %s\n", orders[orderIndex].customerName);
        printf("Order ID: %d\n", orders[orderIndex].orderID);
        printf("Table Number: %d\n", orders[orderIndex].tableNumber);
        printf("---------------------------------------------\n");
        printf("Subtotal:       GBP %.2f\n", orders[orderIndex].subtotal);
        printf("Discount:       GBP %.2f\n", orders[orderIndex].discount);
        printf("Service Charge: GBP %.2f\n", orders[orderIndex].serviceCharge);
        printf("Final Total:    GBP %.2f\n", orders[orderIndex].finalTotal);
        printf("---------------------------------------------\n");
        printf("Payment complete. Thank you for visiting!\n");
    }
}

/*
    Function: updateOrderStatus
    Purpose:
    Allows a worker to update the status of an order.
    If an order is completed or cancelled, the table is reset.
*/
void updateOrderStatus(Order orders[], int orderSize, Table tables[], int tableSize)
{
    int orderID;
    int orderIndex;
    int tableIndex;
    int choice;
    int wasActive;

    orderID = getValidInt("Enter the order ID to update: ", 1, orderSize);
    orderIndex = findOrderIndexByID(orders, orderSize, orderID);

    if (orderIndex == -1)
    {
        printf("\nOrder not found.\n");
    }
    else
    {
        wasActive = orders[orderIndex].isActive;

        printf("\nCurrent status: %s\n", orders[orderIndex].status);
        printf("Choose the new status:\n");
        printf("1. Preparing\n");
        printf("2. Ready\n");
        printf("3. Served\n");
        printf("4. Completed\n");
        printf("5. Cancelled\n");

        choice = getValidInt("Enter choice: ", 1, 5);

        switch (choice)
        {
            case 1:
                strcpy(orders[orderIndex].status, "Preparing");
                orders[orderIndex].isActive = true;
                break;

            case 2:
                strcpy(orders[orderIndex].status, "Ready");
                orders[orderIndex].isActive = true;
                break;

            case 3:
                strcpy(orders[orderIndex].status, "Served");
                orders[orderIndex].isActive = true;
                break;

            case 4:
                strcpy(orders[orderIndex].status, "Completed");
                orders[orderIndex].isActive = false;
                break;

            case 5:
                strcpy(orders[orderIndex].status, "Cancelled");
                orders[orderIndex].isActive = false;
                break;
        }

        if (wasActive == true && orders[orderIndex].isActive == false)
        {
            tableIndex = findTableIndexByNumber(tables, tableSize, orders[orderIndex].tableNumber);

            if (tableIndex != -1)
            {
                tables[tableIndex].activeOrderCount = 0;
                tables[tableIndex].occupied = false;
                tables[tableIndex].assignedGroupSize = 0;
                tables[tableIndex].currentBill = 0.0;
            }
        }

        printf("Order %d status changed to %s.\n",
               orders[orderIndex].orderID,
               orders[orderIndex].status);
    }
}

/* Display all menu items in a formatted table. */
void displayMenu(const MenuItem menu[], int size)
{
    int i;

    printf("\n=============================================\n");
    printf("                  MENU\n");
    printf("=============================================\n");
    printf("%-8s %-25s %-15s %-10s %-12s\n", "ID", "Item", "Category", "Price", "Status");
    printf("-----------------------------------------------------------------\n");

    for (i = 0; i < size; i++)
    {
        printf("%-8d %-25s %-15s GBP %-6.2f %-12s\n",
               menu[i].itemID,
               menu[i].name,
               menu[i].category,
               menu[i].price,
               menu[i].available ? "Available" : "Unavailable");
    }
}

/* Display all tables and their current status. */
void displayTables(const Table tables[], int size)
{
    int i;

    printf("\n=============================================\n");
    printf("              TABLE STATUS\n");
    printf("=============================================\n");
    printf("%-8s %-10s %-12s %-15s %-10s\n", "Table", "Capacity", "Status", "Group Size", "Bill");
    printf("-----------------------------------------------------------------\n");

    for (i = 0; i < size; i++)
    {
        printf("%-8d %-10d %-12s %-15d GBP %-6.2f\n",
               tables[i].tableNumber,
               tables[i].capacity,
               tables[i].occupied ? "Occupied" : "Free",
               tables[i].assignedGroupSize,
               tables[i].currentBill);
    }
}

/* Display active orders so workers can monitor current restaurant activity. */
void displayActiveOrders(const Order orders[], int orderSize)
{
    int i;
    int found = 0;

    printf("\n=============================================\n");
    printf("              ACTIVE ORDERS\n");
    printf("=============================================\n");

    for (i = 0; i < orderSize; i++)
    {
        if (orders[i].orderID != 0 && orders[i].isActive == true)
        {
            printf("Order ID: %d | Customer: %s | Table: %d | Status: %s\n",
                   orders[i].orderID,
                   orders[i].customerName,
                   orders[i].tableNumber,
                   orders[i].status);

            found = 1;
        }
    }

    if (found == 0)
    {
        printf("There are no active orders at the moment.\n");
    }
}

/*
    Function: displayRestaurantSummary
    Purpose:
    Shows overall restaurant performance, including order counts,
    revenue, and table availability.
*/
void displayRestaurantSummary(const Table tables[], int tableCount, const Order orders[], int orderCount)
{
    int i;
    int totalOrders = 0;
    int activeOrders = 0;
    int completedOrders = 0;
    int cancelledOrders = 0;
    int occupiedTables = 0;
    int freeTables = 0;
    float totalRevenue = 0.0;
    float averageBill = 0.0;

    for (i = 0; i < orderCount; i++)
    {
        if (orders[i].orderID != 0)
        {
            totalOrders++;

            if (orders[i].isActive == true)
            {
                activeOrders++;
            }

            if (strcmp(orders[i].status, "Completed") == 0)
            {
                completedOrders++;
                totalRevenue += orders[i].finalTotal;
            }

            if (strcmp(orders[i].status, "Cancelled") == 0)
            {
                cancelledOrders++;
            }
        }
    }

    for (i = 0; i < tableCount; i++)
    {
        if (tables[i].occupied == true)
        {
            occupiedTables++;
        }
        else
        {
            freeTables++;
        }
    }

    if (completedOrders > 0)
    {
        averageBill = totalRevenue / completedOrders;
    }

    printf("\n=============================================\n");
    printf("            RESTAURANT SUMMARY\n");
    printf("=============================================\n");
    printf("Total Orders: %d\n", totalOrders);
    printf("Active Orders: %d\n", activeOrders);
    printf("Completed Orders: %d\n", completedOrders);
    printf("Cancelled Orders: %d\n", cancelledOrders);
    printf("Total Revenue: GBP %.2f\n", totalRevenue);
    printf("Average Completed Bill: GBP %.2f\n", averageBill);
    printf("Occupied Tables: %d\n", occupiedTables);
    printf("Free Tables: %d\n", freeTables);
}

/* Initialise menu items. This function sets values in an array of structs. */
void initialiseMenu(MenuItem menu[], int size)
{
    menu[0] = (MenuItem){101, "Jollof Rice", "Main", 12.50, true};
    menu[1] = (MenuItem){102, "Chicken Burger", "Main", 10.99, true};
    menu[2] = (MenuItem){103, "Margherita Pizza", "Main", 11.75, true};
    menu[3] = (MenuItem){104, "Caesar Salad", "Starter", 7.25, true};
    menu[4] = (MenuItem){105, "Pepper Soup", "Starter", 8.40, true};
    menu[5] = (MenuItem){106, "Grilled Salmon", "Main", 15.80, true};
    menu[6] = (MenuItem){107, "French Fries", "Side", 4.50, true};
    menu[7] = (MenuItem){108, "Chapman", "Drink", 3.99, true};
    menu[8] = (MenuItem){109, "Orange Juice", "Drink", 3.50, true};
    menu[9] = (MenuItem){110, "Chocolate Cake", "Dessert", 5.25, true};
    menu[10] = (MenuItem){111, "Ice Cream", "Dessert", 4.75, true};
    menu[11] = (MenuItem){112, "Beef Suya", "Starter", 9.20, false};
}

/* Initialise table data so all table values start in a known state. */
void initialiseTables(Table tables[], int size)
{
    int capacities[MAX_TABLES] = {2, 2, 4, 4, 6, 6, 8, 8};
    int i;

    for (i = 0; i < size; i++)
    {
        tables[i].tableNumber = i + 1;
        tables[i].capacity = capacities[i];
        tables[i].occupied = false;
        tables[i].assignedGroupSize = 0;
        tables[i].currentBill = 0.0;
        tables[i].activeOrderCount = 0;
    }
}

/* Initialise orders so no order contains random memory values. */
void initialiseOrders(Order orders[], int size)
{
    int i;

    for (i = 0; i < size; i++)
    {
        orders[i].orderID = 0;
        orders[i].tableNumber = 0;
        strcpy(orders[i].customerName, "None");
        orders[i].itemCount = 0;
        orders[i].subtotal = 0.0;
        orders[i].discount = 0.0;
        orders[i].serviceCharge = 0.0;
        orders[i].finalTotal = 0.0;
        orders[i].studentDiscountApplied = false;
        orders[i].isActive = false;
        strcpy(orders[i].status, "None");
    }
}

/* Search for the first free table that can fit the customer's group size. */
int findFreeTableForGroup(const Table tables[], int tableSize, int groupSize)
{
    int i;
    int tableIndex = -1;

    for (i = 0; i < tableSize && tableIndex == -1; i++)
    {
        if (tables[i].occupied == false && tables[i].capacity >= groupSize)
        {
            tableIndex = i;
        }
    }

    return tableIndex;
}

/* Search for an order using its order ID. */
int findOrderIndexByID(const Order orders[], int orderSize, int orderID)
{
    int i;
    int index = -1;

    for (i = 0; i < orderSize && index == -1; i++)
    {
        if (orders[i].orderID == orderID)
        {
            index = i;
        }
    }

    return index;
}

/* Search for a menu item using its item ID. */
int findMenuIndexByID(const MenuItem menu[], int menuSize, int itemID)
{
    int i;
    int index = -1;

    for (i = 0; i < menuSize && index == -1; i++)
    {
        if (menu[i].itemID == itemID)
        {
            index = i;
        }
    }

    return index;
}

/* Search for a table using its table number. */
int findTableIndexByNumber(const Table tables[], int tableSize, int tableNumber)
{
    int i;
    int index = -1;

    for (i = 0; i < tableSize && index == -1; i++)
    {
        if (tables[i].tableNumber == tableNumber)
        {
            index = i;
        }
    }

    return index;
}

/* Find the first unused order slot in the orders array. */
int findEmptyOrderSlot(const Order orders[], int orderSize)
{
    int i;
    int index = -1;

    for (i = 0; i < orderSize && index == -1; i++)
    {
        if (orders[i].orderID == 0)
        {
            index = i;
        }
    }

    return index;
}

/* Calculate the subtotal by matching ordered item IDs with menu prices. */
float calculateOrderSubtotal(const Order *order, const MenuItem menu[], int menuSize)
{
    float subtotal = 0.0;
    int i;
    int j;

    for (i = 0; i < order->itemCount; i++)
    {
        for (j = 0; j < menuSize; j++)
        {
            if (order->itemIDs[i] == menu[j].itemID)
            {
                subtotal += menu[j].price * order->quantities[i];
            }
        }
    }

    return subtotal;
}

/* Apply a 10 percent large-order discount and optional 5 percent student discount. */
float calculateDiscount(float subtotal, bool studentDiscountApplied)
{
    float discount = 0.0;

    if (subtotal > 50.0)
    {
        discount += 0.10 * subtotal;
    }

    if (studentDiscountApplied == true)
    {
        discount += 0.05 * subtotal;
    }

    return discount;
}

/* Calculate optional 12.5 percent service charge. */
float calculateServiceCharge(float discountedSubtotal, bool applyServiceCharge)
{
    float serviceCharge = 0.0;

    if (applyServiceCharge == true)
    {
        serviceCharge = 0.125 * discountedSubtotal;
    }

    return serviceCharge;
}

/* Calculate the final bill after discount and service charge. */
float calculateFinalTotal(float subtotal, float discount, float serviceCharge)
{
    return subtotal - discount + serviceCharge;
}

/*
    Function: getValidInt
    Purpose:
    Safely receives integer input and rejects wrong types or out-of-range values.
*/
int getValidInt(const char prompt[], int min, int max)
{
    char input[100];
    char extra;
    int value = 0;
    int valid = 0;

    while (valid == 0)
    {
        printf("%s", prompt);
        fgets(input, sizeof(input), stdin);

        if (sscanf(input, " %d %c", &value, &extra) != 1)
        {
            printf("Invalid input. Please enter a whole number.\n");
        }
        else if (value < min || value > max)
        {
            printf("Input out of range. Please enter a number between %d and %d.\n", min, max);
        }
        else
        {
            valid = 1;
        }
    }

    return value;
}

/* Reuses getValidInt to force a 1 for yes or 0 for no answer. */
int getYesNoChoice(const char prompt[])
{
    return getValidInt(prompt, 0, 1);
}

/* Safely gets text input and rejects empty strings. */
void getTextInput(const char prompt[], char text[], int length)
{
    int valid = 0;

    while (valid == 0)
    {
        printf("%s", prompt);
        fgets(text, length, stdin);
        text[strcspn(text, "\n")] = 0;

        if (strlen(text) == 0)
        {
            printf("Input cannot be empty. Please try again.\n");
        }
        else
        {
            valid = 1;
        }
    }
}

/* Gives the user time to read output before returning to the next menu. */
void pauseScreen(void)
{
    char input[10];

    printf("\nPress Enter to continue...");
    fgets(input, sizeof(input), stdin);
}

/* Prints the current date and time on the final bill. */
void printCurrentDateTime(void)
{
    time_t currentTime;
    char timeText[50];

    time(&currentTime);
    strftime(timeText, sizeof(timeText), "%d/%m/%Y %H:%M:%S", localtime(&currentTime));

    printf("Date/Time: %s\n", timeText);
}
