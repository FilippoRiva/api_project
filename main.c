#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "pathfind.c"

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
    int firstTest = 111;
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
        //if(!checkList()){
        //printf("line = %d",counter);
        //};
    }
    /*
    while(currentStation->nextStation!=NULL){
        station* toFree = currentStation;
        currentStation = currentStation->nextStation;
        fullFreeStation(toFree);
    }
     */
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

