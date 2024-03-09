#include <linux/kernel.h>
#include <linux/rbtree.h>
#include <linux/hashtable.h>
#include <linux/types.h>
#include <linux/rbtree.h>
#include <linux/module.h>
#include <linux/slab.h>

struct my_rb_node_data {
    struct rb_node rb_node;  // Red-Black Tree Node
    s64 cumulative_time;     // Key for the Red-Black Tree
    u32 hash_result;         // Value associated with the key
    int pid;                 // Value associated with the key
};

struct rb_root my_rbtree = RB_ROOT; // Initialize the Red-Black Tree root
// DEFINE_HASHTABLE(my_rbtree, 10);  // Initialize HashTable with size 10

// Function to insert a node into the Red-Black Tree
void my_rb_node_data_insert(struct my_rb_node_data *new_data, struct rb_root *root) {
    struct rb_node **new_node = &(root->rb_node), *parent = NULL;

    // Traverse the Red-Black Tree to find the insertion point
    while (*new_node) {
        struct my_rb_node_data *this_data = container_of(*new_node, struct my_rb_node_data, rb_node);

        parent = *new_node;
        if (new_data->cumulative_time < this_data->cumulative_time)
            new_node = &((*new_node)->rb_left);
        else if (new_data->cumulative_time > this_data->cumulative_time)
            new_node = &((*new_node)->rb_right);
        else
            return; // Node with the same key already exists, handle accordingly
    }

    // Initialize the new node and insert it into the Red-Black Tree
    rb_link_node(&new_data->rb_node, parent, new_node);
    rb_insert_color(&new_data->rb_node, root);
}

// Function to search for a node in Red-Black Tree based on cumulative time
struct my_rb_node_data *search_node_by_time(s64 cumulative_time) {
    struct rb_node *node = my_rbtree.rb_node;

    struct my_rb_node_data *this_data;

    while (node) {
        this_data = container_of(node, struct my_rb_node_data, rb_node);

        if (cumulative_time < this_data->cumulative_time)
            node = node->rb_left;
        else if (cumulative_time > this_data->cumulative_time)
            node = node->rb_right;
        else
            return this_data;  // Found the node
    }

    return NULL;  // Node not found
}

// Function to iterate over the Red-Black Tree and print cumulative time for each node
void iterate_and_print_rb_tree(struct rb_node *node) {
    if (node != NULL) {
        struct my_rb_node_data *entry = rb_entry(node, struct my_rb_node_data, rb_node);

        iterate_and_print_rb_tree(node->rb_left);

        pr_info("Node: cumulative_time=%lld, hash_result=%u, pid=%d\n",
                entry->cumulative_time, entry->hash_result, entry->pid);

        iterate_and_print_rb_tree(node->rb_right);
    }
}

// Function to delete a node from Red-Black Tree and HashTable
void delete_node(struct my_rb_node_data *data) {
    if (!data) {
        pr_info("Node not found for deletion\n");
        return;
    }
    rb_erase(&(data->rb_node), &my_rbtree);
}

// Function to recursively traverse and free the Red-Black Tree
void cleanup_rb_tree(struct rb_node *node) {
    if (node != NULL) {
        struct my_rb_node_data *entry = rb_entry(node, struct my_rb_node_data, rb_node);
        
        cleanup_rb_tree(node->rb_left);
        cleanup_rb_tree(node->rb_right);
        
        rb_erase(node, &RB_ROOT);  // Remove the node from the Red-Black Tree
        kfree(entry);
    }
}

static int example_usage_two(void) {
    struct my_rb_node_data data1 = { .cumulative_time = 10, .hash_result = 123, .pid = 1 };
    struct my_rb_node_data data2 = { .cumulative_time = 5, .hash_result = 456, .pid = 2 };
    struct my_rb_node_data data3 = { .cumulative_time = 15, .hash_result = 789, .pid = 3 };
    struct my_rb_node_data data4 = { .cumulative_time = 25, .hash_result = 999, .pid = 4 };
    struct my_rb_node_data data5 = { .cumulative_time = 75, .hash_result = 111, .pid = 5 };
    struct my_rb_node_data data6 = { .cumulative_time = 2, .hash_result = 345, .pid = 6 };
    struct my_rb_node_data *result;
    s64 cumulative_time;

    cumulative_time = 100;

    // Insert nodes into the Red-Black Tree
    my_rb_node_data_insert(&data1, &my_rbtree);
    my_rb_node_data_insert(&data2, &my_rbtree);
    my_rb_node_data_insert(&data3, &my_rbtree);
    my_rb_node_data_insert(&data4, &my_rbtree);
    my_rb_node_data_insert(&data5, &my_rbtree);
    my_rb_node_data_insert(&data6, &my_rbtree);

    // Add more nodes or perform other actions as needed

    pr_info("Red-Black Tree initialized and nodes inserted.\n");

    pr_info("Searching for node with cumulative_time=%lld\n", cumulative_time);
    pr_info("Expecting the node to not be found\n");
    result = search_node_by_time(cumulative_time);
    if (result)
        pr_info("Node found: hash_result=%u\n", result->hash_result);
    else
        pr_info("Node not found\n");

    pr_info("Searching for node with cumulative_time=%lld\n", cumulative_time);
    pr_info("Expecting the node to be found\n");
    cumulative_time = 15;
    result = search_node_by_time(cumulative_time);
    if (result)
        pr_info("Node found: hash_result=%u\n", result->hash_result);
    else
        pr_info("Node not found\n");

    pr_info("Red-Black Tree initialized and nodes inserted.\n");
    pr_info("Iterating over Red-Black Tree and printing cumulative time, jenkins hash and process ID for each node:\n");
    iterate_and_print_rb_tree(my_rbtree.rb_node);

    pr_info("Running: delete_node(result) on the node we found\n");
    delete_node(result);

    return 0;
}

int my_init(void) {
  int result;
  pr_info("Hello world!\n");
  result = example_usage_two();
  pr_info("Result of example_usage_two: %d\n", result);
  return 0;
}

void my_exit(void) {
  pr_info("Cleaning up module.\n");
  cleanup_rb_tree(RB_ROOT.rb_node);
}

MODULE_LICENSE("GPL");
module_init(my_init);
module_exit(my_exit);
