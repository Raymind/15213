void cache_simulator(int operator_address, struct Line** cache, int s, int b,
                                    counter count, int E)
{
    /* we want the set bits, tag bits.
    we can get the set bits by right shifting by the number of block bits
    and then bit masking.
    we know the number of set bits from s*/

    printf("%s %d\n", "s:", s);
    printf("%s %d\n", "b:", b);
    printf("%s %d\n", "e:", E);

    printf("%s %d\n", "Hits:", count->hits);
    printf("%s %d\n", "Misses:", count->misses);
    printf("%s %d\n", "evictions:", count->evictions);
    printf("\n");
    printf("%s,%d\n", "opadd:", operator_address);

    bool flag = true; // flag to check if cache set is full;true by default
    int set = (operator_address >> b) & bitMask((s-1), 0);
    int tagged = ((unsigned int)operator_address>>(b+s));
    printf("%s, %d\n", "setbits:", set);
    printf("%s, %d\n", "tag bits:", tagged);

    for (int i = 0; i < E; i++)
    {
    count -> timeCounter += 1; //update time counter always

    struct Line curr = cache[set][i]; // current line in cache

    int valid = curr.valid;
    if (valid == 0) //this means that the cache set isn't full
        flag = false;

    int tag = curr.tag;
    printf("%s, %d\n", "DUDE VALID", curr.valid);
    printf("%s, %d\n", "DUDE TAG", curr.tag);
    printf("%s, %d\n", "DUDE TIME_PLACED", curr.time_placed);

    if(valid == 1 && tag == tagged) //its a hit
        {
        curr.time_placed = count->timeCounter;
        //make it the time counter of the universal struct
        count -> hits += 1; //increment the number of hits by 1
        return;
        }
    }
    count -> misses += 1;
        //have to increment time counter at every step regardless
        if (flag) //cache is full then evict
        {
            count -> evictions += 1;
            int min = cache[set][0].time_placed;
            int min_index = 0;
            for (int j = 0; j < E; j++)
            {
                struct Line min_find = cache[set][j];
                if(min_find.time_placed < min)
                {
                    min = min_find.time_placed;
                    min_index = j;
                }
            }
            cache[set][min_index].valid = 1;
            cache[set][min_index].tag = tagged;
            cache[set][min_index].time_placed = count->timeCounter;
        }
        else{
            /*only a miss..
            since its not full we know that there's a valid bit of 0*/
            for (int k = 0; k < E; k++)
            {
                struct Line current = cache[set][k];
                if (current.valid == 0)
                {
                    current.valid = 1;
                    current.time_placed = count->timeCounter;
                    current.tag  = tagged;
                    return;
                }
            }
        }
}

