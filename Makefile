CC = g++ -Wno-write-strings
SERVER_FILE = simple_server.cpp
HTTP_SERVER_FILE = http_server.cpp

.PHONY: kill server

server: $(SERVER_FILE)
	@$(CC) $(SERVER_FILE) -o server

kill: 
	@fuser -n tcp -k 8080

clean:
	rm -f server

client:
	wget http://localhost:8080/apart1/flat11
