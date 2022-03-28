class MAP 
{
private:
    int map_size;        //Size of Map
    int *Mapping;       // Frame Number for each map

public:
    //Constructor of class MAP to initalize all the values
    //Input: (Size of Map)
    MAP(int);

    //Function to get the frame number for the corresponding page number
    //Inputs:(Page Number)
    //Retuns: Frame Number (Corresponding to the page number)
    int mapgetFrameNumber(int pageNumber);

    //Function to Insert the frame number
    //Inputs: (Page Number, Frame Number)
    bool insertFrameNumber(int pageNumber, int frameNumber);

    //Calculating the total size
    //Retun: The size of mapping at leaf level
    int mapsize();
};
