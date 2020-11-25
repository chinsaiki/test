
//The extent_hooks_t structure comprises function pointers which are described individually below. jemalloc uses these 
//functions to manage extent lifetime, which starts off with allocation of mapped committed memory, in the simplest 
//case followed by deallocation. However, there are performance and platform reasons to retain extents for later reuse. 
//Cleanup attempts cascade from deallocation to decommit to forced purging to lazy purging, which gives the extent 
//management functions opportunities to reject the most permanent cleanup operations in favor of less permanent (and 
//often less costly) operations. All operations except allocation can be universally opted out of by setting the hook 
//pointers to NULL, or selectively opted out of by returning failure. Note that once the extent hook is set, the 
//structure is accessed directly by the associated arenas, so it must remain valid for the entire lifetime of the arenas.
//
//
//--------------------------------------------------------------------------------------------------------------------
typedef void *(*extent_alloc_t)( extent_hooks_t *extent_hooks,
	  							  void *new_addr,
	  							  size_t size,
	  							  size_t alignment,
	  							  bool *zero,
	  							  bool *commit,
	  							  unsigned arena_ind);


//--------------------------------------------------------------------------------------------------------------------
//
//An extent allocation function conforms to the extent_alloc_t type and upon success returns a pointer to size bytes of 
//mapped memory on behalf of arena arena_ind such that the extent's base address is a multiple of alignment, as well as 
//setting *zero to indicate whether the extent is zeroed and *commit to indicate whether the extent is committed. Upon 
//error the function returns NULL and leaves *zero and *commit unmodified. The size parameter is always a multiple of 
//the page size. The alignment parameter is always a power of two at least as large as the page size. Zeroing is 
//mandatory if *zero is true upon function entry. Committing is mandatory if *commit is true upon function entry. If 
//new_addr is not NULL, the returned pointer must be new_addr on success or NULL on error. Committed memory may be 
//committed in absolute terms as on a system that does not overcommit, or in implicit terms as on a system that 
//overcommits and satisfies physical memory needs on demand via soft page faults. Note that replacing the default 
//extent allocation function makes the arena's arena.<i>.dss setting irrelevant.
//
//
//--------------------------------------------------------------------------------------------------------------------
typedef bool (*extent_dalloc_t)( extent_hooks_t *extent_hooks,
							  void *addr,
							  size_t size,
							  bool committed,
							  unsigned arena_ind);
//--------------------------------------------------------------------------------------------------------------------
//
//An extent deallocation function conforms to the extent_dalloc_t type and deallocates an extent at given addr and size 
//with committed/decommited memory as indicated, on behalf of arena arena_ind, returning false upon success. If the 
//function returns true, this indicates opt-out from deallocation; the virtual memory mapping associated with the 
//extent remains mapped, in the same commit state, and available for future use, in which case it will be automatically 
//retained for later reuse.
//
//
//--------------------------------------------------------------------------------------------------------------------
typedef void (*extent_destroy_t)( extent_hooks_t *extent_hooks,
								  void *addr,
								  size_t size,
								  bool committed,
								  unsigned arena_ind);
//--------------------------------------------------------------------------------------------------------------------
//
//An extent destruction function conforms to the extent_destroy_t type and unconditionally destroys an extent at given 
//addr and size with committed/decommited memory as indicated, on behalf of arena arena_ind. This function may be 
//called to destroy retained extents during arena destruction (see arena.<i>.destroy).
//
//
//--------------------------------------------------------------------------------------------------------------------
typedef bool (*extent_commit_t)( extent_hooks_t *extent_hooks,
								  void *addr,
								  size_t size,
								  size_t offset,
								  size_t length,
								  unsigned arena_ind);
//--------------------------------------------------------------------------------------------------------------------
//
//An extent commit function conforms to the extent_commit_t type and commits zeroed physical memory to back pages 
//within an extent at given addr and size at offset bytes, extending for length on behalf of arena arena_ind, returning 
//false upon success. Committed memory may be committed in absolute terms as on a system that does not overcommit, or 
//in implicit terms as on a system that overcommits and satisfies physical memory needs on demand via soft page faults. 
//If the function returns true, this indicates insufficient physical memory to satisfy the request.
//
//
//--------------------------------------------------------------------------------------------------------------------
typedef bool (*extent_decommit_t)( extent_hooks_t *extent_hooks,
								  void *addr,
								  size_t size,
								  size_t offset,
								  size_t length,
								  unsigned arena_ind);
//--------------------------------------------------------------------------------------------------------------------
//
//An extent decommit function conforms to the extent_decommit_t type and decommits any physical memory that is backing 
//pages within an extent at given addr and size at offset bytes, extending for length on behalf of arena arena_ind, 
//returning false upon success, in which case the pages will be committed via the extent commit function before being 
//reused. If the function returns true, this indicates opt-out from decommit; the memory remains committed and 
//available for future use, in which case it will be automatically retained for later reuse.
//
//
//--------------------------------------------------------------------------------------------------------------------
typedef bool (*extent_purge_t)( extent_hooks_t *extent_hooks,
							  void *addr,
							  size_t size,
							  size_t offset,
							  size_t length,
							  unsigned arena_ind);
//--------------------------------------------------------------------------------------------------------------------
//
//An extent purge function conforms to the extent_purge_t type and discards physical pages within the virtual memory 
//mapping associated with an extent at given addr and size at offset bytes, extending for length on behalf of arena 
//arena_ind. A lazy extent purge function (e.g. implemented via madvise(...MADV_FREE)) can delay purging indefinitely 
//and leave the pages within the purged virtual memory range in an indeterminite state, whereas a forced extent purge 
//function immediately purges, and the pages within the virtual memory range will be zero-filled the next time they are 
//accessed. If the function returns true, this indicates failure to purge.
//
//
//--------------------------------------------------------------------------------------------------------------------
typedef bool (*extent_split_t)( extent_hooks_t *extent_hooks,
							  void *addr,
							  size_t size,
							  size_t size_a,
							  size_t size_b,
							  bool committed,
							  unsigned arena_ind);
//--------------------------------------------------------------------------------------------------------------------
//
//An extent split function conforms to the extent_split_t type and optionally splits an extent at given addr and size 
//into two adjacent extents, the first of size_a bytes, and the second of size_b bytes, operating on committed/
//decommitted memory as indicated, on behalf of arena arena_ind, returning false upon success. If the function returns 
//true, this indicates that the extent remains unsplit and therefore should continue to be operated on as a whole.
//
//
//--------------------------------------------------------------------------------------------------------------------
typedef bool (*extent_merge_t)( extent_hooks_t *extent_hooks,
							  void *addr_a,
							  size_t size_a,
							  void *addr_b,
							  size_t size_b,
							  bool committed,
							  unsigned arena_ind);
//--------------------------------------------------------------------------------------------------------------------
//
//An extent merge function conforms to the extent_merge_t type and optionally merges adjacent extents, at given addr_a 
//and size_a with given addr_b and size_b into one contiguous extent, operating on committed/decommitted memory as 
//indicated, on behalf of arena arena_ind, returning false upon success. If the function returns true, this indicates 
//that the extents remain distinct mappings and therefore should continue to be operated on independently.

//arena.<i>.extent_hooks (extent_hooks_t *) rw
//Get or set the extent management hook functions for arena <i>. The functions must be capable of operating on all 
//extant extents associated with arena <i>, usually by passing unknown extents to the replaced functions. In practice, 
//it is feasible to control allocation for arenas explicitly created via arenas.create such that all extents originate 
//from an application-supplied extent allocator (by specifying the custom extent hook functions during arena creation). 
//However, the API guarantees for the automatically created arenas may be relaxed -- hooks set there may be called in a 
//"best effort" fashion; in addition there may be extents created prior to the application having an opportunity to 
//take over extent allocation.
typedef extent_hooks_s extent_hooks_t;
struct extent_hooks_s {
    extent_alloc_t  *alloc;
    extent_dalloc_t  *dalloc;
    extent_destroy_t *destroy;
    extent_commit_t  *commit;
    extent_decommit_t *decommit;
    extent_purge_t  *purge_lazy;
    extent_purge_t  *purge_forced;
    extent_split_t  *split;
    extent_merge_t  *merge;
};


