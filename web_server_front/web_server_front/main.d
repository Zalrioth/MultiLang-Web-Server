//http://forum.dlang.org/post/fqhleghrjamyyjjddzaj@forum.dlang.org
//http://arsdnet.net/dcode/book/chapter_02/03/client.d
//https://www.tutorialspoint.com/d_programming/d_programming_concurrency.htm


/*
> void main() {
>     Socket server = new TcpSocket();
>     server.setOption(SocketOptionLevel.SOCKET, SocketOption.REUSEADDR, true);
>     server.bind(new InternetAddress(8080));
>     server.listen(1);
>
>     while(true) {
>         Socket client = server.accept();
>
>         char[1024] buffer;
>         auto received = client.receive(buffer);
>
>         writefln("The client said:\n%s", buffer[0.. received]);
>
>         enum header =
>             "HTTP/1.0 200 OK\nContent-Type: text/html; charset=utf-8\n\n";
>
>         string response = header ~ "Hello World!\n";
>         client.send(response);
>
>         client.shutdown(SocketShutdown.BOTH);
>         client.close();
>     }
> }*/

import std.conv : to;
import std.socket;
import std.stdio;
import std.concurrency;
import core.thread;

shared Socket processSocket;

shared Socket[int] handles;

void main()
{
	spawn(&connectToProcessor);
	listenForClients();
}

void connectToProcessor()
{
	Socket socket = new Socket(AddressFamily.INET,  SocketType.STREAM, ProtocolType.TCP);
	socket.connect(new InternetAddress("127.0.0.1", 8810));

	processSocket = cast(shared)socket;

	while (true)
	{
		char[1024] buffer;
		auto received = socket.receive(buffer);

		//writeln("Processor said: ", buffer[0 .. received]);

		Socket client = cast(Socket)handles[123];
		client.send(buffer[0 .. received]);
	}
}

void listenForClients()
{
	Socket server = new TcpSocket();
	server.setOption(SocketOptionLevel.SOCKET, SocketOption.REUSEADDR, true);
	server.bind(new InternetAddress(8808));
	server.listen(1);

	while(true)
	{
		Socket client = server.accept();

		char[1020] buffer;
		auto received = client.receive(buffer);

		writefln("The client said:\n%s", buffer[0.. received]);

		char[4] id = "123 ";
		char[1024] fullNelson = id ~ buffer;

		string response = "Hello World!\n";
		Socket socket = cast(Socket)processSocket;
		socket.send(fullNelson[0.. received+4]);

		handles[123] = cast(shared)client;
	}
}