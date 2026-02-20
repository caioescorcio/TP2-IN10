#include "age.h"
#include <stdio.h>

static const float Price = 1.5;
float computePrice(int age){
    int limitAge;
    int *plimit = &limitAge;
    recupAge(plimit);
    if (age>limitAge){
        return Price;
    }
    else {
        return Price/2;
    }
}
