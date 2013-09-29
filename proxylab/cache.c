#include <stdio.h>
#include <stdlib.h>
#include "csapp.h"
#include <time.h>
#include "cache.h"

/* cache.c Our cache is an lru cache that is based on
 * a doubly linked list. the cacheline struct has
 * the previous and next pointers, as well as a pointer
 * to the data and a pointer to the key which is the url.
 * Since the cache is a lru cache we will put the least recently
 * used cacheline at the end of the cache, and the most recently
 * used cacheline at the beginning.
 */

#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

cacheLine* head;
int TOTALCACHESIZE = 0;

/* inCache - takes uri as its argument and searches the
 * cache i.e. linked list from the head until it finds
 * the uri stored in the cache or until it reaches the end
 * of the linked list and returns NULL
*/
cacheLine* inCache(char * uri)
{
    cacheLine* node = head;
    while(node != NULL)
    {
        if(!strcmp(node->uri, uri))
            return node;
        node = node -> next;
    }
    return NULL;
}

/*findLru - searches through the linked list and uses the inserted
 * time value to find the cacheLine that was least recently used
 * or the one with the least inserted time
*/
cacheLine* findLRU()
{
    cacheLine* minNode;
    minNode = head;
    cacheLine* node = head;
    while(node != NULL)
    {
        if(node->inserted_time <  minNode->inserted_time)
            minNode = node;
        node = node->next;
    }
    return minNode;
}

/*freeLine takes the cacheLine and frees the the uri and data
 * which were malloced in connection hanler and server_to_client
 * respetively, and it frees the cacheLine itself.
*/
void freeLine(cacheLine * lru)
{
    Free(lru->uri);
    Free(lru->data);
    Free(lru);
}

/* evictFromCache takes the size that needs to be inserted as the
 * argument and evicts the least recently used cacheLines until there is
 * enough space in the cache to store this new size
*/
void evictFromCache(int size){
    while(TOTALCACHESIZE + size > MAX_CACHE_SIZE)
    {
        cacheLine * lru = findLRU();
        if(lru != NULL){
            TOTALCACHESIZE -= lru->size;

            if(lru->next == NULL && lru->prev ==NULL)
            {
                head = NULL;
                TOTALCACHESIZE=0;
            }

            else if(lru->prev == NULL)
            {
                lru->next->prev = NULL;
                lru->next = NULL;
            }

            else if(lru->next == NULL)
            {
                lru->prev->next = NULL;
                lru->prev = NULL;
            }

            else
            {
                lru->prev->next = lru->next;
                lru->next->prev = lru->prev;
                lru->next = NULL;
                lru->prev = NULL;
            }

            freeLine(lru);
        }
    }
}


/* writeToCache takes uri, data, and size as its arguments. It takes
 * care of 6 cases.
 * 1) The uri or data is NULL or the size is too large
 * 2) The size is too large and it isnt written to the cache
 * 3) The cache is empty and head is NULL
 * 4) The size causes the total size of the cache to exced max cache size
 * 5) The uri that needs to be stored is already in the cache
 * 6) The uri is not in the cache and needs to be inserted now
*/
void writeToCache(char * uri, char * data, int size)
{
    //Case 1 - just return and don't add to the cache
    if(data == NULL || uri == NULL || size <=0)
        return;

    //Case 2 - since the size is too big just free uri and data and return
    if(size > MAX_OBJECT_SIZE)
    {
        Free(uri);
        Free(data);
        return;
    }

    //Case 3 - create a new cacheLine and set the head of the cache to this
    if(TOTALCACHESIZE == 0){
        head = Malloc(sizeof(struct cacheline));
        head-> next = NULL;
        head-> prev = NULL;
        head->uri = uri;
        head -> size = size;
        head->data = data;
        head->inserted_time = time(0);
        TOTALCACHESIZE += size;
        return;
    }

    //Case 4 - evict the cache until there is enough space to add this new data
    if(TOTALCACHESIZE + size > MAX_CACHE_SIZE)
        evictFromCache(size);

    cacheLine* newLine = inCache(uri);

    //Case 5 - the uri is already in the cache so we update data, inserted time
    if(newLine != NULL)
    {
        TOTALCACHESIZE-= newLine->size;
        newLine->data = data;
        newLine->inserted_time = time(0);
        newLine->size = size;
        TOTALCACHESIZE += size;
        return;
    }

    //Case 6 - the uri is not in the cache so we create a new cacheLine and
    //add it to the begining of the cache i.e. the front of the linked list
    newLine = Malloc(sizeof(struct cacheline));
    newLine->uri = uri;
    newLine->data = data;
    newLine->prev = NULL;
    newLine->next = head;
    newLine->size = size;
    newLine->inserted_time = time(0);
    head->prev = newLine;
    head = newLine;
    TOTALCACHESIZE += size;
}


/* readFromCache takes uri as the argument and searches the cache using
 * inCache to see if the uri is already stored in the cache. If the uri is
 * not stored in the cache then return NULL otherwise update the inserted time
 * and return the cacheLine
*/
cacheLine* readFromCache(char * uri)
{
    cacheLine * node = inCache(uri);
    if(node == NULL)
        return NULL;
    else
    {
        node->inserted_time = time(0);
        return node;
    }
}