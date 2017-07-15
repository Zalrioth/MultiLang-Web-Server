import std.stdio;
import std.file;
import std.json;
import core.memory;

struct Settings
{
    short port;
    short workers;
}

shared Settings settings;

extern (C) int initSettings()
{
    GC.disable;

    settings.port = 80;
    settings.workers = 1;

    string settingsContent;

    try {
        settingsContent = readText("settings.conf");
    }
    catch (FileException e)
    {
        writeln("Error: Can't read settings file, using default values.");
    }

    immutable JSONValue settingsFile = parseJSON(settingsContent);
    immutable JSONValue portCheck = settingsFile["Port"];
    immutable JSONValue workerCheck = settingsFile["Workers"];

    GC.free(cast(void*)settingsContent.ptr);

    if (portCheck.type() == JSON_TYPE.INTEGER)
        settings.port = cast(short) portCheck.integer;
    else
        writeln("Error: Incorrect port format, defaulting to 8808.");

    if (workerCheck.type() == JSON_TYPE.INTEGER)
        settings.workers = cast(short) workerCheck.integer;
    else
        writeln("Error: Incorrect worker format, defaulting to 1.");

    return 0;
}

extern (C) short getPort()
{
    return settings.port;
}
extern (C) short getWorkers()
{
    return settings.workers;
}