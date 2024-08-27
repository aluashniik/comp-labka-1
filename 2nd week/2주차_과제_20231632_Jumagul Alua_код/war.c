#include <stdio.h>
#include "war.h" //header file

long long res[10]={0};//to store the results

void calc(int n, int coef){ //function counts of the each digit in n 
    while (n>0){
        res[n%10] += coef; //stores the count of the digit in the res array 
        n/=10;//move to the next digit
    }
}

void sol (int A, int B, int coef){
    while (A%10 && A<=B){ //until A is multiply by 10, and B is greater than A
        calc(A, coef); //calculates the result 
        A++;//move to the next, so ones digit should be 0  
    }

    if(A>B) return;//if A is greater than B, exit
 
    while (B%10 != 9 && A<=B){ //B's ones digit should be 9
       calc(B, coef);//calculates the result
       B--;//go down, checking to the previous number 
    }

    long long tmp = (B/10 - A/10 +1); //calculates multiples of 10 from A to B

    for (int i=0; i<10; i++){
        res[i] += tmp * coef;//stores the counts of digits to res array
    }

    sol(A/10, B/10, coef * 10);//to process the next range of numbers
}

void print_res(){
    for (int i=0; i<10; i++){
        printf("%lld ", res[i]);//prints the count of each digit
    }
}

