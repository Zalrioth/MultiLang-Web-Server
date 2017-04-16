require "socket"

server = TCPServer.new("localhost", 8810)
loop do
  server.accept do |client|
    message = client.gets
    client << message # echo the message back
  end
end