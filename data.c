#include <stdlib.h>
#include "structs.c"

struct Station *deleteSuccessorDuplicate(station *station);

//add station to the highwayTree creating a maxHeap using the maxHeap utilities
station *addStation(int position, unsigned short int carCount, unsigned short int capacity, int *initialCars);

//returns the next node;
station *deleteSuccessorDuplicate(station *station);

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

void printTree(station* station, int space);


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

station* getHighwayMin(){
    station* currentNode = highwayTree.root;
    if(currentNode==NULL) return NULL;
    while(currentNode->leftChild!=NULL){
        currentNode = currentNode->leftChild;
    }
    return currentNode;
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
        newCar->neededChanges = 0;
        reverseRunningCarList.first = newCar;
        reverseRunningCarList.last = newCar;
        reverseRunningCarList.size = 1;
        return;
    } else {
        newCar->neededChanges = reverseRunningCarList.first->neededChanges+1;
        reverseRunningCarList.last->nextCar = newCar;
        reverseRunningCarList.last = newCar;
        reverseRunningCarList.size++;
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
    for(int i = 0; i < initialSize; i++){
        if(reverseRunningCarList.first->startStation->position - reverseRunningCarList.first->autonomy > currentStation->position){
           reverseRunningCar *toFree = reverseRunningCarList.first;
            if(reverseRunningCarList.first == reverseRunningCarList.last) {
                //printf("LASTFIRST Erasing pos : %d aut : %d c : %d",reverseRunningCarList.first->startStation->position, reverseRunningCarList.first->autonomy, reverseRunningCarList.first->neededChanges);
                reverseRunningCarList.last = NULL;
                reverseRunningCarList.first = NULL;
            } else {
                //printf("FIRST Erasing pos : %d aut : %d c : %d",reverseRunningCarList.first->startStation->position, reverseRunningCarList.first->autonomy, reverseRunningCarList.first->neededChanges);
                reverseRunningCarList.first = reverseRunningCarList.first->nextCar;
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
    //printf("Size = 0");
    reverseRunningCarList.size = 0;
    reverseRunningCarList.first = NULL;
    reverseRunningCarList.last= NULL;
}

reverseRunningCar *calculateBestReverseRunningCar(station* currentStation){
    reverseRunningCar *bestCar = reverseRunningCarList.first;
    reverseRunningCar *currentCar = bestCar;
    reverseRunningCar *prevCar = NULL;
    int initialSize = reverseRunningCarList.size;
    for(int i = 1; i < initialSize+1; i++){
        if(currentCar==NULL) break;
        if(currentCar->neededChanges > bestCar->neededChanges) break;
        else if(currentCar->startStation->position - currentCar->autonomy > currentStation->position){
            if(prevCar !=NULL) prevCar->nextCar = currentCar->nextCar;
            reverseRunningCar *toFree = currentCar;
            currentCar = currentCar->nextCar;
            //printf("MIDDLE Erasing pos : %d aut : %d c : %d",currentCar->startStation->position, currentCar->autonomy, currentCar->neededChanges);
            reverseRunningCarList.size--;
            if(toFree == reverseRunningCarList.last){
                reverseRunningCarList.last = prevCar;
            }
            free(toFree);
        } else if(currentCar->startStation->position < bestCar->startStation->position){
            //printf("Car at pos %d is better then car at post %d", currentCar->startStation->position, bestCar->startStation->position);
            bestCar = currentCar;
            prevCar = currentCar;
            currentCar = currentCar->nextCar;
        } else if(currentCar == reverseRunningCarList.first){
            //printf("Current car is first in the list %d", currentCar->startStation->position);
            bestCar = currentCar;
            prevCar = currentCar;
            currentCar = currentCar->nextCar;
        }
    }
    return bestCar;
}

//-------------------------- to remove

void printReverseList(station* position){
    if(position == NULL) return;
    int size = reverseRunningCarList.size;
    int counter = 0;
    reverseRunningCar *car = reverseRunningCarList.first;
    printf("printing cars at position %d\n",position->position);
    while(car != NULL){
        counter++;
        printf("st : %d, aut : %d, c : %d\n", car->startStation->position, car->autonomy, car->neededChanges);
        car= car->nextCar;
    }
    if(counter != size) {
        printf("What\n");
    }
    printf("\n");
}

void printList(){
    station * min = getHighwayMin();
    int prev = -1;
    while(min!=NULL){
        //printf("%d ",min->position);
        if(prev > min->position) printf("!!! %d minor than %d",prev, min->position);
        min= min->nextStation;
    }
    printf("\n");
}

void printTree(station* station, int space){
    if(station == NULL) return;
    space += 3;
    printTree(station->rightChild,space);
    for(int i = 0; i < space; i++) printf(" ");
    printf("%d\n",station->position);
    printTree(station->leftChild,space);
}

void printInfo(){
    //printList();
    //printTree(highwayTree.root,0);
}
