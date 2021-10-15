#!/bin/bash
for i in {1..3}
    do
        echo "spawning a client ${i}"
        ./client 10.0.2.15 8080 input-files/Transactions.txt &
        sleep 0.3s
    done


#https://devhints.io/bash

# for i in {1..4}
#     do  
#         echo "spawning a client $i"
#         sleep 0.3s
#     done