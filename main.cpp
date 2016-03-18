#include <Imagine/Graphics.h>
#include <iostream>
#include "CR.h"

//using namespace std;

int main()
{
    //Arbre des prix dans le modèle de Cox Ross
    //cout<<"Le prix de l'option donné par le modèle de Cox Ross est de "<<prix(100,0.3,10,0.05)<<"€"<<endl;

    //Calcul itératif de u[0] du théorème
    //Pricer_it(100, 120, 0.3, 15, 0.05, 1);

    //Calcul du prix du Call européen grace à la formule de Black_Scholes
    //BS(100, 100, 0.05, 0.3);




    //On applique la méthode de Monte Carlo

    //On s'arrte dès que le PayOff est supérieur au strike
    //Pricer_MC_naif(100, 100, 0.3, 100, 1, 5000, 0.05,1);


    //Condition d'arret du théorème
    //Pricer_MC(100, 100, 0.3, 100, 5000, 0.05, 1);


    //Résultats de de la méthode itérative

    //Histogramme des temps d'arret
    //histo(100,100,0.3,20,100,0.05,1);

    //Simulation de trajectoire
    //Pricer_MC_display(100, 100, 0.3, 10, 0.05, 2);


    //Comparaison des stratégies naives et itératives
    //MC_naive(100,100,0.3,20,100,0.05,100,2);



    //Évaluation de la méthode itérative

    // Comparaison du prix du Call méthode itérative / Formule de Black-Scholes
    //MC_BS_Call(100,100,0.3,200,100000,0.05,1);

    //Comparaison de u[0] et du prix donné par la formule de Black-Scholes
    //U_BS(100,100,0.3,500,0.05,1);


    return 0;
}
