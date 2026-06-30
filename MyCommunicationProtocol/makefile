all:
	mkdir -p build
	gcc -pthread src/client/client.c src/server/network\ manager/network_manager.c src/server/client\ manager/client_manager.c -o build/client
	gcc -pthread src/server/server.c src/server/network\ manager/network_manager.c src/server/client\ manager/client_manager.c -o build/server

clean:
	rm -f build/client build/server
