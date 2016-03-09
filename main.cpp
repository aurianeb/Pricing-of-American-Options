#include <Imagine/Graphics.h>
#include <iostream>
#include "CR.h"

//using namespace std;

int main()
{
    //Tree(100,120,5,0.5,0.6,-0.3,0.1,1);
    //histo(100,120,0.5,100,50,0,1);
    //Pricer(valeur initiale, strike, sigma, probabilité de a, nb de période)
    //Pricer_it(100, 120, 3, 0.5, 5, 0.1, 1);
    //Pricer_rec(100, 120, 3, 0.5, 5, 0.1);
    //Pricer_MC_naif(100, 120, 3, 0.5, 5, 1, 20, 0.1);
    Pricer_MC(100, 120, 4, 0.5, 25, 20, 0.1, 2);
    //arbre(100,120,0.3,-0.2, 0.5, 20, 0.1,1);


    /*noeud *n;
    n = new noeud;
    n->cree_fils();
    //n->creer_generation();
    //n->fb()->set_valeur(3);
    n->creer_arbre(5);
    display(n,5);
*/
    return 0;
}



