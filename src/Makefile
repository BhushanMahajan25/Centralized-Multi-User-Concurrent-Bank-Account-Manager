#makefile
#cut paste this comment in documentation
#General introduction of Makefile:
#Makefile is used to automate the build process for a C++ application

CC = g++

CFLAGS = -c --std=c++11

OBJDIR = obj

all : server client

server : $(OBJDIR)/Transaction.o $(OBJDIR)/User.o $(OBJDIR)/fileHandling.o $(OBJDIR)/clientSocketQueue.o $(OBJDIR)/serverMain.o
	$(CC) $(OBJDIR)/Transaction.o $(OBJDIR)/User.o $(OBJDIR)/fileHandling.o $(OBJDIR)/clientSocketQueue.o $(OBJDIR)/serverMain.o -o server

client : $(OBJDIR)/Transaction.o $(OBJDIR)/User.o $(OBJDIR)/fileHandling.o $(OBJDIR)/clientMain.o
	$(CC) $(OBJDIR)/Transaction.o $(OBJDIR)/User.o $(OBJDIR)/fileHandling.o $(OBJDIR)/clientMain.o -o client

$(OBJDIR)/serverMain.o : serverSrc/serverMain.cpp
	$(CC) $(CFLAGS) serverSrc/serverMain.cpp -o $(OBJDIR)/serverMain.o

$(OBJDIR)/clientMain.o : clientSrc/clientMain.cpp
	$(CC) $(CFLAGS) clientSrc/clientMain.cpp -o $(OBJDIR)/clientMain.o

$(OBJDIR)/Transaction.o : services/Transaction.cpp
	$(CC) $(CFLAGS) services/Transaction.cpp -o $(OBJDIR)/Transaction.o

$(OBJDIR)/User.o : services/User.cpp
	$(CC) $(CFLAGS) services/User.cpp -o $(OBJDIR)/User.o

$(OBJDIR)/fileHandling.o : services/fileHandling.cpp
	$(CC) $(CFLAGS) services/fileHandling.cpp -o $(OBJDIR)/fileHandling.o

$(OBJDIR)/clientSocketQueue.o : services/clientSocketQueue.cpp
	$(CC) $(CFLAGS) services/clientSocketQueue.cpp -o $(OBJDIR)/clientSocketQueue.o

clean : 
	rm $(OBJDIR)/* server client