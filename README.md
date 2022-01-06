# BFS_DFS27NOV
C Software project incorporating Breadth First Search and Depth First search on a hoover named Hoovi.


Hoovi's Operation
1. HOOVI normally starts exploring the building by going from the charging room into the lift and then travels to all locations (rooms, lobbies, lift) it can find a way into.
2. HOOVI has sensors to assess whether a room is dirty or clean and will only clean up those rooms perceived as dirty. If a room is clean, it will move straight on to the next room.
3. HOOVI monitors its battery level whilst travelling or cleaning. If the battery level becomes critically low, HOOVI should return to the charging station on the ground floor immediately.
4. Even though HOOVI will generally travel along the same route during cleaning, the functionality described in 2. means that in the event the robot needs to return to the charger, it may be in any room on any floor when it needs to return to the charger room.
5. HOOVI can determine the location in which it currently is and has a basic built-in model of the building (that tells it where it can go from a given room), as illustrated in the floor plans.
6. HOOVI must perform a simulated search from the place it is currently in to find the charging room. It has sufficient memory to remember any rooms it has considered going to during that search.

The Project task comes with a given floor plan.

The task is to write a search program for HOOVI’s hardware platform with which it can find the shortest path (i.e. least number of rooms travelled) from its current location to the charger room. Extract the relevant problem-solving knowledge from the description above and devise a suitable state representation to capture the information about locations. Devise a non-trivial successor function that determines where HOOVI can go from any given place. For testing your program, assume three scenarios
where HOOVI is:
I) in room K3.2. II) in the lift on the second floor. III) in room F1.1
