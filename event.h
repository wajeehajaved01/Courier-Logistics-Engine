#ifndef EVENT_H
#define EVENT_H
#include<ctime>
#include<string>
#include<iostream>


using namespace std;

class event{
public:
    time_t timestamp;
    string location;
     string action;

    event(string loc,string act){
        timestamp=time(nullptr);
        location=loc;
        action=act;
    } 

    void printEvent(){
        cout<<"\nTime "<<ctime(&timestamp);
        cout<<"Location : "<<location<<endl ;
        cout<<"Action : "<<action<<endl;
    }


};

#endif /* EVENT_H */
