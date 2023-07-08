#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
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

//returns the next node;
station *deleteSuccessorDuplicate(station *station);

//add station to the highwayTree creating a maxHeap using the maxHeap utilities
station *addStation(int position, unsigned short int carCount, unsigned short int capacity, int *initialCars);

//returns a station if the station exists, NULL otherwise.
station *getStation(int position);

//demolishes a station if present using the recursive utils
station* eraseNode(station *station);

//removes a node recursively
station* removeNode(station* root, int pos);

//adds a car using the utility maxHeap and tree functions
void addCar(int position, int autonomy);

//removes a car using the utility maxHeap and tree functions
void removeCar(int position, int autonomy);

//builds a max heap of carsMaxHeap
maxHeap *sortCars(unsigned short int carCount, int capacity,int* initialCars);

//adds an int variable  to a max heap;
void addToMaxHeap(maxHeap *maxHeap, int autonomy);

//removes a node to a max heap and fixes the position using heapify and fixMaxHeapInsert
bool removeFromMaxHeap(maxHeap *heap, int autonomy);

//corrects the max heap positioning of a certain node when creating a max heap or removing a node
void heapify(maxHeap* heap, int idx);

//corrects the max heap positioning of a certain node after node insertion
void fixMaxHeapInsert(maxHeap *heap, int idx);

//addCars to the runningCarsList;
void addCarToRunningCarList(int autonomy, station* startStation);



//tree utils -----------------------------------------------------------------------------------------------------

station *addStation(int position, unsigned short int carCount, unsigned short int capacity, int *initialCars) {
    station *newStation = (station *) malloc(sizeof(station));
    newStation->position = position;
    maxHeap* newMaxHeap = sortCars(carCount, capacity, initialCars);
    newStation->carsMaxHeap = newMaxHeap;
    newStation->carsMaxHeap->cars = initialCars;
    newStation->carsMaxHeap->size = carCount;
    newStation->bestPreviousStation = NULL;
    if (highwayTree.root == NULL) {
        highwayTree.root = newStation;
        newStation->leftChild = NULL;
        newStation->rightChild = NULL;
        newStation->nextStation = NULL;
        newStation->prevStation = NULL;
        return newStation;
    } else {
        station *currentStation = highwayTree.root;
        while (true) {
            if (currentStation->position > newStation->position) {
                if (currentStation->leftChild == NULL) {
                    //adding to the tree
                    currentStation->leftChild = newStation;
                    newStation->leftChild = NULL;
                    newStation->rightChild = NULL;

                    //adding to the list
                    newStation->nextStation = currentStation;
                    newStation->prevStation = currentStation->prevStation;
                    currentStation->prevStation = newStation;
                    if (newStation->prevStation != NULL){
                        newStation->prevStation->nextStation = newStation;
                    }
                    return newStation;
                } else {
                    currentStation = currentStation->leftChild;
                }
            } else if (currentStation->position < newStation->position) {
                if (currentStation->rightChild == NULL) {
                    //adding to the tree
                    currentStation->rightChild = newStation;

                    //adding to the list
                    newStation->prevStation = currentStation;
                    newStation->nextStation = currentStation->nextStation;
                    newStation->leftChild = NULL;
                    newStation->rightChild = NULL;
                    currentStation->nextStation = newStation;
                    if (newStation->nextStation != NULL)
                        newStation->nextStation->prevStation = newStation;
                    return newStation;
                } else {
                    currentStation = currentStation->rightChild;
                }
            } else {
                return NULL;
            }
        }
    }
}

station *getStation(int position) {
    struct Station *currentStation = highwayTree.root;
    if (currentStation == NULL) return NULL;
    while (true) {
        if (currentStation->position == position) return currentStation;
        else if (currentStation->position > position) {
            if (currentStation->leftChild != NULL) {
                currentStation = currentStation->leftChild;
            } else {
                return NULL;
            }
        } else if (currentStation->position < position) {
            if (currentStation->rightChild != NULL) {
                currentStation = currentStation->rightChild;
            } else {
                return NULL;
            }
        }
    }
}

station* removeNode(station* root, int pos){
    station* parent = NULL;
    station* current = root;
    bool isLeftChild = false;

    if(root == NULL) return root;

    while(current != NULL){ //node search
        if(current->position == pos) break;
        parent = current;
        if(current->position > pos){
            current = current->leftChild;
            isLeftChild = true;
        } else {
            current = current->rightChild;
            isLeftChild = false;
        }
    }

    if(current == NULL){ //no node found
        printf("non demolita\n");
        return root;
    }

    if(parent == NULL){ //root removal
        highwayTree.root = eraseNode(current);
        printf("demolita\n");
        return highwayTree.root;
    }

    if(isLeftChild) { //left child removal
        parent->leftChild = eraseNode(current);
    } else { //right child removal
        parent->rightChild = eraseNode(current);
    }

    printf("demolita\n");
    return root;
}

station* eraseNode(station *station) {
    if(station == NULL) return station;

    //fixing list
    if(station->prevStation!=NULL) station->prevStation->nextStation = station->nextStation;
    if(station->nextStation!=NULL) station->nextStation->prevStation= station->prevStation;

    if(station->leftChild == NULL && station->rightChild == NULL){ //no leaves
        free(station);
        return NULL;
    }
    if(station->leftChild != NULL && station->rightChild != NULL){ //both leaves
        struct Station *successor;
        successor = deleteSuccessorDuplicate(station);
        //copying data
        station->position = successor->position;
        free(station->carsMaxHeap->cars);
        free(station->carsMaxHeap);
        station->carsMaxHeap = successor->carsMaxHeap;
        station->prevStation = successor->prevStation;
        station->nextStation = successor->nextStation;
        if(station->nextStation!=NULL) station->nextStation->prevStation = station;
        if(station->prevStation!=NULL) station->prevStation->nextStation = station;
        free(successor);
    } else if (station->leftChild != NULL){ //only left leave
        station = station->leftChild;
        //todo free
    } else { //only right leave
        station = station->rightChild;
        //todo free
    }

    return station;
}

station *deleteSuccessorDuplicate(station *station) {
    struct Station *parent = station;
    station = station->rightChild;
    bool right = station->leftChild == NULL;
    while(station->leftChild != NULL){
        parent = station;
        station = station->leftChild;
    }

    if(right) {
        parent->rightChild = station->rightChild;
    } else {
        parent->leftChild = station->rightChild;
    }

    station->rightChild = NULL;
    return station;
}

//max heap utils -----------------------------------------------------------------------------------------------------

void addCar(int position, int autonomy) {
    station* st = getStation(position);
    if(getStation(position) == NULL){
        printf("non aggiunta\n");
        return;
    }
    addToMaxHeap(st->carsMaxHeap, autonomy);
    printf("aggiunta\n");
}

void removeCar(int position, int autonomy) {
    station* st = getStation(position);
    if(getStation(position) == NULL){
        printf("non rottamata\n");
        return;
    }
    if(removeFromMaxHeap(st->carsMaxHeap, autonomy)){
        printf("rottamata\n");
    } else {
        printf("non rottamata\n");
    }
}

maxHeap *sortCars(unsigned short int carCount, int capacity,int* initialCars){
    maxHeap *newHeap = (maxHeap*) malloc(sizeof(maxHeap));
    newHeap->size = carCount;
    newHeap->capacity=capacity;
    newHeap->cars = initialCars;
    for(int i = carCount/2+1; i > 0; i--){
        heapify(newHeap, i);
    }
    return newHeap;
}

void addToMaxHeap(maxHeap *maxHeap, int autonomy) {
    if(maxHeap->size == 0 ){
        int* newHeap = (int*) malloc(sizeof(int) * 1);
        maxHeap->cars = newHeap;
        maxHeap->capacity = 1;
    } else if(maxHeap->size == maxHeap->capacity){
        int* reallocatedCars = (int*) realloc(maxHeap->cars,sizeof(int) * maxHeap->capacity*2);
        maxHeap->cars = reallocatedCars;
        maxHeap->capacity = maxHeap->capacity*2;
    }
    maxHeap->size = maxHeap->size+1;
    maxHeap->cars[maxHeap->size-1] = autonomy;
    fixMaxHeapInsert(maxHeap,maxHeap->size);
}

bool removeFromMaxHeap(maxHeap *heap, int autonomy) {
    for(int i = 0; i < heap->size; i++){
        if(heap->cars[i]==autonomy){
            heap->cars[i] = 0; //avoiding reassignment and non-erasure of the value
            heap->cars[i] = heap->cars[heap->size-1];
            heap->size--;
            //if it is minor then one of his children
            heapify(heap,i+1); //one based
            fixMaxHeapInsert(heap,i+1);
            return true;
        }
    }
    return false;
}

void heapify(maxHeap* heap, int idx){
    int left = idx*2;
    int right = idx*2+1;
    int max = idx;
    if(left <= heap->size && heap->cars[left-1] > heap->cars[max-1]){
        max = left;
    }
    if(right <= heap->size && heap->cars[right-1] > heap->cars[max-1]){
        max = right;
    }
    if(max!=idx){
        int maxValue = heap->cars[max-1];
        heap->cars[max-1] = heap->cars[idx-1];
        heap->cars[idx-1] = maxValue;
        heapify(heap, max);
        return;
    }
}

//one based
void fixMaxHeapInsert(maxHeap *heap, int idx) {
    if(idx==1) return;
    while(heap->cars[idx/2-1]< heap->cars[idx-1]){
        int temp = heap->cars[idx/2-1];
        heap->cars[idx/2-1] = heap->cars[idx-1];
        heap->cars[idx-1] = temp;
        idx = idx/2;
        if(idx==1) return;
    }
}

//running cars list utils ---------------------------------------------------------------------------------

void addCarToRunningCarList(int autonomy, station* startStation){
    runningCar *newCar = (runningCar*) malloc(sizeof(runningCar));
    newCar->autonomy = autonomy;
    newCar->startStation = startStation;
    newCar->nextCar = NULL;
    if(runningCarList.currentBestCar==NULL){
        runningCarList.currentBestCar = newCar;
        runningCarList.lastCar = newCar;
        runningCarList.size = 1;
        return;
    } else {
        runningCarList.lastCar->nextCar = newCar;
        runningCarList.lastCar = newCar;
        runningCarList.size++;
        return;
    }
}

void addCarToReverseRunningCarList(int autonomy, station* startStation){
    reverseRunningCar *newCar = (reverseRunningCar*) malloc(sizeof(reverseRunningCar));
    newCar->autonomy = autonomy;
    newCar->startStation = startStation;
    newCar->nextCar = NULL;
    if(reverseRunningCarList.first==NULL){
        newCar->prevCar = NULL;
        newCar->neededChanges = 0;
        reverseRunningCarList.first = newCar;
        reverseRunningCarList.last = newCar;
        reverseRunningCarList.size = 1;
        reverseRunningCarList.currentBestCar = newCar;
        reverseRunningCarList.nextBestCar = NULL;
        return;
    } else {
        newCar->neededChanges = reverseRunningCarList.first->neededChanges+1;
        reverseRunningCarList.last->nextCar = newCar;
        newCar->prevCar = reverseRunningCarList.last;
        reverseRunningCarList.last = newCar;
        reverseRunningCarList.size++;
        reverseRunningCarList.nextBestCar = newCar;
        return;
    }
}

void deleteDeadCars(station* currentStation){
    int initialSize = runningCarList.size;
    for(int i = 0; i < initialSize; i++){
        if(runningCarList.currentBestCar->startStation->position + runningCarList.currentBestCar->autonomy < currentStation->position){
            runningCar *toFree = runningCarList.currentBestCar;
            runningCarList.currentBestCar = runningCarList.currentBestCar->nextCar;
            free(toFree);
            runningCarList.size--;
        } else {
            break;
        }
    }
}

void reverseHeadDeleteDeadCars(station* currentStation){
    int initialSize = reverseRunningCarList.size;
    int minChanges = reverseRunningCarList.first->neededChanges;
    for(int i = 0; i < initialSize; i++){
        if(reverseRunningCarList.first->startStation->position - reverseRunningCarList.first->autonomy > currentStation->position){
            reverseRunningCar *toFree = reverseRunningCarList.first;
            if(reverseRunningCarList.first == reverseRunningCarList.last) {
                reverseRunningCarList.last = NULL;
                reverseRunningCarList.first = NULL;
                reverseRunningCarList.currentBestCar = NULL;
                reverseRunningCarList.nextBestCar = NULL;
            } else {
                reverseRunningCarList.first = reverseRunningCarList.first->nextCar;
                reverseRunningCarList.first->prevCar = NULL;
                if(reverseRunningCarList.first->neededChanges > minChanges){
                    reverseRunningCarList.currentBestCar = reverseRunningCarList.nextBestCar;
                    minChanges = reverseRunningCarList.currentBestCar->neededChanges;
                    reverseRunningCarList.nextBestCar = NULL;
                }
            }
            free(toFree);
            reverseRunningCarList.size--;
        } else {
            break;
        }
    }
}

void clearRunningCarList(){
    while(runningCarList.currentBestCar!=NULL){
        runningCar *toFree = runningCarList.currentBestCar;
        runningCarList.currentBestCar = runningCarList.currentBestCar->nextCar;
        free(toFree);
    }
    runningCarList.size = 0;
    runningCarList.currentBestCar = NULL;
    runningCarList.lastCar = NULL;
}

void clearReverseRunningCarList(){
    while(reverseRunningCarList.first!=NULL){
        reverseRunningCar *toFree = reverseRunningCarList.first;
        reverseRunningCarList.first = reverseRunningCarList.first->nextCar;
        free(toFree);
    }
    reverseRunningCarList.size = 0;
    reverseRunningCarList.first = NULL;
    reverseRunningCarList.last= NULL;
    reverseRunningCarList.currentBestCar = NULL;
    reverseRunningCarList.nextBestCar = NULL;
}

reverseRunningCar *calculateBestReverseRunningCar(station* currentStation){
    reverseRunningCar *bestCar = reverseRunningCarList.currentBestCar;
    while(bestCar->startStation->position - bestCar->autonomy > currentStation->position){
        bestCar->prevCar->nextCar = bestCar->nextCar;
        if(bestCar->nextCar != NULL) bestCar->nextCar->prevCar = bestCar->prevCar;
        reverseRunningCar *toFree = bestCar;
        bestCar = bestCar->prevCar;
        reverseRunningCarList.currentBestCar = bestCar;
        if(reverseRunningCarList.last == toFree){
            reverseRunningCarList.last = bestCar;
        }
        free(toFree);
        reverseRunningCarList.size --;
    }
    return bestCar;
}

//----------------------------------------------------------------------------------------------------------------------


void recursivePrintSolution(station *currentNode, int initialPosition, int endPosition) {
    if(currentNode->position == initialPosition) {
        printf("%d ",initialPosition);
        return;
    }
    recursivePrintSolution(currentNode->bestPreviousStation,initialPosition, endPosition);
    if(currentNode->position==endPosition) printf("%d",currentNode->position);
    else printf("%d ",currentNode->position);
}

void printSolution(station *endNode, int initialPosition, int endPosition) {
    recursivePrintSolution(endNode, initialPosition, endPosition);
    printf("\n");
}

void findPath(int initialPosition, int endPosition) {
    station* firstStation = getStation(initialPosition);
    station* lastStation = getStation(endPosition); //not necessary
    if(firstStation==NULL || lastStation==NULL){
        printf("nessun percorso\n");
    }
    station *currentStation;
    addCarToRunningCarList(firstStation->carsMaxHeap->cars[0],firstStation);
    currentStation = firstStation->nextStation;
    while(true){
        //impossible path (no station | last station)
        if(currentStation == NULL || currentStation->position > endPosition) {
            printf("nessun percorso\n");
            break;
        }

        //erasing dead cars
        deleteDeadCars(currentStation);

        //checking if impossible path
        if(runningCarList.size==0){
            printf("nessun percorso\n");
            break;
        }

        //setting bestPreviousStation
        currentStation->bestPreviousStation = runningCarList.currentBestCar->startStation;

        //end case
        if(currentStation->position == endPosition){
            printSolution(currentStation,initialPosition,endPosition);
            break;
        }

        //adding new car if it may be better than the best one
        if(runningCarList.lastCar->autonomy + runningCarList.lastCar->startStation->position <
           currentStation->carsMaxHeap->cars[0] + currentStation->position){
            addCarToRunningCarList(currentStation->carsMaxHeap->cars[0],currentStation);
        }

        currentStation = currentStation->nextStation;
    }
    clearRunningCarList();
}

void reverseFindPath(int initialPosition, int endPosition) {
    station* firstStation = getStation(initialPosition);
    station* lastStation = getStation(endPosition); //not necessary
    if(firstStation==NULL || lastStation==NULL){
        printf("nessun percorso\n");
    }
    station *currentStation;
    addCarToReverseRunningCarList(firstStation->carsMaxHeap->cars[0],firstStation);
    currentStation = firstStation->prevStation;
    while(true){
        //no end station
        if(currentStation == NULL || currentStation->position < endPosition){
            printf("nessun percorso\n");
            break;
        }

        //erasing dead cars
        reverseHeadDeleteDeadCars(currentStation);

        //impossible path
        if(reverseRunningCarList.size == 0){
            printf("nessun percorso\n");
            break;
        }

        //calculating best car
        reverseRunningCar *bestCar = calculateBestReverseRunningCar(currentStation);

        //updating best prev station
        currentStation->bestPreviousStation = bestCar->startStation;

        //end
        if(currentStation->position == endPosition){
            printSolution(currentStation,initialPosition,endPosition);
            break;
        }

        //adding new car
        addCarToReverseRunningCarList(currentStation->carsMaxHeap->cars[0],currentStation);

        currentStation = currentStation->prevStation;
    }
    clearReverseRunningCarList();
}


//----------------------------------------------------------------------------------------------------------------------

int main();

void readInput();

void moveCursor(int position);

int readInt();

int *readIntArray(int size, int capacity);

bool checkList();


char* concat(const char* a, const char* b, const char* c){
    return strcat(strcat(strcpy(malloc(strlen(a) + strlen(b) + strlen(c) +  1),a), b), c);
}

int main() {
    /*
    int firstTest = 1;
    int lastTest = 111;
    for(int i = firstTest; i <= lastTest; i++){
        char path[40] = "/home/phil/CLionProjects/api/test/open_\0";
        char txt[5] = ".txt\0";
        char outTxt[17] = ".output.mine.txt\0";
        int testNumber = i;
        int testNumberSize = floor(log10(testNumber)) + 1;
        char number[4];
        number[testNumberSize] = '\0';
        for(int j = testNumberSize-1; j>=0; j--){
            number[j] = (char) (testNumber%10+'0');
            testNumber = testNumber/10;
        }
        char* in = concat(path,number,txt);
        char* out = concat(path,number,outTxt);

        fclose(stdin);
        stdin = fopen(in,"r");

        fclose(stdout);
        stdout = fopen(out,"w");

        runningCarList.currentBestCar = NULL;
        runningCarList.lastCar = NULL;
        runningCarList.size = 0;
        reverseRunningCarList.first = NULL;
        reverseRunningCarList.last = NULL;
        reverseRunningCarList.size = 0;
        reverseRunningCarList.currentBestCar = NULL;
        reverseRunningCarList.nextBestCar = NULL;
        highwayTree.root = NULL;

        readInput();
    }
     */
    runningCarList.currentBestCar = NULL;
    runningCarList.lastCar = NULL;
    runningCarList.size = 0;
    reverseRunningCarList.first = NULL;
    reverseRunningCarList.last = NULL;
    reverseRunningCarList.size = 0;
    reverseRunningCarList.currentBestCar = NULL;
    reverseRunningCarList.nextBestCar = NULL;
    highwayTree.root = NULL;

    readInput();
}

void readInput(){
    int counter = 0;
    while (true) {
        int new_char;
        new_char = getchar();
        counter++;
        if (new_char == EOF) {
            break;
        }
        switch (new_char) {
            case 'a' : {
                moveCursor(8);
                if (((char) getchar()) == 's') {
                    //printInfo();
                    moveCursor(8);
                    int position = readInt();
                    unsigned short int carCount = readInt();
                    unsigned short int capacity = (unsigned short int) pow(2,ceil(log2(carCount))) ;
                    capacity = capacity != 0 ? capacity : 1;
                    int *cars = readIntArray(carCount,capacity);
                    if(addStation(position, carCount, capacity, cars)!=NULL)
                        printf("aggiunta\n");
                    else printf("non aggiunta\n");
                } else {
                    //printInfo();
                    moveCursor(4);
                    int position = readInt();
                    int autonomy = readInt();
                    addCar(position, autonomy);
                }
                break;
            }
            case 'd' : {
                //printInfo();
                moveCursor(18);
                int position = readInt();
                removeNode(highwayTree.root,position);
                //printInfo();
                break;
            }
            case 'r' : {
                moveCursor(12);
                int position = readInt();
                int autonomy = readInt();
                removeCar(position, autonomy);
                break;
            }
            case 'p' : {
                moveCursor(18);
                int initialPosition = readInt();
                int endPosition = readInt();
                if (initialPosition < endPosition) findPath(initialPosition, endPosition);
                else reverseFindPath(initialPosition, endPosition);
                break;
            }
            default: {
                while (getchar() != '\n');
            }
        }
    }
    station* currentStation = highwayTree.root;
    while(currentStation->leftChild!=NULL){
        currentStation = currentStation->leftChild;
    }
    while(currentStation->nextStation!=NULL){
        station* toFree = currentStation;
        currentStation = currentStation->nextStation;
        fullFreeStation(toFree);
    }
}

void moveCursor(int positions) {
    for(int i = 0; i < positions; i++) getchar();
}

int readInt() {
    int value = 0;
    int digit;
    while ((digit = getchar()) != ' ' && digit != '\n') {
        value = value * 10 + digit - '0';
    }
    return value;
}

int *readIntArray(int size, int capacity) {
    int *cars = (int*) malloc(sizeof(int) * capacity);
    for (int i = 0; i < size; i++) {
        cars[i] = readInt();
    }
    return cars;
}