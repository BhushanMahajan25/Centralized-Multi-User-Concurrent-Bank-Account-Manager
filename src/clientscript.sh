for i in {1..3}; do
    echo "spawning a client $i"
    ./client 10.0.0.237 8080 input-files/Transactions.txt &
    sleep 0.5s
done


#https://devhints.io/bash