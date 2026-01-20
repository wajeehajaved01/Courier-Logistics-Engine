#include <iostream>
#include <cstdlib> // Needed for system("cls")
#include "SortingSystem.h"
#include "TrackingSystem.h"
#include "RoutingSystem.h"
#include "CourierOperationsSystem.h"
#include "colors.h"
#include <limits> // Required for numeric_limits

using namespace std;

void validateInput()
{
    if (cin.fail())
    {
        cin.clear();                                         // Reset error flags
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the bad input
        cout << Red << "Invalid input! Please enter the correct data type: " << Reset;
    }
}

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pressEnterToContinue()
{
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

int main()
{
    SortingSystem sortingSystem;
    sortingSystem.loadParcelsFromFile("parcels.txt");
    TrackingSystem trackingSystem;
    trackingSystem.loadTrackingLog("tracking_audit.txt");
    RoutingSystem routingSystem(10);
    routingSystem.loadRoutesFromFile("routes.txt");
    CourierOperationsSystem courierSystem;

    int choice;

    do
    {
        // clearScreen();
        cout << Bold << Cyan << "\n\t\t============================================" << Reset << endl;
        cout << Bold << Cyan << "\t\t   SWIFTEX INTELLIGENT LOGISTICS SYSTEM" << Reset << endl;
        cout << Bold << Cyan << "\t\t============================================" << Reset << endl;

        cout << Yellow << "\n\t[ MODULE 1: INTELLIGENT SORTING ]" << Reset << endl;
        cout << "\t   1. Register & Add Parcel (Real-time Insertion)" << endl;
        cout << "\t   2. View Sorted Parcels (Priority/Weight/Zone)" << endl;

        cout << Yellow << "\n\t[ MODULE 2: PARCEL ROUTING ]" << Reset << endl;
        cout << "\t   3. Calculate Shortest Delivery Route (Dijkstra)" << endl;
        cout << "\t   4. Report Traffic Block (Dynamic Recalculation)" << endl;

        cout << Yellow << "\n\t[ MODULE 3: TRACKING & HISTORY ]" << Reset << endl;
        cout << "\t   5. Track Current Status (Hash Lookup)" << endl;
        cout << "\t   6. View Full Lifecycle Audit Log (History Stack)" << endl;

        cout << Yellow << "\n\t[ MODULE 4: COURIER OPERATIONS ]" << Reset << endl;
        cout << "\t   7. COURIER OPERATIONS" << endl;

        cout << "\n\t9. Exit\n\n";
        cout << "\t\tEnter choice: ";
        cin >> choice;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << Red << "Invalid input.\n"
                 << Reset;
            continue;
        }
        // DONE register parcel(1)
        if (choice == 1)
        {
            int id, priority;
            double weight;
            string src, dest;

            cout << Blue << "\n--- Enter Parcel Details ---" << Reset << endl;

            // --- STEP 1: UNIQUE ID INPUT ---
            while (true)
            {
                cout << "Enter ID: ";
                if (!(cin >> id))
                {
                    validateInput();
                    continue;
                }

                if (parcel::isDuplicateID(id, "parcel.txt"))
                {
                    cout << Red << "Error: Parcel ID " << id << " already exists! Try again." << Reset << endl;
                }
                else
                {
                    break; // ID is unique, move to next field
                }
            }

            // --- STEP 2: PRIORITY ---
            cout << "Enter Priority (3: Overnight, 2: 2-Day, 1: Normal): ";
            while (!(cin >> priority) || priority < 1 || priority > 3)
            {
                if (cin.fail())
                    validateInput();
                else
                    cout << Red << "Invalid range! Enter 1, 2, or 3: " << Reset;
            }

            // --- STEP 3: WEIGHT ---
            cout << "Enter Weight: ";
            while (!(cin >> weight) || weight <= 0)
            {
                if (cin.fail())
                    validateInput();
                else
                    cout << Red << "Weight must be positive: " << Reset;
            }

            // --- STEP 4: ROUTING ---
            routingSystem.displayAvailableCities();

            // Source validation
            while (true)
            {
                cout << "Enter Source City: ";
                cin >> src;
                if (routingSystem.getCityID(src) != -1)
                    break;
                cout << Red << "We don't deliver from " << src << ". Try again." << Reset << endl;
            }

            // Destination validation
            while (true)
            {
                cout << "Enter Destination City: ";
                cin >> dest;
                if (routingSystem.getCityID(dest) != -1)
                    break;
                cout << Red << "Destination not on map. Try again." << Reset << endl;
            }

            // --- STEP 5-8: PROCESSING ---
            parcel p(id, priority, weight, src, dest);
            sortingSystem.addParcel(p);
            courierSystem.addPickupParcel(p);

            parcel *permanentPtr = sortingSystem.getLastAddedParcelPtr();
            if (permanentPtr != nullptr)
            {
                trackingSystem.registerParcel(permanentPtr);
            }

            sortingSystem.saveParcelsToFile("parcel.txt");
            cout << Green << "Successfully added to system!\n"
                 << Reset;
            pressEnterToContinue();
        }
        // DONE sort parcel(2)
        else if (choice == 2)
        {
            int subChoice;
            do
            {
                // clearScreen();

                cout << Yellow << "\n\t\t===== Sorting Module ===== \n\n"
                     << Reset << endl;
                cout << "\t\t\t1. View by Priority (Overnight/2-Day/Normal)\n";
                cout << "\t\t\t2. View by Weight Category (Heavy/Light)\n";
                cout << "\t\t\t3. View by Destination Zone\n";
                cout << "\t\t\t4. Withdraw Highest Priority Parcel (Real-time)\n";
                cout << "\t\t\t5. Return to home\n";
                cout << "\t\t\tEnter sub-choice: ";
                cin >> subChoice;

                if (subChoice == 1)
                {

                    int prio;
                    cout << "Enter Priority (3:Overnight, 2:2-Day, 1:Normal): ";
                    cin >> prio;
                    sortingSystem.displayByCategory(1, to_string(prio));

                    cout << "\nPress Enter to return...";
                    cin.ignore(); // Clear buffer
                    cin.get();    // Wait for user to see the data
                }
                else if (subChoice == 2)
                {
                    string cat;
                    cout << "Enter Category (Heavy/Light): ";
                    cin >> cat;
                    sortingSystem.displayByCategory(2, cat);
                }
                else if (subChoice == 3)
                {
                    string zone;
                    cout << "Enter destination: ";
                    cin >> zone;
                    sortingSystem.displayByCategory(3, zone);
                }
                else if (subChoice == 4)
                {
                    // Real-time withdrawal logic
                    parcel p = sortingSystem.getNextParcel();
                    if (p.parcelID != -1)
                    {
                        cout << Green << "Withdrawing Parcel ID: " << p.parcelID << Reset << endl;
                        courierSystem.addPickupParcel(p);
                        // After withdrawal, update the file to reflect current heap
                        sortingSystem.saveParcelsToFile("parcel.txt");
                    }
                }

                else if (subChoice == 5)
                {
                    break;
                }

            } while (subChoice != 5);
        }
        // DONE shortest path(3)
        else if (choice == 3)
        {
            int subChoice;
            string startCity, endCity;

            routingSystem.displayAvailableCities();
            cout << "Enter Source City: ";
            cin >> startCity;
            cout << "Enter Destination City: ";
            cin >> endCity;

            int u = routingSystem.getCityID(startCity);
            int v = routingSystem.getCityID(endCity);

            if (u != -1 && v != -1)
            {
                routingSystem.calculateShortestPath(u, v);
                pressEnterToContinue();
            }
            else
            {
                cout << Red << "Invalid city names entered!" << Reset << endl;
            }
        }
        // DONE traffic block(4)
        else if (choice == 4)
        {
            string cityU, cityV;
            routingSystem.displayAvailableCities();
            cout << "Enter the two cities to BLOCK (e.g., Lahore Karachi): ";
            cin >> cityU >> cityV;

            int u = routingSystem.getCityID(cityU);
            int v = routingSystem.getCityID(cityV);
            int pID;
            if (u != -1 && v != -1)
            {

                // FIXED CALL: Now passing all 4 required arguments
                routingSystem.blockRoute(u, v, trackingSystem);
                
            }
            else
            {
                cout << Red << "Invalid cities!" << Reset << endl;
            }
        }
        // DONE track Parcel(5)
        else if (choice == 5)
        {
            int id;
            cout << Bold << Yellow << "\n--- ENTER TRACKING ID ---" << Reset << endl;
            cout << "ID: ";
            if (!(cin >> id))
            {
                validateInput();
            }
            else
            {
                // No need to reload the whole log into memory here;
                // Just search the table and scan the file for the specific ID.
                trackingSystem.trackParcel(id);
                // pressEnterToContinue();
            }
        }
        // DONE  show history(6)
        else if (choice == 6)
        {
            // Call the audit log here!
            trackingSystem.showFullAuditLog();
        }

        // DONE move parcel(7)
        else if (choice == 7)
        {
            int opChoice;
            do
            {
                cout << Bold << Cyan << "\n\t--- COURIER OPERATIONS ENGINE ---" << Reset << endl;
                cout << "\t1. Move: Pickup -> Warehouse" << endl;
                cout << "\t2. Assign to Dispatch (Load into Heap)" << endl;
                cout << "\t3. Move: Dispatch -> Transit (Extract Highest Priority)" << endl;
                cout << "\t4. Process Final Delivery Outcome" << endl; // Updates to DELIVERED/FAILED/RETURNED
                cout << "\t---------------------------------" << endl;
                cout << "\t5. [UNDO] Reverse Last Action" << endl;
                cout << "\t6. Courier Operations Status" << endl;
                cout << "\t7. Back to Main Menu" << endl;
                cout << "\n\tChoice: ";
                cin >> opChoice;

                if (opChoice == 1)
                    courierSystem.moveToWarehouse(trackingSystem);
                else if (opChoice == 2)
                    courierSystem.assignRider();
                else if (opChoice == 3)
                    courierSystem.moveToTransit(trackingSystem);
                else if (opChoice == 4)
                {
                    courierSystem.processFinalDelivery(trackingSystem);
                }
                else if (opChoice == 5)
                    courierSystem.undoLastAction(trackingSystem);

                else if (opChoice == 6)
                {
                    //   system("cls");  // Windows only

                    courierSystem.refreshDashboard();
                    pressEnterToContinue();
                }
                else if (opChoice == 7)
                {
                    break;
                }

            } while (opChoice != 7);
        }

        else if (choice == 9)
        {
            trackingSystem.saveTrackingLog("tracking_audit.txt");
            cout << Yellow << "Exiting system...\n"
                 << Reset;
        }
        else
        {
            cout << Red << "Invalid choice.\n"
                 << Reset;
        }

    } while (choice != 9);

    return 0;
}
