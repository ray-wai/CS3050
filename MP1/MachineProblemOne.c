//Raymond Waidmann
//18157816
//rcw5k2

//Greg Ryterski
//18186949
//gjr7dz

//CS3050 Machine Problem 1
//September 22, 2020

/*How team members worked together:
	We both live in the same house, so we worked together in the same room on the assignment. It is not necessary to detail the breakdown of responsibilites
	as nearly the entire assignment was a joint and collaborative effort. 
*/

/*Comment on algorithm efficiency:
	The total running time is proportional to O(V+E)
	See Comments in the main program that detail further detail the running times of specific sections of code
	Total time complexity: O(1)+O(E)+O(V)+O(Elog(E))+O(V+E) = O(V+Elog(E))
		The only reason this is not O(V+E) is because of the while loop in line 145. As stated in the insertList function comment, we are unsure
		how to build the ajacency list in increasing order without ever having to compare vertices, considering the input file can contain the edges 
		in ANY arbitrary order...
*/

#include <stdio.h>
#include <stdlib.h>
#define FILE_SIZE 100

typedef struct nodestruct {    
	int item;    
	struct nodestruct *next;
	} Node;
	
int insertList(Node**, int, int);
void printList(Node**, int);
void freeList(Node**, int*, int);

int main (void)
{
	//Prompting user for input file name and opening that file.
	//Error checking to ensure file was opened properly
	//Efficiency: O(1)
	char filename[FILE_SIZE];
	printf("\nEnter file name including the dot and file type extension, then press enter: ");
	scanf("%s", filename);
	FILE *myfile;
	myfile = fopen(filename, "r"); 
	if (myfile == NULL) 
	{
		printf("\n\n*****COULD NOT OPEN FILE!!!*****\n\n");
		return (0);
	}
	
	//Iterating through the input file and determining the number of lines (# lines = # edges) by incrementing everytime a newline is discovered
	//Error chcking to ensure the input file is not empty
	//Efficiency: O(E); the for loop running time is proportional to the size of the input file, which we know is proportional to the number of edges
	int E = 0; //Edges
	char c;
	for (c = getc(myfile); c != EOF; c = getc(myfile)) if ( c == '\n') E = E + 1;
	rewind(myfile); //putting the myfile pointer back to the beginning of the file
	if (E == 0) 
	{
		printf("\n\nINPUT FILE DOES NOT CONTAIN ANY EDGES\n\n");
		return(0);
	}
	
	//Dynamically allocating an array to hold each entry of the input file
	//Reading in the integers from the file into that array
	//Determining the Maximum number in the input file (max # = # vertices)
	//Efficiency: O(E); the for loops running times are proportional to E, which we know is proportional to the number of edges
    int * edgeArray = malloc(sizeof(int)*(2*E));
	if (edgeArray == NULL) //unsuccessful malloc
	{
		printf("\n***Failed Malloc Initialization (1)***\n"); 
		return(0);
	}
    for (int i = 0; i < (2*E); i++) fscanf(myfile, "%d", &edgeArray[i]);
	int V = 0; //vertices
	for (int i = 0; i < (2*E); i++) if (edgeArray[i] > V) V = edgeArray[i];
	
	//Dynamically allocating an array of pointers to Nodes to hold the ajacency list for each vertice
	//Dynamically allocating the first node in in each ajacency list as a dummy node
	//Freeing all previously allocated memory if malloc fails
	//Efficiency: O(V); the for loop running time for dummy node allocation is proportional to the max number in the input file (ie. the # of vertices)
	Node ** adjList = malloc((sizeof(Node*))*V);
	if (adjList == NULL) 
	{
		printf("\n***Failed Malloc Initialization (2)***\n"); //unsuccessful malloc
		free(edgeArray);
		return(0);
	}
	for (int i = 0; i < V; i++) //dummy node allocation
	{
		adjList[i] = malloc(sizeof(Node));
		if (adjList[i] == NULL) //unsuccessful malloc
		{
			printf("\n***Failed Malloc Initialization (3)***\n"); 
			free(edgeArray);
			for (int j = 0; j < i; j++) free(adjList[j]); //adjList[i]->next->next may not exist for i in {0, V} and thus we cannot call freeList here
			free(adjList);
			return(0);
		}
		adjList[i]->item = 0; //each pointer in the array points to a dummy with item = 0 and, initially, next pointer = NULL
		adjList[i]->next = NULL;
	}
	
	//Iterating through edgeArray and adding to the adjList's as appropriately by calling insertList
	//Freeing all previously allocated memory if malloc fail
	//Running time proportional to running time insertList (see comments above function)
	int error = 0;
	for (int i = 0; i < (2*E); i = i+2)
	{
		error = insertList(adjList, edgeArray[i], edgeArray[i+1]); //Insertion 1
		if (error == 0) //failed malloc
		{
			printf("\n***Failed Malloc Initialization (4)***\n"); 
			freeList(adjList, edgeArray, V);
			return(0);
		}
		error = insertList(adjList, edgeArray[i+1], edgeArray[i]); //Insertion 2
		if (error == 0) //failed malloc
		{
			printf("\n***Failed Malloc Initialization (5)***\n"); 
			freeList(adjList, edgeArray, V);
			return(0);
		}
	}
	
	printList(adjList, V); //printing the adjacency lists (Running time proportional to running time of printList (see comments above function))
	freeList(adjList, edgeArray, V); //freeing all mallocced memory (Running time proportional to running time of freeList (see comments above function))
	fclose(myfile);
	return(0);
}

//insertList
/*
	This function inserts vertice y into vertice x's ajacency list
	The while loop ensures insertion in increasing order by moving the temp pointer to the node previous to where the insertion is to occur
	It is worth noting the order of the conditions in the while loop is important, 
		If they are reversed and an item needs to be inserted at the end of linked list, temp->next->item does not exist and the program has a runtime error
		
	Return values:
		0: failed malloc (program will print error message and call freeList in main)
		1: successful insertion 
		
	Time complexity: O(log(E)); the while loop traverses over the prexisting ajacency list and for larger graphs, this travel will take an 
	increasingly longer amount of time to successfully execute. As a group, we are unsure how to create a sorted ajacency list without this step,
	given that the edges are arbitrarily listed in the input file. insertList is called E times, and thus has a total time complexity O(Elog(E))
*/
int insertList(Node** ADJLIST, int x, int y)
{
	Node* temp;
	Node* newNode;
	temp = ADJLIST[x-1]; //the ajacency list for vertice x
	while(temp->next != NULL && temp->next->item < y) temp = temp->next; 	//average time complexity O(log(E))
	newNode = malloc(sizeof(Node));
	if (newNode == NULL) return (0); //unsuccessful malloc 
	newNode->item = y; //adding y to the ajacency list for x
	newNode->next = temp->next; //if insertion at the end of the linked list, temp->next is simply NULL
	temp->next = newNode; 
	return(1);
}

//printList:
/*
	Printing off the ajacency lists 
	Efficiency: O(V+E); the for loop runs V times and, across all for loops, the while loop inside runs a total proportional to  
		the total number of nodes in the ajacency list which must be proportional to the number of edges
*/
void printList(Node** ADJLIST, int V)
{
	puts("");
	Node* temp;
	for (int i = 0; i < V; i++)
	{
		printf("The ajacency list for vertice %d is: ", (i+1));
		temp = ADJLIST[i]->next;
		if (temp == NULL) printf("Vertice %d has degree 0", (i+1)); 
		while (temp != NULL)
		{
			printf("%d ", temp->item);
			temp = temp->next;
		}
		puts("");
	}
}

//freeList:
/*
	Freeing all the mallocced memory at the end of the program, or if malloc fails during the insertList function
	Efficiency: O(V+E); the for loop runs V times and, across all for loops, the while loop inside runs a total proportional to
		the total number of nodes in the ajacency list which must be proportional to the number of edges
*/
void freeList(Node** ADJLIST, int* EDGEARRAY, int V)
{
	Node * temp;
	free(EDGEARRAY);
	for (int i = 0; i < V; i++)
	{
		while ((ADJLIST)[i]->next != NULL)
		{
			temp = ADJLIST[i]->next->next;
			free(ADJLIST[i]->next);
			ADJLIST[i]->next = temp;
		}
	}

	for (int i = 0; i < V; i++) free(ADJLIST[i]);
	free(ADJLIST);
	printf("\nMallocced memory has been freed!\n");
}