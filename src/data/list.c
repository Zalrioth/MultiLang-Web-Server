#include "list.h"

List *construct_list()
{
    List *list = (List *)malloc(sizeof(List));
    list->size = 0;
    list->next = NULL;

    return list;
}

void destruct_list(List *listHandle)
{
    NODE_LIST *current = listHandle->next;
    NODE_LIST *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }

    free(listHandle);
}

int insert(List *listHandle, NODE_LIST *item)
{
    item->next = NULL;

    if (listHandle->next == NULL)
        listHandle->next = item;
    else
    {
        NODE_LIST *current = listHandle->next;
        while (current->next != NULL)
            current = current->next;
        current->next = item;
    }

    listHandle->size++;

    return 1;
}

char *check(List *listHandle, char *search)
{
    NODE_LIST *current = listHandle->next;
    while (current != NULL)
    {
        if (strcmp(current->key, search) == 0)
            return current->value;

        current = current->next;
    }

    return listHandle->defaultFolder;
}