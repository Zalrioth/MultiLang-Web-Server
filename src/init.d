import std.stdio;
import std.file;
import std.json;
import core.memory;
import std.regex;
import std.array;
import std.conv : to;

extern (C) void add_host(void *listPointer, char *host, char *folder);

extern (C) int initSettings(void *listPointer, short *port, short *workers)
{
    GC.disable;
    
    string settingsContent;

    try
        settingsContent = readText("settings.conf");
    catch (FileException e)
        writeln("Error: Can't read settings file, using default values.");

    try
    {
        foreach (content; matchAll(settingsContent, r"\w+ \{([^}]*)\}"))
        {
            immutable auto processString = content[0].split(" ");

            immutable string action = processString[0];
            immutable string data = join(processString[1..$]);
            immutable JSONValue dataJSON = parseJSON(data);

            if (action == "Global")
            {
                immutable JSONValue portCheck = dataJSON["Port"];
                immutable JSONValue workerCheck = dataJSON["Workers"];

                if (portCheck.type() == JSON_TYPE.INTEGER)
                    *port = cast(short) portCheck.integer;
                else
                    writeln("Error: Incorrect port format, defaulting to 8808.");

                if (workerCheck.type() == JSON_TYPE.INTEGER)
                    *workers = cast(short) workerCheck.integer;
                else
                     writeln("Error: Incorrect worker format, defaulting to 1.");
            }
            else if (action == "Server")
            {
                immutable JSONValue hostCheck = dataJSON["Host"];
                immutable JSONValue folderCheck = dataJSON["Folder"];

                foreach (host; hostCheck.array)
                {
                    immutable string hostString = host.str ~ "\0";
                    immutable string folderString = folderCheck.str ~ "\0";

                    add_host(listPointer, cast(char*)hostString.ptr, cast(char*)folderString.ptr);
                }
            }
        }
    }
    catch (Exception e)
    {
        writeln("Error parsing settings file");
        return 1;
    }

    GC.collect();

    return 0;
}