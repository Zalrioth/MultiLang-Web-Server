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

List *construct_list()
{
    List *list = (List *)malloc(sizeof(List));
    list->size = 0;
    list->next = NULL;

    return list;
}

destruct_list(List *listHandle)
{
    // TODO
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