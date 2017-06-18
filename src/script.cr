require "socket"

puts "Hello World"

sock = Socket.tcp(Socket::Family::INET)
sock.connect "127.0.0.1", 9010
sock.puts("dis werks")
#sock.close

loop do
    message, client_addr = sock.receive
end