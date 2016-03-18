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
    return S_0*(puiss(1+a, i)*puiss(1+b,n-i));
}


// Approximation de la loi normale de Waissi and Rossin
float normale(float z) {
    float b1 = -0.0004406;
    float b2 =  0.0418198;
    float b3 =  0.9;
    return 1.0 / (1.0 + exp(-sqrt(M_PI)*(b1*puiss(z,5.0) + b2*puiss(z,3.0) + b3*z)));
}

// Black Scholes
float BS(float S_0,float K,float r,float sigma) {
    float d1 = (log(S_0/K) + (r+0.5*puiss(sigma,2)))/sigma;
    float d2 = (log(S_0/K) + (r-0.5*puiss(sigma,2)))/sigma;
    float call = S_0*normale(d1) - K*exp(-r)*normale(d2);
    cout<<"Le prix du Call européen calculé par la formule de Cox-Ross est de : "<<call<<"€"<<endl;
    return call;
}



//Méthode récursive
//Complexité exponentielle (inexploitable en pratique)
float Pricer_rec_Call(float S_0, float K, float a, float b, float p, int N, int m, float r){
    if(m==N){return CallPayOff(S_0,K);}
    else{return p*Pricer_rec_Call(S_0*(1+a),K,a,b,p,N,m+1,r)+(1-p)*Pricer_rec_Call(S_0*(1+b),K,a,b,p,N,m+1,r);}
}

float Pricer_rec_Put(float S_0, float K, float a, float b, float p, int N, int m, float r){
    if(m==N){return PutPayOff(S_0,K);}
    else{return p*Pricer_rec_Put(S_0*(1+a),K,a,b,p,N,m+1,r)+(1-p)*Pricer_rec_Put(S_0*(1+b),K,a,b,p,N,m+1,r);}
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
float Pricer_it(float S_0, float K, float sigma, int N, float r, int choix){

    float a = sigma/sqrt(N) +r/N;
    float b = -sigma/sqrt(N) +r/N;
    float* u;
    u = new float[N+1];
    float p = (b-r/N)/(b-a);

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


    //On remonte les périodes j
    //À chaque période j, on calule toutes les valeurs de u possibles
    for(int j=N-1; j>=0; j--){
        for(int i = 0; i<=j; i++){
            //sum(Pu(j+1,.)(x)) dans le théorème
            u[i] = (u[i+1]*(1-p)+u[i]*p)/(1+r/N);
            //Est ce que je vends/j'achète maintenant ?
            if(choix == 1 && Evolution(S_0, a, b, i, j, N, r) - K > u[i]/(1+r/N)){
                u[i] = Evolution(S_0, a, b, i, j, N,r) - K;
            }
            else if(choix == 2 && K - Evolution(S_0, a, b, i, j, N, r) > u[i]/(1+r/N)){
                u[i] = K - Evolution(S_0, a, b, i, j, N,r);
            }
        }
    }

    return u[0];

}






//Deux méthodes de Monte Carlo : naive et s'appuyant sur le théorème



//Calcule u(n,x) du théorème

couple u(noeud* bt, float S_0, float K, float p, float a, float b, float r, int i, int j,int N, int choix){
    float u_next = (p*(bt->fh()->get_valeur()).prix+(1-p)*(bt->fb()->get_valeur()).prix)/(1+r/N);

    couple cp;

    if(choix==1){
        //cout<<(max(CallPayOff(Evolution(S_0,a,b,i,j,N,r),K),u_next) == u_next) << "     "<<p<<endl;
        cp.prix = max(CallPayOff(Evolution(S_0,a,b,i,j,N,r),K),u_next);
        cp.ta = (CallPayOff(Evolution(S_0,a,b,i,j,N,r),K) < u_next);
        return cp;
    }
    else{
        //cout<<(max(PutPayOff(Evolution(S_0,a,b,i,j,N,r),K),u_next) == u_next) << "     "<<p<<endl;
        cp.prix = max(PutPayOff(Evolution(S_0,a,b,i,j,N,r),K),u_next);
        cp.ta = (PutPayOff(Evolution(S_0,a,b,i,j,N,r),K) < u_next);
        return cp;
    }
}

noeud* arbre (float S_0, float K, float a, float b, float p, int N, float r, int choix){
    int i ;
    noeud *arbre, *pt, *at, *bt;
    arbre = new noeud;
    //arbre = cree_fils_haut();
    pt = arbre;

    //On créé l'arbre
    for (int i = 0 ; i < N ; i++)
    {
        arbre->creer_generation(pt);
        pt = pt->fh();
    };

    int j = N;
    couple cp;

    //Maintenant on peut remplir l'arbre
    for(at = pt; at != NULL; at = at->pb()){
        i = j;
        for(bt = at; bt != NULL; bt = bt->frere_bas()){
            if(bt->fh()==NULL){
                if(choix==1){
                    cp.prix = CallPayOff(Evolution(S_0, a, b, i, N, N, r),K);
                    cp.ta = 0;
                    bt->set_valeur(cp);
                    i--;
                }
                else{
                    cp.prix = PutPayOff(Evolution(S_0, a, b, i, N, N, r),K);
                    cp.ta = 0;
                    bt->set_valeur(cp);
                    i--;
                }
            }
            else{
                bt->set_valeur(u(bt,S_0,K,p,a,b,r,i,j,N,choix));
                i--;
            }
        }
        j--;
    }
    //display_traj(arbre,N,S_0,K,choix);
    //display(arbre,N);
    return (arbre);
}


//Retourne le prix de l'option pour une certaine trajectoire

couple Trajectoire_naive(float S_0, float K, float a, float b, float p, int N, float seuil, int choix){
    float val = S_0;
    int j = 0;

    if(choix==1){
        while(j<N &&  CallPayOff(val,K)<seuil){
            j++;
            float f = (float)rand()/(float)(RAND_MAX);
            if(f<p){
                val *= (1+a);
            }
            else{
                val *= (1+b);
            }
        }
    }
    if(choix==2){
        while(j<N && PutPayOff(val,K)<seuil){
            j++;
            float f = (float)rand()/(float)(RAND_MAX);
            if(f<p){
                val *= (1+a);
            }
            else{
                val *= (1+b);
            }
        }
    }
    couple ret;
    if(choix==1){ret.prix = CallPayOff(val,K);}
    else{ret.prix = PutPayOff(val,K);}
    ret.ta = j;
    return ret;
}


float Pricer_MC_naif(float S_0, float K, float sigma, int N, float seuil, int M, float r, int choix){

    float a = sigma/sqrt(N)+r/N;
    float b = -sigma/sqrt(N)+r/N;
    couple ret;
    float prix = 0;
    float* vec;
    vec = new float[M];

    // Prob risque neutre
    float p = (b - r/N)/(b - a);

    //Window W = openWindow(500,500);
    //drawArrow(0,490,490,490,BLACK);
    //drawArrow(10,500,10,10,BLACK);

    //vec stocke les valeurs de chaque trajectoire
    for(int i = 0; i<M; i++){
        ret = Trajectoire_naive(S_0, K, a, b, p, N, seuil, choix);
        prix += ret.prix*puiss(1/(1+r/N),ret.ta);
        vec[i] = ret.prix*puiss(1/(1+r/N),ret.ta);
        //On affiche les temps d'arret
        //float prix_BS = BS(S_0,K,r,sigma,choix);
        //drawCircle(10+i,500-10*ret.ta-10,1,RED);
    }
    prix = prix/M;
    //click();


    cout<<"Le prix de l'option calculé par la méthode de Monte Carlo est de "<<prix<<"€"<<endl;
    float sd = standard_dev(vec,M)/sqrt(M);
    cout<<"Un intervalle de confiance : ["<<prix-1.96*sd<<" ; "<<prix+1.96*sd<<"]"<<endl;
    return prix;

}



couple Trajectoire(float S_0, float K, float a, float b, float p, int N, int choix, noeud* v){
    float val = S_0;
    int j = 0;


    if(choix==1){
        while(j<N && (CallPayOff(val,K)!=(v->get_valeur()).prix || CallPayOff(val,K) == 0)){
            j++;
            float f = (float)rand()/(float)(RAND_MAX);
            if(f<p){
                val *= (1+a);
                v = v->fh();
            }
            else{
                val *= (1+b);
                v = v->fb();
            }
        }
    }
    if(choix==2){
        while(j<N && (PutPayOff(val,K)!=(v->get_valeur()).prix || PutPayOff(val,K) == 0)){
            j++;
            float f = (float)rand()/(float)(RAND_MAX);
            if(f<p){
                val *= (1+a);
                v = v->fh();
            }
            else{
                val *= (1+b);
                v = v->fb();
            }
        }
    }
    couple ret;
    if(choix==1){ret.prix = CallPayOff(val,K);}
    else{ret.prix = PutPayOff(val,K);}
    ret.ta = j;
    return ret;
}




float Pricer_MC(float S_0, float K, float sigma, int N, int M, float r, int choix){
    /*int choix;
    cout<<"Quel est le type de votre option ?"<<endl;
    cout<<"1 - Call"<<endl;
    cout<<"2 - Put"<<endl;
    cin>>choix;*/


    float a = sigma/sqrt(N)+r/N;
    float b = -sigma/sqrt(N)+r/N;
    couple ret;
    float prix = 0;
    float* vec;
    vec = new float[M];

    // Prob risque neutre
    float p = (b - r/N)/(b - a);

    noeud* v = arbre(S_0,K,a,b,p,N,r,choix);

    //Window W = openWindow(500,500);
    //drawArrow(0,490,490,490,BLACK);
    //drawArrow(10,500,10,10,BLACK);
    float prix_BS = BS(S_0,K,r,sigma);

    //vec stocke les valeurs de chaque trajectoire
    for(int i = 0; i<M; i++){
        ret = Trajectoire(S_0, K, a, b, p, N, choix, v);
        prix += ret.prix*puiss(1/(1+r/N),ret.ta);
        vec[i] = ret.prix*puiss(1/(1+r/N),ret.ta);
        //On affiche les temps d'arret

        //drawCircle(10+i,500-10*ret.ta-10,1,RED);
    }
    prix = prix/M;
    //click();

    cout<<(v->get_valeur()).prix<<"   "<<prix_BS<<endl;
    cout<<"Le prix de l'option calculé par la méthode de Monte Carlo est de "<<prix<<"€"<<endl;
    float sd = standard_dev(vec,M)/sqrt(M);
    cout<<"Un intervalle de confiance : ["<<prix-1.96*sd<<" ; "<<prix+1.96*sd<<"]"<<endl;
    return (v->get_valeur()).prix;

}


void MC_BS_Call(float S_0, float K, float sigma, int N, int M, float r, int choix){
    float a = sigma/sqrt(N)+r/N;
    float b = -sigma/sqrt(N)+r/N;
    couple ret;
    float prix = 0;
    float* vec;
    vec = new float[M];

    // Prob risque neutre
    float p = (b - r/N)/(b - a);

    noeud* v = arbre(S_0,K,a,b,p,N,r,choix);

    float prix_BS = BS(S_0,K,r,sigma);
    cout<<prix_BS<<endl;
    Window W = openWindow(M/100+20,500);
    drawArrow(0,490,M/100+10,490,BLACK);
    drawArrow(10,500,10,10,BLACK);
    drawLine(10,500-30*prix_BS-10, 10*M+10, 500-30*prix_BS-10,BLUE);
    for(int i = 1; i<=N; i++){
        drawLine(10+100*i,500-5,10+100*i,500-15,BLACK);
        drawString(10+100*i, 500, to_string(10000*i),BLACK);
    }

    for(int i = 0; i<M; i++){
        ret = Trajectoire(S_0, K, a, b, p, N, choix, v);
        prix += ret.prix*puiss(1/(1+r/N),ret.ta);
        drawCircle(i/100+10,500-30*prix/i-10,1,RED);
    }
    click();
}



//Trace le graphe des valeurs du temps d'arret pour un certain nombre de valeurs de sigma tirées aléatoirement
void histo(float S_0, float K, float sigma, int N, int M, float r, int choix){

    int* ta;
    ta = new int[N+1];
    for(int i = 0; i<N+1; i++){
        ta[i]=0;
    }

    float a = sigma/sqrt(N)+r/N;
    float b = -sigma/sqrt(N)+r/N;
    // Prob risque neutre
    float p = (b - r/N)/(b - a);
    noeud* v = arbre(S_0,K,a,b,p,N,r,choix);

    int width = 20*N;
    int height = M+10;
    //On construit le repere
    Window W = openWindow(width+70,height+30);
    drawArrow(0,height,width + 60,height,BLACK);
    drawArrow(30,height,30,5,BLACK);
    for(int i = 1; i<=N; i++){
        drawLine(30+20*i,height,30+20*i,height+10,BLACK);
        drawString(30+20*(i-1), height+20, to_string(i),BLACK);
        drawLine(20,height-50*i,30,height-50*i,BLACK);
        drawString(0, height-50*i, to_string(50*i),BLACK);
    }

    //On rempli le vecteur des fréquences des temps d'arret


    for(int i = 0; i<M; i++){
        couple ret = Trajectoire(S_0, K, a, b, p, N, choix, v);
        ta[ret.ta]++;
    }

    for(int i = 0; i<=N; i++){
        drawRect(10+20*i,height-ta[i],10,ta[i],RED);
    }

    click();
}

void U_BS(float S_0, float K, float sigma, int N, float r, int choix){
    float a = sigma/sqrt(N)+r/N;
    float b = -sigma/sqrt(N)+r/N;
    // Prob risque neutre
    float p = (b - r/N)/(b - a);

    float prix_BS = BS(S_0,K,r,sigma);

    cout<<prix_BS<<endl;
    noeud* v = arbre(S_0,K,a,b,p,N/10,r,choix);

    int e = 50;
    int width = N+20;
    int height = e*int(prix_BS)+20;

    Window W = openWindow(width,height);
    drawArrow(0,height-20,width-10,height-20,BLACK);
    drawArrow(20,height,20,10,BLACK);
    drawLine(20,height - e*prix_BS-20+e*prix_BS/2, width, height - e*prix_BS-20+e*prix_BS/2,BLUE);


    for(int i = 0; i<=N; i++){
        drawLine(20+100*i,height-10,20+100*i,height-20,BLACK);
        drawString(20+100*i, height, to_string(100*i),BLACK);
        drawLine(10,height-50*i,20,height-50*i,BLACK);
        drawString(10, height-50*i, to_string(i+int(prix_BS/2)+1),BLACK);
    }

    for(int i = 1; i<N; i++){
        //cout<<prix_BS<<"    "<<u<<endl;
        float u = Pricer_it(S_0,K,sigma,i,r,choix);
        drawCircle(i+20,height-e*u-20+e*prix_BS/2,1,RED);
    }
    click();
}




couple Trajectoire_display(float S_0, float K, float a, float b, float p, int N, float r, int choix){
    float val = S_0;
    int j = 0;
    int y0 = 25*(N+1);

    noeud* v = arbre(S_0,K,a,b,p,N,r,choix);
    display(v,N);

    if(choix==1){
        while(j<N && CallPayOff(val,K)!=(v->get_valeur()).prix){
            j++;
            float f = (float)rand()/(float)(RAND_MAX);
            if(f<p){
                val *= (1+a);
                v = v->fh();
                drawLine(50*j,y0,50*(j+1),y0-25,GREEN);
                y0 = y0-25;
            }
            else{
                val *= (1+b);
                v = v->fb();
                drawLine(50*j,y0,50*(j+1),y0+25,GREEN);
                y0 = y0+25;
            }
        }
    }
    if(choix==2){
        while(j<N && PutPayOff(val,K)!=(v->get_valeur()).prix){
            j++;
            float f = (float)rand()/(float)(RAND_MAX);
            if(f<p){
                val *= (1+a);
                v = v->fh();
                drawLine(50*j,y0,50*(j+1),y0-25,GREEN);
                y0 = y0-25;
            }
            else{
                val *= (1+b);
                v = v->fb();
                drawLine(50*j,y0,50*(j+1),y0+25,GREEN);
                y0 = y0+25;
            }
        }
    }

    drawCircle(50*(j+1),y0,10,RED);
    couple ret;
    if(choix==1){ret.prix = CallPayOff(val,K);}
    else{ret.prix = PutPayOff(val,K);}
    ret.ta = j;
    click();
    return ret;
}




float Pricer_MC_display(float S_0, float K, float sigma, int N, float r, int choix){
    /*int choix;
    cout<<"Quel est le type de votre option ?"<<endl;
    cout<<"1 - Call"<<endl;
    cout<<"2 - Put"<<endl;
    cin>>choix;*/


    float a = sigma/sqrt(N)+r/N;
    float b = -sigma/sqrt(N)+r/N;
    couple ret;
    float prix = 0;

    // Prob risque neutre
    float p = (b - r/N)/(b - a);

    ret = Trajectoire_display(S_0, K, a, b, p, N, r, choix);


    cout<<"Le prix de l'option calculé par la méthode de Monte Carlo est de "<<ret.prix<<"€"<<endl;

}


void prix(float S_0, float sigma, int N, float r){
    noeud *arbre, *pt, *at, *bt;
    arbre = new noeud;
    //arbre = cree_fils_haut();
    pt = arbre;
    float a = sigma/sqrt(N)+r/N;
    float b = -sigma/sqrt(N)+r/N;

    int i, j;
    j = 0;
    //On créé l'arbre
    for (i = 0 ; i < N ; i++)
    {
        for(at = pt; at != NULL; at = at->frere_bas()){
            couple cp;
            cp.prix = Evolution(S_0,a,b,i-j,i,N,r);
            cp.ta = 1;
            at->set_valeur(cp);
            j++;
        }
        arbre->creer_generation(pt);
        pt = pt->fh();
        j = 0;
    };
    for(at = pt; at != NULL; at = at->frere_bas()){
        couple cp;
        cp.prix = Evolution(S_0,a,b,i-j,i,N,r);
        cp.ta = 1;
        at->set_valeur(cp);
        j++;
    }

    display(arbre,N);
}




//Comparaison avec méthode naive

void MC_naive(float S_0, float K, float sigma, int N, int M, float r, float seuil, int choix){
    int* ta;
    ta = new int[N+1];
    for(int i = 0; i<N+1; i++){
        ta[i]=0;
    }

    float a = sigma/sqrt(N)+r/N;
    float b = -sigma/sqrt(N)+r/N;
    // Prob risque neutre
    float p = (b - r/N)/(b - a);
    noeud* v = arbre(S_0,K,a,b,p,N,r,choix);

    int width = 10*seuil+20;
    int height = 2*S_0+10;
    //On construit le repere
    Window W = openWindow(width,height);
    drawArrow(0,height-20,width -10,height-20,BLACK);
    drawArrow(20,height,20,5,BLACK);

    float ret = Pricer_MC(S_0,K,sigma,N,M,r,choix);
    drawLine(20,height-10*ret-20,width,height-10*ret-20, RED);

    for(int i = 1; i<=seuil; i++){
        drawLine(20+100*i,height-20,20+100*i,height-10,BLACK);
        drawString(20+100*i, height, to_string(10*i),BLACK);
        drawLine(20,height-100*i,10,height-100*i,BLACK);
        drawString(0, height-100*i, to_string(10*i),BLACK);
    }


    for(int i = 0; i<seuil; i++){
        float ret2 = Pricer_MC_naif(S_0, K, sigma, N, i, M, r, choix);
        drawCircle(10*i+20,height-10*ret2-20, 1, BLUE);
    }

    click();
}
