extern void handle_client(int client, char *command, char *getFile, char *host);

#define BUFFER_SIZE 4096

struct arg_struct
{
    int connection;
    char request[BUFFER_SIZE];
};

int transmitData(void *arguments, List *hostList)
{
    struct arg_struct *args = arguments;
    read(args->connection, args->request, BUFFER_SIZE - 1);

    char *command, *encoding, *host, *getFile, *httpVersion;

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
        host = "hellodomain";

    if (command && getFile)
        handle_client(args->connection, command, getFile, Check(hostList, host));
    else
        return 1;

    free(args);

    return 0;
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