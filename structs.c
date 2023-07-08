typedef struct Station station;
typedef struct MaxHeap maxHeap;
typedef struct RunningCar runningCar;
typedef struct ReverseRunningCar reverseRunningCar;

struct Station {
    int position;
    maxHeap *carsMaxHeap;
    station *nextStation;
    station *prevStation;
    station *leftChild;
    station *rightChild;
    station *bestPreviousStation;
};

struct HighwayTree {
    station *root;
} highwayTree;

struct MaxHeap {
    int *cars;
    int size;
    int capacity;
};

struct RunningCar {
    int autonomy;
    station *startStation;
    runningCar *nextCar;
};

struct RunningCarList {
    runningCar *currentBestCar;
    runningCar *lastCar;
    int size;
} runningCarList;

struct ReverseRunningCar {
    int autonomy;
    int neededChanges;
    reverseRunningCar *nextCar;
    reverseRunningCar *prevCar;
    station *startStation;
};

struct ReverseRunningCarList {
    reverseRunningCar *first;
    reverseRunningCar *last;
    reverseRunningCar *currentBestCar;
    reverseRunningCar *nextBestCar;
    int size;
} reverseRunningCarList;

void fullFreeStation(station *station){
    free(station->carsMaxHeap->cars);
    free(station->carsMaxHeap);
    free(station);
}
