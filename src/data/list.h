#ifndef _LIST_H_INCLUDED_
#define _LIST_H_INCLUDED_

#include <stdlib.h>
#include <string.h>

typedef struct Node_List
{
    struct Node_List *next;
    char *key;
    char *value;
} NODE_LIST;

typedef struct List
{
    NODE_LIST *next;
    int size;
    char *defaultFolder;
} List;

List *construct_list();
void destruct_list(List *listHandle);
int insert(List *listHandle, NODE_LIST *item);
char *check(List *listHandle, char *search);

#endif /* _LIST_H_INCLUDED_ */