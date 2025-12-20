#ifndef PARCELSORTER_H
#define PARCELSORTER_H
#include"parcel.h"
#include<fstream>
#include<vector>
 
class ParcelCompare {
public:
    bool operator()(parcel* a, parcel* b) {
        return a->priority < b->priority; // higher priority first
    }
};

class Queue {
private:
    struct Node {
        parcel* data;
        Node* next;
    };
    Node* front;
    Node* rear;

public:

    Queue() : front(nullptr), rear(nullptr) {}
    ~Queue() {
        while (!empty()) {
            pop();
        }
    }
    void push(parcel* p) {
        Node* newNode = new Node{p, nullptr};
        if (rear) {
            rear->next = newNode;
            rear = newNode;
        } else {
            front = rear = newNode;
        }
    }
    parcel* pop() {
        if (!front) return nullptr;
        parcel* p = front->data;
        Node* temp = front;
        front = front->next;
        if (!front) rear = nullptr;
        delete temp;
        return p;
    }
    bool empty() {
        return front == nullptr;
    }
    parcel* peek() {
        return front ? front->data : nullptr;
    }
};

// Custom Priority Queue implementation using dynamic array (max-heap based on priority)
class PriorityQueue {
private:
    parcel** heap;
    int capacity;
    int size;
    void swap(parcel*& a, parcel*& b) {
        parcel* temp = a;
        a = b;
        b = temp;
    }
    void heapify(int i) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        if (left < size && heap[left]->priority > heap[largest]->priority) largest = left;
        if (right < size && heap[right]->priority > heap[largest]->priority) largest = right;
        if (largest != i) {
            swap(heap[i], heap[largest]);
            heapify(largest);
        }
    }
public:
    PriorityQueue(int cap = 100) : capacity(cap), size(0) {
        heap = new parcel*[capacity];
    }
    ~PriorityQueue() {
        delete[] heap;
    }
    void push(parcel* p) {
        if (size == capacity) {
            // For simplicity, assume capacity is sufficient; in real code, resize
            return;
        }
        heap[size] = p;
        int i = size;
        size++;
        while (i > 0 && heap[(i - 1) / 2]->priority < heap[i]->priority) {
            swap(heap[i], heap[(i - 1) / 2]);
            i = (i - 1) / 2;
        }
    }
    parcel* top() {
        if (size == 0) return nullptr;
        return heap[0];
    }
    void pop() {
        if (size == 0) return;
        heap[0] = heap[size - 1];
        size--;
        heapify(0);
    }
    bool empty() {
        return size == 0;
    }
    int getsize() const {
    return size;
}

parcel* at(int index) const {
    if (index < 0 || index >= size) return nullptr;
    return heap[index];
}

};

enum WeightCategory {
    LIGHT,
    MEDIUM,
    HEAVY
};


class ParcelSorter {
private:
    PriorityQueue priorityHeap;
    Queue zoneBuckets[4];  // Fixed zones: 0=A, 1=B, 2=C, 3=D
    Queue weightBuckets[3]; // 0=LIGHT, 1=MEDIUM, 2=HEAVY

    int getZoneIndex(const string &zone) {
        if (zone == "ZoneA") return 0;
        if (zone == "ZoneB") return 1;
        if (zone == "ZoneC") return 2;
        if (zone == "ZoneD") return 3;
        return -1;
    }

    int getWeightIndex(double weight) {
    if (weight <= 2.0) return LIGHT;
    if (weight <= 5.0) return MEDIUM;
    return HEAVY;
}


    void logToFile(const string &msg) {
        ofstream file("sorting.txt", ios::app);
        file << msg << endl;
        file.close();
    }

public:
    ParcelSorter() {
        // zoneBuckets is an array, no resize needed
    }

    void insertParcel(parcel* p) {
        priorityHeap.push(p);

        int idx = getZoneIndex(p->zone);
        if (idx != -1) {
            zoneBuckets[idx].push(p);
        }

       int weightIdx = getWeightIndex(p->weight);
       weightBuckets[weightIdx].push(p);


        logToFile("Inserted Parcel : " + p->parcelID + "| Priority : " + to_string(p->priority) + "|Zone : " + p->zone+" | Weight : " + to_string(p->weight) );
    }

    parcel* getNextByPriority() {
    while (!priorityHeap.empty()) {
        parcel* p = priorityHeap.top();
        priorityHeap.pop();

        if (p->status == RETURNED) {
            continue; // lazy deletion
        }

        logToFile("Selected by Priority : " + p->parcelID);
        return p;
    }
        return nullptr;
    }

parcel* getNextFromZone(const string &zone) {
    int idx = getZoneIndex(zone);
    if (idx == -1) return nullptr;

    while (!zoneBuckets[idx].empty()) {
        parcel* p = zoneBuckets[idx].pop();
        if (p->status == RETURNED) {
            continue;
        }
        logToFile("Selected from " + zone + " : " + p->parcelID);
        return p;
    }
    return nullptr;
}


    bool isEmpty() {
        return priorityHeap.empty();
    }

parcel* getNextByWeight(WeightCategory category) {
    while (!weightBuckets[category].empty()) {
        parcel* p = weightBuckets[category].pop();

        // Lazy deletion check
        if (p->status == RETURNED) {
            continue;
        }

        logToFile("Selected by Weight Category : " +
                  to_string(category) +
                  " | Parcel : " + p->parcelID);

        return p;
    }
    return nullptr;
}



bool withdrawParcelByID(const string& id) {
    // Scan heap array (real-time logical withdrawal)
    PriorityQueue pq;
    for (int i = 0; i < priorityHeap.getsize(); i++) {
        parcel* p = priorityHeap.at(i);
        if (p->parcelID == id && p->status != RETURNED) {
            p->status = RETURNED;
            logToFile("Withdrawn Parcel : " + id);
            return true;
        }
    }
    return false;
}

};

#endif /* PARCELSORTER_H */
