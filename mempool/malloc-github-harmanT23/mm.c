/*
 * This implementation replicates the implicit list implementation
 * provided in the textbook
 * "Computer Systems - A Programmer's Perspective"
 * Blocks are never coalesced or reused.
 * Realloc is implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

//#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "derpaNet",
    /* First member's full name */
    "Harmanprit Tatla",
    /* First member's email address */
    "harman.tatla@mail.utoronto.ca",
    /* Second member's full name (do not modify this as this is an individual lab) */
    "",
    /* Second member's email address (do not modify this as this is an individual lab)*/
    ""
};

/*************************************************************************
 * Basic Constants and Macros
 * You are not required to use these macros but may find them helpful.
 *************************************************************************/
#define WSIZE       sizeof(void *)            /* word size (bytes) */
#define DSIZE       (2 * WSIZE)            /* doubleword size (bytes) */
#define CHUNKSIZE   (1<<9)      /* initial heap size (bytes) */

#define MAX(x,y) ((x) > (y)?(x) :(y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p)          (*(uintptr_t *)(p))
#define PUT(p,val)      (*(uintptr_t *)(p) = ((uintptr_t)val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)     (GET(p) & ~(DSIZE - 1))
#define GET_ALLOC(p)    (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)        ((char *)(bp) - WSIZE)
#define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

#define MAX_SIZES 9
#define INDEX_OFFSET 4
#define MAX_CHUNK_SIZE (1 << 12)
#define MIN_CHUNK_SIZE (1 << 5)

//Globals
void* heap_listp = NULL;
void* seg_lists[9] = { 0 };
/***********************Helper Functions***********************************/
static inline int mm_check();
static inline size_t getPowerOfTwoExp(size_t val);
static inline size_t nextPowerOfTwoIndex(size_t sizeToRound);
static inline void *searchList(size_t index, size_t reqSize);
static inline void insertIntoList(void* bp, size_t index);
static inline void removeFromList(void* bp, size_t index);
static inline size_t SplitBlockPolicy(void* bp, size_t asize);
static inline void SplitBlock(void* bp, size_t asize);
static inline void* SplitBlockLogical(void* bp, size_t asize);
static inline void *coalesce(void *bp);
static inline void *expandRight(void *bp, size_t asize);
static inline void *extend_heap(size_t words);
static inline void *find_fit(size_t asize);
static inline void *place(void* bp, size_t asize);
/*******************************************************************************/

/**********************************************************
 * mm_init
 * Initialize the heap, including "allocation" of the
 * prologue and epilogue
 **********************************************************/
int mm_init(void)
{
    seg_lists[0] = NULL; //Holds blocks of size > 4096
    seg_lists[1] = NULL; //Size 32
    seg_lists[2] = NULL; //Size Range: 33 - 64
    seg_lists[3] = NULL; //Size Range: 65 - 128
    seg_lists[4] = NULL; //Size Range: 129 - 256
    seg_lists[5] = NULL; //Size Range: 257 - 512
    seg_lists[6] = NULL; //Size Range: 513 - 1024
    seg_lists[7] = NULL; //Size Range: 1025 - 2048
    seg_lists[8] = NULL; //Size Range: 2049 - 4096
    
    if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp, 0);                         // alignment padding
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));   // prologue header
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));   // prologue footer
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));    // epilogue header
    
    void* extendBlock = NULL;
//    for (int i = 5; i < 10; i++)
//    {
//        if ((extendBlock = extend_heap((1 << i)/WSIZE)) != NULL)
//        {
//            insertIntoList(extendBlock, i - INDEX_OFFSET);
//        }
//    }
    if ((extendBlock = extend_heap(CHUNKSIZE/WSIZE)) != NULL)
    {
        insertIntoList(extendBlock, 5);
    }
    
    return 0;
}

/**********************************************************
 * getPowerOfTwoExp
 * Returns exponent of a power of two number otherwise asserts
 **********************************************************/
static inline size_t getPowerOfTwoExp(size_t val)
{
    size_t i = 1;
    size_t exponent = 0;
    
    while (!(i & val))
    {
        i = (i << 1);
        exponent++;
    }
    return exponent;
}

/********************************************************************
 * nextPowerOfTwo
 * Returns next rounded power of 2 (i.e. input 5 output 8)
 * Note: Uses Sean Anderson's Algorithm:
 * https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
 **********************************************************************/
static inline size_t nextPowerOfTwoIndex(size_t sizeToRound)
{
    sizeToRound--;
    sizeToRound |= sizeToRound >> 1;
    sizeToRound |= sizeToRound >> 2;
    sizeToRound |= sizeToRound >> 4;
    sizeToRound |= sizeToRound >> 8;
    sizeToRound |= sizeToRound >> 16;
    sizeToRound++;
    return getPowerOfTwoExp(sizeToRound) - INDEX_OFFSET;
}

/**********************************************************
 * searchList
 * Given list index, requested size, search given list until
 * we find a first fit (i.e. foundSize >= reqSize).
 *
 * Returns ptr to first fit block if found otherwise NULL
 **********************************************************/
static inline void* searchList(size_t index, size_t reqSize)
{
    void* bp = seg_lists[index]; //Get header of LL
    
    while (GET_SIZE(HDRP(bp)) < reqSize && (void*)GET(bp) != NULL)
    {
        bp = (void*)GET(bp);
    }
    if (GET_SIZE(HDRP(bp)) >= reqSize)
    {
        return bp;
    }
    return NULL;
}

/**********************************************************
 * insertIntoList
 * Insert into segregated linked list. Add entry to front.
 *
 * bp points to first payload block of free block to
 * insert into head of free list specified by index.
 *
 * First payload block of free block is next ptr.
 **********************************************************/
static inline void insertIntoList(void* bp, size_t index)
{
//    printf("****Inserting****\n");
//    printf("bp %p index %d\n");
    if (seg_lists[index] == NULL) //Empty list case
    {
        seg_lists[index] = bp; //set head
        PUT(bp, 0); //set next ptr to NULL
        PUT(bp + WSIZE, 0); //set prev ptr to NULL
        
//        printf("bp %p index %d\n");
    }
    else //Non-empty
    {
        PUT(seg_lists[index] + WSIZE, bp); //Set the former head's prev ptr to new head.
        PUT(bp, seg_lists[index]); //next ptr of bp now points to former head of LL
        PUT(bp + WSIZE, 0); //prev ptr of new head now points to NULL
        seg_lists[index] = bp; //new head of LL
    }
}

/**********************************************************
 * removeFromList
 * Removes a linked list element from a list
 specified using index.
 
 bp is list element to remove.
 **********************************************************/
static inline void removeFromList(void* bp, size_t index)
{
    /* Three Scenarios when deleting a block from free list
     *      1. Head of free list -->  prev == NULL && next != NULL
     *      2. Middle of free list --> prev != NULL && next != NULL
     *      3. Tail of free list --> prev != NULL && next == NULL
     *      4. One element free list --> prev == NULL && next == NULL
     */
    void* currNode = bp;
    
    void* prev = (void*)GET(currNode + WSIZE);
    void* next = (void*)GET(currNode);
//
//    printf("*****REMOVE******\n");
//    printf("prev %p curr %p next %p index %d\n", prev, bp, next, index);
//    printf("bp size %lu\n", GET_SIZE(HDRP(bp)));
    if (prev == NULL && next != NULL) //Head
    {
        PUT(next + WSIZE, 0); //next's prev ptr now points to NULL
        seg_lists[index] = next;
    }
    else if (prev != NULL && next != NULL) //Middle
    {
        PUT(prev, next); //prev nxt ptr now points to next's nxt ptr
        PUT(next + WSIZE, prev); // next's prev ptr now points to prev's next ptr
    }
    else if (prev != NULL && next == NULL) //Tail
    {
        PUT(prev, 0); //prev's next ptr now points to NULL
    }
    else if (prev == NULL && next == NULL) //One element free list
    {
        seg_lists[index] = NULL;
    }
    else
    {
        assert(0); //should never happen
    }
}

/**********************************************************
 * SplitBlockPolicy
 * Evaluates whether a block should be split
 * ss
 * bp points first payload of free block
 * asize is the size of the block we want to fit in bp block
 *
 * Note: asize includes overhead of footer + header
 **********************************************************/
static inline size_t SplitBlockPolicy(void* bp, size_t asize)
{
    size_t bsize = GET_SIZE(HDRP(bp));
    //If > 32 we know it will be DSIZE aligned.
    return (bsize - asize) > 32 ? 1:0;
}

/**********************************************************
 * SplitBlock
 * Split block into two parts. Left chunk is size asize
 * right chunk is assumed to be a multiple of 32. Finally
 * inserts unused chunk into respective list and return
 * unalloc'd left chunk.
 *
 * bp points to header of free block
 * asize is the size of the block we want to fit in bp block
 *
 * Note: asize includes overhead of footer + header
 **********************************************************/
static inline void SplitBlock(void* bp, size_t asize)
{
    size_t bsize = GET_SIZE(HDRP(bp));
    
    size_t rightChunkSize = bsize - asize;
    
    char* leftChunkp = (char*)bp; //ptr to first pay load of left chunk
    char* rightChunkp = (char*)bp + asize; //ptr to first payload of right chunk
    
    //Adjust parameters of left chunk.
    PUT(HDRP(leftChunkp), PACK(asize, 1));
    PUT(FTRP(leftChunkp), PACK(asize, 1));
    
    //Adjust parameters of right chunk
    PUT(HDRP(rightChunkp), PACK(rightChunkSize, 0));
    PUT(FTRP(rightChunkp), PACK(rightChunkSize, 0));
    PUT(rightChunkp, 0); //nxt ptr
    PUT(rightChunkp + WSIZE, 0); //prev ptr
    
    //rightChunkp = coalesce(rightChunkp);
    
    /*
     * Determine which list this free block belongs to:
     *      1. Power of two and fits within specified ranges.
     *      2. Wildcard insert into wildcard slot
     */
    if (rightChunkSize >= MIN_CHUNK_SIZE && rightChunkSize <= MAX_CHUNK_SIZE)
    {
        size_t index_i = nextPowerOfTwoIndex(rightChunkSize);
        if (seg_lists[index_i] == NULL) seg_lists[index_i] = rightChunkp;
        else insertIntoList(rightChunkp, index_i);
    }
    else
    {
        if ((seg_lists[0] == NULL)) seg_lists[0] = rightChunkp;
        else insertIntoList(rightChunkp, 0);
    }
}


/**********************************************************
 * SplitBlockLogical
 * Split block into two parts. Left chunk is size asize
 * right chunk is assumed to be a multiple of 32. Finally
 * inserts unused chunk into respective list and return
 * unalloc'd left chunk.
 *
 * bp points to header of free block
 * asize is the size of the block we want to fit in bp block
 *
 * Note: asize includes overhead of footer + header
 **********************************************************/
static inline void* SplitBlockLogical(void* bp, size_t asize)
{
    size_t bsize = GET_SIZE(HDRP(bp));
    size_t rightChunkSize = bsize - asize;
    
//    printf("*****Splitting******\n");
//    printf("avail size %lu req size %lu carve size %lu\n", bsize, asize, rightChunkSize);
//
    //Alloc right chunk
    if (asize >= 128) //Tweakable value
    {
        char* leftChunkp = (char*)bp; //ptr to first pay load of left chunk
        char* rightChunkp = (char*)bp + rightChunkSize; //ptr to first payload of right chunk
//        printf("left %p and right %p\n", leftChunkp, rightChunkp);
        //Adjust parameters of left chunk.
        PUT(HDRP(rightChunkp), PACK(asize, 1));
        PUT(FTRP(rightChunkp), PACK(asize, 1));
        
        //Adjust parameters of left chunk
        PUT(HDRP(leftChunkp), PACK(rightChunkSize, 0));
        PUT(FTRP(leftChunkp), PACK(rightChunkSize, 0));
        PUT(leftChunkp, 0); //nxt ptr
        PUT(leftChunkp + WSIZE, 0); //prev ptr
        
        //leftChunkp = coalesce(leftChunkp);
        
        /*
         * Determine which list this free block belongs to:
         *      1. Power of two and fits within specified ranges.
         *      2. Wildcard insert into wildcard slot
         */
        
        if (rightChunkSize >= MIN_CHUNK_SIZE && rightChunkSize <= MAX_CHUNK_SIZE)
        {
            size_t index_i = nextPowerOfTwoIndex(rightChunkSize);
            if (seg_lists[index_i] == NULL) seg_lists[index_i] = leftChunkp;
            else insertIntoList(leftChunkp, index_i);
        }
        else
        {
            if ((seg_lists[0] == NULL)) seg_lists[0] = leftChunkp;
            else insertIntoList(leftChunkp, 0);
        }
        
        return rightChunkp;
        
    }
    
    //Alloc left chunk
    else
    {
        char* leftChunkp = (char*)bp; //ptr to first pay load of left chunk
        char* rightChunkp = (char*)bp + asize; //ptr to first payload of right chunk
        
        //Adjust parameters of left chunk.
        PUT(HDRP(leftChunkp), PACK(asize, 1));
        PUT(FTRP(leftChunkp), PACK(asize, 1));
        
        //Adjust parameters of right chunk
        PUT(HDRP(rightChunkp), PACK(rightChunkSize, 0));
        PUT(FTRP(rightChunkp), PACK(rightChunkSize, 0));
        PUT(rightChunkp, 0); //nxt ptr
        PUT(rightChunkp + WSIZE, 0); //prev ptr
        
        //rightChunkp = coalesce(rightChunkp);
        
        /*
         * Determine which list this free block belongs to:
         *      1. Power of two and fits within specified ranges.
         *      2. Wildcard insert into wildcard slot
         */
        if (rightChunkSize >= MIN_CHUNK_SIZE && rightChunkSize <= MAX_CHUNK_SIZE)
        {
            size_t index_i = nextPowerOfTwoIndex(rightChunkSize);
            if (seg_lists[index_i] == NULL) seg_lists[index_i] = rightChunkp;
            else insertIntoList(rightChunkp, index_i);
        }
        else
        {
            if ((seg_lists[0] == NULL)) seg_lists[0] = rightChunkp;
            else insertIntoList(rightChunkp, 0);
        }
        
        //mm_check();
        
        return leftChunkp;
    }
}

/**********************************************************
 * coalesce
 * Covers the 4 cases discussed in the text:
 * - both neighbours are allocated
 * - the next block is available for coalescing
 * - the previous block is available for coalescing
 * - both neighbours are available for coalescing
 **********************************************************/
static inline void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));
    size_t index;
    
//    printf("****COALESCING****\n");
//    printf("prev %p curr %p next %p\n", PREV_BLKP(bp), bp,  NEXT_BLKP(bp));
//    printf("prev sz %lu curr sz %lu next sz %lu\n", GET_SIZE(HDRP(PREV_BLKP(bp))), GET_SIZE(HDRP(bp)),  GET_SIZE(HDRP(NEXT_BLKP(bp))));
    //int dc = mm_check();
    
    if (prev_alloc && next_alloc) {       /* Case 1 */
        //int dc = mm_check();
        return bp;
    }
    
    else if (prev_alloc && !next_alloc) { /* Case 2 */
        //Remove next block from free list
        size_t nextSize = GET_SIZE(HDRP(NEXT_BLKP(bp)));
        
        if (nextSize >= MIN_CHUNK_SIZE && nextSize <= MAX_CHUNK_SIZE)
        {
            index = nextPowerOfTwoIndex(nextSize);
            removeFromList(NEXT_BLKP(bp), index);
        }
        else
        {
            removeFromList(NEXT_BLKP(bp), 0);
        }
        
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        //int dc = mm_check();
        return (bp);
    }
    
    else if (!prev_alloc && next_alloc) { /* Case 3 */
        //Remove prev block from free list
        size_t prevSize = GET_SIZE(HDRP(PREV_BLKP(bp)));
        
        if (prevSize >= MIN_CHUNK_SIZE && prevSize <= MAX_CHUNK_SIZE)
        {
            index = nextPowerOfTwoIndex(prevSize);
            removeFromList(PREV_BLKP(bp), index);
        }
        else
        {
            removeFromList(PREV_BLKP(bp), 0);
        }
        
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        //int dc = mm_check();
        return (PREV_BLKP(bp));
    }
    
    else {            /* Case 4 */
        
        size_t nextSize = GET_SIZE(HDRP(NEXT_BLKP(bp)));
        size_t prevSize = GET_SIZE(HDRP(PREV_BLKP(bp)));
        //int dc = mm_check();
        if (nextSize >= MIN_CHUNK_SIZE && nextSize <= MAX_CHUNK_SIZE)
        {
            index = nextPowerOfTwoIndex(nextSize);
            removeFromList(NEXT_BLKP(bp), index);
        }
        else
        {
            removeFromList(NEXT_BLKP(bp), 0);
        }
        
        if (prevSize >= MIN_CHUNK_SIZE && prevSize <= MAX_CHUNK_SIZE)
        {
            index = nextPowerOfTwoIndex(prevSize);
            removeFromList(PREV_BLKP(bp), index);
        }
        else
        {
            removeFromList(PREV_BLKP(bp), 0);
        }
        
        
        size += GET_SIZE(HDRP(PREV_BLKP(bp)))  +
        GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size,0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size,0));
        
        return (PREV_BLKP(bp));
    }
    
    return NULL;
}

/**********************************************************
 * Expand Right
 * Determines if the given ptr can be coalesced
 * with its right neighbour to expand ptr to given
 * size or more
 **********************************************************/
static inline void *expandRight(void *bp, size_t asize)
{
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t nextSize = GET_SIZE(HDRP(NEXT_BLKP(bp)));
    
    //If alloc'd and next size non-zero return NULL.
    if (next_alloc && nextSize) return NULL;
    
    size_t size = GET_SIZE(HDRP(bp));
    
    if (!next_alloc && ((size + nextSize) >= asize))
    {
        if (nextSize >= MIN_CHUNK_SIZE && nextSize <= MAX_CHUNK_SIZE)
        {
            size_t index = nextPowerOfTwoIndex(nextSize);
            removeFromList(NEXT_BLKP(bp), index);
        }
        else
        {
            removeFromList(NEXT_BLKP(bp), 0);
        }
        
        size += nextSize;
        PUT(HDRP(bp), PACK(size, 1));
        PUT(FTRP(bp), PACK(size, 1));
        return (bp);
    }
    
    else
    {
        //Extend heap and coalesce
        void* extendedBlock = NULL;
        if ((extendedBlock = extend_heap((asize - size)/WSIZE)) != NULL)
        {
            //Coalesce current and next block.
            size += GET_SIZE(HDRP(extendedBlock));
            PUT(HDRP(bp), PACK(size, 1));
            PUT(FTRP(bp), PACK(size, 1));
            return (bp);
        }
    }
    
    return NULL;
}

/**********************************************************
 * extend_heap
 * Extend the heap by "words" words, maintaining alignment
 * requirements of course. Free the former epilogue block
 * and reallocate its new header
 **********************************************************/
static inline void *extend_heap(size_t words)
{
    char *bp;
    size_t size;
    
    /* Allocate an even number of words to maintain alignments */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if ( (bp = mem_sbrk(size)) == (void *)-1 )
        return NULL;
    
    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));                // free block header
    PUT(bp, 0);                                  // free block next ptr NULL
    PUT(bp + WSIZE, 0);                          // free block prev ptr NULL
    PUT(FTRP(bp), PACK(size, 0));                // free block footer
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));        // new epilogue header
    return bp;
}


/**********************************************************
 * find_fit
 * Traverse the heap searching for a block to fit asize
 * Return NULL if no free blocks can handle that size
 * Assumed that asize is aligned
 **********************************************************/
static inline void * find_fit(size_t asize)
{
    void *bp;
    
    //Determine if requested size falls in size ranges for segregated free list
    if (asize >= MIN_CHUNK_SIZE && asize <= MAX_CHUNK_SIZE)
    {
        size_t index = nextPowerOfTwoIndex(asize);
        if (seg_lists[index] != NULL && (bp = searchList(index, asize)) != NULL)
        {
            removeFromList(bp, index); //remove it from free list.
            return bp;
        }
        /* Go through each free list in ascending order of size until
         * you find one that is non-empty and has required element of size.
         * */
        else
        {
            size_t i = ++index;
            for (; i < MAX_SIZES; i++)
            {
                if (seg_lists[i] != NULL && (bp = searchList(i, asize)) != NULL) break;
            }
            if (i != MAX_SIZES)
            {
                removeFromList(bp, i); //remove it from free list.
                return bp;
            }
        }
    }
    
    /* Check wildcard slot which contains chunks for requests >= MAX_CHUNK_SIZE.
     * Do this when above search fails for in range size request or for out of range.
     * */
    if (seg_lists[0] != NULL && (bp = searchList(0, asize)) != NULL)
    {
        removeFromList(bp, 0); //remove it from free list.
        return bp;
    }
    
    return NULL;
}

/**********************************************************
 * place
 * Mark the block as allocated
 **********************************************************/
static inline void* place(void* bp, size_t asize)
{
    size_t bsize = GET_SIZE(HDRP(bp));
    //Decide whether to split
    if (SplitBlockPolicy(bp, asize))
    {
        //Splits into left/right chunks. Right inserted into corresponding free list.
        return SplitBlockLogical(bp, asize);
    }
    else
    {
        PUT(HDRP(bp), PACK(bsize, 1));
        PUT(FTRP(bp), PACK(bsize, 1));
        return bp;
    }
    
    return NULL;
}

/**********************************************************
 * mm_free
 * Free the block and coalesce with neighbouring blocks
 **********************************************************/
void mm_free(void *bp)
{
//    printf("***Free***\n");
//    printf("pointer is %p\n", bp);
    if(bp == NULL){
        return;
    }
    size_t size = GET_SIZE(HDRP(bp));
    
    void* freeBlock;
    
    //Unallocate block
    PUT(HDRP(bp), PACK(size,0));
    PUT(bp, 0); //Set bps next ptr to NULL
    PUT(bp + WSIZE, 0); //Set bps next ptr to NULL
    PUT(FTRP(bp), PACK(size,0));
        
    if ((freeBlock = coalesce(bp)) != NULL)
    {
        size = GET_SIZE(HDRP(freeBlock));
        if (size >= MIN_CHUNK_SIZE && size <= MAX_CHUNK_SIZE)
        {
            size_t index = nextPowerOfTwoIndex(size);
            insertIntoList(freeBlock, index);
        }
        else
        {
            insertIntoList(freeBlock, 0);
        }
    }
    
    //int dc = mm_check();
}


/**********************************************************
 * mm_malloc
 * Allocate a block of size bytes.
 * The type of search is determined by find_fit
 * The decision of splitting the block, or not is determined
 *   in place(..)
 * If no block satisfies the request, the heap is extended
 **********************************************************/
void *mm_malloc(size_t size)
{
    size_t asize; /* adjusted block size */
    size_t extendsize; /* amount to extend heap if no fit */
    char * bp;
    
    /* Ignore spurious requests */
    if (size == 0)
        return NULL;
    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)
        asize = 32;
    
    else
        asize = ((size + 31) >> 4) << 4;
    
    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL) {
        bp = place(bp, asize);
        //int dc = mm_check();
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;
    bp = place(bp, asize);
   // int dc2 = mm_check();
    return bp;
}

/**********************************************************
 * mm_realloc
 * Implemented simply in terms of mm_malloc and mm_free
 *********************************************************/
void *mm_realloc(void *ptr, size_t size)
{
    /* If size == 0 then this is just free, and we return NULL. */
    if(size == 0){
        mm_free(ptr);
        return NULL;
    }
    /* If oldptr is NULL, then this is just malloc. */
    if (ptr == NULL)
        return (mm_malloc(size));
    
    void *oldptr = ptr;
    size_t oldPtrSize = GET_SIZE(HDRP(ptr));
    size_t asize = ((size + 31) >> 4) << 4;
    
    /************************************
     * Possibilities regarding size
     * 1. size < oldPtrSize
     * 2. size = oldPtrSize
     * 3. size > oldPtrSize
     ***************************************
     */
    if (asize == oldPtrSize)
    {
        return ptr;
    }
    else if (asize < oldPtrSize)
    {
        //See if we can split
        if (SplitBlockPolicy(oldptr, asize))
        {
            SplitBlock(oldptr, asize);
        }
        return ptr;
    }
    else
    {
        if ((oldptr = expandRight(ptr, asize)) != NULL)
        {
            return oldptr;
        }
        else
        {
            oldptr = ptr;
            void *newptr;
            size_t copySize;
            
            newptr = mm_malloc(size);
            
            if (newptr == NULL)
                return NULL;
            
            /* Copy the old data. */
            copySize = oldPtrSize;
            
            if (size < copySize)
                copySize = size;
            memcpy(newptr, oldptr, copySize);
            mm_free(oldptr);
            return newptr;
        }
    }
    return NULL;
}

/**********************************************************
 * mm_check
 * Check the consistency of the memory heap
 * Return nonzero if the heap is consistant.
 *********************************************************/
int mm_check(void){
    for (char* bp = heap_listp + 32; GET_SIZE(HDRP(bp)) != 0; bp = NEXT_BLKP(bp)) {
        printf("address [%p], header: [size: %zu, alloc: %zu], footer: [size: %zu, alloc: %zu]\n", bp, GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)), GET_SIZE(FTRP(bp)), GET_ALLOC(FTRP(bp)));
    }
    return 1;
}
