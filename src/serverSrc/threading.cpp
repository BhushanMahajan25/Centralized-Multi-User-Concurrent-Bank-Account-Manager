#include "../headers/common.hpp"
#include "../headers/threading.hpp"

void* myTurn(void* arg){
    int* ptr = (int*) arg;
    *ptr = 0;
    for(int i = 0; i < 8; i++){
        sleep(1);
        cout<<"myTurn() :: "<<i<<endl;
        (*ptr)++;
    }
    return ptr;
}

void yourTurn(){
    for(int i = 0; i < 5; i++){
        sleep(1);
        cout<<"yourTurn() :: "<<i<<endl;
    }
}

int main(){
    pthread_t newThread;
    int* retVal;
    int i = 0;
    pthread_create(&newThread, NULL, myTurn, &i);

    yourTurn();

    pthread_join(newThread, (void *_Nullable *) &retVal); //(void *_Nullable *)
    cout<<"thread is done:: "<<*retVal<<endl;

    return 0;
}