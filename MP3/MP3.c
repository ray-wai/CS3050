 //Raymond Waidmann
//18157816
//rcw5k2

//Greg Ryterski
//18186949
//gjr7dz

//CS3050 Machine Problem 3
//October 28, 2020

/*How team members worked together:
    We both live in the same house, so we worked together in the same room on the assignment. It is not necessary to detail the breakdown of responsibilites
    as nearly the entire assignment was a joint and collaborative effort.
*/

/*Comment on algorithm efficiency:
    See function descriptions for comments. Total complexity is O(n) + O(log(n)) = O(nlog(n))
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define FILE_SIZE 100
#define ARRAYSIZE 1000

int* BUILD_MAX_HEAP(FILE*);
void MAX_HEAPIFY(int*, int);
void INSERT(int*, int);
void HEAP_CHANGE_KEY(int*, int, int);
void DELETE(int*, int);

/*Steps
 1. Read first line for size of heap n
 2. Read the data A[1] through A[n] into the heap
 3. Follow the commands remaining in the input file
    E: extract max
    I v: insert new key v
    C i v: change key at A[i] to v
    D i: delete key at A[i]
 4. After all commands, output the final content of the heap on a single line
    (test file will fit on one line w/ numbers seperated by a single space)

 Notes:
    all operations should be implemented using the cyclic-shift idea
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

    //Reading the size of the heap from the first line of the file
    //Initializing the heap
    int * A = BUILD_MAX_HEAP(myfile);
    if (A == NULL)
    {
        printf("\n\n$$$$$Failed Malloc Initialization (1)$$$$$\n\n");
        return(0);
    }

    char c;
    int x, y;
    while(!(feof(myfile))) //Runs till the end of the file
    {
        fscanf(myfile, "%c", &c); //grabs the elements from the file
        //Extract (Delete A[1])
        if (c == 'E')
        {
            DELETE(A, 1);
        }

        //Insert
        if (c == 'I')
        {
            fscanf(myfile, "%d", &x);
            INSERT(A, x);
        }

        //Change key
        if (c == 'C')
        {
            fscanf(myfile, "%d %d", &x, &y);
            HEAP_CHANGE_KEY(A, x, y);
        }

        //Delete
        if (c == 'D')
        {
            fscanf(myfile, "%d", &x);
            DELETE(A, x);
        }
    }

    puts("");
    for (int i = 1; i <= A[0]; i++) printf("Array[%d] = %d\n", i, A[i]); //Prints the elements in the array
    puts("");

    free(A); //frees all allocated memory
    return(0);
}



/* BUILD_MAX_HEAP:
    This function takes the file pointer and scans the size, then mallocs the heap array
    The function reads in the values from the file, iterates through MAX_HEAPIFY, and returns the resulting max heap
    Running time is O(n)
*/
int* BUILD_MAX_HEAP(FILE* MYFILE)
{
    int size;
    fscanf(MYFILE, "%d\n", &size);
    int * HEAP = malloc(sizeof(int)*((ARRAYSIZE)+1));
    if (HEAP == NULL) return(NULL);
    
    HEAP[0] = size; //Storing the size of the array in index 0
    for (int i = 1; i <= size; i++) fscanf(MYFILE, "%d\n", &HEAP[i]);
    for (int i = (size+1); i <= ARRAYSIZE; i++) HEAP[i] = 0; //initializing rest of the array to 0
    int floorsize = floor(size/2);
    for (int i = floorsize; i > 0; i--) MAX_HEAPIFY(HEAP,i);
    return(HEAP);
}



/* MAX_HEAPIFY:
    This function works down the heap and corrects errors to maintain the max heap properties
    Runs Recursively through the heap
    Running time is O(log(n))
*/
void MAX_HEAPIFY(int* HEAP, int I)
{
    int L = 2*I; //Left
    int R = L+1; //Right
    int largest;

    //grabs the largest element in the heap
    if (L <= HEAP[0] && HEAP[L] > HEAP[I]) largest = L;
    else largest = I;
    if (R <= HEAP[0] && HEAP[R] > HEAP[largest]) largest = R;

    if (largest != I) //if the largest is not equal to the key it runs again
    {
        HEAP[(HEAP[0]+1)] = HEAP[I]; //HEAP[size+1]; for a given size heap, this is the first unused index.
        HEAP[I] = HEAP[largest];
        HEAP[largest] = HEAP[(HEAP[0]+1)];
        HEAP[(HEAP[0]+1)] = 0; //doing it this way prevents us from having to use a temp variable.
        MAX_HEAPIFY(HEAP, largest);
    }
}



/* INSERT:
    This function inserts a new key into the array by incrementing the size, then using cyclic shift
    to move elements in the array and insert the new key such that the max heap properties are maintained.
    Running time is O(log(n))
 */
void INSERT(int* HEAP, int KEY)
{
    HEAP[0] += 1; //increments the size of the heap
    int I = HEAP[0]; //assigns I to the size of the heap stored in HEAP[0]
    int FLOORINDEX = floor(I/2); //gets the index by taking the floor of the

    //runs through the heap to find where to input the new node
    while(I > 1 && HEAP[FLOORINDEX] < KEY)
    {
        HEAP[I] = HEAP[FLOORINDEX];
        I = I/2;
        FLOORINDEX = floor(I/2);
    }
    HEAP[I] = KEY; //makes the new node hold the value of the key
}



/* HEAP_CHANGE_KEY:
    This function changes the key in HEAP[I] to KEY
        - if the keys are equal, there is nothing to change and simply return.
        - if HEAP[I] < KEY, the heap property only needs fixing above the changed key
        - if HEAP[I] > KEY, the heap property only needs fixing below the changed key
        Running time is O(log(n))
*/
void HEAP_CHANGE_KEY(int* HEAP, int I, int KEY)
{
    if(HEAP[I] == KEY) return; //No key replacement required

    //HEAP_INCREASE_KEY
    if(HEAP[I] < KEY)
    {
        int FLOORINDEX = floor(I/2); //gets the index which is passed by I

        while(I > 1 && HEAP[FLOORINDEX] < KEY) //increases the heap to find where the changekey is to be inserted
        {
            HEAP[I] = HEAP[FLOORINDEX];
            I = I/2;
            FLOORINDEX = floor(I/2); //gets the new index value for comparison
        }
        HEAP[I] = KEY; //sets the new value of the node
    }

    //HEAP_DECREASE_KEY (HEAP[INDEX] > KEY) (MAX_HEAPIFY)
    else
    {
        HEAP[I] = KEY;
        MAX_HEAPIFY(HEAP, I); //corrects errors to maintain max heap properties
    }
}



/*DELETE:
    DELETE (1. set key = A[n] where n = size of heap, 2. n = n-1, 3. call HEAP_CHANGE_KEY(A,i,key))
    Running time equivalent to that of HEAP_CHANGE_KEY
*/
void DELETE(int* HEAP, int I)
{
    int KEY = HEAP[(HEAP[0])]; //assigns the key to be HEAP[size]
    HEAP[0] -= 1; //decrements the size of the heap that is stored in HEAP[0]
    HEAP_CHANGE_KEY(HEAP, I, KEY);
}
