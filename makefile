ROOT = .
INC = $(ROOT)/inc
OBJ = $(ROOT)/obj
SRC = $(ROOT)/src

GG = grupo33
CC = gcc
FLAGS = -I$(INC) -lm -Wall

CLIENT_OBJ = $(OBJ)/data.o $(OBJ)/entry.o $(OBJ)/list.o $(OBJ)/table.o $(OBJ)/message.o $(OBJ)/network_client.o $(OBJ)/client_stub.o $(OBJ)/table-client.o
SERVER_OBJ = $(OBJ)/data.o $(OBJ)/entry.o $(OBJ)/list.o $(OBJ)/table.o $(OBJ)/message.o $(OBJ)/network_client.o $(OBJ)/table_skel.o $(OBJ)/primary_backup.o $(OBJ)/table-server.o

all: clean table-server table-client

table-server: $(SERVER_OBJ)
	$(CC) $(SERVER_OBJ) -o table-server -lpthread

table-client: $(CLIENT_OBJ)
	$(CC) $(CLIENT_OBJ) -o table-client
	
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

$(OBJ)/client_stub.o: $(SRC)/client_stub.c $(INC)/client_stub.h $(INC)/client_stub-private.h
	$(CC) $(FLAGS) -c $(SRC)/client_stub.c -o $(OBJ)/client_stub.o

$(OBJ)/table_skel.o: $(SRC)/table_skel.c $(INC)/table_skel.h $(INC)/table_skel-private.h
	$(CC) $(FLAGS) -c $(SRC)/table_skel.c -o $(OBJ)/table_skel.o

$(OBJ)/table-server.o: $(SRC)/table-server.c $(INC)/inet.h $(INC)/network_client-private.h $(INC)/table.h
	$(CC) $(FLAGS) -c $(SRC)/table-server.c -o $(OBJ)/table-server.o

$(OBJ)/primary_backup.o: $(SRC)/primary_backup.c $(INC)/primary_backup-private.h
	$(CC) $(FLAGS) -c $(SRC)/primary_backup.c -o $(OBJ)/primary_backup.o
	
clean:
	rm -fr $(OBJ)/*.o *.log table-client table-server
	
zip_work:
	zip -r $(GG) $(OBJ) $(INC)/*.h $(SRC)/*.c *.sh *.md makefile	
	