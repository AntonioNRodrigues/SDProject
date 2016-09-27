ROOT = .
INC = $(ROOT)/inc
OBJ = $(ROOT)/obj
SRC = $(ROOT)/src

CC = gcc
FLAGS = -I$(INC) -lm -Wall

T_DATA_OBJ = $(OBJ)/data.o $(OBJ)/test_data.o
T_ENTRY_OBJ = $(OBJ)/data.o $(OBJ)/entry.o $(OBJ)/test_entry.o

all: clean test_data test_entry

test_data: $(T_DATA_OBJ) 
	$(CC) $(T_DATA_OBJ) -o testData

test_entry:$(T_ENTRY_OBJ)
	$(CC) $(T_ENTRY_OBJ) -o testEntry
	
$(OBJ)/data.o: $(SRC)/data.c $(INC)/data.h 
	$(CC) $(FLAGS) -c $(SRC)/data.c -o $(OBJ)/data.o

$(OBJ)/entry.o: $(INC)/data.h $(SRC)/entry.c $(INC)/entry.h
	$(CC) $(FLAGS) -c $(SRC)/entry.c -o $(OBJ)/entry.o
	
$(OBJ)/test_data.o: $(SRC)/test_data.c $(INC)/data.h
	$(CC) $(FLAGS) -c $(SRC)/test_data.c -o $(OBJ)/test_data.o
	
$(OBJ)/test_entry.o: $(SRC)/test_entry.c $(INC)/entry.h $(INC)/data.h
	$(CC) $(FLAGS) -c $(SRC)/test_entry.c -o $(OBJ)/test_entry.o

clean:
	rm -fr *.o test_data test_entry 