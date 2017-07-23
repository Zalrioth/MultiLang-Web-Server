extern void handle_client(int client, char *command, char *getFile, char *host);

#define BUFFER_SIZE 4096

struct arg_struct
{
    int connection;
    char request[BUFFER_SIZE];
};

int transmit_data(void *arguments, List *hostList)
{
    struct arg_struct *args = arguments;
    read(args->connection, args->request, BUFFER_SIZE - 1);

    char *command, *encoding, *host, *getFile, *httpVersion;
    //char *command, *host, *getFile;

    command = strtok(args->request, " ");
    getFile = strtok(NULL, " ");
    httpVersion = strtok(NULL, "\n");
    char *headerToken = strtok(NULL, " ");

    while (headerToken)
    {
        /*if (strcmp(headerToken, "Host:") == 0)
            host = strtok(NULL, "\n");
        else if (strcmp(headerToken, "Accept-Encoding:") == 0)
            encoding = strtok(NULL, "\n");
        else
            strtok(NULL, "\n");*/

        if (strcmp(headerToken, "Host:") == 0)
            host = strtok(NULL, "\r");
        else if (strcmp(headerToken, "Accept-Encoding:") == 0)
            encoding = strtok(NULL, "\r");
        strtok(NULL, "\n");

        headerToken = strtok(NULL, " ");
    }

    /*printf("command: %s\n", command);
    printf("getFile: %s\n", getFile);
    printf("host: %s\n", host);
    printf("encoding: %s\n", encoding);*/

    //char *findFile = (char *)malloc(sizeof(char) * 1024);
    //strcpy(findFile, Check(hostList, host));
    //strcat(findFile, getFile);

    //printf("wants ~%s~\n", host);
    //printf("found ~%s~\n", Check(hostList, host));
    //printf("searching for %s\n", host);

    if (!host)
        host = hostList->defaultFolder;

    if (command && getFile)
        handle_client(args->connection, command, getFile, check(hostList, host));
    else
        return 1;

    free(args);

    return 0;
}

int send_message(int client, char *message)
{
    if (write(client, message, strlen(message)) == -1)
        return -1;
    return 0;
}

int send_data(int client, char *message, long length)
{
    if (write(client, message, length) == -1)
        return -1;
    return 0;
}

void shutdown_client(int client)
{
    shutdown(client, SHUT_WR);
    char buffer[200];
    while (read(client, buffer, 200) > 0)
        ;
    close(client);
}