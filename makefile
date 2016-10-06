ROOT = .
INC = $(ROOT)/inc
OBJ = $(ROOT)/obj
SRC = $(ROOT)/src

GG = grupo33
CC = gcc
FLAGS = -I$(INC) -lm -Wall

T_DATA_OBJ = $(OBJ)/data.o $(OBJ)/test_data.o
T_ENTRY_OBJ = $(OBJ)/data.o $(OBJ)/entry.o $(OBJ)/test_entry.o
T_LIST_OBJ = $(OBJ)/data.o $(OBJ)/entry.o $(OBJ)/list.o $(OBJ)/test_list.o

all: clean test_data test_entry test_list

test_data: $(T_DATA_OBJ) 
	$(CC) $(T_DATA_OBJ) -o test_data

test_entry:$(T_ENTRY_OBJ)
	$(CC) $(T_ENTRY_OBJ) -o test_entry

test_list: $(T_LIST_OBJ)
	$(CC) $(T_LIST_OBJ) -o test_list
	
$(OBJ)/data.o: $(SRC)/data.c $(INC)/data.h 
	$(CC) $(FLAGS) -c $(SRC)/data.c -o $(OBJ)/data.o

$(OBJ)/entry.o: $(INC)/data.h $(SRC)/entry.c $(INC)/entry.h
	$(CC) $(FLAGS) -c $(SRC)/entry.c -o $(OBJ)/entry.o
	
$(OBJ)/list.o: $(SRC)/list.c $(INC)/list.h $(INC)/list-private.h
	$(CC) $(FLAGS) -c $(SRC)/list.c -o $(OBJ)/list.o
	
$(OBJ)/test_data.o: test_data.c $(INC)/data.h
	$(CC) $(FLAGS) -c test_data.c -o $(OBJ)/test_data.o
	
$(OBJ)/test_entry.o: test_entry.c $(INC)/entry.h $(INC)/data.h
	$(CC) $(FLAGS) -c test_entry.c -o $(OBJ)/test_entry.o

$(OBJ)/test_list.o: test_list.c $(INC)/list.h
	$(CC) $(FLAGS) -c test_list.c -o $(OBJ)/test_list.o

clean:
	rm -fr $(OBJ)/*.o test_data test_entry test_list
		
valgrindData:
	valgrind -v --leak-check=full --track-origins=yes --log-file=valgrindData.log ./testData

valgrindEntry:
	valgrind -v --leak-check=full --track-origins=yes --log-file=valgrindEntry.log ./testEntry

valgrindList:
	valgrind -v --leak-check=full --track-origins=yes --log-file=valgrindList.log ./testList

	