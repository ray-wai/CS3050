
//Raymond Waidmann
//18157816
//rcw5k2

//Greg Ryterski
//18186949
//gjr7dz

//CS3050 Machine Problem 2
//September 22, 2020

/*How team members worked together:
    We both live in the same house, so we worked together in the same room on the assignment. It is not necessary to detail the breakdown of responsibilites
    as nearly the entire assignment was a joint and collaborative effort.
*/

/*Comment on algorithm efficiency:
    The total running time is proportional to O(V+E)
    See Comments in the main program that detail further detail the running times of specific sections of code
    Total time complexity: O(1)+O(E)+O(V)+O(V+E) = O(V+E)
*/

#include <stdio.h>
#include <stdlib.h>
#define FILE_SIZE 100

typedef struct nextstruct {
    int item;
    struct nextstruct *next;
    } NxtNode;

typedef struct nodestruct {
    int item;    //adjacent vertice numbers
    int color;   //0 = white, 1 = black, this variable is also used to increment outdegree of SCC's near the end of the program
    int discovery;
    int finish;
    int SCC;
    NxtNode *next;
    } Node;

int insertList(Node**, int, int);
void DFS(Node**, int, int*, int*, int*, int);
void reInit(Node**, Node**, int);
void freeList(Node**, Node**, int*, int);
/*
void printFinishTimeOrder(int*, int);
void printDiscoveryFinish(Node**, int); //these are functions that were used to check for accuracy during development, but are not necessary for the final implementation
void printSCCs(Node**, int);
void printList(Node**, int);
*/

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
    if (E == 0)
    {
        printf("\n\nINPUT FILE DOES NOT CONTAIN ANY EDGES\n\n");
        return(0);
    }
    rewind(myfile); //putting the myfile pointer back to the beginning of the file



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
    //Freeing all previously allocated memory if malloc fails
    //Efficiency: O(V); the for loop running time for dummy node allocation is proportional to the max number in the input file (ie. the # of vertices)
    Node ** adjList = malloc((sizeof(Node*))*V);
    if (adjList == NULL)
    {
        printf("\n***Failed Malloc Initialization (2)***\n"); //unsuccessful malloc
        free(edgeArray);
        return(0);
    }
    for (int i = 0; i < V; i++) //"dummy node" allocation
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
        adjList[i]->color = 0;
        adjList[i]->discovery = 0;
        adjList[i]->finish = 0;
        adjList[i]->SCC = 0;
        adjList[i]->next = NULL;
    }

    //second adjList array for GT, nearly exact same code as adjList above, same time complexity
    //allocating two arrays takes up a bit more memory than creating a struct with two pointers, but this implementation makes it easier to implement functions
        //as the architectures for both adjList and adjListGT are identical.
    Node ** adjListGT = malloc((sizeof(Node*))*V);
    if (adjListGT == NULL)
    {
        printf("\n***Failed Malloc Initialization (4)***\n"); //unsuccessful malloc
        free(edgeArray);
        for (int i = 0; i < V; i++) free(adjList[i]);
        free(adjList);
        return(0);
    }
    for (int i = 0; i < V; i++) //"dummy node" allocation
    {
        adjListGT[i] = malloc(sizeof(Node));
        if (adjListGT[i] == NULL) //unsuccessful malloc
        {
            printf("\n***Failed Malloc Initialization (5)***\n"); //unsuccessful malloc
            free(edgeArray);
            for (int j = 0; j < V; j++) free(adjList[j]);
            free(adjList);
            for (int k = 0; k < i; k++) free(adjListGT[k]);
            free(adjListGT);
            return(0);
        }
        adjListGT[i]->item = 0; //each pointer in the array points to a dummy with item = 0 and, initially, next pointer = NULL
        adjListGT[i]->color = 0;
        adjListGT[i]->discovery = 0;
        adjListGT[i]->finish = 0;
        adjListGT[i]->SCC = 0;
        adjListGT[i]->next = NULL;
    }



    //Iterating through edgeArray and adding to the adjList's as appropriately by calling insertList
    //Freeing all previously allocated memory if malloc fail
    //Running time proportional to running time insertList (see comments above function)
    int error = 0;
    for (int i = 0; i < (2*E); i = i+2)
    {
        error = insertList(adjList, edgeArray[i], edgeArray[i+1]); //Insertion 1 (G)
        if (error == 0) //failed malloc
        {
            printf("\n***Failed Malloc Initialization (6)***\n");
            freeList(adjList, adjListGT, edgeArray, V);
            return(0);
        }

        error = insertList(adjListGT, edgeArray[i+1], edgeArray[i]); //Insertion 2 (GT)
        if (error == 0) //failed malloc
        {
            printf("\n***Failed Malloc Initialization (7)***\n");
            freeList(adjList, adjListGT, edgeArray, V);
            return(0);
        }
    }



    //Calling DFS on G
    //count is used to index the FinishTimeOrder array which contains the vertices of G in INCREASING finish times
    //DESCREASING finish times are necessary, but they finish recursive DFS calls in increasing order...
    //complexity proportional to DFS; see comments above function
    int time = 0;
    int count = 0;
    int * FinishTimeOrder = malloc(sizeof(int)*(V));
    if (FinishTimeOrder == NULL) //unsuccessful malloc
    {
        printf("\n***Failed Malloc Initialization (8)***\n");
        freeList(adjList, adjListGT, edgeArray, V);
        return(0);
    }
    for (int i = 0; i < V; i++) if (adjList[i]->color == 0) DFS(adjList, i, &time, &count, FinishTimeOrder, 1); //DFS on G

/*
    printFinishTimeOrder(FinishTimeOrder, V); //printing the array that shows the finish time order of DFS for the vertices in G
    printDiscoveryFinish(adjList, V); //printing the discovery and finish times for the first DFS
*/

    //DFS on GT
    //strongconcomp will be stored in adjListGT->SCC and increments everytime a new recursion of DFS of G transpose occurs
    //notice the for loop runs in decreasing order; this is because the FinishTimeOrder Array finishes in increasing order, but we need decreasing order
    //notice strongconcomp replaces FinishTimeOrder in these calls to DFS
    //at the completion of this for loop, strongconcomp = the number of strongly connected components in the graph +1
    //complexity proportional to DFS; see comments above function
    time = 0; //resetting time to 0
    int strongconcomp = 1;
    for (int j = V-1; j >= 0; j--)
    {
        if (adjListGT[(FinishTimeOrder[j])]->color == 0)
        {
            DFS(adjListGT, (FinishTimeOrder[j]), &time, &count, &strongconcomp, 2);
            strongconcomp++; //if recursion completes, the next call to DFS in this loop will be a new SCC, so it is incremented as such here
        }
    }

/*
    printDiscoveryFinish(adjListGT, V); //discovery and finish times for second traversal of DFS
    printSCCs(adjListGT, V); //SCC that each vertice belongs to
    printList(adjList, V); //printing the adjacency lists for G
    printList(adjListGT, V); //printing the adjacency lists for GT
*/



    //freeing the appropriate amount of memory in our array of pointers to use them for GSCC ajacency lists
    //Running time must be linear; in worst case, this is simply freeList function which we know is O(V+E)
    reInit(adjList, adjListGT, strongconcomp);
    
    
    
    //This for loop creates the first GSCC adjList that includes duplicates
    //if statement executes when the SCCs of connected vertices in the input file are not identical.
    //This is in linear time since it bascially rescans through the input file, which we know is O(E).
    for (int i = 0; i < (2*E); i = i+2)
    {
        if (adjListGT[((edgeArray[i])-1)]->SCC != adjListGT[((edgeArray[i+1])-1)]->SCC) //-1 since edgeArray contains vertices, but the indeces in adjList are one less
        {
            //IF THIS LINE ACTIVE, YIELDS INDEGREE
            //error = insertList(adjList, adjListGT[((edgeArray[i])-1)]->SCC, adjListGT[((edgeArray[i+1])-1)]->SCC); //Insertion 1 (G) (insert for outgoing edges)
            //IF THIS LINE ACTIVE, YIELDS OUTDEGREE
            error = insertList(adjList, adjListGT[((edgeArray[i+1])-1)]->SCC, adjListGT[((edgeArray[i])-1)]->SCC); //(insert for incoming edges)
            if (error == 0) //failed malloc
            {
                printf("\n***Failed Malloc Initialization (6)***\n");
                freeList(adjList, adjListGT, edgeArray, V);
                return(0);
            }
        }
    }



    //This loop reverses the input file (ensuring sorted insertion) and also prevents duplicate insertions in the if statement:
        //Pseudocode for if statement:
            //if (no nodes inserted yet OR item value of the node we've already inserted is != to the one we are about to insert)
    //adjListGT->color is used as a counter to count the number of non duplicate insertions into the linked list. (was reset in reInit function)
    //Analogous to machine problem one code
    for (int i = (strongconcomp-2); i >= 0; i--) //-2 because there are strongconcomp-1 SCC's and we need to subtract 1 more to get the correct index in the adjList array
    {
        NxtNode* temp_main = adjList[i]->next;
        while(temp_main != NULL)
        {
            if(adjListGT[((temp_main->item)-1)]->next == NULL || i+1 != adjListGT[((temp_main->item)-1)]->next->item)
            {
                NxtNode* newNode = malloc(sizeof(NxtNode));
                if (newNode == NULL)
                {
                    printf("\n***Failed Malloc Initialization (6)***\n");
                    freeList(adjList, adjListGT, edgeArray, V);
                    return(0);
                }

                newNode->item = i+1;
                newNode->next = adjListGT[((temp_main->item)-1)]->next; //-1 is necessary since adjList[0] for vertice 1, adjList[1] for vertice 2, ...
                adjListGT[((temp_main->item)-1)]->next = newNode;
                adjListGT[((temp_main->item)-1)]->color++;
            }

            temp_main = temp_main->next;
        }
    }



    //Printing the Output
    //See Lines 249-252 for indegree vs outdegree
    //This loop reverses the outdegrees so that they can be printed in increasing order
    //Running time proportional to the number of SCC's which we know is proportional to number of vertices plus edges; O(V+E)
    puts("");
    Node* temp;
    int start = 0;
    int end = strongconcomp-2;
    while(start < end){
        if(adjListGT[start]->color >= adjListGT[end]->color){
            temp = adjListGT[start];
            adjListGT[start] = adjListGT[end];
            adjListGT[end] = temp;
            start++;
            end--;
        } else{
            start++;
            end--;
        }
     }

    for (int i = 0; i < strongconcomp-1; i++) //printing output to screen
    {
        printf ("%d ", adjListGT[i]->color);
    }
    puts("");


/*
    printList(adjList, V); //printing the adjacency lists for GSCC after initial insertion
    printList(adjListGT, V); //printing the adjacency lists for GSCC after "reverse insertion" eliminating duplicates
*/


    //freeing all mallocced memory (Running time proportional to running time of freeList (see comments above function))
    //free(FinishTimeOrder) not included in freeList function since if it were, we could only be able to call freeList once (here in the program)
        //leaving it out of the function, we are able to call freeList anytime an adjacency list insertion fails (overall, this is less lines of code)
    freeList(adjList, adjListGT, edgeArray, V);
    free(FinishTimeOrder);
    fclose(myfile);
    return(0);
}



/* insertList
    This function inserts vertice y into vertice x's ajacency list

    Return values:
        0: failed malloc (program will print error message and call freeList in main)
        1: successful insertion

    Time complexity: O(1)
*/
int insertList(Node** ADJLIST, int x, int y)
{
    Node* temp;
    NxtNode* newNode;
    temp = ADJLIST[x-1]; //the ajacency list for vertice x
    newNode = malloc(sizeof(NxtNode));
    if (newNode == NULL) return (0); //unsuccessful malloc
    newNode->item = y; //adding y to the ajacency list for x
    newNode->next = temp->next; //if insertion at the end of the linked list, temp->next is simply NULL
    temp->next = newNode;
    return(1);
}



/*Depths first search:
    This function performs DFS recursively. DFS visits neighboring vertices labeling discovery times
    and once it can no longer do so, backtracks and labels finish times appropriately.

    Int M is a mode:
    M = 1: DFS for G
        - Mode 1 keeps track of the order of finish times as they occur via the FINISHORDER array. This is used in the second call to DFS (M=2) on G transpose
    M = 2: DFS for G transpose (GT)
        - the main algorithm is the same, but the pointers are different and FINISHORDER does not matter for G transpose and instead numbers the SCC's (strongconcomp)

    DFS operates in linear time (O(V+E)) since it visits every vertice and edge only once.
*/
void DFS(Node** ADJLIST, int I, int* TIME, int* COUNT, int* FINISHORDER, int M)
{
    (*TIME)++;
    ADJLIST[I]->color = 1; //black
    ADJLIST[I]->discovery = *TIME;
    NxtNode* temp = ADJLIST[I]->next;

    while(temp !=  NULL)
        {
            int x = (temp->item)-1; //minus one since ADJLIST[0] is for vertice 1, ADJLIST[1] is for V = 2, etc...
            if (ADJLIST[x]->color == 0)
            {
                if (M == 1) DFS(ADJLIST, x, TIME, COUNT, FINISHORDER, 1);
                if (M == 2) DFS(ADJLIST, x, TIME, COUNT, FINISHORDER, 2);
            }
            temp = temp->next;
        }

    if (M == 1) //G
    {
        FINISHORDER[(*COUNT)] = I;
        (*COUNT)++;
    }

    if (M == 2) //GT
    {
        ADJLIST[I]->SCC = *FINISHORDER; //FINISHORDER is actually strongconcomp when Mode = 2, hence the star operator
    }

    (*TIME)++;
    ADJLIST[I]->finish = *TIME;
}



/*reInit:
    Freeing the adjacency lists we needed in DFS to create the adjLists for GSCC
    The freeing is a bit tedious, but it ends up being more efficient since we do not have to malloc any more memory.
    In worst case scenario, complexity is O(V+E) since it would free every edge and every vertice (freeList)
*/
void reInit(Node** ADJLIST, Node**ADJLISTGT, int SCC)
{
    for (int i = 0; i < SCC-1; i++) //strongconcomp actually = 5 since it gets incremented once more after numbering the last SCC, hence -1
    {
        NxtNode* temp_main;
        while (ADJLIST[i]->next != NULL) //freeing GT adjList
        {
            temp_main = ADJLIST[i]->next->next;
            free(ADJLIST[i]->next);
            ADJLIST[i]->next = temp_main;
        }

        ADJLISTGT[i]->color = 0; //we will use this to count the number of SCC's later
        while (ADJLISTGT[i]->next != NULL) //freeing GT adjList
        {
            temp_main = ADJLISTGT[i]->next->next;
            free(ADJLISTGT[i]->next);
            ADJLISTGT[i]->next = temp_main;
        }
    }
}



/* freeList:
    Freeing all the mallocced memory at the end of the program, or if malloc fails during the insertList function
    Efficiency: O(V+E); the for loop runs V times and, across all for loops, the while loop inside runs a total proportional to
        the total number of nodes in the ajacency lists which must be proportional to the number of edges
*/
void freeList(Node** ADJLIST, Node** ADJLISTGT, int* EDGEARRAY, int V)
{
    NxtNode * temp;
    free(EDGEARRAY);
    for (int i = 0; i < V; i++)
    {
        while ((ADJLIST)[i]->next != NULL) //freeing G adjList
        {
            temp = ADJLIST[i]->next->next;
            free(ADJLIST[i]->next);
            ADJLIST[i]->next = temp;
        }

        while ((ADJLISTGT)[i]->next != NULL) //freeing GT adjList
        {
            temp = ADJLISTGT[i]->next->next;
            free(ADJLISTGT[i]->next);
            ADJLISTGT[i]->next = temp;
        }
    }

    for (int i = 0; i < V; i++) free(ADJLIST[i]); //G
    free(ADJLIST);
    for (int i = 0; i < V; i++) free(ADJLISTGT[i]); //GT
    free(ADJLISTGT);
    printf("\nMallocced memory has been freed!\n");
}

/* //these are functions that were used to check for accuracy during development, but are not necessary for the final implementation
void printFinishTimeOrder(int* FTO, int V)
{
    printf("\nFinish Time array first time: ");
    for (int i = 0; i < V; i++) printf("%d ", FTO[i]);
}

void printDiscoveryFinish(Node** ADJLIST, int V)
{
    puts("");
    printf("Discovery and Finish Times: \n");
    for (int i = 0; i < V; i++)
    {
        printf("Vertice %d Discovery, Finish: %d %d\n", i+1, ADJLIST[i]->discovery, ADJLIST[i]->finish);
    }
    puts("");
}

void printSCCs(Node** ADJLIST, int V)
{
    printf("\nSCC's:\n");
    for (int i = 0; i < V; i++)
    {
        printf("Vertice %d: %d\n", i+1, ADJLIST[i]->SCC);
    }
}

//printList:

    Printing off the ajacency lists
    Efficiency: O(V+E); the for loop runs V times and, across all for loops, the while loop inside runs a total proportional to
        the total number of nodes in the ajacency list which must be proportional to the number of edges

void printList(Node** ADJLIST, int V)
{
    puts("");
    NxtNode* temp;

    printf("\nAJACENCY LIST\n");
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
*/
