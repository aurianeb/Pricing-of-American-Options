#include <iostream>
#include "CR.h"

using namespace std;

int main()
{
    //Pricer(valeur initiale, strike, a>0, b<0, probabilité de a, nb de période)
    Pricer(100, 120, 3, -4, 0.5, 5);
    return 0;
}

