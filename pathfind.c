#include "data.c"

void printList(station* currentStation);

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
        printList(currentStation);

        //impossible path
        if(reverseRunningCarList.size == 0){
            printf("nessun percorso\n");
            break;
        }

        //calculating best car
        reverseRunningCar *bestCar = calculateBestReverseRunningCar(currentStation);
        printList(currentStation);

        //updating best prev station
        currentStation->bestPreviousStation = bestCar->startStation;

        //end
        if(currentStation->position == endPosition){
            printSolution(currentStation,initialPosition,endPosition);
            break;
        }

        //adding new car
        addCarToReverseRunningCarList(currentStation->carsMaxHeap->cars[0],currentStation);
        printList(currentStation);

        currentStation = currentStation->prevStation;
    }
    clearReverseRunningCarList();
}

void printList(station* currentStation) {
    if(!DEBUG) return;
    int counter = 0;
    reverseRunningCar *current = reverseRunningCarList.first;
    while(current!=NULL){
        printf("pos : %d aut : %d c : %d\n",current->startStation->position, current->autonomy, current->neededChanges);
        counter ++;
        if(counter == reverseRunningCarList.size){
            if(current != reverseRunningCarList.last){
                printf("current station : %d\n", currentStation->position);
                exit(-1);
            }
        }
        current = current->nextCar;
        if(counter> reverseRunningCarList.size) {
            printf("current station : %d\n", currentStation->position);
            exit(-1);
        }
    }
    if(counter!=reverseRunningCarList.size){
        printf("current station : %d\n", currentStation->position);
        exit(-1);
    }
}
