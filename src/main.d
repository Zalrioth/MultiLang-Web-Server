import std.stdio;
import std.concurrency;
import std.file;
import std.json;
import std.conv : to;
import core.thread;

//https://stackoverflow.com/questions/1630597/how-to-use-a-c-library-from-d
//https://stackoverflow.com/questions/10062750/call-cc-from-d-language
//https://stackoverflow.com/questions/27607130/undefined-reference-to-dlsym-and-dlopen
//http://forum.dlang.org/thread/mailman.278.1281680325.13841.digitalmars-d@puremagic.com
//https://stackoverflow.com/questions/26784804/a-simple-makefile-with-chain-dependencies-linker-input-file-unused-because-lin
//https://dlang.org/phobos/std_file.html
//https://dlang.org/spec/const3.html
//http://forum.dlang.org/post/zkecjcettkqgwenkpylc@forum.dlang.org
//https://stackoverflow.com/questions/26923976/calling-a-d-function-directly-from-c
//http://nomad.so/2015/09/working-with-files-in-the-d-programming-language/
//https://forum.dlang.org/post/mailman.1844.1462448128.26339.digitalmars-d-learn@puremagic.com
//http://forum.dlang.org/post/op.v967sqj2x8p62v@simendsjo-desktop
//https://stackoverflow.com/questions/8811806/how-to-cast-a-char-to-string-in-d
//https://dlang.org/library/std/stdio/file.tell.html
//http://forum.dlang.org/post/mailman.106.1331075945.4860.digitalmars-d-learn@puremagic.com
//http://forum.dlang.org/post/mailman.4350.1405630460.2907.digitalmars-d-learn@puremagic.com
//https://www.tutorialspoint.com/d_programming/d_programming_structs.htm
//https://stackoverflow.com/questions/14886171/test-if-an-associative-array-contains-a-key-in-d
//http://www.digitalmars.com/d/archives/digitalmars/D/learn/Read_integer_from_console_and_sleep_35542.html

extern (C) void runMain(short port);
extern (C) void sendMessage(int client, char* message);
extern (C) void sendData(int client, char* message, long length);
extern (C) void shutdownClient(int client);
extern (C) char* readFile(char* request, long* length);

struct Item {
    char* data;
    long length;
}

shared Item[string] cache;

void main()
{
    writeln("Hello World from D!");

    immutable JSONValue settings = parseJSON(readText("settings.conf"));

    immutable JSONValue portCheck = settings["Port"];

    short port = 8808;

    if (portCheck.type() == JSON_TYPE.INTEGER)
    {
        port = cast(short)portCheck.integer;
    }
    else
    {
        writeln("Error: incorrect port format, defaulting to 8808.");
    }

    // TODO: Change hard coding to dynamic
    Item testItem;
    char[] asdasd = "html/index.html\0".dup;
    testItem.data = readFile(asdasd.ptr, &testItem.length);
    cache["/index.html"] = cast(shared)testItem;

    Item testItem2;
    char[] asdasd2 = "html/index.html\0".dup;
    testItem2.data = readFile(asdasd2.ptr, &testItem2.length);
    cache["/"] = cast(shared)testItem2;

    Item testItem3;
    char[] asdasd3 = "html/mario.png\0".dup;
    testItem3.data = readFile(asdasd3.ptr, &testItem3.length);
    cache["/mario.png"] = cast(shared)testItem3;

    Item testItem4;
    char[] asdasd4 = "html/shrek.png\0".dup;
    testItem4.data = readFile(asdasd4.ptr, &testItem4.length);
    cache["/shrek.png"] = cast(shared)testItem4;

    Item testItem5;
    char[] asdasd5 = "html/sanic.png\0".dup;
    testItem5.data = readFile(asdasd5.ptr, &testItem5.length);
    cache["/sanic.png"] = cast(shared)testItem5;

    Item testItem6;
    char[] asdasd6 = "html/doge.png\0".dup;
    testItem6.data = readFile(asdasd6.ptr, &testItem6.length);
    cache["/doge.png"] = cast(shared)testItem6;

    Item testItem7;
    char[] asdasd7 = "html/favicon.ico\0".dup;
    testItem7.data = readFile(asdasd7.ptr, &testItem7.length);
    cache["/favicon.ico"] = cast(shared)testItem7;

    spawn(&runMain, port);
    writeln("End of D!");
}

extern (C) int checkCache(int client, char[] request)
{
    string getRequest = to!string(request.ptr);

    //writeln("\x1B[31m" ~ "d side request is: " ~ getRequest ~ " for client " ~ to!string(client) ~ "\x1B[37m");

    char* data;
    long dataLength;

    Item* checkValue = cast(Item*)(getRequest in cache);

    if(checkValue)
    {
        writeln("\x1B[32m" ~ "Read from cache: " ~ getRequest~ "\x1B[37m");
        data = cast(char*)cache[getRequest].data;
        dataLength = cache[getRequest].length;
    }
    else
    {
        writeln("Skipped cache read");
        return -1;
    }

    char[] sending = ("HTTP/1.0 200 OK\r\n" ~ "Server: Ingot\r\n" ~ "Content-length: " ~ to!string(dataLength)~ "\r\n"
    ~ "Content-Type: text/html\r\n" ~ "Connection: close\r\n\r\n").dup;

    sendMessage(client, sending.ptr);
    sendData(client, data, dataLength);

    shutdownClient(client);
    return 1;
}