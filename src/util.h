#ifndef BTREE_UTIL_H
#define BTREE_UTIL_H

#include "btree.h"
#include <math.h>

void moveEntry(BTreeNode *from, BTreeNode *to, int positionFrom, int positionTo);

void moveChildNodes(BTreeNode *from, BTreeNode *to, int positionFrom, int positionTo);

void addChildNodes(BTreeNode *to, int parentIndex, BTreeNode *lChild, BTreeNode *rChild);

void
addNode(BTreeNode *to, char *date, char *description, BTreeNode *lChild, BTreeNode *rChild, int leafs);

BTreeNode *binarySearch(BTreeNode *node, char *search, int add, int recursive);

#endif //BTREE_UTIL_H