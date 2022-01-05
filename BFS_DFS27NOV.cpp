#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>    // time()
#define MAX 600 // use for maximum available constants for array

// use struct to define a State representation
typedef struct State {
	char room; // room label letter   Lift = O   Charging Room = Q     Lobby = P
	int roomNumber;  // room label number   
	int floor; // floor   Ground = 1, First = 2, Second = 3
	int index;  // index;
	bool isClean; // a sign of whether it has been cleaned or not
	bool isVisited; // a bool variable to note if the room has being visited
} State;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//// variables at global scope
int vfront = -1; // next pos in the visited list
int front = 0; // front pos of the queue
int rear = -1; // rear pos of the queue
int stateCount = 0; // amount of States in the waiting list
int searchCost = 0; // a counter to measure the number of search iterations
State queue[MAX] = { 0 };  // This is the FIFO waiting queue - we put all States yet to be examined in here
State visited[MAX] = { 0 };// This is the visited states list - we put all states we have already examined in here
int bfs_or_dfs = 1;        // store users input whether to use BFS or DFS
State goalHoovi; // state used to define the goal for hoovi i.e the charging room
bool use_battery_alert;    // store users input whether to use battery level alert
bool show_status_message;  // store users input whether to show status message about cleaning and paths
int battery_level = 100;   // store current battery level
const int battery_level_critical = 20; // store constant for critical battery level
State currentState;        //current room being processed
int numberOfVertix;        // number of vertices present in the graph
int predecessor[MAX];      //predecessor of each vertex in shortest path
int pathLength[MAX];
int status[MAX];

struct State listRooms[MAX];

int roomCounter = 0;
int floorMap[MAX][MAX];

// function prototypes
void addRoom(char, int, int, int);
void addFloor(int);
void addRoomLinks(int, int);
void linkFloors(int);
void createFloorMap();
void resetVisitState();
void cleanRooms(int);
int getStateIndex(State);
State getState(State);
void printState(State);
void printRoomStatus(State, bool);
void push(State);
State pop();
State peek();
bool isQueueEmpty();
int addToVisited(State);
int equalStates(State, State);
int getRoomByIndex(int);
State getRoomStateByIndex(int);
int hasBeenVisited(State);
void addToFringe(char, int, int, int);
int isGoalState(State);
void generateSuccessorsBFS(State, int);
void generateSuccessorsDFS(State, int);
bool successorsFullScanBFS(State);
bool successorsFullScanDFS(State);
void findPath(State, State);
void modifiedBFSUsingEdges(State, State);
int getMinimumVertex();
void generateSearchPath(State);
void generateRoutineSchedule(State);
void goalTestBfs();
State startHISearchState();
void showHumanInterface();

int main() {
	createFloorMap(); // Map all vertices and Edges to create a complete unweighted graph of the entire floor

	goalHoovi.floor = 1;
	goalHoovi.room = 'Q';
	goalHoovi.roomNumber = 0;
	goalHoovi.isClean = true;
	goalHoovi.index = 0;

	showHumanInterface();
	return 0;
};

//Use to test for the states involved in the assessments
void goalTestBfs() {
	State goal = getState(goalHoovi);// final destination which is the charging room
	State k32 = { 'K', 2, 3, 0, false, false };// the start point in room K3.2
	State lift2 = { 'O', 0, 2, 0, false, false }; //in the lift on the second floor
	State f11 = { 'F', 1, 1, 0, false, false };// the start point in room F1.1

	//Test scenarios 1 when in room K3.2
	modifiedBFSUsingEdges(getState(k32), goal);

	//Test scenarios 1 when in the lift on the second floor
	modifiedBFSUsingEdges(getState(lift2), goal);

	//Test scenarios 1 when in room F1.1
	modifiedBFSUsingEdges(getState(f11), goal);
}

//add room to the listRoom array to form the floor plan using edges and vertex
void addRoom(char room, int roomNo, int floor, int index) {
	State s;
	s.floor = floor;
	s.room = room;
	s.roomNumber = roomNo;
	s.floor = floor;
	s.isVisited = false;
	s.isClean = false;
	s.index = index;

	listRooms[roomCounter] = s; 				// store s
	roomCounter++;					// increase rear index 
}

//use to add room to the provided floor.
void addFloor(int floor) {
	if (floor == 1)
		addRoom('Q', 0, floor, 0); // add the charging room to the floor 

	addRoom('O', 0, floor, roomCounter); // add the Lift to the floor 
	addRoom('P', 0, floor, roomCounter); // add the lobby to the floor 

	for (char r = 'A'; r <= 'N'; r++)// iterate through characters A to N for the office and receptions
	{
		if (r != 'I' && r != 'J') {
			addRoom(r, 1, floor, roomCounter);// add a reception
			addRoom(r, 2, floor, roomCounter);// ass the inner office
		}
	}
}

//this function forms edge between vertices start and end and assign a weight of 1
void addRoomLinks(int start, int end) {
	floorMap[start][end] = 1;
	floorMap[end][start] = 1;
}

// uses to form the vertices for each provided floors
void linkFloors(int floor) {
	int i;
	State lobby;
	State lift;
	State charger;
	State room1;
	State room2;

	for (i = 0; i < roomCounter; i++) {
		if (listRooms[i].floor == 1 && listRooms[i].roomNumber == 0 && listRooms[i].room == 'Q') {
			charger = listRooms[i];
			numberOfVertix++;
		}
		else if (listRooms[i].floor == floor && listRooms[i].roomNumber == 0 && listRooms[i].room == 'P') {
			lobby = listRooms[i];
			if (lift.room != 'Ã') {
				addRoomLinks(lift.index, lobby.index);
				numberOfVertix++;
			}
		}
		else if (listRooms[i].floor == floor && listRooms[i].roomNumber == 0 && listRooms[i].room == 'O') {
			lift = listRooms[i];
			if (floor == 1) {
				addRoomLinks(charger.index, lift.index);
				numberOfVertix++;
			}
		}
		else if (listRooms[i].floor == floor + 1 && listRooms[i].roomNumber == 0 && listRooms[i].room == 'O') {
			State lift1 = listRooms[i];
			addRoomLinks(lift.index, lift1.index);
			numberOfVertix++;
		}
		else if (listRooms[i].floor == floor && listRooms[i].roomNumber == 1) {
			room1 = listRooms[i];
			if (lobby.room != 'Ã')
			{
				addRoomLinks(lobby.index, room1.index);
				numberOfVertix++;
			}
		}
		else if (listRooms[i].floor == floor && listRooms[i].roomNumber == 2) {
			room2 = listRooms[i];
			if (room1.room != 'Ã') {
				addRoomLinks(room1.index, room2.index);
				numberOfVertix++;
			}
		}
	}
}

//use a join all function involved in creating the floor map or graph together
void createFloorMap() {
	numberOfVertix = -1;
	int i, j;
	for (i = 0; i < MAX; i++) // set adjacency {
		for (j = 0; j < MAX; j++) // matrix to 0
			floorMap[i][j] = 0;

	//use the addFloors method to create the floors
	addFloor(1);//add Ground floor to map 
	addFloor(2);//add first floor to map
	addFloor(3);//add second floor to map

	linkFloors(1); // link all rooms in ground floor to create a graph edge
	linkFloors(2);// link all rooms in first floor to create a graph edge
	linkFloors(3);// link all rooms in first floor to create a graph edge
	numberOfVertix = numberOfVertix * 2;
}

//set the visit status of all the State object in the list of rooms to not visited (a Boolean value of false)
void resetVisitState() {
	for (int i = 0; i < roomCounter; i++) {
		listRooms[i].isVisited = false;
	}
}

//randomly set some rooms to clean. this function set the number of room provided in the function to clean. 
//its used to simulate HOOVI sensor for detecting if some rooms are clean and skip the cleaning process
void cleanRooms(int number) {
	time_t t;
	int i;
	/* Intializes random number generator */
	srand((unsigned)time(&t));

	for (i = 0; i < number; i++) {
		int j = rand() % roomCounter;
		listRooms[j].isClean = true;
	}

}

// get the index of a state in the list of rooms 
int getStateIndex(State state) {
	for (int i = 0; i < roomCounter; i++)
		if (listRooms[i].floor == state.floor && listRooms[i].roomNumber == state.roomNumber && listRooms[i].room == state.room)
			return i;
	return -1;
}

// get the state from the list of rooms using a state object
State getState(State state) {
	for (int i = 0; i < roomCounter; i++)
		if (listRooms[i].floor == state.floor && listRooms[i].roomNumber == state.roomNumber && listRooms[i].room == state.room)
			return listRooms[i];
}

//use this function to print human readable output from the State object
void printState(State s) {
	if (s.room == 'Q')
		printf(" Ground Floor Charging Room ");
	else if (s.room == 'P' && s.floor == 1)
		printf(" Ground Floor Lobby ");
	else if (s.room == 'O' && s.floor == 1)
		printf(" Ground Floor Lift ");
	else if (s.room == 'P' && s.floor != 1)
		printf(" %d Floor Lobby ", s.floor - 1);
	else if (s.room == 'O' && s.floor != 1)
		printf(" %d Floor Lift ", s.floor - 1);
	else
		printf(" %c%d.%d ", s.room, s.floor, s.roomNumber);
}

//this function prints human readable output which includes status, paths, or cleaning condition of a room
void printRoomStatus(State s, bool showStatus)
{

	if (s.room >= 'A' && s.room <= 'N')
	{
		printf("\nI am currently in room %c%d.%d", s.room, s.floor, s.roomNumber);
	}
	else if (s.room == 'Q')
	{
		printf("\nI am now in the Charging Room");
	}
	else if (s.room == 'O')
	{
		printf("\nI am currently in Lift.%d", s.floor);
	}
	else if (s.room == 'P')
	{
		printf("\nI am currently in Lobby.%d", s.floor);
	}
	if (showStatus == true) {
		if (show_status_message) {// check if the user has 
			if (s.isClean)
				printf("\n\n*** Room is clean: moving to next room ***\n ");
			else {
				printf("\n\n*** Room is not clean: preparing to clean room ***\n ");
			}
		}
		else {
			printf("\n");
		}
	}
	else {
		printf(", now moving to the next room...\n");
	}
}

// push adds a State to the rear of the queue
void push(State s) {
	rear++;					// increase rear index 
	queue[rear] = s; 				// store s
	stateCount++;				// increase the count of States in the queue
}

// pop retrieves a State from the front of the queue
State pop()
{
	/*if (stateCount > 0) {	*/		// check if there are items in the queue
	if (bfs_or_dfs == 1)
	{
		State s = queue[front];		// get State at front index
		front++;				// increase front index to point at the next State
		stateCount--; 		// decrement State counter
		return s;       			// pass State back to the point of call
	}
	else
	{
		State s = queue[rear];   // if use dfs ,use the stack way
		rear--;
		stateCount--;
		return s;
	}
	/*}*/
}

//takes a look at the item at the top of the stack
State peek() {
	return queue[rear]; // return topmost state to the caller
}

//check if the queue is empty by checking the state of the stateCount variable
bool isQueueEmpty() {
	if (bfs_or_dfs == 1)
		return stateCount == 0;
	else
		return stateCount == -1;

	//return stateCount <= 0 ;
}

// addToVisited takes a state as an argument and adds it to the visited list
// returns the position in the list  at which the state was stored 
// (we need this information for maintaining parent links)
int addToVisited(State s) {
	vfront++;				// increase list index
	visited[vfront] = s;	// add state at index
	s.isVisited = true;
	return vfront;			// return storage index for s
}

// equalStates takes two States as input and compares their internal variables a and b 
// if both a and b values are equal, this function will return 1, otherwise 0;
int equalStates(State s1, State s2) {
	if (s1.room == s2.room && s1.roomNumber == s2.roomNumber && s1.floor == s2.floor)   // DONE: insert your State rep format - DEVISE AN EXPRESSION TO EVALUATE EQUALITY BETWEEN NODES
		return 1;
	else
		return 0;
}

//get index from floor map where state has not been visited using vertices
int getRoomByIndex(int roomIndex) {
	for (int i = 0; i < roomCounter; i++)
		if (floorMap[roomIndex][i] == 1 && listRooms[i].isVisited == false)
			return i;
	return -1;
}

//get State object from floor map where state has not been visited using vertices
State getRoomStateByIndex(int roomIndex) {
	for (int i = 0; i < roomCounter; i++)
		if (floorMap[roomIndex][i] == 1 && listRooms[i].isVisited == false)
			return listRooms[i];
}

// hasBeenVisited takes a state as input and compares it to all states stored in the "visited" list
// returns 1 if the state has been visited before
// returns 0 if the state 
int hasBeenVisited(State s) {
	int i;
	for (i = 0; i < vfront; i++) {
		if (equalStates(visited[i], s)) {
			s.isVisited = true;
			return 1;
		}
	}
	return 0;
}

// addToFringe takes a state as input and checks if this state has not been explored yet 
// If the state was not previously visited, then we recognize the state 
// to be "at the fringe" of its parent and add it to the waiting queue
// otherwise, the function returns to the point of call without doing anything 
void addToFringe(char a, int b, int f, int p) {
	State s = { a,b,f,p };

	if (!hasBeenVisited(s))		// check if s was visited before
		push(s);			// if not, then add to queue
	return;
}

// Takes a state as input and checks if this state is the goal state
// Returns 1 if this is so, and 0 if the state is not the goal
int isGoalState(State s) {
	// DONE: YOUR GOAL TEST HERE
	if (s.room == goalHoovi.room && s.roomNumber == goalHoovi.roomNumber && s.floor == goalHoovi.floor)
		return 1;
	else
		return 0;
}

// Takes a single state and that state's storage index in the visited list as input
// It then generates all valid successor states (children) of this state using BFS
void generateSuccessorsBFS(State s, int p)
{
	if (s.room >= 'A' && s.room <= 'N') {
		if (s.roomNumber == 1)
		{
			addToFringe('P', 0, s.floor, p); // add looby
			addToFringe(s.room, s.roomNumber + 1, s.floor, p);//add reception 
		}
		else
		{
			addToFringe(s.room, s.roomNumber - 1, s.floor, p);//add office
		}
	}

	if (s.room == 'P') {
		addToFringe('O', 0, s.floor, p);  //add lift
		for (char l = 'A'; l <= 'N'; l++)
		{
			addToFringe(l, 1, s.floor, p);
		}

	}

	if (s.room == 'O') {
		if (s.floor == 1)
		{
			addToFringe('Q', 0, s.floor, p); // add charge room
		}
		if (s.floor - 1 >= 1)
		{
			addToFringe('O', 0, s.floor - 1, p); // lift down
		}
		if (s.floor + 1 <= 3)
		{
			addToFringe('O', 0, s.floor + 1, p); // lift up
		}
		addToFringe('P', 0, s.floor, p);  //add lobby
	}

	if (s.room == 'Q')
	{
		addToFringe('O', 0, s.floor, p);
	}
}

// Takes a single state and that state's storage index in the visited list as input
// It then generates all valid successor states (children) of this state using DFS
void generateSuccessorsDFS(State s, int p) {
	if (s.room >= 'A' && s.room <= 'N') {
		if (s.roomNumber == 1)
		{
			addToFringe(s.room, s.roomNumber + 1, s.floor, p);//add reception 
			addToFringe('P', 0, s.floor, p); // add lobby
		}
		else
		{
			addToFringe(s.room, s.roomNumber - 1, s.floor, p);//add office
		}
	}

	if (s.room == 'P') {

		for (char l = 'A'; l <= 'N'; l++)
		{
			addToFringe(l, 1, s.floor, p);
		}
		addToFringe('O', 0, s.floor, p);  //add lift
	}

	if (s.room == 'O') {

		addToFringe('P', 0, s.floor, p);  //add lobby
		if (s.floor + 1 <= 3)
		{
			addToFringe('O', 0, s.floor + 1, p); // lift up
		}
		if (s.floor - 1 >= 1)
		{
			addToFringe('O', 0, s.floor - 1, p); // lift down
		}
		if (s.floor == 1)
		{
			addToFringe('Q', 0, s.floor, p); // add charge room
		}
	}

	if (s.room == 'Q')
	{
		addToFringe('O', 0, s.floor, p);
	}
}

//use Breadth-First-Search to search all rooms and perform routine search and cleaning
bool successorsFullScanBFS(State startingState) {
	bool isLowBattery = false;

	int index = getStateIndex(startingState);//get the index of this particular state from the list of states

	//check if the state is valid using the index and if not exit
	if (index == -1) {
		printf("State doesn't exist on map.");
		return true;
	}

	//mark first node as visited to start moving
	listRooms[index].isVisited = true;

	//output the room details
	printRoomStatus(listRooms[index], true);

	//insert the starting state in queue
	push(startingState);
	int unvisitedVertex;

	//keep checking until the queue is empty
	while (!isQueueEmpty()) {
		if (use_battery_alert)  //use to flag that battery level simulation should occur
		{
			battery_level--; // reduce battery level by 1 for each search HOOVI perform
			if (battery_level <= battery_level_critical)
			{
				isLowBattery = true; //set battery level flag to low battery and stop the search
				break;
			}
		}

		//get the unvisited state which is at front of the queue
		State notVistedState = pop();

		//no adjacent vertex  found
		while ((unvisitedVertex = getRoomByIndex(notVistedState.index)) != -1) {
			/*if (listRooms[unvisitedVertex].isVisited == false) {*/

			currentState = listRooms[unvisitedVertex]; // set the global current state variable to the current search room
			listRooms[unvisitedVertex].isVisited = true;
			printRoomStatus(listRooms[unvisitedVertex], true);
			//HOOVI checks if the room is dirty using its sensors and cleans it if dirty.
			if (listRooms[unvisitedVertex].isClean == false) {
				listRooms[unvisitedVertex].isClean == true;
				battery_level--;// consume more battery to clean the room

				if (show_status_message) {
					printf("\n*** Room is now cleaned. ***\n ");
					if (use_battery_alert)
						printf("\n*** Battery level is now at %d percent ***\n ", battery_level);
				}
			}
			/*printState(listRooms[unvisitedVertex]);*/
			push(listRooms[unvisitedVertex]);
			/*}
			if (unvisitedVertex == descIndex)
				return true;*/
		}
	}

	//if not critical battery status and queue is empty, search is complete, reset the visited flag 
	if (!isLowBattery)
	{
		resetVisitState();// reset the visit state to not visited (false)
		return true; //return true to indicate a successful finished
	}
	return false; //return false to indicate battery was low and HOOVI needs to return to charging room and continue when charged
}

//use Depth-First-Search to search all rooms and perform routine search and cleaning
bool successorsFullScanDFS(State startingState) {
	int i;
	bool isLowBattery = false;
	int index = getStateIndex(startingState);//get the index of this particular state from the list of states

	//check if the state is valid using the index and if not exit
	if (index == -1) {
		printf("State doesn't exist on map."); //
		return true;
	}

	//mark first node as visited to start moving
	listRooms[index].isVisited = true;

	//output the room details
	printRoomStatus(listRooms[index], true);

	//insert the starting state in queue
	push(startingState);

	//keep checking until the queue is empty
	while (!isQueueEmpty()) {
		if (use_battery_alert)  //use to flag that battery level simulation should occur
		{
			battery_level--; // reduce battery level by 1 for each search HOOVI performs
			if (battery_level <= battery_level_critical)
			{
				isLowBattery = true; //set battery level flag to low battery and stop the search
				break;
			}
		}

		//get the unvisited vertex at top of the stack
		int unvisitedVertex = getRoomByIndex(peek().index);

		//no adjacent vertex found
		if (unvisitedVertex == -1) {
			pop();
		}
		else {
			currentState = listRooms[unvisitedVertex]; // set the global current state variable to the current search room
			listRooms[unvisitedVertex].isVisited = true;
			printRoomStatus(listRooms[unvisitedVertex], true);
			//HOOVI check if the room is dirty using it sensors and cleans it if dirty.
			if (listRooms[unvisitedVertex].isClean == false) {
				listRooms[unvisitedVertex].isClean == true;
				battery_level--;// consume more battery to clean the room

				if (show_status_message) {
					printf("\n*** Room is now cleaned. ***\n ");
					if (use_battery_alert)
						printf("\n*** Battery is now %d percent ***\n ", battery_level);
				}
			}
			push(listRooms[unvisitedVertex]);
		}
	}

	//if not critical battery status and queue is empty, search is complete, reset the visited flag 
	if (!isLowBattery)
	{
		resetVisitState();// reset the visit state to not visited (false)
		return true; //return true to indicate a successful finish
	}
	return false; //return false to indicate battery was low and HOOVI needs to return to charging room and continue when charged
}

//use to find the shortest path between two states using modified BFS
void findPath(State sState, State dState)
{
	int i, u;
	int path[MAX];          /*stores the shortest path*/
	int shortdist = 0;      /*length of shortest path*/
	int count = 0;          /*number of vertices in the shortest path*/
	int s = sState.index;
	int d = dState.index;

	/*Store the full path in the array path*/
	while (d != s)
	{
		count++;
		path[count] = d;
		u = predecessor[d];
		shortdist += floorMap[u][d];
		d = u;
	}
	count++;
	path[count] = s;

	for (i = count; i >= 1; i--)
		printRoomStatus(listRooms[path[i]], false);

	printf("\nShortest Path is: ");

	for (i = count; i >= 1; i--) {
		printState(listRooms[path[i]]);
		if (i != 1)
			printf("==>");
	}
	printf("\n\n%d rooms create the shortest path to the Charging Room.\n\n", count);
}

//A modified version of BFS using edges, path and weight
void modifiedBFSUsingEdges(State startState, State destState)
{
	int i, current;
	int s = startState.index;
	/* Make all vertices temporary */
	for (i = 0; i < numberOfVertix; i++)
	{
		predecessor[i] = -1;
		pathLength[i] = 9999;
		status[i] = 0;
	}
	/* Make pathLength of source vertex equal to 0 */
	pathLength[s] = 0;

	while (true)
	{
		/*Search for temporary vertex with minimum pathLength
		and make it current vertex*/
		current = getMinimumVertex();

		if (current == -1)
			break;

		status[current] = 1;

		for (i = 0; i < numberOfVertix; i++)
		{
			/*Checks for adjacent temporary vertices */
			if (floorMap[current][i] != 0 && status[i] == 0)
				if (pathLength[current] + floorMap[current][i] < pathLength[i])
				{
					predecessor[i] = current;  /*Relabel*/
					pathLength[i] = pathLength[current] + floorMap[current][i];
				}
		}
	}

	findPath(startState, destState);
}

/*Returns minimum value of the vertex which edges are connected to*/
int getMinimumVertex()
{
	int i;
	int min = 9999; // set minimum path length to infinity or a high value
	int k = -1; // initialize vertex to -1 for no vertex exists

	//loop through the total number of vertex generated when creating the floor map or Edge
	for (i = 0; i < numberOfVertix; i++)
	{
		if (status[i] == 0 && pathLength[i] < min)
		{
			min = pathLength[i];
			k = i;
		}
	}
	return k; //return vertex found
}

//this function is used to generate a search pattern either for BFS (modifiedBFSUsingEdges) or DFS (generateSuccessorsDFS)
void generateSearchPath(State startState) {

	if (bfs_or_dfs == 1)
	{
		State s = getState(startState);
		State d = getState(goalHoovi);
		modifiedBFSUsingEdges(s, d);
		return;
	}

	State s;
	int parentIndex = 0;
	stateCount = 0;
	push(startState);

	char dest[1000];
	while (stateCount > 0) {
		s = pop();						// get a state from the queue
		parentIndex = addToVisited(s);	// add this state to the visited list and retrieve storage index
		printRoomStatus(s, false);

		// GOAL TEST
		if (isGoalState(s)) {
			// if the current state is the goal, then print the solution
			printf("\nShortest Path is: ");

			for (int i = 0; i < searchCost + 1; i++) {
				printState(visited[i]);
				if (i != searchCost)
					printf("==>");
			}
			printf("\n%d rooms create the shortest path to the Charging Room!\n", searchCost);

			break;
		}
		// if current state s is not the goal, then run successor function
		else {
			generateSuccessorsDFS(s, parentIndex); // use dfs
		}
		// increment search iterations counter
		searchCost++;
	}

}

//this function is used to generate a search pattern for searching all the rooms either using BFS(successorsFullScanBFS) or DFS(successorsFullScanDFS)
void generateRoutineSchedule(State startState) {
	bool searchResult;
	State s = getState(startState); // get the full state from the list of states with its index

	State endState;
	endState.floor = 1;
	endState.room = 'Q';
	endState.roomNumber = 0;

	State d = getState(endState);// get the full state from the list of states with its index

	if (bfs_or_dfs == 1) {
		bool searchResult = successorsFullScanBFS(s); // using BFS full scan 
		if (!searchResult) // if not true; that means the cleaning didnt finish because battery is low
		{
			modifiedBFSUsingEdges(s, d);

			if (show_status_message) {
				printf("\n*** Charging started... ***\n ");
				if (use_battery_alert)
					printf("\n*** Battery is now fully charged ***\n");
			}
			battery_level = 100;

			generateRoutineSchedule(s);// start the routine in a recursive loop
		}
		else {
			printf("\nAll rooms have been searched and cleaned...\n ");
		}
	}
	else {

		bool searchResult = successorsFullScanDFS(s);
		if (!searchResult) {
			modifiedBFSUsingEdges(s, d);

			if (show_status_message) {
				printf("\n*** Charging started... ***\n ");
				if (use_battery_alert)
					printf("\n*** Battery is now fully charged ***\n");
			}
			battery_level = 100;

			generateRoutineSchedule(s);// start the routine in a recursive loop
		}
		else {
			printf("\nAll rooms have been searched and cleaned...\n ");
		}
	}
}

//this function creates a state based on user input from the command window
State startHISearchState()
{
	printf("Choose where you would like to begin, please.\n\nSelect a floor from these options - 1, 2 or 3:\n");
	int floor_number;
	int room_number = 0;
	scanf("%d", &floor_number);
	while (floor_number > 3 || floor_number < 1)
	{
		printf("An unavailable floor was chosen. Please select a floor from these options - 1, 2 or 3:\n");
		scanf("%d", &floor_number);
	}
	printf("Which room spanning from A to N would you like? Otherwise O is for the Lift, P is for the Lobby, and Q is for the Charging Room):\n");
	char room_letter;
	scanf("\n%c", &room_letter);
	while (room_letter > 'Q' || room_letter < 'A')
	{
		printf("An unavailable room was chosen, please select from the given options:\n");
		scanf("%c", &room_letter);
	}
	if (room_letter == 'Q')
	{
		printf("You picked the charging room which is only available on the ground floor. Taking you to the ground floor\n");
		printf("\nI am starting from the Charging Room. The goal is the Charging Room, now searching for the shortest path..\n");
		floor_number = 0;
	}
	else if (room_letter >= 'A' && room_letter <= 'N')
	{
		printf("Please choose if you would like the Reception or Office. Reception - 1, Office - 2:\n");
		scanf("%d", &room_number);
		while (room_number != 1 && room_number != 2)
		{
			printf("An unavailable option has been chosen. Please choose either the Reception or Office. Reception - 1, Office - 2:\n");
			scanf("%d", &room_number);
		}
		printf("\nI shall start from room %c%d.%d. The goal is the Charging Room, now searching for the shortest path..\n", room_letter, floor_number, room_number);
	}
	else
	{
		printf("\nI shall start from room %c.%d. The goal is the Charging Room, now searching for the shortest path..\n", room_letter, floor_number);
	}
	State start_search = { room_letter, room_number, floor_number, 0, false, false };

	return start_search;
}

/*Returns an interactive section between the program and a user.
Basically user input / involvement with the solution*/
void showHumanInterface()
{
	char yesNo;
	int searchPattern;
	int c;
	printf("************************************************************************************\n");
	printf("******Hi! My name is Hoovi, a battery-operated automatic vacuum cleaner robot.******\n");
	printf("************************************************************************************\n");
pionter:
	printf("\nWould you rather use BFS or DFS to search ? Choose 1 for BFS and 2 DFS:  ");
	scanf(" %d", &bfs_or_dfs);

	while (bfs_or_dfs < 1 || bfs_or_dfs > 2)
	{
		printf("Invalid response for search criteria. please enter value of 1 or 2.\n");
		scanf("%d", &bfs_or_dfs);
		goto  pionter;
	}

	if (bfs_or_dfs == 1)
		stateCount = 0;
	else
		stateCount = -1;

	printf("\nSimulate battery notification and alerts? ([Y]es /[N]o):  ");
	scanf(" %c", &yesNo);
	while (yesNo != 'N' && yesNo != 'Y' && yesNo != 'n' && yesNo != 'y')
	{
		printf("Invalid response. please enter value of Y or N.\n");
		scanf(" %c", &yesNo);
	}

	if (yesNo == 'Y' || yesNo == 'y')
		use_battery_alert = true;
	else
		use_battery_alert = false;

	printf("\nEnable status message about activities? ([Y]es /[N]o):  ");
	scanf(" %c", &yesNo);
	while (yesNo != 'N' && yesNo != 'Y' && yesNo != 'n' && yesNo != 'y')
	{
		printf("Invalid response. please enter value of Y or N.\n");
		scanf(" %c", &yesNo);
	}

	if (yesNo == 'Y' || yesNo == 'y')
		show_status_message = true;
	else
		show_status_message = false;

	printf("\nI am exploring the building... be back in a second!\n");
	printf("\nI am back. You can select from any of the two search criteria to begin searching!\n");
	printf("**********************************************************************\n");
	printf("\n     1.  Move HOOVI from the charging room to any room.\n");
	printf("\n     2.  Search and generate cleaning pattern for its routine.\n");
	printf("\n     Choose 1 OR 2 to continue:  ");
	printf("\n     NOTE: TO PICK ANOTHER OPTION AFTER CHOOSING ONE YOU MUST RESTART THE PROGRAM  \n");
	printf("\n**********************************************************************\n");

	scanf("%d", &searchPattern);

	while (searchPattern != 1 && searchPattern != 2)
	{
		//
		//
		printf("Invalid response. please enter value of 1 or 2.\n");
		scanf("%d", &searchPattern);
	//	
	}
	if (searchPattern == 1) {
		State hiState = startHISearchState();
		generateSearchPath(hiState);
	}
	else if (searchPattern == 2) {
		int rooms = 0;
		printf("\nAll rooms are currently dirty. You can start with full search and cleaning of rooms OR set some rooms as clean.\n");
		printf("How many rooms do you want to set as clean? (Enter 0 to skip. Maximum of 50)\n");
		scanf("%d", &rooms);

		while (rooms < 0 || rooms > 50)
		{
		//	
		//	
			printf("Invalid response. please enter value from 0 to 50.\n");
			scanf("%d", &rooms);
		//	
		}
		if (rooms != 0) {
			cleanRooms(rooms);
		}
		generateRoutineSchedule(goalHoovi);
	}
}
