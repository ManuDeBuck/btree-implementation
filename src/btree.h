#ifndef btree
#define btree

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define N 600
#define DATE 20


typedef struct BTreeNode BTreeNode;

/**
 * Struct representing a node within a BTree
 */
struct BTreeNode {
    BTreeNode *parent; // pointer to the parent
    int size; // amount of entries in the node
    int recursiveSize; // amount of nodes in and underneath this top
    int leaf; // leaf: 1, not a leaf: 0
    char *dateKeys[N]; // The keys of the entries in the tree
    char *descriptionValues[N]; // The descriptions of the entries in the tree
    BTreeNode *childNodes[N + 1]; // Pointers to the N + 1 child nodes of the current node/ entries
};

/**
 * Struct representing a BTree
 */
typedef struct BTree {
    BTreeNode *root; // Pointer to the root BTreeNode
    int size; // The total amount of entries in the tree
} BTree;

/*
 * Functions for the creation of BTree's and BTreeNode's
 */
BTree *createBTree();

BTreeNode *createBTreeNode(int leaf);

/*
 * Functions for adding entries to the BTree
 */
void insertEntry(BTree *tree, char *date, char *description);

void addAndRebalanceRecursive(BTree *tree, BTreeNode *node, char *date, char *description,
                              BTreeNode *lChild, BTreeNode *rChild, int leafs);

void overwriteEntry(BTreeNode *node, char *date, char *description);

void addEntryToTop(BTree *tree, BTreeNode *node, char *date, char *description, BTreeNode *lChildAdd,
                   BTreeNode *rChildAdd, int leafs);

void rebalanceRecursive(BTree *tree, BTreeNode *node, char *date, char *description,
                        BTreeNode *lChildAdd, BTreeNode *rChildAdd, int leafs);

void updateRecursiveSizes(BTreeNode *node, int update);

BTreeNode *searchNode(BTreeNode *node, char *search, int add, int recursive);

void recalculateRecursiveSizes(BTreeNode *node);

/*
 * Functions for searching the description of entries
 */
char *getEntry(BTree *tree, char *date);

/*
 * Functions for removal of entries
 */
int removeEntry(BTree *tree, char *date);

/*
 * Functions for counting between entries
 */
int countBetweenEntries(BTree *tree, char *start, char *end);

void countBetweenRecursive(BTreeNode *node, char *begin, char *end, int *count, int untillEnd, int fromStart);

int searchIndex(BTreeNode *node, char *search, int isStartIndex, int exactMatch);

/*
 * Functions for free-ing nodes, entries and the tree
 */
void freeNode(BTreeNode *node);

void freeBTree(BTree *tree);

/*
 * Functions for debugging purposes
 */
void printTree(BTreeNode *node, int depth);

#endif