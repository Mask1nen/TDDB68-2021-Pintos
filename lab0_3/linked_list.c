#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>
typedef struct list_item listNode;
struct list_item {
  int val;
  listNode * next;
};

int main(int argc, char const *argv[]) {
  listNode root;
  root.val = -1;
  root.next = NULL;

  test(&root);
}

void append(listNode *first, int x){
  if(first->next==NULL){
    listNode *newNode = (listNode *)malloc(sizeof(listNode));
    newNode->val = x;
    newNode->next = NULL;
    first->next = newNode;
  }else{
    append(first->next, x);
  }
}

void prepend(listNode *first, int x){
    listNode *newNode = (listNode *)malloc(sizeof(listNode));
    newNode->val = x;
    newNode->next = first->next;
    first->next = newNode;
}

void print(listNode *first){
  if(first->next==NULL){
    printf("%i\n", first->val);
    return;
  }else{
    printf("%i\n", first->val);
    print(first->next);
  }
}

void insert_sorted(listNode *first, int x){
  if(first->next->val > x || first->next == NULL){
    prepend(first, x);
  }else{
    insert_sorted(first->next, x);
  }
}

void clear(listNode *first){
  if(first->next == NULL && first->val == -1) return;
  if(first->next == NULL){
    first->next = NULL;
    if(first->val != -1){
      free(first);
    }
    return;
  }else{
    clear(first->next);
    first->next = NULL;
    if(first->val != -1){
      free(first);
    }
    return;
  }
}

void test(listNode *first){
  clear(first);
  printf("%s\n", "after clear of empty list");
  print(first);
  for (size_t i = 0; i < 10; i++) {
    append(first, i);
    prepend(first, i);
  }
  printf("%s\n", "after append + prepend loop");
  print(first);
  clear(first);
  printf("%s\n", "after clear of non-empty list");
  print(first);
  prepend(first, 15);
  printf("%s\n", "after prepend on empty list");
  print(first);

  for (size_t i = 10; i > 1; i-=2) {
    insert_sorted(first, i);
  }
  printf("%s\n", "after insert_sorted");
  print(first);
  clear(first);
  return;
}
