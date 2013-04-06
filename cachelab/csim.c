#include "cachelab.h"
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h> //to use booleans
#include <stdio.h>
#include <math.h> // to use inbuilt power function

int bitMask(int highbit, int lowbit) {
/* we first create a number consisting of all 1s.
   then, we left-shift by highbit and lowbit so that everything
   above highbit is a 1, and everything above lowbit is a 1. Then,
   we need to left shift the highbit number by 1 because bit counting
   begins with 0. After this, we xor hi and lo in order to obtain
   the bits in which there are 1s in high and 0s in lo.
   and then we & with lo in order to just get those particular bits.
   after that we have the bitmask as required.
*/
  int i = ~0; //0xFF
  int hi = i << highbit;
  int lo = i << lowbit;
  hi = hi << 1 ;
  return (hi^lo) & lo;
}

struct cLine{
    int valid; // to store valid bit for the cache line
    int tag; // to store tag bit for the cache line
    int time_placed; //to store at what time the cache was updated (LRU policy)
};

struct Counter{
    int hits; //total number of hits
    int misses; //total number of misses
    int evictions; //total number of evictions
    int time_counter; //to keep track of time passed (to give unique timestamp)
};

struct Counter* count_init( ){
    struct Counter* keeper = (struct Counter*) malloc(sizeof(struct Counter));
    //to initialize hits, misses, evictions, and time counter to zero in struct
    keeper -> hits = 0;
    keeper -> misses = 0;
    keeper -> evictions = 0;
    keeper -> time_counter = 0;
    return keeper;
}

struct cLine** cache_init(int S, int E) //S == pow(2,S)
{
    struct cLine** cache = (struct cLine**)malloc(S * E * sizeof(struct cLine*));
    for (int i = 0; i < S; i++) //iterating through the number of sets
    {
        cache[i] = (struct cLine*)malloc(E * sizeof(struct cLine));
        //initializing valid bit, tag bit and time_placed to 0
        cache[i] -> valid = 0;
        cache[i] -> tag = 0;
        cache[i] -> time_placed = 0;
    }
    return cache;
}

void cache_free(struct cLine** cache, int E)
{
    for (int i = 0; i < E; i ++ )
    {
        free(cache[i]); //freeing every struct that the index holds
    }
    free(cache); //freeing the cache entirely
}

void cache_simulate(int address, int s, int b, int E,
                                struct cLine** cache, struct Counter* keeper)
{
    int set_bits = (address>>b) & bitMask(s-1,0);
    /*to extract the set bits. first we right shift by number of block
    bits since we don't care about those. Then, we bitMask it to just extract
    the set bits (and zero out the tag bits) */
    int tag_bits = ((unsigned int)address>>(b+s));
    /*we extract tag_bits by simply right shifting by block+set bits
       since everything to the left of block and set bits are tag bits*/
    bool flag = true;
    //we have a boolean flag to check if the cache is full. true by default.
    for (int i = 0; i < E; i++)
    {
        /*time_counter in struct needs to get updated by 1 each time
        to ensure uniqueness.*/
        keeper->time_counter += 1;
        //we iterate over the number of cache lines
        int valid_curr = (cache[set_bits][i]).valid;
        //extract valid bit from struct
        int tag_curr = cache[set_bits][i].tag;
        //extract tag bits from struct
        if (valid_curr == 0)
            //cache is not full then set boolean flag to false
            flag = false;
        if (valid_curr == 1 && tag_curr == tag_bits)
            {
                //update time to current time
                cache[set_bits][i].time_placed = keeper->time_counter;
                //update hits by 1 if hit happens
                keeper->hits += 1;
                //return from function
                return;
            }
    }
    //otherwise it has to be a miss
    keeper -> misses += 1;
    if(flag)
    {
        //eviction is a special case of miss
        keeper -> evictions += 1;
        /*set time_min to the time in the first
        cache line's struct*/
        int time_min = cache[set_bits][0].time_placed;
        //initialize minimum index to 0
        int index_min = 0;
        //for loop through the set to find minimum time & index
        for (int i = 0; i < E; i++)
        {
            if(cache[set_bits][i].time_placed < time_min)
            {
                //update accordingly
                time_min = cache[set_bits][i].time_placed;
                index_min = i;
            }
        }
        //update the tag bits to the current tag bits
        cache[set_bits][index_min].tag = tag_bits;
        //update timer
        cache[set_bits][index_min].time_placed = keeper->time_counter;
    }
    else{
        for (int i = 0; i < E; i++)
        {
            //if there's a cache line that's empty
            if(cache[set_bits][i].valid == 0)
            {
                //set valid bit to 1 and then assign current time and tag bits
                cache[set_bits][i].valid = 1;
                cache[set_bits][i].time_placed = keeper->time_counter;
                cache[set_bits][i].tag = tag_bits;
                return;
            }
        }
    }
}

int main(int argc, char** argv)
{
    //simply initializing variables for command line argument
    int s = 0;
    int E = 0;
    int b = 0;
    //for the input trace file
    FILE * pFile;
    //to perform switch; thanks recitation slides
    int opt;

//for the input of trace file
    char x;
    int y;
    int c;

    while((opt = getopt(argc, argv, "s:E:b:c:t:")) != -1)
    {
        switch(opt)
        {
            //used atoi to convert string to int.
            //sets size
            case 's':
                s = atoi(optarg);
                break;
            //cache lines size
            case 'E':
                E = atoi(optarg);
                break;
            //block bits size
            case 'b':
                b = atoi(optarg);
                break;
            //input trace file
            case 't':
                pFile = fopen(optarg, "r");
                break;
            default:
                //otherwise
                printf("%s", "Wrong argument!\n");
                break;
        }
    }
    //to get the number of possible set bits
    int S = pow(2,s);
    //initializing the cache and the struct to hold the counters
    struct cLine** cache = cache_init(S,E);
    struct Counter* keeper = count_init( );
    //parsing the trace file
    while(fscanf(pFile, " %c %x,%d", &x, &y, &c) > 0)
    {
        switch(x)
        {
            //if load / store we call once
            case 'L':
            case 'S':
                cache_simulate(y, s, b, E, cache, keeper);
                break;
            case 'M':
            //for modify (load and store) we call once each for load and store
                cache_simulate(y, s, b, E, cache, keeper);
                cache_simulate(y, s, b, E, cache, keeper);
                break;
            default:
            //break otherwise
                break;
        }
    }
    //remember to close the file
    fclose(pFile);
    //for grading
    printSummary(keeper->hits, keeper->misses, keeper->evictions);
    //golden rule : free what you malloc
    cache_free(cache, E);
    free(keeper);
    //done! :)
    return 0;
    //#goodbyeworld
}