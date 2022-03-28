#include "level.h"
#include <iostream>

using namespace std;

Level::Level(int depth, bool LeafLevel, PAGETABLE *ptpointer) 
{
    //Storing all the variables passed in the function
    Level::depth = depth;          
    Level::isLeaf = LeafLevel;
    Level::pageTablePointer = ptpointer;
    Level::mapPointer = nullptr;
    
    if (isLeaf)     //If the the current level is leaf level then the nextlevel pointer is NULL
    {
        Level::nextLevel = nullptr;
    } 
    else 
    {
        int nextLevelSize = Level::pageTablePointer->entryCount[depth];  //Getting the size of next level 
        Level::nextLevel = new Level *[nextLevelSize];
        for (int i = 0; i < nextLevelSize; i++) 
        {   
            nextLevel[i] = nullptr;     //Initalizig all the values of next level array to NULL
        }
    }
}

bool Level::insertlevel(unsigned int virtualAddress, unsigned int frame) 
{
    unsigned int Mask = pageTablePointer->BitmaskArray[depth];     //Extracting Mask for the current level
    unsigned int shift = pageTablePointer->ShiftArray[depth];      //Extracting the Shift for the current level
    unsigned int page = (virtualAddress & Mask) >> shift;          //Calulating the page for the current level

    if (isLeaf)                                                     //If the current level is leaf level
    {   
        if (mapPointer == nullptr)   //If the map pointer is NULL
        {
            mapPointer = new MAP(pageTablePointer->entryCount[depth]);   
        }
        return mapPointer->insertFrameNumber(page, frame);          //Insert the frame number 
    }
    if (nextLevel[page] == nullptr)         
    {
        bool isLeaftemp = pageTablePointer->NumofLevel == depth + 2;
        nextLevel[page] = new Level(depth + 1, isLeaftemp, pageTablePointer);
    }
    return nextLevel[page]->insertlevel(virtualAddress, frame);
}

int Level::levelgetFramenumber(unsigned int virtualAddress) 
{
    unsigned int Mask = pageTablePointer->BitmaskArray[depth];      //Extracting Mask for the current level
    unsigned int shift = pageTablePointer->ShiftArray[depth];       //Extracting the Shift for the current level
    unsigned int page = (virtualAddress & Mask) >> shift;           //Calulating the page for the current level
    
    if (isLeaf)                                     //If the current level is leaf level
    {
        if (mapPointer == nullptr)                  //If Map Ppointer is NULL then the frame number not present
        {
            return -1;
        } 
        else 
        {
            return mapPointer->mapgetFrameNumber(page);
        }
    } 
    else 
    {
        if (nextLevel[page] == nullptr)                //If Next level Pointer is NULL then the frame number not present
        {
            return -1;
        } 
        else 
        {
            return nextLevel[page]->levelgetFramenumber(virtualAddress);
        }
    }
}

int Level::levelsize() {
    int levelSize = 0;
    int tempbytes = 0;
    levelSize += sizeof(nextLevel) + sizeof(mapPointer);
    if (isLeaf) 
    {
        if (mapPointer == nullptr)                          //If Map Ppointer is NULL return just the size of variables for level
        {
            return levelSize;
        } 
        else
        {
            return (levelSize+mapPointer->mapsize());       //Else calcualte the size of map 
        }
    }
    for (int i = 0; i < pageTablePointer->entryCount[depth]; i++)
    {
        if (nextLevel[i] != nullptr)                //If the nextlevel pointer is not null calulate the size for next level
        {
            tempbytes += nextLevel[i]->levelsize();
        }
        else                                        //If its null just add 1 bytes for empty space
        {
            tempbytes +=1;
        }
    }
    return (levelSize+tempbytes);
}