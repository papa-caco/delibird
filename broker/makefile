all: clean broker

broker:
	gcc -I"/home/utnso/workspace/tp-2020-1c-Los-Que-Aprueban/delibird" -Wno-switch-enum -Werror -g -fPIC src/broker_serializ.c -c -o debug/broker_serializ.o -lcommons -lpthread -ldelibird
	gcc -I"/home/utnso/workspace/tp-2020-1c-Los-Que-Aprueban/delibird" -Wno-switch-enum -Werror -g -fPIC src/broker_oper.c -c -o debug/broker_oper.o -lcommons -lpthread -ldelibird
	gcc -I"/home/utnso/workspace/tp-2020-1c-Los-Que-Aprueban/delibird" -Wno-switch-enum -Werror -g -fPIC src/broker_utils.c -c -o debug/broker_utils.o -lcommons -lpthread -ldelibird
	gcc -I"/home/utnso/workspace/tp-2020-1c-Los-Que-Aprueban/delibird" -Wall -Werror -g -fPIC src/broker.c -c -o debug/broker.o -lcommons -lpthread -ldelibird
	gcc -L"/home/utnso/workspace/tp-2020-1c-Los-Que-Aprueban/delibird/build" -Wall -Werror -g debug/broker.o debug/broker_utils.o debug/broker_serializ.o  debug/broker_oper.o -o bin/broker -lcommons -lpthread -ldelibird

clean:
	rm -f ./debug/*.o ./bin/broker
