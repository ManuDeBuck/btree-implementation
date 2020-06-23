#include "./btree.h"
#include "./util.h"


/**
 * Search a node recursively.
 * @param node The node where the searching begins
 * @param search The entity key we are searching
 * @param add 1 if we search a node where to add search, 0 otherwise
 * @param recursive 1 if we should search recursive, 0 if we should only search the top
 * @return
 */
BTreeNode *searchNode(BTreeNode *node, char *search, int add, int recursive) {
  return binarySearch(node, search, add, recursive);
}

/**
 * Insert or overwrite an entry in the BTree.
 * @param tree The tree where we want to add an entry
 * @param date The date we want to add
 * @param description A description of the date
 */
void insertEntry(BTree *tree, char *date, char *description) {
  BTreeNode *node = searchNode(tree->root, date, 1, 1);

  if (searchNode(node, date, 0, 0) != NULL) {
    // De date zit al in de node
    overwriteEntry(node, date, description);
  } else {
    // We moeten de date voor de eerste keer toevoegen
    addAndRebalanceRecursive(tree, node, date, description, NULL, NULL, 1);
    tree->size += 1;
  }
}

/**
 * Overwrite the description of an entry.
 * @param node The node where the entry should be overwritten
 * @param date The date of the entry
 * @param description The description which we want to use to overwrite
 */
void overwriteEntry(BTreeNode *node, char *date, char *description) {
  int index = searchIndex(node, date, 0, 1);

  if (index >= 0) {
    free(date); // This date will not be used anymore

    if (node->descriptionValues[index] == NULL) {
      // If we re-add it it isn't removed anymore so we should change gravestone, and recursively update sizes!
      node->descriptionValues[index] = description;
      updateRecursiveSizes(node, +1);
    }
  } // else: should not be able to happen
}

/**
 * Add a new entry to the tree in the node and/ or rebalance the tree after adding
 * @param tree The tree we are rebalancing
 * @param node The node we are currently adding/ rebalancing
 * @param date The date "-"
 * @param description The description "-"
 * @param lChildAdd The optional left-child of the entry
 * @param rChildAdd The optional right-child of the entry
 * @param leafs If we are working with leafs (1) or not (0)
 */
void addAndRebalanceRecursive(BTree *tree, BTreeNode *node, char *date, char *description,
                              BTreeNode *lChildAdd, BTreeNode *rChildAdd, int leafs) {
  if (node == NULL) {
    // We moeten een nieuwe root top aanmaken
    BTreeNode *newRoot = createBTreeNode(0);
    if (newRoot == NULL) {
      freeBTree(tree);
      exit(1);
    }

    tree->root = NULL;
    tree->root = newRoot;

    addEntryToTop(tree, newRoot, date, description, lChildAdd, rChildAdd, 0);
  } else if (node->size + 1 <= N) {
    // We kunnen de date nog toevoegen aan de node
    addEntryToTop(tree, node, date, description, lChildAdd, rChildAdd, leafs);
  } else {
    // We kunnen date niet meer toevoegen en moeten dit probleem dus recursief oplossen
    rebalanceRecursive(tree, node, date, description, lChildAdd, rChildAdd, leafs);
  }
}

/**
 * Add a new entry to a node.
 * @param tree The tree were we are working in
 * @param node The node where we will add an entry
 * @param date The date of the entry
 * @param description The description "-"
 * @param lChildAdd The optional left-child of the entry
 * @param rChildAdd The optional right-child of the entry
 * @param leafs If we are working with leafs (1) or not (0)
 */
void addEntryToTop(BTree *tree, BTreeNode *node, char *date, char *description, BTreeNode *lChildAdd,
                   BTreeNode *rChildAdd, int leafs) {
  int index = node->size;

  while (index > 0 && strcmp(node->dateKeys[index - 1], date) > 0) {
    // Move everything to the right
    node->dateKeys[index] = node->dateKeys[index - 1];
    node->descriptionValues[index] = node->descriptionValues[index - 1];

    node->childNodes[index + 1] = node->childNodes[index];


    index--;
  }
  node->dateKeys[index] = date;
  node->descriptionValues[index] = description;
  node->size++;

  if (!leafs) {
    addChildNodes(node, index, lChildAdd, rChildAdd);
  }
  // All above can be +1-ed
  updateRecursiveSizes(node->parent, +1);
  // This node and all nodes underneath should be recalculated
  node->recursiveSize = -1;
  recalculateRecursiveSizes(node);
}

/**
 * Recalculate the recursiveSize fields of all the nodes below a node.
 * @param node The node wherefrom we should start
 */
void recalculateRecursiveSizes(BTreeNode *node) {
  node->recursiveSize = 0;
  for (int i = 0; i < node->size; i++) {
    if (node->descriptionValues[i] != NULL) node->recursiveSize += 1;
  }
  if (!node->leaf) {
    for (int i = 0; i <= node->size; i++) {
      if (node->childNodes[i]->recursiveSize < 0) {
        recalculateRecursiveSizes(node->childNodes[i]);
      }
      node->recursiveSize += node->childNodes[i]->recursiveSize;
    }
  }
}

/**
 * Rebalance the tree recursively when adding a new entry to a node.
 * @param tree The tree we are rebalancing
 * @param node The node where the new entry is added
 * @param date The date of the new entry
 * @param description The description "-"
 * @param lChildAdd The optional left-child of the entry
 * @param rChildAdd The optional right-child of the entry
 * @param leafs If we are working with leafs (1) or not (0)
 */
void rebalanceRecursive(BTree *tree, BTreeNode *node, char *date, char *description,
                        BTreeNode *lChildAdd, BTreeNode *rChildAdd, int leafs) {
  // Find index where key should be placed
  int index = node->size;
  while (index > 0 && strcmp(node->dateKeys[index - 1], date) > 0) {
    index--;
  }

  char *newParentDate;
  char *newParentDescription;
  BTreeNode *rChild = createBTreeNode(leafs);
  BTreeNode *lChild = createBTreeNode(leafs);
  if (rChild == NULL || lChild == NULL) {
    freeBTree(tree);
    if (rChild != NULL) {
      freeNode(rChild);
    }
  }

  if (index < (N / 2)) {
    // Node zit links van nieuwe midden
    newParentDate = node->dateKeys[(N / 2) - 1];
    newParentDescription = node->descriptionValues[(N / 2) - 1];

    // we should not free because it is still used
    node->descriptionValues[(N / 2) - 1] = NULL;
    node->dateKeys[(N / 2) - 1] = NULL;

    node->size--;

    for (int i = (N / 2); i < N; i++) {
      moveEntry(node, rChild, i, i - (N / 2));
      if (!leafs) {
        moveChildNodes(node, rChild, i, i - (N / 2));
      }
    }
    if (!leafs) {
      moveChildNodes(node, rChild, N, N / 2);
    }

    for (int i = 0; i < (N / 2) - 1; i++) {
      moveEntry(node, lChild, i, i);
      if (!leafs) {
        moveChildNodes(node, lChild, i, i);
      }
    }
    if (!leafs) {
      moveChildNodes(node, lChild, N / 2 - 1, N / 2 - 1);
    }

    addNode(lChild, date, description, lChildAdd, rChildAdd, leafs);
  } else if (index == (N / 2)) {
    // Node belandt net in midden
    newParentDate = date;
    newParentDescription = description;

    if (!leafs) {
      rChild->childNodes[0] = rChildAdd;
      rChildAdd->parent = rChild;
    }
    for (int i = (N / 2); i < N; i++) {
      moveEntry(node, rChild, i, i - (N / 2));
      if (!leafs) {
        moveChildNodes(node, rChild, i + 1, i - (N / 2) + 1);
      }
    }

    for (int i = 0; i < (N / 2); i++) {
      moveEntry(node, lChild, i, i);
      if (!leafs) {
        moveChildNodes(node, lChild, i, i);
      }
    }
    if (!leafs) {
      lChild->childNodes[N / 2] = lChildAdd;
      lChildAdd->parent = lChild;
    }
  } else {
    // Node zit rechts van midden
    newParentDate = node->dateKeys[(N / 2)];
    newParentDescription = node->descriptionValues[(N / 2)];

    node->descriptionValues[(N / 2)] = NULL;
    node->dateKeys[N / 2] = NULL;

    node->size--;

    for (int i = 0; i < (N / 2); i++) {
      moveEntry(node, lChild, i, i);
      if (!leafs) {
        moveChildNodes(node, lChild, i, i);
      }
    }
    if (!leafs) {
      moveChildNodes(node, lChild, N / 2, N / 2);
    }

    for (int i = (N / 2) + 1; i < N; i++) {
      moveEntry(node, rChild, i, (i - 1) - (N / 2));
      if (!leafs) {
        moveChildNodes(node, rChild, i, i - (N / 2) - 1);
      }
    }
    if (!leafs) {
      moveChildNodes(node, rChild, N, N / 2 - 1);
    }

    addNode(rChild, date, description, lChildAdd, rChildAdd, leafs);
  }
  lChild->recursiveSize = -1;
  rChild->recursiveSize = -1;
  addAndRebalanceRecursive(tree, node->parent, newParentDate, newParentDescription, lChild,
                           rChild, 0);
  freeNode(node);
}

/**
 * Create a BTree.
 * @return A created BTree
 */
BTree *createBTree() {
  // initialize BTree
  BTree *tree = (BTree *) malloc(sizeof(BTree));
  if (tree == NULL) {
    return NULL;
  }
  tree->size = 0;
  tree->root = createBTreeNode(1);
  if (tree->root == NULL) {
    free(tree);
    return NULL;
  }
  return tree;
}

/**
 * Create a BTreeNode
 * @param leaf if it will be a leaf or not.
 * @return An initialised BTreeNode
 */
BTreeNode *createBTreeNode(int leaf) {
  BTreeNode *node = malloc(sizeof(BTreeNode));
  if (node == NULL) {
    return NULL;
  }
  node->parent = NULL;
  for (int i = 0; i < N; i++) {
    node->dateKeys[i] = NULL;
    node->descriptionValues[i] = NULL;
    node->childNodes[i] = NULL;
  }
  node->childNodes[N] = NULL;

  node->size = 0;
  node->recursiveSize = 0;

  node->leaf = leaf;
  return node;
}

/**
 * A function especially used for debugging and printing the BTree.
 * @param node The node we currently are printing
 * @param depth The current depth of the node
 */
void printTree(BTreeNode *node, int depth) {
  for (int i = 0; i < node->size; i++) {
    if (node->leaf < 1 && node->childNodes[i] != NULL && i == 0) {
      printTree(node->childNodes[i], depth + 1);
    }
    printf("Key: %s - Value: %s. Depth: %i. RecursiveSize: %i.\n", node->dateKeys[i],
           node->descriptionValues[i],
           depth, node->recursiveSize);
    if (node->leaf < 1 && node->childNodes[i + 1] != NULL) {
      printTree(node->childNodes[i + 1], depth + 1);
    }
  }
}

/**
 * Find the description of an entry in the tree.
 * @param tree The BTree we are searching in
 * @param date The key of the entry we are searching
 * @return The entry description or NULL if date is not found
 */
char *getEntry(BTree *tree, char *date) {
  BTreeNode *node = searchNode(tree->root, date, 0, 1);
  if (node == NULL) return NULL;

  int index = searchIndex(node, date, 1, 0);
  if (index >= 0 && strcmp(node->dateKeys[index], date) == 0 && node->descriptionValues[index] != NULL) {
    return node->descriptionValues[index];
  }
  return NULL;
}

/**
 * Remove an entry if the tree.
 * @param tree The Tree where we want to remove an entry
 * @param date The date of the entry we want to remove
 * @return 0 if failed (date was not in Tree), 1 if succeeded
 */
int removeEntry(BTree *tree, char *date) {
  BTreeNode *node = searchNode(tree->root, date, 0, 1);
  if (node == NULL) {
    return 0;
  }

  int index = searchIndex(node, date, 1, 1);

  if (index >= 0 && node->descriptionValues[index] != NULL) {
    // We found the node and it didn't have a gravestone yet.
    free(node->descriptionValues[index]);
    node->descriptionValues[index] = NULL;
    updateRecursiveSizes(node, -1);
    return 1;
  }
  return 0;
}

/**
 * Update the recursiveSize field of all the parent nodes.
 * @param node The node where we should start with updating (this node will be updated too)
 * @param update The value with which we will update the recursiveSize field
 */
void updateRecursiveSizes(BTreeNode *node, int update) {
  BTreeNode *cur = node;
  while (cur != NULL) {
    cur->recursiveSize += update;
    cur = cur->parent;
  }
}

/**
 * Count the entries in the BTree between begin and end (both inclusive).
 * @param tree The Tree where we will be counting
 * @param begin The begin boundary
 * @param end The end boundary
 * @return The count of entries between begin and end
 */
int countBetweenEntries(BTree *tree, char *begin, char *end) {
  int count = 0;
  if (tree->root->size > 0) {
    countBetweenRecursive(tree->root, begin, end, &count, 0, 0);
  }
  return count;
}

/**
 * Recursively count the entries in the BTree between begin and end (both inclusive).
 * @param node The current node we are processing
 * @param begin The begin boundary
 * @param end The end boundary
 * @param count The pointer to the count of entries between begin and end
 * @param untillEnd If we know we can count all recursiveSizes of all children in this node till the end
 * @param fromStart "-" from the beginning
 */
void countBetweenRecursive(BTreeNode *node, char *begin, char *end, int *count, int untillEnd, int fromStart) {
  int startIndex;
  int endIndex;
  if (!fromStart) {
    startIndex = searchIndex(node, begin, 1, 0); // Index van top waar links van de begin zit
  } else {
    startIndex = 0;
  }
  if (!untillEnd) {
    // We zijn niet zeker dat end onder of in deze top zit
    endIndex = searchIndex(node, end, 0, 0); // Index van de top waar rechts van het einde zit
  } else {
    endIndex = node->size - 1;
  }

  if (startIndex == endIndex) {
    // Start zit links, end zit rechts
    if (node->descriptionValues[startIndex] != NULL) *count += 1;
    if (!node->leaf) {
      if (strcmp(node->dateKeys[startIndex], begin) == 0 && strcmp(begin, end) != 0) {
        countBetweenRecursive(node->childNodes[endIndex + 1], begin, end, count, 0, 1);
      } else if (strcmp(node->dateKeys[endIndex], end) == 0 && strcmp(begin, end) != 0) {
        countBetweenRecursive(node->childNodes[startIndex], begin, end, count, 1, 0);
      } else if (strcmp(node->dateKeys[startIndex], begin) != 0 && strcmp(begin, end) != 0) {
        // De startindex top is verschillend van beide
        countBetweenRecursive(node->childNodes[startIndex], begin, end, count, 1, 0);
        countBetweenRecursive(node->childNodes[endIndex + 1], begin, end, count, 0, 1);
      }
    }
  } else if (startIndex < endIndex) {
    // Start zit ergens links, end zit ergens rechts, enkele toppen tussen
    for (int index = startIndex; index <= endIndex; index += 1) {
      if (node->descriptionValues[index] != NULL) *count += 1;
    }
    if (!node->leaf) {
      if (strcmp(node->dateKeys[startIndex], begin) != 0) {
        countBetweenRecursive(node->childNodes[startIndex], begin, end, count, 1, 0);
      }
      for (int index = startIndex + 1; index <= endIndex; index += 1) {
        *count += node->childNodes[index]->recursiveSize;
      }
      if (strcmp(node->dateKeys[endIndex], end) != 0) {
        countBetweenRecursive(node->childNodes[endIndex + 1], begin, end, count, 0, 1);
      }
    }
  } else if (startIndex - 1 == endIndex) {
    // Zit volledig links of volledig rechts ervan
    if (!node->leaf) {
      countBetweenRecursive(node->childNodes[endIndex + 1], begin, end, count, 0, 0);
    }
  }
}

/**
 * Search the index of a key in a node.
 * @param node The node where we are searching in
 * @param search The key we are searching
 * @param isStartIndex The index of where search should be (1) if left of index, (0) if right of index
 * @return The wanted index where key should be placed or is placed
 */
int searchIndex(BTreeNode *node, char *search, int isStartIndex, int exactMatch) {
  int start = 0;
  int stop = node->size;

  while (start <= stop) {
    int mid = floor((start + stop) / 2);

    if (strcmp(node->dateKeys[mid], search) < 0) {
      if (!exactMatch && (mid + 1 >= node->size || strcmp(node->dateKeys[mid + 1], search) > 0)) {
        // zit rechts van mid, links van mid + 1
        return mid + isStartIndex;
      }
      start = mid + 1;
    } else if (strcmp(node->dateKeys[mid], search) == 0) {
      // This is index we are searching for
      return mid;
    } else {
      if (!exactMatch && (mid - 1 < 0 || strcmp(node->dateKeys[mid - 1], search) < 0)) {
        // Zit links van mid, rechts van mid - 1
        return mid + (isStartIndex - 1);
      }
      stop = mid - 1;
    }
  }
  return -1;
}

/**
 * Function for free-ing a node and all it's children recursively.
 * @param node The current node we are freeing
 */
void freeNode(BTreeNode *node) {
  for (int i = 0; i < node->size; i++) {
    free(node->dateKeys[i]);
    node->dateKeys[i] = NULL;
    free(node->descriptionValues[i]);
    node->descriptionValues[i] = NULL;
    if (!node->leaf) {
      if (i == 0) {
        freeNode(node->childNodes[i]);
      }
      freeNode(node->childNodes[i + 1]);
    }
  }
  // Free the current node itself too
  free(node);
}

/**
 * Free a BTree.
 * @param tree The Tree we want to free
 */
void freeBTree(BTree *tree) {
  freeNode(tree->root);
  tree->root = NULL;

  free(tree);
}