# Kernel Data Structures II and Kernel Module

Recall LinkedList

struct list_head {
  struct list_head *next, *pev;
}

struct car {
  struct list_head list;
  unsigned int max_speed;
  unsigned int price_in_dollars;
};

struct list_head my_car_list;


struct car *my_car = list_entry(car_list_ptr, struct car, list);


How to get the pointer of the containing data structure (struct car) from its list?
	- Use list_entry(ptr, type, member)
	- Just a pointer arithmetic

Iterating over a LL: O(n)

Struct list_head p;

Struct car *current_car;

list_for_each (p, &my_carl_list) {
  current_car = list_entry(p, struct car, list);
  printk(KERN_INFO "Price: %ld\n", current_car->price_in_dollars);
}

// simpler
list_for_each_entry(current_car, &my_car_list, list) {
  printk(KERN_INFO "Price: %ld\n", current_car->price_in_dollars);
}


![Recap: Linux Hash Table](./images/recap_linux_hash_table.png)

![Recap: Linux red-black tree (or rbtree)](./images/recap_linux_red_black_tree.png)


Agenda

Memory allocation in the kernel
More kernel data structures
	- Radix tree
	- Xarray
	- Bitmap
Kernel module


Memory allocation in kernel
Two types of memory allocation functions are provided
	- kmalloc(size, gfp_mask) 
		○ kfree(address)
	- vmalloc(size)
		○ vfree(address)
Gfp_mask is used to specify
	- Which types of pages can be allocated
	- Whether the allocator can wait for more memory to be freed
Frequently used: gfp_mask
	- GFP_KERNEL: a caller might sleep
	- GFP_ATOMIC: prevent a caller to sleep -> higher chance of failure 

Kmalloc(size, gfp_mask)
Allocate virtually and physically contiguous memory
	- Where physically contiguous memory necessary?
	
![kmalloc() example](./images/kmalloc_example.png)

vmalloc(size)
Allocate memory that is virtually contiguous, but not physically contiguous
No size limit other than the amount of free RAM
Memory allocator might sleep to get more free memory
Unit of allocation is a page (4KB)

![Radix tree (or Trie)](./images/radix_tree_or_trie.png)


Linux radix tree
Mapping between a long integer key and a pointer value
Each node has 64 slots
Slots are indexed by a 6-bit (2^6 = 64 64) portion of the key
	- Tags, parent pointer, offset in parent, etc

![Radix Tree inset](./images/linux_radix_tree_inset.png)

 
 Recall the linux kernel radix tree is optimized to use a unsigned long as an index

From wiki: " Radix trees are useful for constructing associative arrays with keys that can be expressed as strings."


CS 594- W03L1 - kern mod + radix tree, xarray, bitmap

Linux radix tree example
The most important user is the page chache
	- Page cache: a cache (in RAM) for pages from the disk
	- Every time we look up a page in a file, we consult the radix tree to see if the page is already in the cache
	- Use tags to maintain the status page
		(e.g., PAGECACHE_TAG_DIRTY or PAGECACHE_TAG_WRITEBACK)




Rbtree vs radix tree vs hash table
	- RB trees are a type of self-balancing binary search tree, used for storing sortable key/value data pairs
	- Radix trees are used to efficiently store sparse arrays and thus use long integer indexes to insert/access delete nodes
	- Hash tables are not kept sorted to be easily traveresed in order and must be tuned for a specific size


Xarray
A nicer API replacement for Linux radix tree (since 4.19)
An automatically resizing array of pointers indexed by an unsigned long
Entries may have up to three tab gits (get/set/clear)
You can iterate over entries 
You can extract a batch of entries
Embeds a spinlock
Loads are store-free using RCU

Xarray API

```c
#include <linux/xarray.h>
/** Define an Xarray */
DEFINE_XARRAY(array_name);
/* or */
Struct xarray array;
Xa_init(&array);

/** storing a value into an Xarray is done with */

void *xa_store(struct xarrau *xa, unsigned long index, void *entry gfp_t gfp);

/** an entry can be removed by called */
Void *xa_erase(struct xarray *xa, unsigned long index);

/** storing a value only if the current value stored there matches old */
Void *xa_cmpxchg(struct xarray *xa, unsigned long index, void *old, void *entry, gfp_t gfp);
```
