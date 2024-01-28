The Professor begins by covering various Linux Data structures

HashTable List is a new one I've not heard before

We should be familiar with LinkedLists and DoublyLinkedLists

Linux doubly linked lists are a bit different in that the HEAD node itself doesn't have data, but does itself have pointers to next and prev where the prev points to the tail

The benefit of using this different structure:

- List APIs only need to accept the list_head structures
  - E.g. list_add(struct list_head *new, struct list_head *head)- Manipulate the list_head ovject
- If we want to find the parent structure use container_of()
	
How to get the eleemnt?

Use "list_entry(ptr, type, member)"

```c
struct car *my_car = list_entry(car_list_ptr, struct car, list)
```


----------
Define a linked list
Static (compile-time) definition:

```c
Struct car my_car {
  .max_speed = 150,
  .price_in_dollars = 10000,
  .list = LIST_HEAD_INIT(my_car.list) /* runs INIT */
}
```

Dynamic (runtime def) most commonly used:

```c
struct car *my_car = kmalloc(sizeof(*my_car), GKP_KERNELY);
My_car->max_speed = 150;
My_car->price_in_dollars = 10000.0;
INIT_LIST_HEAD(&my_car->list);  /* runs INIT */
```

--------------

Linked list usage in the Linux Kernel

Kernel code makes extensive use of linked lists:

- A list of threads under the same PID
- A list of superblocks of a file system
- And many more

Linux Hash Table

A simple fixed-size open chaining hash table

- The size of bucket array is fixed at init as a 2^N
- Each bucker has a singly linked list to resolve hash collision for instances where keys have more than 1 value 
- Time complexity: O(1)
- Basically each "bucket" is a hlist_head and each entry an hlist_node

Hash_for_each(name, bkt, obj, member) …
Name=the hash table
Bkt=integer to use as cursor to point to the bucket 
Obj=the type * to use as a look cursor for each entry
Member= the name of the hlist_node within the struct

Use cases for hash tables include, network device drivers, file systems, etc.

---------------------

Binary Tree

Nodes have zero one or two children, root has no parent, other nodes have 1

Binary Search Tree


It's a binary tree
Left children < parent
Right children > parent
Search and ordered traversal are efficient


Balanced binary search tree

Depth of all leaves differ by at most one
Puts a boundary on the worst-case operations

Red-black tree

Following properties are maintained during tree modifications:
 - the path from a node to one of it's leaves contain the same number of black nodes as the shortest path to any of its other leaves
Fast search, insert, delete operations: O(log^N)

Linux provides interfaces to access members of a RB tree, insert nodes into a tree and to find keys in your tree



Linux RB Tree Example

Completely Fair Scheduling (CFS)

- Default task scheduler in Linux
- Each task has "vruntime", which represents how much time a task has run
- CFS always picks a process with thte smallest "vrunctime" for fairness
- Per-task "vruntime" structure is maintained in a Red-Black Tree


Design patters of kernel data structures
Embedding its pointer structure

- List_head, hlist_node, rb_node
- Programmers have full control of placement of fields in the structure (put fields closer to improve cache utilization)
- A structure can easily be on two or more lists quite independently, simply by having having multiple "list_head" fields
- "container_of", "list_entry", and "rb_entry" are used to get its emberring data structure 

Design Patterns of Linux Kernel Strucutres

( go back to PPT and get this slide)

Further Reading

LKD3 (main text book) - Chapter 6 (kernel data structures)
