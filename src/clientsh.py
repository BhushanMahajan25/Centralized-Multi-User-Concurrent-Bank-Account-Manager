import os
import time

for i in range(1,4):
    print("spawning client {}\n".format(i))
    os.system("./client 10.0.2.15 8080 input-files/Transactions.txt &")
    time.sleep(0.3)
