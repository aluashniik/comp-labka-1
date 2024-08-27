#include <stdio.h>
#include "war.h"//header file

int main(){
    long long N;//'long long' to hold large numbers
    scanf("%lld", &N);//gets user input 

    sol(1, N, 1);//calls function to calculate the results
    
    print_res();//calls function to print the results

    return 0;
}

