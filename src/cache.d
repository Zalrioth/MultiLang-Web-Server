import std.stdio;
import std.concurrency;
import std.file;
import std.json;
import std.conv : to;
import std.zlib;
import core.memory;
import std.parallelism;

extern (C) void listenForScripts();

extern (C) void sendMessage(int client, char* message);
extern (C) void sendData(int client, char* message, long length);
extern (C) void shutdownClient(int client);

struct Item
{
    char* data;
    ubyte[] zippedData;
    char* head;
    char* zippedHead;
    long length;
    long zippedLength;
}

shared Item[string] cache;

struct Settings
{
    short port;
    short workers;
}

shared Settings settings;

extern (C) void collectGarbage()
{
    GC.collect();
}

extern (C) int runMain()
{
    GC.disable;

    writeln("Hello World from D!");

    spawn(&listenForScripts);

    immutable JSONValue settingsFile = parseJSON(readText("settings.conf"));
    immutable JSONValue portCheck = settingsFile["Port"];
    immutable JSONValue workerCheck = settingsFile["Workers"];

    settings.port = 8808;
    settings.workers = 1;

    if (portCheck.type() == JSON_TYPE.INTEGER)
        settings.port = cast(short) portCheck.integer;
    else
        writeln("Error: incorrect port format, defaulting to 8808.");

    if (workerCheck.type() == JSON_TYPE.INTEGER)
        settings.workers = cast(short) workerCheck.integer;
    else
        writeln("Error: incorrect worker format, defaulting to 1.");

    auto cacheFiles = dirEntries("html", SpanMode.breadth);
    foreach (cFile; parallel(cacheFiles, settings.workers))
        cache[cFile.name] = cacheItem(cFile.name);

    cache["html/"] = cache["html/index.html"];

    writeln("End of D!");

    return 0;
}

extern (C) void getSettings(short* port, short* workers)
{
    *port = settings.port;
    *workers = settings.workers;
}

shared(Item) cacheItem(string name)
{
    Item tItem;
    auto getFile = cast(ubyte[]) read(name);
    tItem.data = cast(char*) getFile.ptr;
    tItem.length = getFile.length;

    auto comp = new Compress(9, HeaderFormat.gzip);
    tItem.zippedData ~= cast(ubyte[]) comp.compress(getFile);
    tItem.zippedData ~= cast(ubyte[]) comp.flush();
    tItem.zippedLength = tItem.zippedData.length;
    tItem.head = cast(char*)("HTTP/1.0 200 OK\r\nServer: Ingot\r\nContent-length: " ~ to!string(
            tItem.length) ~ "\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n").ptr;
    tItem.zippedHead = cast(char*)("HTTP/1.0 200 OK\r\nServer: Ingot\r\nContent-length: " ~ to!string(
            tItem.zippedLength) ~ "\r\nContent-Type: text/html\r\nContent-Encoding: gzip"
            ~ "\r\nTransfer-Encoding: gzip\r\nConnection: close\r\n\r\n").ptr;
    return cast(shared) tItem;
}

extern (C) int checkCache(int client, char* command, char* request)
{
    immutable string getCommand = to!string(command);

    if (getCommand != "GET")
        return -1;

    immutable string getRequest = "html" ~ to!string(request);

    Item* checkValue = cast(Item*)(getRequest in cache);

    if (!checkValue)
    {
        writeln("Read from disc");
        return -1;
    }

    /*sendMessage(client, cast(char*) cache[getRequest].zippedHead);
    sendData(client, cast(char*) cache[getRequest].zippedData,
            cast(long) cache[getRequest].zippedLength);*/

    sendMessage(client, cast(char*) cache[getRequest].head);
    sendData(client, cast(char*) cache[getRequest].data, cast(long) cache[getRequest].length);

    shutdownClient(client);

    return 1;
}
