#ifndef _LEVEL_H
#define _LEVEL_H

#include "map.h"
#include "pagetable.h"

class PAGETABLE;

class Level {
private:
    int depth;                      //Depth of the level
    bool isLeaf;                    //Boolean for a leaf level
    PAGETABLE *pageTablePointer;    //Page Table pointer
    Level **nextLevel;              // Next Level Pointer
    MAP *mapPointer;                //Map Pointer
public:
    //Constructor for class Level
    //Input: (Current Depth , Current node isleaf level or not , Page Table Pointer)
    Level(int Depth, bool isLeaf, PAGETABLE *pageTablePtr);

    //Function to insert the frame number for the corresponding page number
    //Inputs: (Logical Address , Frame Number)
    bool insertlevel(unsigned int virtualAddress, unsigned int frameNumber);

    //Function to get the frame number for the corresponding page number
    //Inputs: (Logical Address)
    //Return: Frame Number corresponding to Logical Address
    int levelgetFramenumber(unsigned int virtualAddress);

    //Calculating the total size
    //Return: The total size for (computed for each level)
    int levelsize();

};
#endif //_LEVEL_H