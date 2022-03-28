#include "pagetable.h"
#include <stdlib.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <math.h>

#define ADDRESS_SIZE 32  //No of bits for virtual Address 


PAGETABLE::PAGETABLE(unsigned int numlevel, int *Levelbits) 
{
    PAGETABLE::NumofLevel = numlevel;                       //Storing the total number of levels
    BitmaskArray = new unsigned int[NumofLevel + 1];        //Initalzing the Mask Array of size(Number of levels)
    ShiftArray = new unsigned int[NumofLevel + 1];          //Initializing the Shift Array of size(Number of levels)
    entryCount = new unsigned int[NumofLevel + 1];          //Initializing the Entry Count Array of size(Number of levels)
    rootNode = nullptr;                                     //Initializing the Root Node Pointer to NULL 

    unsigned int totalbits = 0;
    for (int i = 0; i < NumofLevel; i++) 
    {
        totalbits += Levelbits[i];                  //Calculating the total bits used for page number
    }
    
    offset = ADDRESS_SIZE-totalbits;                //Calculating and Storing the number of offset bit

    unsigned int bitMask;
    unsigned int OffsetBitCount = offset;
    
    BitmaskArray[numlevel] = (unsigned int) (pow(2,offset)- 1);
    ShiftArray[numlevel] = 0;
    entryCount[numlevel] = (unsigned int) (pow(2,offset));
    
    for (int i = numlevel - 1; i >= 0; i--) 
    {
        int bits = Levelbits[i];
        bitMask = (unsigned int) (pow(2,bits)- 1);
        BitmaskArray[i] = bitMask << OffsetBitCount;
        ShiftArray[i] = OffsetBitCount;
        entryCount[i] = (unsigned int) (pow(2,bits));
        OffsetBitCount += bits;
    }

};

bool PAGETABLE::insertpagetb(unsigned int virtualAddress, unsigned int frameNumber) 
{
    if (rootNode == nullptr)   //If the rootNode Pointer is NULL create a new LEVEL with depth 0 and assign it to root node pointer
    {
        rootNode = new Level(0, 1 == NumofLevel, this);
    }
    return rootNode->insertlevel(virtualAddress, frameNumber);    //Else call the insert() from the class level
}

int PAGETABLE::getFrameNumber(unsigned int virtualAddress) 
{
    if (rootNode == nullptr)                                 //If the root node pointer is NULL then the frame number not present and return -1 
    {
        return -1;                                             
    }
    return rootNode->levelgetFramenumber(virtualAddress);          //Else call the getFrameNumber() from the class level
}

int PAGETABLE::getOffset(unsigned int virtualAddress) 
{
    unsigned int offsetmask = (unsigned int) (1 << offset) - 1;  //Calculating the offset mask
    unsigned int Addroffset = (virtualAddress & offsetmask) ;   //Calculating the offset of the logical address
    return Addroffset; //Return the offset of the Virtual Address
}

unsigned int *PAGETABLE::getPageateachlevel(unsigned int virtualAddress)  
{
    unsigned int *temppage = new unsigned int[NumofLevel + 1];              //Initialzing a temp array
    for (int i = 0; i < NumofLevel; i++) 
    {
        temppage[i] = (virtualAddress & BitmaskArray[i]) >> ShiftArray[i];  //Calcualting the page for eeach level
        
    }
    return temppage;                //Return the page number for each level
}

unsigned int PAGETABLE::getPhysicalAddr(unsigned int virtualAddress, int frame) 
{
    unsigned int frameNumber = frame;           
    unsigned int pageSize = entryCount[NumofLevel];
    unsigned int PhysicalLocation = frameNumber * pageSize;   //Calculating the pysical address
    unsigned int Addroffset = getOffset(virtualAddress);      //Calculating the Address Offset
    return PhysicalLocation + Addroffset;       //Returning the Physical Address
}

unsigned int PAGETABLE::pagetablesize() {  
    int bytes = (sizeof(BitmaskArray)+sizeof(ShiftArray)+sizeof(entryCount))*NumofLevel;        //Calculating the bytes for BITMASK,SHIFTARRAY and ENTRYACOUNT Array times number of levels
    bytes= bytes + rootNode->levelsize();
    return bytes;           //Returning the total bytes calculed of page tabel
}

