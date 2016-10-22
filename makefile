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
T_TABLE_OBJ = $(OBJ)/data.o $(OBJ)/entry.o $(OBJ)/list.o $(OBJ)/table.o $(OBJ)/test_table.o
T_MESSAGE_OBJ = $(OBJ)/data.o $(OBJ)/entry.o $(OBJ)/list.o $(OBJ)/table.o $(OBJ)/message.o $(OBJ)/test_message.o
CLIENT_OBJ = $(OBJ)/data.o $(OBJ)/entry.o $(OBJ)/list.o $(OBJ)/table.o $(OBJ)/message.o $(OBJ)/network_client.o $(OBJ)/table-client.o
SERVER_OBJ = $(OBJ)/data.o $(OBJ)/entry.o $(OBJ)/list.o $(OBJ)/table.o $(OBJ)/message.o  $(OBJ)/network_client.o $(OBJ)/table-server.o

all: clean test_data test_entry test_list test_table test_message server client

server: $(SERVER_OBJ)
	$(CC) $(SERVER_OBJ) -o table-server

client: $(CLIENT_OBJ)
	$(CC) $(CLIENT_OBJ) -o table-client
	
test_data: $(T_DATA_OBJ) 
	$(CC) $(T_DATA_OBJ) -o test_data

test_entry:$(T_ENTRY_OBJ)
	$(CC) $(T_ENTRY_OBJ) -o test_entry

test_list: $(T_LIST_OBJ)
	$(CC) $(T_LIST_OBJ) -o test_list
	
test_table: $(T_TABLE_OBJ)
	$(CC) $(T_TABLE_OBJ) -o test_table
	
test_message: $(T_MESSAGE_OBJ)
	$(CC) $(T_MESSAGE_OBJ) -o test_message

#test_table_client: $(T_TABLE_CLIENT_OBJ)
#	$(CC) $(T_TABLE_CLIENT_OBJ) -o test_table_client

#test_network_client: $(T_NETWORK_CLIENT_OBJ)
#	$(CC) $(T_NETWORK_CLIENT_OBJ) -o test_network_client
	
$(OBJ)/data.o: $(SRC)/data.c $(INC)/data.h 
	$(CC) $(FLAGS) -c $(SRC)/data.c -o $(OBJ)/data.o

$(OBJ)/entry.o: $(INC)/data.h $(SRC)/entry.c $(INC)/entry.h
	$(CC) $(FLAGS) -c $(SRC)/entry.c -o $(OBJ)/entry.o
	
$(OBJ)/list.o: $(SRC)/list.c $(INC)/list.h $(INC)/list-private.h
	$(CC) $(FLAGS) -c $(SRC)/list.c -o $(OBJ)/list.o
	
$(OBJ)/table.o: $(SRC)/table.c $(INC)/table.h $(INC)/table-private.h
	$(CC) $(FLAGS) -c $(SRC)/table.c -o $(OBJ)/table.o

$(OBJ)/message.o: $(SRC)/message.c $(INC)/message.h $(INC)/message-private.h
	$(CC) $(FLAGS) -c $(SRC)/message.c -o $(OBJ)/message.o

$(OBJ)/table-client.o: $(SRC)/table-client.c $(INC)/network_client-private.h $(INC)/table-private.h
	$(CC) $(FLAGS) -c $(SRC)/table-client.c -o $(OBJ)/table-client.o

$(OBJ)/network_client.o: $(SRC)/network_client.c $(INC)/inet.h $(INC)/network_client-private.h
	$(CC) $(FLAGS) -c $(SRC)/network_client.c -o $(OBJ)/network_client.o

$(OBJ)/table-server.o: $(SRC)/table-server.c $(INC)/inet.h $(INC)/network_client-private.h $(INC)/table.h
	$(CC) $(FLAGS) -c $(SRC)/table-server.c -o $(OBJ)/table-server.o

$(OBJ)/test_data.o: test_data.c $(INC)/data.h
	$(CC) $(FLAGS) -c test_data.c -o $(OBJ)/test_data.o
	
$(OBJ)/test_entry.o: test_entry.c $(INC)/entry.h $(INC)/data.h
	$(CC) $(FLAGS) -c test_entry.c -o $(OBJ)/test_entry.o

$(OBJ)/test_list.o: test_list.c $(INC)/list.h
	$(CC) $(FLAGS) -c test_list.c -o $(OBJ)/test_list.o

$(OBJ)/test_table.o: test_table.c $(INC)/table.h
	$(CC) $(FLAGS) -c test_table.c -o $(OBJ)/test_table.o

$(OBJ)/test_message.o: test_message.c $(INC)/message.h
	$(CC) $(FLAGS) -c test_message.c -o $(OBJ)/test_message.o

#$(OBJ)/test_table_client.o: test_table_client.c $(INC)network_client-private.h
#	$(CC) $(FLAGS) -c test_table_client.c -o $(OBJ)test_table_client.o

#S(OBJ)/test_network_client.o: network_client.c $(INC)inet.h $(INC)network_client-private.h
#	$(CC) $(FLAGS) -c test_network_client.c -o $(OBJ)test_network_client.o

clean:
	rm -fr $(OBJ)/*.o test_data test_entry test_list test_table test_message
	
zip_work:
	zip -r $(GG) $(OBJ) $(INC)/*.h $(SRC)/*.c *.md makefile	
		
valgrindData:
	valgrind -v --leak-check=full --track-origins=yes --log-file=valgrindData.log ./test_data

valgrindEntry:
	valgrind -v --leak-check=full --track-origins=yes --log-file=valgrindEntry.log ./test_entry

valgrindList:
	valgrind -v --leak-check=full --track-origins=yes --log-file=valgrindList.log ./test_list

valgrindTable:
	valgrind -v --leak-check=full --track-origins=yes --log-file=valgrindTable.log ./test_table

valgrindMessage:
	valgrind -v --leak-check=full --track-origins=yes --log-file=valgrindMessage.log ./test_message

#valgrindTableClient:
#	valgrind -v --leak-check=full --track-origins=yes --log-file=valgrindTableClient.log ./test_table_client

#valgrindNetworkClient:
#	valgrind -v --leak-check=full --track-origins=yes --log-file=valgrindNetworkClient.log ./test_network_client	