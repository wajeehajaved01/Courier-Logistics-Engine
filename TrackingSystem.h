#ifndef TRACKINGSYSTEM_H
#define TRACKINGSYSTEM_H

#include "hashTable.h"
#include "stack.h"
#include "parcel.h"
#include <ctime>

using namespace std;

class TrackingSystem
{
private:
    Stack<string> eventHistory; // Event log (undo / audit)
    HashTable parcelTable;      // Fast parcel lookup

public:
    parcel *getParcel(int id)
    {
        return parcelTable.search(id);
    }

    TrackingSystem(int historySize = 200);

    // Core tracking operations
    void registerParcel(parcel *p);
    void updateParcelStatus(int parcelID, ParcelStatus newStatus);
    void trackParcel(int parcelID);

    // File handling
    void initializeFromLedger(const string &filename);
    void saveTrackingLog(const string &filename);
    void loadTrackingLog(const string &filename);

    void showFullAuditLog();

    string getCurrentTime()
    {
        time_t now = time(0);
        char *dt = ctime(&now);
        string ts(dt);
        if (!ts.empty() && ts.back() == '\n')
            ts.pop_back(); // Remove newline
        return ts;
    }
    parcel *findParcel(int id);


    void logUndo(int parcelID, ParcelStatus revertedTo);
};

parcel *TrackingSystem::findParcel(int id)
{
    // Since you are using a HashTable, we call its search method
    // This is much faster (O(1)) than a Linked List search (O(n))
    return parcelTable.search(id);
}
// Constructor
TrackingSystem::TrackingSystem(int historySize)
    : eventHistory(historySize) {}

/*
Registers parcel into tracking system
*/
void TrackingSystem::registerParcel(parcel *p)
{
    if (p == NULL)
        return;

    parcelTable.insert(p);

    // Enhanced Log Entry with more fields
    string log = string("[") + getCurrentTime() + "] NEW REGISTRATION | " +
                 "ID: " + to_string(p->parcelID) + " | " +
                 "From: " + (p->source) + " | " +
                 "To: " + (p->destination) + " | " +
                 "Weight: " + to_string((int)p->weight) + "kg";

    if (!eventHistory.isFull())
        eventHistory.push(log);
}

void TrackingSystem::updateParcelStatus(int parcelID, ParcelStatus newStatus)
{
    parcel *p = parcelTable.search(parcelID);

    if (!p)
    {

        cout << Red << "[Tracking] Parcel ID " << parcelID << " not found in database!" << Reset << endl;
        return;
    }

    // Update the actual object
    p->updateStatus(newStatus);

    // Create the Audit Log Entry
    string log = "[" + getCurrentTime() + "] ID: " + to_string(parcelID) +
                 " | New Status: " + p->getStatusString(newStatus);

    // Save to Stack (Requirement 4: Audit Log/Undo)
    if (!eventHistory.isFull())
    {
        eventHistory.push(log);
    }

    // Persist to File immediately (Audit Trail)
    ofstream fout("tracking_audit.txt", ios::app);
    fout << log << endl;
    fout.close();
}

void TrackingSystem::trackParcel(int parcelID)
{
    // 1. Search in Hash Table (RAM)
    parcel *p = parcelTable.search(parcelID);

    if (p == NULL)
    {

        initializeFromLedger("parcel.txt");
        p = parcelTable.search(parcelID);
    }
    if (p == NULL)
    {

        cout << Red << "\n[Error] Parcel ID " << parcelID << " not found in active records." << Reset << endl;
        return;

    }

    // 2. Display Current Status
    p->displayInfo();

    // 3. Display Historical Audit Trail from File
    cout << Bold << Cyan << "\n--- Journey History (Audit Log) ---" << Reset << endl;
    ifstream fin("tracking_audit.txt");
    string line;
    bool foundHistory = false;

    while (getline(fin, line))
    {
        // Search for the ID string within the log line
        if (line.find("ID: " + to_string(parcelID)) != string::npos)
        {
            cout << " > " << line << endl;
            foundHistory = true;
        }
    }

    if (!foundHistory)
    {
        cout << Red << " No previous movements recorded for this parcel." << Reset << endl;
    }
    fin.close();
    cout << "-----------------------------------" << endl;
}

void TrackingSystem::logUndo(int parcelID, ParcelStatus revertedTo)
{
    parcel *p = parcelTable.search(parcelID);

    string log = "[" + getCurrentTime() + "] UNDO ACTION | ID: " + to_string(parcelID) +
                 " | Reverted to: " + p->getStatusString(revertedTo);

    // Save to tracking_audit.txt
    ofstream fout("tracking_audit.txt", ios::app);
    if (fout.is_open())
    {
        fout << log << endl;
        fout.close();
    }

    // Optional: Push to history stack for UI view
    if (!eventHistory.isFull())
        eventHistory.push(log);
}

 
// Saves tracking history to .txt file
// In your main function or TrackingSystem constructor

void TrackingSystem::initializeFromLedger(const string &filename)
{
    ifstream fin(filename);
    string line;
    while (getline(fin, line))
    {
        parcel p = parcel::fromFileString(line);
        if (p.parcelID != -1)
        {
            parcelTable.insert(&p);
        }
    }
    fin.close();
}

void TrackingSystem::saveTrackingLog(const string &filename)
{
    ofstream fout(filename.c_str());

    if (!fout.is_open())
    {
        cerr << "[TrackingSystem] Cannot write log file." << endl;
        return;
    }

    Stack<string> tempStack = eventHistory;

    while (!tempStack.isEmpty())
    {
        fout << tempStack.pop() << endl;
    }

    fout.close();
}

/*
    Loads tracking history safely from file
*/
void TrackingSystem::loadTrackingLog(const string &filename)
{
    ifstream fin(filename.c_str());

    if (!fin.is_open())
    {
        cerr << "[TrackingSystem] Log file not found." << endl;
        return;
    }

    string line;
    while (getline(fin, line))
    {
        if (!eventHistory.isFull())
            eventHistory.push(line);
    }

    fin.close();
}

void TrackingSystem::showFullAuditLog()
{
    cout << Yellow << "\n\t\t===== GLOBAL AUDIT LOG (Most Recent First) =====\n\n"
         << Reset;

    Stack<string> temp = eventHistory; // Uses Copy Constructor
    while (!temp.isEmpty())
    {
        cout << temp.pop() << endl;
    }
}
#endif
