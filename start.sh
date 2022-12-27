#!/bin/bash

echo "Compiling all files...."
make server
echo "Starting the HTTP server..."

read -p "Enter the port: " PORT

while [ 0 -le 1 ] 
do 
	make server && ./server $PORT 
	echo "----------------------------------------------END OF RESPONSE----------------------------------------------"
done

echo "Server Terminated"
