#include "data.c"

bool checkReverseRunningCars();

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
        //checkReverseRunningCars();
        //printf("pre erase\n");
        //printReverseList(currentStation);

        //no end station
        if(currentStation == NULL || currentStation->position < endPosition){
            printf("nessun percorso\n");
            break;
        }

        //erasing dead cars
        //printReverseList(currentStation);
        reverseHeadDeleteDeadCars(currentStation);
        //checkReverseRunningCars();

        //printReverseList(currentStation);

        //impossible path
        if(reverseRunningCarList.size == 0){
            printf("nessun percorso\n");
            break;
        }

        //calculating best car
        //printReverseList(currentStation);
        reverseRunningCar *bestCar = calculateBestReverseRunningCar(currentStation);
        //checkReverseRunningCars();

        //printf("Best car found : pos %d, aut %d, ch %d\n",bestCar->startStation->position, bestCar->autonomy, bestCar->neededChanges);

        //updating best prev station
        currentStation->bestPreviousStation = bestCar->startStation;

        //end
        if(currentStation->position == endPosition){
            printSolution(currentStation,initialPosition,endPosition);
            break;
        }

        //adding new car
        addCarToReverseRunningCarList(currentStation->carsMaxHeap->cars[0],currentStation);
        //checkReverseRunningCars();

        currentStation = currentStation->prevStation;
    }
    clearReverseRunningCarList();
}

bool checkReverseRunningCars() {
    int size = reverseRunningCarList.size;
    //printf("\nCurrent size : %d",size);
    int counter = 0;
    reverseRunningCar *curr = reverseRunningCarList.first;
    for(int i = 0; i < reverseRunningCarList.size; i++){
        if(curr==NULL) {
            //printf("NOPE\n");
            return false;
        }
        counter++;
        //printf("p %d a %d c %d\n",curr->startStation->position,curr->autonomy,curr->neededChanges);
        if(curr->nextCar == NULL && i != reverseRunningCarList.size-1){
            //printf("NOPE\n");
            return false;
        }
        if(counter == size){
            if(curr!=reverseRunningCarList.last){
                //printf("NOPE\n");
                return false;
            }
        }
        curr = curr->nextCar;
    }
    if(counter != size) {
        //printf("NOPE\n");
        return false;
    }
    return true;
}

