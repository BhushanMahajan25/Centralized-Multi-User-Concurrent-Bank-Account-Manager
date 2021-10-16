# Implemetation

## Server:
a.	*POSIX threads* are created to handle multiple clients.

b.	Number of threads are defined as macro. Also, a pool of threads is created in advanced so that when a client connects with the server, a thread can be assigned to it.

c.	Now, since threads are created in advanced, each thread will check if a client connection is established or not. This will result in wastage of CPU cycles. To overcome this problem, condition variables are used.

d.	When a client is connected to the server, its file descriptor is pushed into `STL queue`. At the same time threads are checking if the connection has been established or not. The problem of race condition arises when the queue is empty or multiple threads try to access the queue. Here, condition variable tells thread to wait until there is a connection with a client. As soon as a connection is established, the queue is locked using mutex and connected client’s socket descriptor is pushed into the queue. Condition variable signals the threads and mutex lock is revoked. This is how, a thread is assigned to newly connected client synchronously.

e.	When multiple clients send transaction requests, concurrently, to the server, i.e multiple threads want to access the function ```updateRecords()``` at the same time. Here, the function gets locked by mutex so that each thread will access the function one at a time. After using the function, mutex lock is revoked.
f.	The same case happens with the function ```addInterest()```.
g.	Here, vector of mutexes is created to acquire lock on a particular account number.

## Tradeoff
For the sake of simplicity, timestamps from Transactions.txt are not used. Instead, all the transactions are stored into vector of Transaction class and sent to the server by iterating the vector using for loop.

## Extension to this project
Parallelism can be added where number of threads are assigned to a single client and perform transactions in batches (# of threads).
