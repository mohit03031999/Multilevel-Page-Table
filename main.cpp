#define HIT 0
#define MISS 1
#define ALL_LEVEL_BITS 28

#include "unistd.h"
#include "tracereader.h"
#include "pagetable.h"
#include "output_mode_helpers.h"
#include "tlb.h"


#include <stdlib.h>
#include <string>
#include <iostream>
#include <cmath>

using namespace std;

//Helper for inserts into Page Table
//Inputs: (Pagetable object, Logical Addrress, Frame Number)
int pagetableInsert(PAGETABLE *pt, unsigned int logicalAddress, unsigned int frame);

//Returns frame number for the specified logical address
//Inputs: (Pagetable Object, Logical Address)
int getFrame(PAGETABLE *pagetable, unsigned int logicalAddress);

//Returns the page number
//Inputs: (Pagetable Object, Number of levels)
unsigned int getpagenumber(unsigned int *pages, int level);

int main(int argc, char *argv[]) 
{
    OutputOptionsType * output = (OutputOptionsType *)malloc(sizeof(OutputOptionsType)); //For specifying the output mode
    storetlb tlbdata;                       // For storing the mapping in TLB
    string inputFileName;                   // Inputfile                         
    bool limitflag = false;                 // Boolean flag for the total number of address to be processes (False if no value is passed by the user)
    bool outmodeflag = false;               //Boolean Flag for the type of outmode (False if no output type is passed by the user)

    FILE *traceFilepointer;                 //Pointer to read trace file
    int Limit = 0;                          // Variable to store the number of address to be processed 
    int cachecapacity = 0;                  // Variable to store the TLB size
    p2AddrTr currentTrace;              

    unsigned int pthits = 0;                // Variable to store Page Table Hits
    unsigned int ptmisses = 0;              // Variable to store Page Table Miss
    unsigned int tlbhits = 0;               //Variable to store TLB Hits
    int page_size= 0 ;
    
    int frame = -1;                         // Counter for frame Number
    
    char option ;
    while ((option = getopt(argc, argv, "n:c:o:")) != -1) 
    {
        int count = 0;
        string outMode;
        switch (option) 
        {
            case 'n' :
                limitflag = true;
                Limit = atoi(optarg);
                if (Limit < 0)      //Checking if trace limit is less than 0
                {
                    cout << "Trace Limit must be a number,greater than or equal to 0"<<std::endl;
                    return(0);
                }
                break;
            case 'c' :
                count = atoi(optarg);  //Retreving the TLB Size specified by the user
                if (count < 0)        //Checking if Cache Capacity is less than 0                             
					{
						cout << "Cache capacity must be a number, greater than or equal to 0"<<std::endl;
						return(0) ;
					}
				else
					{
                        cachecapacity = count;
						break;
					}
            case 'o' :
                outmodeflag = true;
                outMode = string(optarg);             // Setting the boolean to true if the corresponding output type passed by the user
                if (outMode == std::string("bitmasks"))
                {
                    output->bitmasks = true;
                }
                else if (outMode == std::string("virtual2physical"))
                {
                    output->virtual2physical = true;
                }
                else if (outMode == std::string("v2p_tlb_pt"))
                {
                    output->v2p_tlb_pt = true;
                }
                else if (outMode == std::string("vpn2pfn"))
                {
                    output->vpn2pfn = true;
                }
                else if (outMode == std::string("offset"))
                {
                    output->offset = true;
                }
                else if (outMode == std::string("summary"))
                {
                    output->summary = true;
                }
                else
                {
                    output->summary = true;
                }
                break;
            default:
                break;
        }
    }

    if (outmodeflag == false)   // If No Output type is specified by the user make the summary output enable
    {
        output->summary = true;
    }

    int idx = optind;         
    inputFileName = argv[idx];    //Extracting the inputfile
    idx = idx+1;

    int levelCount = argc - idx;        //Calculating the Total number of levels
    int pageTableSizes[levelCount];     // Intializing an array to store the bits at each level 
    int addressSizeUsed = 0;            
    for (int i = 0; idx < argc; i++) 
    {
        pageTableSizes[i] = atoi(argv[idx++]);
        if (pageTableSizes[i] <= 0) 
        {
            cout << "Level " << i << " page table must be at least 1 bit"<<endl;
            exit(1);
        } 
        addressSizeUsed += pageTableSizes[i];       // Calculating the total number of bits from all levels 
    }

    //Error if the total number of bits from all levels grater than 28
    if (addressSizeUsed > ALL_LEVEL_BITS) 
    {
        cout << "Too many bits used in page tables "<<endl;
        exit(1);
    } 

    PAGETABLE pagetable(levelCount, pageTableSizes);  //Calling the constructor for pagetable to initalize all the variables

    TranslationLookAsideBuffer  TranslationLookAsideBuffer(&tlbdata,cachecapacity);  //Calling the constructor for TLB to initalize all the variables

    if ((traceFilepointer = fopen(inputFileName.c_str(), "rb")) == nullptr) 
    {
        cout << "Unable to open <<trace.tr>>" <<std::endl;
        exit(1);
    }

    while (!feof(traceFilepointer)) 
    {
        if (NextAddress(traceFilepointer, &currentTrace)) 
        {
            bool cachehit = false;                      //Initalzing the TLB hit to false
            bool pagetablehit = false;                  //Initalzing the PageTable hit to false

            unsigned int  logicalAddress = currentTrace.addr;                    //Extracting the Logical Address read from the file
            unsigned int *pages = pagetable.getPageateachlevel(logicalAddress);  //Getting the page number for each level for the corresponding Logical Address
            unsigned int completepage = getpagenumber(pages,levelCount);         // Converting the dynamic array to Int for the page number
            
            unsigned int physicalframe  = 0;                                                    // Intializing the Physical frame number to 0
            
            int tlbframe = TranslationLookAsideBuffer.getFrameNumber(&tlbdata,completepage);    //Extracting the frame number corresponding to the page number in TLB
            if (tlbframe == -1)                                                      // If the above function return -1 then the page number->frame number mapping not present in TLB
            {
                int tempframe =0;
                int pagetableframe = getFrame(&pagetable,currentTrace.addr);    //Searching for the page number->frame number mapping in Page Table
                if (pagetableframe == -1)                                       // If the above function return -1 then the page number->frame number mapping not present in Page Table
                {
                    frame = frame+1;                                            //Incrementing the frame number 
                    tempframe = frame;
                    pagetableInsert(&pagetable,currentTrace.addr, tempframe);        // Inserting the page number->frame number mapping in Page Table
                    ptmisses++;                                                 // Incrementing the Page Table Miss
                }
                else
                {
                    pagetablehit = true;                                        // page number->frame number mapping present in Page Table and so making the boolean flag true
                    tempframe = pagetableframe;                                 // Storing the corresponding the frame number in a temp variable
                    pthits++;                                                   // Incrementing the Page Table Hit
                }

                physicalframe = tempframe;
                TranslationLookAsideBuffer.insertintlb(&tlbdata,completepage,tempframe); //Storing the page number->frame number mapping in TLB
            }
            else
            {
                cachehit = true;                            //page number->frame number mapping present in TLB and so making the boolean flag true
                tlbhits = tlbhits + 1;                      // Incrementing the TLB Hit
                physicalframe = tlbframe;
                TranslationLookAsideBuffer.updatelife(&tlbdata,completepage);    // Updating the life of mapping in TLB
                //TranslationLookAsideBuffer.updateagemostrecent(completepage);
            }

            if (output->offset)     //If the Offset output type is enabled
            {
               unsigned int Addroffset = pagetable.getOffset(logicalAddress);
               hexnum(Addroffset);
            }
            if (output->vpn2pfn)        //If the VPN2PFN output type is enabled
            {
                report_pagemap(levelCount, pages, physicalframe);
            }
            if (output->virtual2physical)    //If the Virtual2Physical output type is enabled
            {
               unsigned int dest = pagetable.getPhysicalAddr(logicalAddress,physicalframe);
               report_virtual2physical(logicalAddress,dest);
            }
            if (output->v2p_tlb_pt)     //If the v2p_tlb_pt output type is enabled
            {
                unsigned int dest = pagetable.getPhysicalAddr(logicalAddress,physicalframe);
                report_v2pUsingTLB_PTwalk(logicalAddress, dest, cachehit, pagetablehit);
            }
        }

        if (limitflag)                              //Check if the limit for number of address to be processed was passed by the user
        {
            if (Limit <= ptmisses+pthits+tlbhits)   // If yes, then check for number of addressed processed so far
            {
                break;
            }
        }
    }
    fclose(traceFilepointer);

    int address_processed = ptmisses+pthits+tlbhits;
    page_size = pagetable.entryCount[levelCount];
    
    if (output->summary)  //If the Summary output type is enabled
    {
        report_summary(page_size,tlbhits,pthits,address_processed, frame, pagetable.pagetablesize());
    }
    
    if (output->bitmasks)  //If the BitMaks output type is enabled
    {
        report_bitmasks(levelCount,pagetable.BitmaskArray);
    }
}

//Function to call the page table insert function
int pagetableInsert(PAGETABLE *ptpointer, unsigned int logicalAddr, unsigned int frame)
{
    ptpointer->insertpagetb(logicalAddr, frame);
}

//Function to call the getFrameNumber() from class pagetable
int getFrame(PAGETABLE *ptpointer, unsigned int logicalAddr) 
{
    return ptpointer->getFrameNumber(logicalAddr);
}

//Converting the pages for each level to a single int value
unsigned int getpagenumber(unsigned int *pages, int level)
{
    string numpage;
    for (int i = 0; i < level; i++) 
    {
        numpage  = numpage + std::to_string(pages[i]); 
    }
    unsigned int pagenum= stoi(numpage);
    return pagenum;
}
