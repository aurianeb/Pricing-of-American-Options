#ifndef CR_H
#define CR_H
#endif // CR_H
#include <time.h>
#include <Imagine/Graphics.h>
#include <cmath>
using namespace Imagine;
using namespace std;
#include <cmath>
#include "Binom.h"

float standard_dev(float* vec, int length)
{
    float mean=0;
    float dev=0.0;
    for(int i = 0; i<length; i++){
        mean+=vec[i];
    }
    mean=mean/length;
    for(int i = 0; i<length; i++){
        dev +=(vec[i]-mean)*(vec[i]-mean);
    }
    return sqrt(dev/float(length));
}





//Fonctions auxilliaires
float max(float a, float b){
    if(a>b){return a;}
    else{return b;}
}

float puiss(float a, int n){
    float tmp = 1;
    for(int k = 0; k<n; k++){
        tmp = tmp*a;
    }
    return tmp;
}

float CallPayOff(float S, float K){
    return max(S-K,0);
}

float PutPayOff(float S, float K){
    return max(K-S,0);
}


float Evolution(float S_0, float a, float b, int i, int n, int N, float r){
   // assert(S_0>=0);
    return S_0*(puiss(1+a, i)*puiss(1+b,n-i))*puiss(1+r/N,n);
}




//Arbre des u(n,x)



//Méthode récursive
//Complexité exponentielle (inexploitable en pratique)
float Pricer_rec_Call(float S_0, float K, float a, float b, float p, int N, int m, float r){
    if(m==N){CallPayOff(S_0,K);}
    else{p*Pricer_rec_Call(S_0*(1+a),K,a,b,p,N,m+1,r)+(1-p)*Pricer_rec_Call(S_0*(1+b),K,a,b,p,N,m+1,r);}
}

float Pricer_rec_Put(float S_0, float K, float a, float b, float p, int N, int m, float r){
    if(m==N){PutPayOff(S_0,K);}
    else{p*Pricer_rec_Put(S_0*(1+a),K,a,b,p,N,m+1,r)+(1-p)*Pricer_rec_Put(S_0*(1+b),K,a,b,p,N,m+1,r);}
}

float Pricer_rec(float S_0, float K, float sigma, float p, int N, float r){
    int choix;
    cout<<"Quel est le type de votre option ?"<<endl;
    cout<<"1 - Call"<<endl;
    cout<<"2 - Put"<<endl;
    cin>>choix;
    float u;
    float a = sigma/sqrt(N);
    float b = -a;

    if(choix==1){u = Pricer_rec_Call(S_0,K,a,b,p,N,0,r);}
    else if(choix==2){u = Pricer_rec_Put(S_0,K,a,b,p,N,0,r);}
    cout<<"Le prix de l'option est de "<<u<<"€"<<endl;
    return u;

}



//Méthode itérative
//Complexité linéaire



//Le prix de l'option est u(0, S_0) (théorème)
int Pricer_it(float S_0, float K, float sigma, float p, int N, float r, int choix){
    /*int choix;
    cout<<"Quel est le type de votre option ?"<<endl;
    cout<<"1 - Call"<<endl;
    cout<<"2 - Put"<<endl;
    cin>>choix;
    */

    float a = sigma/sqrt(N);
    float b = -a;
    float* u;
    u = new float[N+1];


    //U est enfait ici u(N,.) du théorème
    if(choix == 1){
        for(int i = 0; i<=N; i++){
            u[i] = CallPayOff(Evolution(S_0, a, b, i, N, N, r), K);
        }
    }
    else if(choix == 2){
        for(int i = 0; i<=N; i++){
            u[i] = PutPayOff(Evolution(S_0, a, b, i, N, N, r), K);
        }
    }

    //On en profite pour calculer le temps d'arret optimal
    int ta = N;

    //On remonte les périodes j
    //À chaque période j, on calule toutes les valeurs de u possibles
    for(int j=N-1; j>=0; j--){
        for(int i = 0; i<=j; i++){
            //sum(Pu(j+1,.)(x)) dans le théorème
            u[i] = u[i+1]*(1-p)+u[i]*p;
            //Est ce que je vends/j'achète maintenant ?
            if(choix == 1 && Evolution(S_0, a, b, i, j, N, r) - K > u[i]/(puiss(1+r/N,i))){
                u[i] = Evolution(S_0, a, b, i, j, N,r) - K;
                ta = j;
            }
            else if(choix == 2 && K - Evolution(S_0, a, b, i, j, N, r) > u[i]/(puiss(1+r/N,i))){
                u[i] = K - Evolution(S_0, a, b, i, j, N,r);
                ta = j;
            }
        }
    }
    cout<<"Le prix de l'option est de "<<u[0]<<"€"<<endl;
    cout<<"Le temps d'arret optimal est "<<ta<<endl;
    return ta;

}


//Trace le graphe des valeurs du temps d'arret pour un certain nombre de valeurs de sigma tirées aléatoirement
void histo(float S_0, float K, float p, float sigma, int N, int M, float r, int choix){
    Window W = openWindow(500,500);
    drawArrow(0,490,490,490,BLACK);
    drawArrow(10,500,10,10,BLACK);
    for(int i = 0; i<M; i++){
        int ta = Pricer_it(S_0, K, sigma, p, N, r, choix);
        cout<<50*sigma<<"   "<<ta<<endl;
        drawCircle(10+50*sigma,500-ta-10,3,RED);
    }
    click();
}




//Deux méthodes de Monte Carlo : naive et s'appuyant sur le théorème



//Calcule u(n,x) du théorème

float u(noeud* bt, float S_0, float K, float p, float a, float b, float r, int i, int j,int N, int choix){
    float u_next = (p*bt->fh()->get_valeur()+(1-p)*bt->fb()->get_valeur())/(1+r);
    if(choix==1){return max(CallPayOff(Evolution(S_0,a,b,i,j,N,r),K),u_next);}
    else{return max(PutPayOff(Evolution(S_0,a,b,i,j,N,r),K),u_next);}
}

noeud* arbre (float S_0, float K, float a, float b, float p, int N, float r, int choix){
    int i ;
    noeud *arbre, *pt, *at, *bt;
    arbre = new noeud;
    //arbre = cree_fils_haut();
    pt = arbre;

    for (int i = 0 ; i < N ; i++)
    {
        arbre->creer_generation(pt);
        pt = pt->fh();
    };

    int j = N;
    i = N;

    //Maintenant on peut remplir l'arbre
    for(at = pt; at != NULL; at = at->pb()){
        i = j;
        for(bt = at; bt != NULL; bt = bt->frere_bas()){
            if(bt->fh()==NULL){
                if(choix==1){
                    bt->set_valeur(CallPayOff(Evolution(S_0, a, b, i, N, N, r),K));
                    i--;
                }
                else{
                    bt->set_valeur(CallPayOff(Evolution(S_0, a, b, i, N, N, r),K));
                    i--;}
            }
            else{
                bt->set_valeur(u(bt,S_0,K,p,a,b,r,i,j,N,choix));
                i--;
            }
        }
        j--;
    }

    return (arbre);
}


//Retourne le prix de l'option pour une certaine trajectoire
float Trajectoire_naive(float S_0, float K, float a, float b, float p, int N, float seuil, int choix, float r){
    float val = S_0;
    int i = 0;
    //On vend dès que l'on put faire un bénéfice (ou à la fin de la période si la situation n'a jamais été favorable)
    if(choix==1){
        while(i<N && CallPayOff(val, K)>seuil){
            i++;
            int k = (float)rand()/(float)(RAND_MAX);
            if(k<p){ val *= (1+a)/(1+r);}
            else{ val *= (1+b)/(1+r);}
        }
        return CallPayOff(val,K);
    }
    if(choix==2){
        while(i<N && PutPayOff(val, K)>seuil){
            i++;
            int k = (float)rand()/(float)(RAND_MAX);
            if(k<p){ val *= (1+a)/(1+r);}
            else{ val *= (1+b)/(1+r);}
        }
        return PutPayOff(val,K);
    }

}

retour Trajectoire(float S_0, float K, float a, float b, float p, int N, float r, int choix, noeud* v){
    float val = S_0;
    int j = 0;
    if(choix==1){
        while(j<N && CallPayOff(val,K)!=v->get_valeur()){
            j++;
            int f = (float)rand()/(float)(RAND_MAX);
            if(f<p){ val *= (1+a)/(1+r);}
            else{ val *= (1+b)/(1+r);}
        }
    }
    if(choix==2){
        while(j<N && PutPayOff(val,K)!=v->get_valeur()){
            j++;
            int f = (float)rand()/(float)(RAND_MAX);
            if(f<p){ val *= (1+a)/(1+r);}
            else{ val *= (1+b)/(1+r);}
        }
    }

    retour ret;
    ret.prix = CallPayOff(val,K);
    ret.ta = j;

    return ret;

}


float Pricer_MC_naif(float S_0, float K, float sigma, float p, int N, float seuil, int M, float r){
    int choix;
    cout<<"Quel est le type de votre option ?"<<endl;
    cout<<"1 - Call"<<endl;
    cout<<"2 - Put"<<endl;
    cin>>choix;

    float a = sigma/sqrt(N);
    float b = -a;

    float u;
    float* vec;
    vec = new float[M];

    //u stocke les valeurs de chaque trajectoire
    for(int i = 1; i<=M; i++){
        cout<<i<<endl;
        u += Trajectoire_naive(S_0, K, a, b, p, N, seuil, choix, r);
        vec[i] = u;
    }
    u = u/M;
    cout<<"Le prix de l'option calculé par la méthode de Monte Carlo est de "<<u<<"€"<<endl;
    float sd = standard_dev(vec,M);
    cout<<"Un intervalle de confiance : ["<<u-1.96*sd<<" , "<<u+1.96*sd<<"]"<<endl;
    return u;

}

void Pricer_MC(float S_0, float K, float sigma, float p, int N, int M, float r, int choix){
    /*int choix;
    cout<<"Quel est le type de votre option ?"<<endl;
    cout<<"1 - Call"<<endl;
    cout<<"2 - Put"<<endl;
    cin>>choix;*/

    float a = sigma/sqrt(N);
    float b = -a;
    retour ret;
    float prix = 0;
    float* vec;
    vec = new float[M];

    Window W = openWindow(500,500);
    drawArrow(0,490,490,490,BLACK);
    drawArrow(10,500,10,10,BLACK);

    noeud* v = arbre(S_0,K,a,b,p,N,r,choix);

    //vec stocke les valeurs de chaque trajectoire
    for(int i = 1; i<=M; i++){
        ret = Trajectoire(S_0, K, a, b, p, N, r, choix, v);
        prix += ret.prix;
        vec[i] = ret.prix;
        //On affiche les temps d'arret
        cout<<ret.ta<<endl;
        drawCircle(10+50*i,500-10*ret.ta-10,3,RED);
    }
    click();

    cout<<"Le prix de l'option calculé par la méthode de Monte Carlo est de "<<u<<"€"<<endl;
    float sd = standard_dev(vec,M);
    cout<<"Un intervalle de confiance : ["<<prix-1.96*sd<<" , "<<prix+1.96*sd<<"]"<<endl;

}



// Approximation de la loi normale de Waissi and Rossin
float normale(float z) {
    float b1 = -0.0004406;
    float b2 =  0.0418198;
    float b3 =  0.9;
    return 1.0 / (1.0 + exp(-sqrt(M_PI)*(b1*puiss(z,5.0) + b2*puiss(z,3.0) + b3*z)));
}

// Black Scholes
float BS(float S_0,float K,float N,float r,float sigma,int choix) {
    float d1 = (log(S_0/K) + (r+0.5*puiss(sigma,2))*N)/(sigma*sqrt(N));
    float d2 = (log(S_0/K) + (r-0.5*puiss(sigma,2))*N)/(sigma*sqrt(N));
    if (choix == 1)
        return S_0*normale(d1) - K*exp(-r*N)*normale(d2);
    else
        return K*exp(-r*N)*normale(-d2) - S_0*normale(d1);
}
