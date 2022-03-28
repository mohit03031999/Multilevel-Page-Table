#include "tlb.h"

TranslationLookAsideBuffer::TranslationLookAsideBuffer(storetlb *tlb , int size) 
{
    //Initaliazing all the TLB variable to zero with size(given by the user)
    tlb->page = new unsigned int[size + 1];  
    tlb->frame = new unsigned int[size + 1];
    tlb->age = new int[size + 1];
    tlb->empty = new bool[size+1];
    tlb->tlbsize = size;

    //Initalizing all the bool values of empty to true because initially the TLB is empty 
    for (int i = 0 ; i < tlb->tlbsize ; i++) 
    {
        tlb->empty[i] = true;
    }
}

int TranslationLookAsideBuffer::getFrameNumber(storetlb *tlb ,unsigned int page) 
{ 
    
    //Searching for the page in TLB Cahce
    for (int i=0;i<tlb->tlbsize;i++)
    {
        if (tlb->empty[i] == false && tlb->page[i] == page) 
        {
            return tlb->frame[i];  //If the page number is found return the frame number
        }
    }
    return -1;  //Else return -1
}

void TranslationLookAsideBuffer::updatelife(storetlb *tlb, unsigned int page) 
{ 
    //If a page number is searched in TLB and found in TLB then upadting the age for that mapping
    for (int i=0;i<tlb->tlbsize;i++) 
    {
        if (tlb->empty[i] == false && tlb->page[i] == page) 
        {
            tlb_life++;
            tlb->age[i] = tlb_life;
        }
    }
}

void TranslationLookAsideBuffer::insertintlb(storetlb *tlb, unsigned int page, int frame) 
{
    int temparray[TLBLIFE];  //Temporary array of size equal to TLBLIFE

    for (int i=0;i<tlb->tlbsize;i++) 
    {
        if (tlb->empty[i])      //If any empty position in Cache then inserting the mapping at that position
        {   
            tlb->page[i] = page;
            tlb->frame[i] = frame;
            tlb->empty[i] = false;
            tlb_life++;
            tlb->age[i] = tlb_life;
            return;
        }
    }

    tlb_life++;
    if (tlb->tlbsize <= TLBLIFE)        //If the TLBSize defined by the user is less than TLBLIFE
    {
        int minage = tlb->age[0];       //Searching for the mapping which has the least life and replacing the cuurent mapping with that mapping
        int position = 0;
        for (int i=1;i<tlb->tlbsize;i++) 
        {
            if (tlb->age[i] < minage) 
            {
                minage = tlb->age[i];
                position = i;
            }
        }
        tlb->page[position] = page;
        tlb->frame[position] = frame;
        tlb->age[position] = tlb_life;
    }

    else            //If the TLBSize defined by the user is greater than TLBLIFE
    {   
        //Searching for the least recent used mapping in the 10 most recent mapping
        int max = 0;
        for(int i=0;i<tlb->tlbsize;i++)
        {
            if(tlb->age[i]>max)
            {
                max=tlb->age[i];
            }
        }
        temparray[0]=max;
        for(int i=1;i<TLBLIFE;i++)
        {
            max=0;
            for(int j=0;j<tlb->tlbsize;j++)
            {
                if(tlb->age[j]>max && tlb->age[j]<temparray[i-1])
                {
                    max=tlb->age[j];
                }
            }
            temparray[i]=max;
        }
        int remove = temparray[TLBLIFE-1];  //The mapping which needs to be replaced

        //Replacing the current mapping with that mapping.
        int position = 0;
        for (int i=0;i<tlb->tlbsize;i++) 
        {
            if (tlb->age[i] == remove) 
            {
                position = i;
            }
        }
        tlb->page[position] = page;
        tlb->frame[position] = frame;
        tlb->age[position] = tlb_life;
    }
}