#include "util.h"

/**
 * Move two entries from one BTreeNode to another.
 * @param from The BTreeNode wherefrom we copy
 * @param to The BTreeNode whereto we copy
 * @param positionFrom The position in the arrays where we copy from
 * @param positionTo The position "-" to
 */
void moveEntry(BTreeNode *from, BTreeNode *to, int positionFrom, int positionTo) {
  to->dateKeys[positionTo] = from->dateKeys[positionFrom];
  to->descriptionValues[positionTo] = from->descriptionValues[positionFrom];

  to->size++;

  // We should not free data because it is being moved and thus still used
  from->dateKeys[positionFrom] = NULL;
  from->descriptionValues[positionFrom] = NULL;

  from->size--;
}

/**
 * Move childnodes from one BTreeNode to another
 * @param from The BTreeNode wherefrom we copy
 * @param to The BTreeNode whereto we copy
 * @param positionFrom The position of the child we copy from
 * @param positionTo The position "-" to
 */
void moveChildNodes(BTreeNode *from, BTreeNode *to, int positionFrom, int positionTo) {
  to->childNodes[positionTo] = from->childNodes[positionFrom];
  to->childNodes[positionTo]->parent = to;

  from->childNodes[positionFrom] = NULL;
}

/**
 * Add child nodes to a node.
 * @param to The node where we should add the children
 * @param parentIndex The index of the parent of the children
 * @param lChild The left child
 * @param rChild The right child
 */
void addChildNodes(BTreeNode *to, int parentIndex, BTreeNode *lChild, BTreeNode *rChild) {
  to->childNodes[parentIndex] = lChild;
  lChild->parent = to;
  to->childNodes[parentIndex + 1] = rChild;
  rChild->parent = to;
}

void
addNode(BTreeNode *to, char *date, char *description, BTreeNode *lChild, BTreeNode *rChild, int leafs) {
  int index = to->size;
  while (index > 0 && strcmp(to->dateKeys[index - 1], date) > 0) {
    to->dateKeys[index] = to->dateKeys[index - 1];
    to->descriptionValues[index] = to->descriptionValues[index - 1];
    to->childNodes[index + 1] = to->childNodes[index];
    index--;
  }
  to->dateKeys[index] = date;
  to->descriptionValues[index] = description;

  to->size++;

  if (!leafs) {
    addChildNodes(to, index, lChild, rChild);
  }
}


/**
 * Do a recursive binary search on a node to search for an entry key.
 * @param node The Node where we should start
 * @param search The entry string we are searching
 * @param add 1: add, 0: not add. Depending on wheter we need it to add a value or just to look a value up the
 * implementations slightly differ.
 * @return NULL or the Node we were searching for
 */
BTreeNode *binarySearch(BTreeNode *node, char *search, int add, int recursive) {
  if (node->leaf && add) {
    // Top is een blad en we willen toevoegen, dus we moeten deze top terug geven
    return node;
  }
  // top is geen blad, dus we moeten verder zoeken d.m.v. binary search

  int start = 0;
  int stop = node->size;
  int mid = floor((start + stop) / 2);

  while (node->size > 0 && start <= stop && mid < node->size) {
    if (strcmp(node->dateKeys[mid], search) < 0) {
      // search is later than key on index
      if (!node->leaf &&
          ((mid + 1 < node->size && strcmp(node->dateKeys[mid + 1], search) > 0) || (mid + 1 >= node->size))) {
        // alleen indien top geen blad is
        // in rechter deel van huidige index, linker van mid + 1
        if (recursive) {
          return binarySearch(node->childNodes[mid + 1], search, add, recursive);
        } else {
          return NULL;
        }
      }
      start = mid + 1;
      // else: ligt nog verder, dus niet rechts van huidige node
    } else if (strcmp(node->dateKeys[mid], search) == 0) {
      // search is already in here, we just return the entry we found
      return node;
    } else {
      // search is earlier than date
      if (!node->leaf && ((mid - 1 >= 0 && strcmp(node->dateKeys[mid - 1], search) < 0) || (mid - 1 < 0))) {
        // alleen indien top geen blad is
        // in linker deel van huidige, rechter van mid - 1
        if (recursive) {
          return binarySearch(node->childNodes[mid], search, add, recursive);
        } else {
          return NULL;
        }
      }
      stop = mid - 1;
    }
    mid = floor((start + stop) / 2);
  }
  // hier komen we alleen als add == 0, wil zeggen dat de entry in geen enkele top gevonden is
  return NULL;
}