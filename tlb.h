#include <array> 

#define TLBLIFE 10  //Number of most recent mapping to be looked in TLB Cache for replacement

typedef struct {

    unsigned int *page ;    //Virtual Page NUmber
    unsigned int *frame;    //Physical Frame Number
    int *age = 0;           //Age for each Maaping stored in TLB
    bool *empty;            // Boolean flag to check if it's empty
    int tlbsize;            // TLB Size
} storetlb;

class TranslationLookAsideBuffer
{
public:
    
    int tlb_life = 0;     //Variable for age of mapping (Incremented each time a mapping is searched or inserted in TLB)

    TranslationLookAsideBuffer(storetlb *tlb , int size);   //Constructor for initializing TLB values

    //Function to search for page number in TLB Cache
    //Inputs:(TLB Pointer, Page number(to be searched in TLB))
    //Return: -1 if mapping not found, else frame number corresponding to page number
    int getFrameNumber(storetlb *tlb ,unsigned int page);  

    //Function to update the age of mapping (inserted or searched) in TLB Cache
    //Inputs:(TLB Pointer, Page number(for which the age needs to be updated))
    void updatelife(storetlb *tlb,unsigned int page);

    //Function to insert the mapping in TLB Cache
    //Inputs:(TLB Pointer, Page number, Frame Number)
    void insertintlb(storetlb *tlb, unsigned int page, int frame);

};
