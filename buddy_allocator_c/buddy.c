/**
 * Akaila Brown
 * Prof. Posnett
 * CSC 139
 * Mar 25, 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define TOTAL_MEMORY (512 * 1024) // 512KB total memory pool
#define MIN_BLOCK_SIZE (4 * 1024) // 4KB
#define MAX_LEVELS 7 // Since 512K / 2^7 = 4K

 typedef struct Node {
    bool is_free;               // True if this block is currently unallocated
    bool is_split;              // True if this block has been split into two smaller blocks
    struct Node* left;          // Pointer to the left child (first half of the split)
    struct Node* right;         // Pointer to the right child (second half of the split)
    struct Node* parent;        // Pointer to the parent node (used for merging)
    size_t size;                // Size of the block in bytes
    size_t mempool_offset;      // Offset in memory_pool representing this block
} Node;

typedef struct {
    Node* root; // Root of the binary tree used for allocation tracking
    char memory_pool[TOTAL_MEMORY]; // Fixed memory pool for all allocations
} BuddyAllocator;

void print_node_details(Node* node, const char* message) {
    if (node == NULL) {
        printf("%s: Node is NULL\n", message);
        return;
    }
    printf("%s: Node size=%zuK, offset=%zuK, is_split=%d, is_free=%d\n",
        message, node -> size / 1024, node -> mempool_offset / 1024, node -> is_split, node -> is_free);
}

void print_tree(Node* node, int depth) {
    if (node == NULL) return;

    for (int i = 0; i < depth; i++) printf("  ");

    if (!node -> is_split)
        printf("%s (%zuK)\n", node -> is_free ? "F" : "A", node -> size / 1024);
    else
        printf("FS (%zuK)\n", node -> size / 1024);

    print_tree(node -> left, depth + 1);
    print_tree(node -> right, depth + 1);
}

Node* create_node(size_t size, size_t mempool_offset, Node* parent) {
    Node* node = (Node*)malloc(sizeof(Node));
    node -> parent = parent;
    node -> is_free = true;
    node -> is_split = false;
    node -> left = NULL;
    node -> right = NULL;
    node -> size = size;
    node -> mempool_offset = mempool_offset;

    return node;
}

BuddyAllocator* create_allocator() {
    // todo
    BuddyAllocator* allocator = malloc(TOTAL_MEMORY);
    allocator -> root = create_node(TOTAL_MEMORY, 0, NULL);
    return allocator;
}

void split(Node* node) {
    //todo
    if (node == NULL) return;
    if (!node -> is_split && node -> size > MIN_BLOCK_SIZE){
        node -> left = create_node(node -> size/2, node -> mempool_offset, node);
        
        if (node -> left == NULL) return;
        
        node -> right = create_node(node -> size/2, node -> mempool_offset + (node -> size/2), node);
        
        if (node -> right == NULL) return;

        node -> is_split = true;
    }
}

Node* allocate_recursive(Node* node, size_t size) {
    // todo

    if (node -> size < size || node == NULL || node -> is_free == false)
        return NULL;

    if (node -> size > size && node -> is_split == false)
        split(node);

    if (node -> is_free == true && node -> is_split == false && node -> size == size){
        node -> is_free = false;
        return node;
    }

    Node* left;
    Node* right;

    left = allocate_recursive(node -> left, size);

    if (left != NULL) return left;

    right = allocate_recursive(node -> right, size);

    if (right != NULL) return right;

    return NULL;
}

 void* allocate(BuddyAllocator* allocator, size_t size) {
    // todo
    if (size < MIN_BLOCK_SIZE) size = MIN_BLOCK_SIZE;
    if (size > TOTAL_MEMORY) return NULL;

    Node* node = allocate_recursive(allocator -> root, size);

    if (node == NULL) 
        return NULL;
    
    return (void*)((allocator -> memory_pool) + (node -> mempool_offset));
 }

void coalesce(Node* node) {
    //todo
    if (node == NULL || node -> parent == NULL) return;

    Node* parent = node -> parent;

    //Both nodes are freed if conditions are met
    if (parent -> left -> is_free && parent -> right -> is_free && !parent -> left -> is_split && !parent -> right -> is_split){
        parent -> left = NULL;
        parent -> right = NULL;
        parent -> is_free = true;
        parent -> is_split = false;

        coalesce(parent);
    }
 }

 void free_recursive(Node* node) {
    //todo
    if (node == NULL) return;

    if (node -> parent -> is_split = false) return;

    node -> is_free = true;
    coalesce(node);
  }

Node* find_node(Node* node, size_t mempool_offset) {
    //todo
    if (node == NULL) return NULL;
    if (node -> mempool_offset == mempool_offset && !node -> is_split) return node;

    if (mempool_offset < node -> right -> mempool_offset && mempool_offset >= node -> left -> mempool_offset)
        return find_node(node -> left, mempool_offset);
    else
        return find_node(node -> right, mempool_offset);
}

void deallocate(BuddyAllocator* allocator, void* ptr) {
    //todo
    if (allocator == NULL || ptr == NULL) return;

    size_t mempooloffset = (char*) ptr - allocator -> memory_pool;

    if (mempooloffset < 0 || mempooloffset >= TOTAL_MEMORY) return;

    Node* node = find_node(allocator -> root, mempooloffset);

    if (node != NULL && !node -> is_free)
        free_recursive(node);
}

void destroy_tree(Node* node) {
    //todo
    if (node == NULL) return;

    destroy_tree(node -> left);
    destroy_tree(node -> right);

    free(node);
}

 void destroy_allocator(BuddyAllocator* allocator) {
    //todo
    destroy_tree(allocator -> root);
    free(allocator);
}

// Main function for testing
int main() {
    BuddyAllocator* allocator = create_allocator();
    print_node_details(allocator -> root, "Root details: ");

    printf("\nInitial Tree\n");
    print_tree(allocator -> root, 0);

    printf("\nAllocating 4KB\n");
    void* block4 = allocate(allocator, 4 * 1024);
    print_tree(allocator -> root, 0);

    printf("\nFreeing 4KB\n");
    deallocate(allocator, block4);
    print_tree(allocator -> root, 0);
    
    printf("\nAllocating 8KB\n");
    void* block5 = allocate(allocator, 8 * 1024);
    print_tree(allocator -> root, 0);

    printf("1");
    printf("\nAllocating 16KB\n");
    void* block1 = allocate(allocator, 16 * 1024);
    print_tree(allocator -> root, 0);

    /*
    printf("\nAllocating 64KB\n");
    printf("2");
    for (int i = 0; i < 12; i++){
        void* block3 = allocate(allocator, 64 * 1024);
    }*/

    printf("\nAllocating 4KB\n");
    void* block6 = allocate(allocator, 4 * 1024);

    print_tree(allocator -> root, 0);
    printf("\nAllocating 8KB\n");
    void* block2 = allocate(allocator, 8 * 1024);
    print_tree(allocator -> root, 0);

    printf("\nFreeing 16KB\n");
    deallocate(allocator, block1);
    print_tree(allocator -> root, 0);

    destroy_allocator(allocator);
    return 0;
}