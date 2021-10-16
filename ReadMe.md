# Centralized Multi-User Concurrent Bank Account Manager.

## Purpose:
To understand and implement concepts of sockets, processes, threads.

## Overview
The project has two important component systems.

a. Bank Server: 
The server program that services online requests for account manipulations and maintains all customer records correctly. Additionally, the bank server can have it's own service that periodically deposits an interest amount to each account based on some fixed rate.

b. Clients: 
Customers are clients of the bank server and use its services to update bank accounts. The operations that can be performed on an account are: withdrawal of an amount from an account and deposit of an amount into an account. 

## Steps to execute the project 
In directory src/

1. Build the project and generate server and client executables.

        make compile 

 
2.	Start the server first.

        ./server <SERVER IP ADDRESS> <PORT> <PATH OF Records.txt>
    You can find Records.txt in  *[src/input-files/Records.txt](https://github.com/BhushanMahajan25/Centralized-Multi-User-Concurrent-Bank-Account-Manager/blob/main/src/input-files/Records.txt)*
3.	Start client

        ./client <SERVER IP ADDRESS> <PORT> <PATH OF Transactions.txt>
    You can find Records.txt in  *[src/input-files/Transactions.txt](https://github.com/BhushanMahajan25/Centralized-Multi-User-Concurrent-Bank-Account-Manager/blob/main/src/input-files/Transactions.txt)*

    ### NOTE
    a. You can spwan multiple clients by running bash script [clientscript.sh](https://github.com/BhushanMahajan25/Centralized-Multi-User-Concurrent-Bank-Account-Manager/blob/main/src/clientscript.sh)
    b. Make sure you change **SERVER IP ADDRESS** and **PORT** as per your requirements.

When the server and clients are running, you can see operation output on terminal window. Also, you can check the log of average time of transactions executions taken by each client in the directory *[src/logs/](https://github.com/BhushanMahajan25/Centralized-Multi-User-Concurrent-Bank-Account-Manager/tree/main/src/logs)*
