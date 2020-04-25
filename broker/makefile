all: clean broker
broker:
	gcc -Wall -Wextra -g -fPIC src/utils.c -c -o	debug/utils.o -lcommons -lpthread
	gcc -Wall -Wextra -g -fPIC src/servidor.c -c -o	debug/servidor.o -lcommons -lpthread
	gcc -Wall -Wextra -g -fPIC src/broker.c -c -o debug/broker.o -lcommons -lpthread
	gcc -Wall -Wextra -g debug/utils.o debug/servidor.o debug/broker.o -o bin/Broker -lcommons -lpthread
clean:
	rm -f ./debug/*.o ./bin/Broker
