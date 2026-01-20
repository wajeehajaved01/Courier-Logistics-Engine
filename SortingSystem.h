#ifndef SORTINGSYSTEM_H
#define SORTINGSYSTEM_H
#include <iostream>
#include <fstream>
#include <string>
#include "TrackingSystem.h"
#include "parcel.h"
#include "heap.h"
#include "colors.h"

using namespace std;

class SortingSystem
{
private:
    Heap<parcel> priorityHeap;

    parcel allParcels[100];
    int parcelCount = 0;

public:
    SortingSystem(int capacity = 100);

    void addParcel(const parcel &p);

    void displayByCategory(int type, string value = "");
    // This now returns a pointer to the STABLE array, not the moving Heap
    parcel *getLastAddedParcelPtr()
    {
        if (parcelCount > 0)
        {
            return &allParcels[parcelCount - 1];
        }
        return nullptr;
    }

    parcel getNextParcel();
    // Inside SortingSystem.h

    // File handling
    void loadParcelsFromFile(const string &filename);
    void saveParcelsToFile(const string &filename);
};

#endif

SortingSystem::SortingSystem(int capacity) : priorityHeap(capacity) {}

/*
    Adds a parcel safely.
    If heap is full or parcel is invalid,
    system does NOT crash.
*/
void SortingSystem::addParcel(const parcel &p)
{
    if (parcelCount < 100)
    {
        allParcels[parcelCount] = p;

        priorityHeap.insert(allParcels[parcelCount]);

        parcelCount++;

        // 1. Immediate Append to the Permanent Ledger
        ofstream fout("parcel.txt", ios::app);
        if (fout.is_open())
        {
            fout << p.toFileString() << endl;
            fout.close();
        }
    }
}

/*
    Returns next highest priority parcel.
    If empty, returns a SAFE default parcel.
*/
parcel SortingSystem::getNextParcel()
{
    if (priorityHeap.isEmpty())
    {
        cerr << Red << "[SortingSystem] No parcels available." << Reset << endl;
        return parcel(); // safe empty parcel
    }

    return priorityHeap.extractMax();
}

/*
    Loads parcels from .txt file safely.
    Corrupted lines are ignored.
*/

void SortingSystem::loadParcelsFromFile(const string &filename)
{
    parcelCount = 0;
    priorityHeap.clear();
    // parcelTable.clear();   // ← VERY important (if you have clear)

    ifstream fin(filename);
    if (!fin.is_open())
        return;

    string line;
    while (getline(fin, line))
    {
        parcel p = parcel::fromFileString(line);
        if (p.parcelID != -1 && parcelCount < 100)
        {
            allParcels[parcelCount] = p;

            priorityHeap.insert(allParcels[parcelCount]);
            // parcelTable.insert(allParcels[parcelCount]); // 🔥 THIS WAS MISSING

            parcelCount++;
        }
    }
    fin.close();
}

/*
    Saves all parcels to .txt file.
    Heap is emptied intentionally (document this in report).
*/
void SortingSystem::saveParcelsToFile(const string &filename)
{
    // We use ios::trunc (default) to refresh the whole file with current statuses
    ofstream fout(filename.c_str());

    if (!fout.is_open())
    {
        cerr << "Error: Could not update parcel file." << endl;
        return;
    }

    for (int i = 0; i < parcelCount; i++)
    {
        fout << allParcels[i].toFileString() << endl;
    }

    fout.close();
}

void SortingSystem::displayByCategory(int type, string value)
{
         loadParcelsFromFile("parcel.txt");

    cout << Green << "\n--- Filtered Parcels ---" << Reset << endl;
    bool found = false;

    // Pre-convert value to int for types that need it to avoid repeated stoi calls
    int intValue = 0;
    if (type == 1 )
    {
        try
        {
            intValue = stoi(value);
        }
        catch (...)
        {
            cout << Red << "Invalid numeric value entered for filtering." << Reset << endl;
            return;
        }
    }

    for (int i = 0; i < parcelCount; i++)
    {
        bool match = false;

        if (type == 1)
        { // By Delivery Priority
            // Force both sides to be int
            if ((int)allParcels[i].priority == intValue)
                match = true;
        }
        else if (type == 2)
        { // By Weight Category
            if (value == "Heavy" && allParcels[i].weight > 10.0)
                match = true;
            if (value == "Light" && allParcels[i].weight <= 10.0)
                match = true;
        }
        else if (type == 3)
        { // By Destination Name
            // No stoi, no (int) cast. Just compare the strings directly.
            if (allParcels[i].destination == value)
                match = true;
        }

        if (match)
        {
            allParcels[i].displayInfo();
            found = true;
        }
    }

    if (!found)
        cout << Red << "No parcels found for: " << value << Reset << endl;
}