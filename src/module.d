import std.conv : to;
import std.socket;
import std.stdio;
import std.concurrency;
import std.random;
import core.thread;
import std.json;

shared Socket[string] handles;

extern (C) void listenForScripts()
{
    Socket server = new TcpSocket();
    server.setOption(SocketOptionLevel.SOCKET, SocketOption.REUSEADDR, true);
    server.bind(new InternetAddress(9010));
    server.listen(1);

    while (true)
    {
        Socket client = server.accept();

        char[1024] buffer;
        string received = to!string(client.receive(buffer));

        writeln("The client is: " ~ received);

        handles[received] = cast(shared) client;
    }
}

extern (C) void processScript(int client, string script, string arguments)
{
    JSONValue data;
    data.object["client"] = client;
    data.object["script"] = script;
    data.object["arguments"] = arguments;

    (cast(Socket) handles[script]).send(data.toString);
}
