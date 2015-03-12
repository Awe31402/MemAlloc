PROJECT=implement_free_malloc
LIB_PATH=./lib

default:
	gcc -g -I$(LIB_PATH) $(PROJECT).c -o $(PROJECT).o -L$(LIB_PATH) -ltlpi
clean:
	rm $(PROJECT)

