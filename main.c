#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

#pragma GCC optimize ("Ofast","unroll-loops")
#pragma GCC target ("bmi")

typedef struct station station;
typedef struct max_heap max_heap;
typedef struct running_car running_car;
typedef struct reverse_running_car reverse_running_car;

struct station {
    int position;
    max_heap *cars_max_heap;
    station *next_station;
    station *prev_station;
    station *left_child;
    station *right_child;
    station *best_previous_station;
};

struct highway_tree {
    station *root;
} highway_tree;

struct max_heap {
    int *cars;
    u_int16_t size;
    u_int16_t capacity;
};

struct running_car {
    int autonomy;
    station *start_station;
    running_car *nextCar;
};

struct running_car_list {
    running_car *current_best_car;
    running_car *last_car;
    u_int16_t size;
} running_cars_list;

struct reverse_running_car {
    int autonomy;
    u_int8_t needed_changes;
    reverse_running_car *next_car;
    reverse_running_car *prev_car;
    station *start_station;
};

struct reverse_running_car_list {
    reverse_running_car *first;
    reverse_running_car *last;
    reverse_running_car *current_best_car;
    reverse_running_car *next_best_car;
    u_int16_t size;
} rev_running_cars_list;

void full_free_station(station *station){
    free(station->cars_max_heap->cars);
    free(station->cars_max_heap);
    free(station);
}

//read input while
void read_input();

//ignores position chars from stdin
void move_cursor(int position);

//reads an int from stdin
int read_int();

//reads an array of ints from stdin
int *read_int_arr(int size, int capacity);

//returns the next node;
station *delete_successor_duplicate(station *station);

//add station to the highway_tree creating a max_heap using the max_heap utilities
station *add_station(int position, u_int16_t car_count, u_int16_t capacity, int *initial_cars);

//returns a station if the station exists, NULL otherwise.
station *get_station(int position);

//demolishes a station if present using the recursive utils
station* erase_node(station *station);

//removes a node recursively
station* remove_node(station* root, int pos);

//adds a car using the utility max_heap and tree functions
void add_car(int position, int autonomy);

//removes a car using the utility max_heap and tree functions
void remove_car(int position, int autonomy);

//builds a max heap of cars max_heap
max_heap *sort_cars(u_int16_t car_count, u_int16_t capacity, int *initial_cars);

//adds an int variable  to a max heap;
void add_to_max_heap(max_heap *max_heap, int autonomy);

//removes a node to a max heap and fixes the position using heapify and fix max_heap insert
bool remove_from_max_heap(max_heap *heap, int autonomy);

//corrects the max heap positioning of a certain node when creating a max heap or removing a node
void heapify(max_heap* heap, int idx);

//corrects the max heap positioning of a certain node after node insertion
void fix_max_heap_insert(max_heap *heap, int idx);

//addCars to the runningCarsList;
void add_car_to_running_cars_list(int autonomy, station* start_station);



//tree utils -----------------------------------------------------------------------------------------------------

station *add_station(int position, u_int16_t car_count, u_int16_t capacity, int *initial_cars) {
    station *new_station = (station *) malloc(sizeof(station));
    new_station->position = position;
    max_heap* new_max_heap = sort_cars(car_count, capacity, initial_cars);
    new_station->cars_max_heap = new_max_heap;
    new_station->cars_max_heap->cars = initial_cars;
    new_station->cars_max_heap->size = car_count;
    new_station->best_previous_station = NULL;
    if (highway_tree.root == NULL) {
        highway_tree.root = new_station;
        new_station->left_child = NULL;
        new_station->right_child = NULL;
        new_station->next_station = NULL;
        new_station->prev_station = NULL;
        return new_station;
    } else {
        station *currentStation = highway_tree.root;
        while (true) {
            if (currentStation->position > new_station->position) {
                if (currentStation->left_child == NULL) {
                    //adding to the tree
                    currentStation->left_child = new_station;
                    new_station->left_child = NULL;
                    new_station->right_child = NULL;

                    //adding to the list
                    new_station->next_station = currentStation;
                    new_station->prev_station = currentStation->prev_station;
                    currentStation->prev_station = new_station;
                    if (new_station->prev_station != NULL){
                        new_station->prev_station->next_station = new_station;
                    }
                    return new_station;
                } else {
                    currentStation = currentStation->left_child;
                }
            } else if (currentStation->position < new_station->position) {
                if (currentStation->right_child == NULL) {
                    //adding to the tree
                    currentStation->right_child = new_station;

                    //adding to the list
                    new_station->prev_station = currentStation;
                    new_station->next_station = currentStation->next_station;
                    new_station->left_child = NULL;
                    new_station->right_child = NULL;
                    currentStation->next_station = new_station;
                    if (new_station->next_station != NULL)
                        new_station->next_station->prev_station = new_station;
                    return new_station;
                } else {
                    currentStation = currentStation->right_child;
                }
            } else {
                return NULL;
            }
        }
    }
}

station *get_station(int position) {
    struct station *curr_st = highway_tree.root;
    if (curr_st == NULL) return NULL;
    while (true) {
        if (curr_st->position == position) return curr_st;
        else if (curr_st->position > position) {
            if (curr_st->left_child != NULL) {
                curr_st = curr_st->left_child;
            } else {
                return NULL;
            }
        } else if (curr_st->position < position) {
            if (curr_st->right_child != NULL) {
                curr_st = curr_st->right_child;
            } else {
                return NULL;
            }
        }
    }
}

station* remove_node(station* root, int pos){
    station* parent = NULL;
    station* current = root;
    bool is_left_child = false;

    if(root == NULL) return root;

    while(current != NULL){ //node search
        if(current->position == pos) break;
        parent = current;
        if(current->position > pos){
            current = current->left_child;
            is_left_child = true;
        } else {
            current = current->right_child;
            is_left_child = false;
        }
    }

    if(current == NULL){ //no node found
        puts("non demolita");
        return root;
    }

    if(parent == NULL){ //root removal
        highway_tree.root = erase_node(current);
        puts("demolita");
        return highway_tree.root;
    }

    if(is_left_child) { //left child removal
        parent->left_child = erase_node(current);
    } else { //right child removal
        parent->right_child = erase_node(current);
    }

    puts("demolita");
    return root;
}

station* erase_node(station *station) {
    if(station == NULL) return station;

    //fixing list
    if(station->prev_station != NULL) station->prev_station->next_station = station->next_station;
    if(station->next_station != NULL) station->next_station->prev_station= station->prev_station;

    if(station->left_child == NULL && station->right_child == NULL){ //no leaves
        free(station);
        return NULL;
    }
    if(station->left_child != NULL && station->right_child != NULL){ //both leaves
        struct station *successor;
        successor = delete_successor_duplicate(station);
        //copying data
        station->position = successor->position;
        free(station->cars_max_heap->cars);
        free(station->cars_max_heap);
        station->cars_max_heap = successor->cars_max_heap;
        station->prev_station = successor->prev_station;
        station->next_station = successor->next_station;
        if(station->next_station != NULL) station->next_station->prev_station = station;
        if(station->prev_station != NULL) station->prev_station->next_station = station;
        free(successor);
    } else if (station->left_child != NULL){ //only left leave
        station = station->left_child;
        //todo free
    } else { //only right leave
        station = station->right_child;
        //todo free
    }

    return station;
}

station *delete_successor_duplicate(station *station) {
    struct station *parent = station;
    station = station->right_child;
    bool right = station->left_child == NULL;
    while(station->left_child != NULL){
        parent = station;
        station = station->left_child;
    }

    if(right) {
        parent->right_child = station->right_child;
    } else {
        parent->left_child = station->right_child;
    }

    station->right_child = NULL;
    return station;
}

//max heap utils -----------------------------------------------------------------------------------------------------

void add_car(int position, int autonomy) {
    station* st = get_station(position);
    if(get_station(position) == NULL){
        puts("non aggiunta");
        return;
    }
    add_to_max_heap(st->cars_max_heap, autonomy);
    puts("aggiunta");
}

void remove_car(int position, int autonomy) {
    station* st = get_station(position);
    if(get_station(position) == NULL){
        puts("non rottamata");
        return;
    }
    if(remove_from_max_heap(st->cars_max_heap, autonomy)){
        puts("rottamata");
    } else {
        puts("non rottamata");
    }
}

max_heap *sort_cars(u_int16_t car_count, u_int16_t capacity, int* initial_cars){
    max_heap *new_heap = (max_heap*) malloc(sizeof(max_heap));
    new_heap->size = car_count;
    new_heap->capacity=capacity;
    new_heap->cars = initial_cars;
    for(int i = car_count / 2 + 1; i > 0; i--){
        heapify(new_heap, i);
    }
    return new_heap;
}

void add_to_max_heap(max_heap *max_heap, int autonomy) {
    if(max_heap->size == 0 ){
        int* newHeap = (int*) malloc(sizeof(int) * 1);
        max_heap->cars = newHeap;
        max_heap->capacity = 1;
    } else if(max_heap->size == max_heap->capacity){
        int* reallocated_cars = (int*) realloc(max_heap->cars, sizeof(int) * max_heap->capacity * 2);
        max_heap->cars = reallocated_cars;
        max_heap->capacity = max_heap->capacity*2;
    }
    max_heap->size = max_heap->size+1;
    max_heap->cars[max_heap->size-1] = autonomy;
    fix_max_heap_insert(max_heap,max_heap->size);
}

bool remove_from_max_heap(max_heap *heap, int autonomy) {
    for(int i = 0; i < heap->size; i++){
        if(heap->cars[i]==autonomy){
            heap->cars[i] = 0; //avoiding reassignment and non-erasure of the value
            heap->cars[i] = heap->cars[heap->size-1];
            heap->size--;
            //if it is minor then one of his children
            heapify(heap,i+1); //one based
            fix_max_heap_insert(heap,i+1);
            return true;
        }
    }
    return false;
}

void heapify(max_heap* heap, int idx){
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
void fix_max_heap_insert(max_heap *heap, int idx) {
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

void add_car_to_running_cars_list(int autonomy, station* start_station){
    running_car *newCar = (running_car*) malloc(sizeof(running_car));
    newCar->autonomy = autonomy;
    newCar->start_station = start_station;
    newCar->nextCar = NULL;
    if(running_cars_list.current_best_car == NULL){
        running_cars_list.current_best_car = newCar;
        running_cars_list.last_car = newCar;
        running_cars_list.size = 1;
        return;
    } else {
        running_cars_list.last_car->nextCar = newCar;
        running_cars_list.last_car = newCar;
        running_cars_list.size++;
        return;
    }
}

void add_car_to_reverse_running_cars_list(int autonomy, station* start_st){
    reverse_running_car *newCar = (reverse_running_car*) malloc(sizeof(reverse_running_car));
    newCar->autonomy = autonomy;
    newCar->start_station = start_st;
    newCar->next_car = NULL;
    if(rev_running_cars_list.first == NULL){
        newCar->prev_car = NULL;
        newCar->needed_changes = 0;
        rev_running_cars_list.first = newCar;
        rev_running_cars_list.last = newCar;
        rev_running_cars_list.size = 1;
        rev_running_cars_list.current_best_car = newCar;
        rev_running_cars_list.next_best_car = NULL;
        return;
    } else {
        newCar->needed_changes = rev_running_cars_list.first->needed_changes + 1;
        rev_running_cars_list.last->next_car = newCar;
        newCar->prev_car = rev_running_cars_list.last;
        rev_running_cars_list.last = newCar;
        rev_running_cars_list.size++;
        rev_running_cars_list.next_best_car = newCar;
        return;
    }
}

void delete_dead_cars(station* curr_st){
    int initialSize = running_cars_list.size;
    for(int i = 0; i < initialSize; i++){
        if(running_cars_list.current_best_car->start_station->position + running_cars_list.current_best_car->autonomy < curr_st->position){
            running_car *toFree = running_cars_list.current_best_car;
            running_cars_list.current_best_car = running_cars_list.current_best_car->nextCar;
            free(toFree);
            running_cars_list.size--;
        } else {
            break;
        }
    }
}

void delete_dead_cars_rev(station* curr_st){
    int initialSize = rev_running_cars_list.size;
    int minChanges = rev_running_cars_list.first->needed_changes;
    for(int i = 0; i < initialSize; i++){
        if(rev_running_cars_list.first->start_station->position - rev_running_cars_list.first->autonomy > curr_st->position){
            reverse_running_car *toFree = rev_running_cars_list.first;
            if(rev_running_cars_list.first == rev_running_cars_list.last) {
                rev_running_cars_list.last = NULL;
                rev_running_cars_list.first = NULL;
                rev_running_cars_list.current_best_car = NULL;
                rev_running_cars_list.next_best_car = NULL;
            } else {
                rev_running_cars_list.first = rev_running_cars_list.first->next_car;
                rev_running_cars_list.first->prev_car = NULL;
                if(rev_running_cars_list.first->needed_changes > minChanges){
                    rev_running_cars_list.current_best_car = rev_running_cars_list.next_best_car;
                    minChanges = rev_running_cars_list.current_best_car->needed_changes;
                    rev_running_cars_list.next_best_car = NULL;
                }
            }
            free(toFree);
            rev_running_cars_list.size--;
        } else {
            break;
        }
    }
}

void clear_running_cars(){
    while(running_cars_list.current_best_car != NULL){
        running_car *toFree = running_cars_list.current_best_car;
        running_cars_list.current_best_car = running_cars_list.current_best_car->nextCar;
        free(toFree);
    }
    running_cars_list.size = 0;
    running_cars_list.current_best_car = NULL;
    running_cars_list.last_car = NULL;
}

void clear_running_cars_rev(){
    while(rev_running_cars_list.first != NULL){
        reverse_running_car *toFree = rev_running_cars_list.first;
        rev_running_cars_list.first = rev_running_cars_list.first->next_car;
        free(toFree);
    }
    rev_running_cars_list.size = 0;
    rev_running_cars_list.first = NULL;
    rev_running_cars_list.last= NULL;
    rev_running_cars_list.current_best_car = NULL;
    rev_running_cars_list.next_best_car = NULL;
}

reverse_running_car *calculate_best_car_rev(station* currentStation){
    reverse_running_car *bestCar = rev_running_cars_list.current_best_car;
    while(bestCar->start_station->position - bestCar->autonomy > currentStation->position){
        bestCar->prev_car->next_car = bestCar->next_car;
        if(bestCar->next_car != NULL) bestCar->next_car->prev_car = bestCar->prev_car;
        reverse_running_car *toFree = bestCar;
        bestCar = bestCar->prev_car;
        rev_running_cars_list.current_best_car = bestCar;
        if(rev_running_cars_list.last == toFree){
            rev_running_cars_list.last = bestCar;
        }
        free(toFree);
        rev_running_cars_list.size --;
    }
    return bestCar;
}

void print_solution_recur(station *currentNode, int initialPosition, int endPosition) {
    if(currentNode->position == initialPosition) {
        printf("%d ",initialPosition);
        return;
    }
    print_solution_recur(currentNode->best_previous_station, initialPosition, endPosition);
    if(currentNode->position==endPosition) printf("%d",currentNode->position);
    else printf("%d ",currentNode->position);
}

void print_solution(station *endNode, int initialPosition, int endPosition) {
    print_solution_recur(endNode, initialPosition, endPosition);
    puts("");
}

void find_path(int initialPosition, int endPosition) {
    station* firstStation = get_station(initialPosition);
    station* lastStation = get_station(endPosition); //not necessary
    if(firstStation==NULL || lastStation==NULL){
        puts("nessun percorso");
    }
    station *currentStation;
    add_car_to_running_cars_list(firstStation->cars_max_heap->cars[0], firstStation);
    currentStation = firstStation->next_station;
    while(true){
        //impossible path (no station | last station)
        if(currentStation == NULL || currentStation->position > endPosition) {
            puts("nessun percorso");
            break;
        }

        //erasing dead cars
        delete_dead_cars(currentStation);

        //checking if impossible path
        if(running_cars_list.size == 0){
            puts("nessun percorso");
            break;
        }

        //setting best_previous_station
        currentStation->best_previous_station = running_cars_list.current_best_car->start_station;

        //end case
        if(currentStation->position == endPosition){
            print_solution(currentStation, initialPosition, endPosition);
            break;
        }

        //adding new car if it may be better than the best one
        if(running_cars_list.last_car->autonomy + running_cars_list.last_car->start_station->position <
           currentStation->cars_max_heap->cars[0] + currentStation->position){
            add_car_to_running_cars_list(currentStation->cars_max_heap->cars[0], currentStation);
        }

        currentStation = currentStation->next_station;
    }
    clear_running_cars();
}

void find_path_rev(int initialPosition, int endPosition) {
    station* firstStation = get_station(initialPosition);
    station* lastStation = get_station(endPosition); //not necessary
    if(firstStation==NULL || lastStation==NULL){
        puts("nessun percorso\n");
    }
    station *currentStation;
    add_car_to_reverse_running_cars_list(firstStation->cars_max_heap->cars[0], firstStation);
    currentStation = firstStation->prev_station;
    while(true){
        //no end station
        if(currentStation == NULL || currentStation->position < endPosition){
            puts("nessun percorso");
            break;
        }

        //erasing dead cars
        delete_dead_cars_rev(currentStation);

        //impossible path
        if(rev_running_cars_list.size == 0){
            puts("nessun percorso");
            break;
        }

        //calculating best car
        reverse_running_car *bestCar = calculate_best_car_rev(currentStation);

        //updating best prev station
        currentStation->best_previous_station = bestCar->start_station;

        //end
        if(currentStation->position == endPosition){
            print_solution(currentStation, initialPosition, endPosition);
            break;
        }

        //adding new car
        add_car_to_reverse_running_cars_list(currentStation->cars_max_heap->cars[0], currentStation);

        currentStation = currentStation->prev_station;
    }
    clear_running_cars_rev();
}

int main() {
    running_cars_list.current_best_car = NULL;
    running_cars_list.last_car = NULL;
    running_cars_list.size = 0;
    rev_running_cars_list.first = NULL;
    rev_running_cars_list.last = NULL;
    rev_running_cars_list.size = 0;
    rev_running_cars_list.current_best_car = NULL;
    rev_running_cars_list.next_best_car = NULL;
    highway_tree.root = NULL;
    read_input();
}

void read_input(){
    int counter = 0;
    while (true) {
        int new_char;
        new_char = getchar_unlocked();
        counter++;
        if (new_char == EOF) {
            break;
        }
        switch (new_char) {
            case 'a' : {
                move_cursor(8);
                if (((char) getchar_unlocked()) == 's') {
                    //printInfo();
                    move_cursor(8);
                    int position = read_int();
                    unsigned short int carCount = read_int();
                    unsigned short int capacity = (unsigned short int) pow(2,ceil(log2(carCount))) ;
                    capacity = capacity != 0 ? capacity : 1;
                    int *cars = read_int_arr(carCount, capacity);
                    if(add_station(position, carCount, capacity, cars) != NULL)
                        puts("aggiunta");
                    else puts("non aggiunta");
                } else {
                    //printInfo();
                    move_cursor(4);
                    int position = read_int();
                    int autonomy = read_int();
                    add_car(position, autonomy);
                }
                break;
            }
            case 'd' : {
                //printInfo();
                move_cursor(18);
                int position = read_int();
                remove_node(highway_tree.root, position);
                //printInfo();
                break;
            }
            case 'r' : {
                move_cursor(12);
                int position = read_int();
                int autonomy = read_int();
                remove_car(position, autonomy);
                break;
            }
            case 'p' : {
                move_cursor(18);
                int initialPosition = read_int();
                int endPosition = read_int();
                if (initialPosition < endPosition) find_path(initialPosition, endPosition);
                else find_path_rev(initialPosition, endPosition);
                break;
            }
            default: {
                while (getchar_unlocked() != '\n');
            }
        }
    }
    station* currentStation = highway_tree.root;
    while(currentStation->left_child != NULL){
        currentStation = currentStation->left_child;
    }
    while(currentStation->next_station != NULL){
        station* toFree = currentStation;
        currentStation = currentStation->next_station;
        full_free_station(toFree);
    }
}

void move_cursor(int positions) {
    for(int i = 0; i < positions; i++) getchar_unlocked();
}

int read_int() {
    int value = 0;
    int digit;
    while ((digit = getchar_unlocked()) != ' ' && digit != '\n') {
        value = value * 10 + digit - '0';
    }
    return value;
}

int *read_int_arr(int size, int capacity) {
    int *cars = (int*) malloc(sizeof(int) * capacity);
    for (int i = 0; i < size; i++) {
        cars[i] = read_int();
    }
    return cars;
}