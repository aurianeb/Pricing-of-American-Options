#ifndef CR_H
#define CR_H
using namespace std;

#endif // CR_H

float puiss(float a, int n){
    float tmp = 1;
    for(int k = 0; k<n; k++){
        tmp = tmp*a;
    }
    return tmp;
}

float CallPayOff(float S, float K){
    if(S>K){return S-K;}
    else{return 0;}
}

float PutPayOff(float S, float K){
    if(S<K){return K-S;}
    else{return 0;}
}

float Evolution(float S_0, float a, float b, int i, int n){
   // assert(S_0>=0);
    return S_0*(puiss(1+a, i)*puiss(1+b,n-i));
}


//Le prix de l'option est u(0, S_0) (théorème)
float Pricer(float S_0, float K, float a, float b, float p, int N){
    int choix;
    cout<<"Quel est le type de votre option ?"<<endl;
    cout<<"1 - Call"<<endl;
    cout<<"2 - Put"<<endl;
    cin>>choix;

    float* u;
    u = new float[N+1];

    //U est enfait ici u(N,.) du théorème
    if(choix == 1){
        for(int i = 0; i<=N; i++){
            u[i] = CallPayOff(Evolution(S_0, a, b, i, N), K);
        }
    }
    else if(choix == 2){
        for(int i = 0; i<=N; i++){
            u[i] = PutPayOff(Evolution(S_0, a, b, i, N), K);
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
            if(choix == 1 && Evolution(S_0, a, b, i, j) - K > u[i]){
                u[i]  = Evolution(S_0, a, b, i, N) - K;
                ta = j;
            }
            else if(choix == 2 && K - Evolution(S_0, a, b, i, j) > u[i]){
                u[i]  = K - Evolution(S_0, a, b, i, N);
                ta = j;
            }
        }
    }
    cout<<"Le prix de l'option de est de "<<u[0]<<"€"<<endl;
    cout<<"Le temps d'arret optimal est "<<ta<<endl;
    return u[0];

}




/*
//On calcule U pour une certaine valeur de SN
//On veut retourner un tableau (tableau des valeurs de u) ce qui n'est pas possible on renvoit donc un pointeur vers ce tableau
//la fonction return int *u = new float[N]
//On le retrouve grace a : int *U = tab_u()

int *tab_u(int N, float K, float a, float b){

    //Je calcule SN
    float S[N];
    S[0] = K;
    for(int i=1; i<N; i++){
        //Dans le modele de CR a et b sont equiprobables
        int v1 = rand() % 2;
        if(v1 == 0)
            S[i] = S[i-1]*(1+a);
        else
            S[i] = S[i-1]*(1+b);
    }

    int *U = new float[N][N];
    for(int i=0; i<N; i++){
        float tmp = puiss(1+a,i)*puiss(1+b,N-i);
        if(tmp>0)
            U[N][i] = tmp;
        else
            U[N][i] = 0;
    }

    for(int i=N-2; i>=0; i--){

        float tmp = 0.5*
    }

}
*/
