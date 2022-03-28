#include "map.h"
#include <iostream>

MAP::MAP(int inputsize) 
{
    MAP::map_size = inputsize;
    MAP::Mapping = new int[map_size];
    for (int i = 0; i < map_size; i++)      //Initialzing all the value of array to -1
    {
        Mapping[i] = -1;
    }
}

int MAP::mapgetFrameNumber(int pagenumber) 
{
    return Mapping[pagenumber];         //Return the frame number for the corresponding page number
}

bool MAP::insertFrameNumber(int pagenumber, int framenumber) 
{
    if (Mapping[pagenumber] != -1) 
    {
        return false;
    }
    Mapping[pagenumber] = framenumber;      //Inserting the frame number for the corresponding page number
    return true;
}

int MAP::mapsize()            // Calculate the Total Size
{
    int tempmapsize = (sizeof(Mapping) + sizeof(int) * map_size);
    return (tempmapsize);
}