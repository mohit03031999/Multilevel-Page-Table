#ifndef _PAGETABLE_H
#define _PAGETABLE_H

#include "level.h"

class PAGETABLE {
public:

    unsigned int NumofLevel;        //To store number of levels
    unsigned int offset;            //To store the offset bits
    unsigned int *BitmaskArray;     //To store the Masking for each level
    unsigned int *ShiftArray;       //To Store the shift for each level 
    unsigned int *entryCount;       //To store the entry count for each level

    int totalMemory = 0;

    //Constructor for class PageTable
    //Inputs: (Number of levels, Bits in each level)
    PAGETABLE(unsigned int numlevel, int *Levelbits); 

    //Functio nto insert the frame number for the corresponding page number
    //Inputs:(Logical Address, Frame Number)
    bool insertpagetb(unsigned int virtualAddress, unsigned int frameNumber);

    //Function to get the Frame Number for the corresponding logical address
    //Inputs:(Logical Address)
    int getFrameNumber(unsigned int);

    //Function to get the offset for the corresponding logical address
    //Inputs:(Logical Address)
    //Return: Offset for the corresponding Logical Address
    int getOffset(unsigned int virtualAddress); 

    //Function to get the page number for the corresponding logical address at each level
    //Inputs:(Logical Address)
    unsigned int *getPageateachlevel(unsigned int virtualAddress);

    //Function to get the Physical Address for the corresponding logical address and frame number
    //Inputs:(Logical Address, Frame Number)
    //Return : Physical Address
    unsigned int getPhysicalAddr(unsigned int virtualAddress, int frame);

    //Function to get the total size of page table
    //Return : Total size of the page table
    unsigned int pagetablesize();

private:
    class Level* rootNode;
};
#endif //_PAGETABLE_H
