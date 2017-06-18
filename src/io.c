extern int checkCache(int client, char *command, char *request);
extern void handle_client(int client, char *command, char *request);

struct arg_struct
{
    int connection;
    char *command;
    char *request;
};

void transmitData(void *arguments)
{
    struct arg_struct *args = arguments;

    char *buffer = malloc(sizeof(char) * 256);
    read(args->connection, buffer, 255);

    char *pch;

    pch = strtok(buffer, " ");
    strcpy(args->command, pch);

    //printf("test value %s\n", args->command);

    pch = strtok(NULL, " ");
    strcpy(args->request, pch);

    if (checkCache(args->connection, args->command, args->request) == -1)
    {
        handle_client(args->connection, args->command, args->request);
    }

    pch = NULL;
    free(buffer);
    free(args->command);
    free(args->request);
    free(args);
}

void sendMessage(int client, char *message)
{
    write(client, message, strlen(message));
}

void sendData(int client, char *message, long length)
{
    write(client, message, length);
}

void shutdownClient(int client)
{
    shutdown(client, SHUT_WR);
    char buffer[200];
    while (read(client, buffer, 200) > 0)
        ;
    close(client);
}