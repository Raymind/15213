struct cacheline{
    struct cacheline* next;
    struct cacheline* prev;
    time_t inserted_time;
    int size;
    char* uri;
    char* data;
 };
typedef struct cacheline cacheLine;

cacheLine* findLRU();
void freeLine(cacheLine * lru);
void evictFromCache(int size);
void writeToCache(char * uri, char * data, int size);
cacheLine* readFromCache(char * uri);
cacheLine* inCache(char * uri);