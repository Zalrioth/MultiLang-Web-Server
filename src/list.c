typedef struct Node_l
{
    struct Node_l *next;
    char *key;
    char *value;
} NODEL;

typedef struct List
{
    NODEL *next;
    int size;
    char *defaultFolder;
} List;

List *ConstructList();
void DestructList(List *list);
int Insert(List *pList, NODEL *item);

List *ConstructList()
{
    List *list = (List *)malloc(sizeof(List));
    list->size = 0;
    list->next = NULL;

    return list;
}

int Insert(List *pList, NODEL *item)
{
    item->next = NULL;

    if (pList->next == NULL)
        pList->next = item;
    else
    {
        NODEL *current = pList->next;
        while (current->next != NULL)
            current = current->next;
        current->next = item;
    }

    pList->size++;

    return TRUE;
}

char *Check(List *pList, char *search)
{
    NODEL *current = pList->next;
    while (current != NULL)
    {
        if (strcmp(current->key, search) == 0)
            return current->value;

        current = current->next;
    }

    return pList->defaultFolder;
}