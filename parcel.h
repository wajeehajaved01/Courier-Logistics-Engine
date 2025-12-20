#ifndef PARCEL_H
#define PARCEL_H
#include<iostream>
#include<string>
#include<vector>
#include"event.h"

using namespace std;

enum ParcelStatus{
   DISPATCHED,
   LOADED,
   IN_TRNSIT,
   DELIVERY_ATTEMPTED,
   RETURNED,
   DELIVERED

};


class parcel{
   
public:

string parcelID;
int priority;
double weight;
string zone;
ParcelStatus status;
vector <event> history;


parcel(string id ,int p ,double w , string z):parcelID(id),priority(p),weight(w),zone(z),status(DISPATCHED){}

void addParcel(string loc, string ac){

   history.emplace_back(loc,ac);

}

void printParcel() {
        cout << "Parcel ID: " << parcelID 
             << ", Priority: " << priority 
             << ", Weight: " << weight 
             << ", Zone: " << zone 
             << ", Status: " << status << endl;

        cout << "History:" ;
        for (auto &e : history) {
            e.printEvent();
        }
        cout << "--------------------------------------------" << endl;
    }

void updateStatus(ParcelStatus newStatus, string location){

status=newStatus;
addParcel(location,"Parcel updated");


}

ParcelStatus getStatus(){
   return status;
}

vector<event>& getHistory(){
   return history;
}

};




#endif /* PARCEL_H */
