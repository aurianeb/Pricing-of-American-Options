#ifndef INTTREE_H
#define INTTREE_H
#include <vector>
#include <iostream>
#include <Imagine/Graphics.h>
using namespace Imagine;
using namespace std;

#endif // INTTREE_H

//Source : Florent Benaych-Georges M2 IFMA UPMC

//Pour pouvoir retourner le prix et le temps d'arret
class couple{
    public :
        int ta;
        float prix;
};

class noeud {
    protected :
        noeud * pere_haut;
        noeud * pere_bas;
        noeud * fils_haut;
        noeud * fils_bas;
        couple valeur ;
    public :
        noeud() : valeur(), fils_haut(NULL), fils_bas(NULL), pere_haut(NULL), pere_bas(NULL) {};
        //Constructeur
        noeud(const noeud &n) : pere_haut(n.pere_haut), pere_bas(n.pere_bas), valeur (n.valeur), fils_haut(n.fils_haut), fils_bas(n.fils_bas) {};
        ~noeud() { ;};
        void creer_generation(noeud *n_node);
        noeud* creer_arbre (int n);
        //noeud* arbre (float S_0, float K, float a, float b, float p, int N, float r, int choix);
//la methode suivante cree un fils a l’objet courant.
//Ce fils est un "fils haut" si l’objet courant n’en a pas, et un "fils bas" //si l’objet courant a deja un "fils haut"
    noeud* cree_fils() {
        noeud *nouveau=new noeud;
        if (fils_haut==NULL) {
            nouveau ->pere_bas=this;
            nouveau->pere_haut=frere_haut();
            if (frere_haut()!=NULL)
                nouveau ->pere_haut ->fils_bas=nouveau;
            fils_haut=nouveau;
        }
        else if (fils_bas == NULL) {
            nouveau ->pere_haut=this;
            nouveau->pere_bas=frere_bas();
            if (frere_bas()!=NULL)
                nouveau ->pere_bas ->fils_haut=nouveau;
            fils_bas=nouveau;
        }
        return nouveau;
    }

    noeud *frere_haut(){
        return (pere_haut==NULL) ? NULL : pere_haut->fils_haut;};
    noeud *frere_bas(){
        return (pere_bas==NULL) ? NULL : pere_bas->fils_bas;};
    noeud *fh(){
        return (fils_haut==NULL) ? NULL : fils_haut;};
    noeud *fb(){
        return (fils_bas==NULL) ? NULL : fils_bas;};
    noeud *ph(){
        return (pere_haut==NULL) ? NULL : pere_haut;};
    noeud *pb(){
        return (pere_bas==NULL) ? NULL : pere_bas;};


    void set_valeur(couple f) {
        valeur = f; }
    couple get_valeur() {
        return valeur;}


};

void noeud::creer_generation(noeud *n_node){
    //pt va parcourir toutes les feuilles

    //this->cree_fils();
    //this->cree_fils();
    noeud *pt;
    noeud *pt_sauv = NULL;

    for (pt = n_node; pt != NULL; pt = pt->frere_bas())
        {
            if ( pt_sauv == NULL)
               {
                    pt ->fils_haut = pt->cree_fils();
               }
            else
                {
                    pt ->fils_haut = pt->frere_haut()->fils_bas;
                    pt->fils_haut->pere_bas = pt;
                }
            pt->fils_bas = pt->cree_fils();
            pt_sauv = pt->fils_bas ;
            //pt->cree_fils();
            //pt->cree_fils();
        } ;
}

//On cree un arbre ainsi : on ajoute 2 fils à chacune des feuilles

noeud* noeud::creer_arbre (int n){
    noeud *arbre, *pt;
    arbre = new noeud;
    //arbre = cree_fils_haut();
    pt = arbre;

    for (int i = 0 ; i < n ; i++)
    {
        arbre->creer_generation(pt);
        pt = pt->fils_haut;
    };
    return (arbre);
}




Window display(noeud *n, int N){
    int length = 50*(N+2);
    Window w = openWindow(length+50,length);
    int x = 0;
    int y0 = 25*(N+1);
    int y1 = 0;
    int compteur = 0;
    noeud *pt, *at;
    for(pt = n; pt != NULL; pt = pt->fh()){
        x++;
        compteur ++;
        for(at = pt; at != NULL; at = at->frere_bas()){
            string s = to_string(int((at->get_valeur()).prix));
            if((at->get_valeur()).ta == 0){drawString(50*x,y0+50*y1,s, RED);}
            if((at->get_valeur()).ta == 1){drawString(50*x,y0+50*y1,s, BLACK);}
            //Pour ne pas avoir de branches qui dépassent de l'arbre
            if(compteur != N+1){
                drawLine(50*x,y0+50*y1,50*(x+1),y0-25+50*(y1+1),BLUE);
                drawLine(50*x,y0+50*y1,50*(x+1),y0+25+50*(y1-1),BLUE);}
            y1++;
        }
        y0 += -25;
        y1 = 0;
    }
    click();
    return w;
    //closeWindow(w);

}

float max(float a, float b){
    if(a<b){return b;}
    else{return a;}
}

float PayOff(float S_0, float K, int choix){
    if(choix == 1){return max(S_0 - K, 0);}
    else{return max(K-S_0,0);}
}

void display_traj(noeud *n, int N, float S_0, float K, int choix){
    int length = 50*(N+2);
    Window w = openWindow(length+50,length);
    int x = 0;
    int y0 = 25*(N+1);
    int y1 = 0;
    int compteur = 0;
    noeud *pt, *at;
    float p = 0.5;

    for(pt = n; pt != NULL; pt = pt->fb()){
        x++;
        compteur ++;
        for(at = pt; at != NULL; at = at->frere_haut()){
            string s = to_string(int((at->get_valeur()).prix));
            if((at->get_valeur()).ta == 0){drawString(50*x,y0+50*y1,s, RED);}
            if((at->get_valeur()).ta == 1){drawString(50*x,y0+50*y1,s, BLACK);}
            //Pour ne pas avoir de branches qui dépassent de l'arbre
            if(compteur != N+1){
                drawLine(50*x,y0+50*y1,50*(x+1),y0-25+50*(y1+1),BLUE);
                drawLine(50*x,y0+50*y1,50*(x+1),y0+25+50*(y1-1),BLUE);}
            y1++;
        }
        y0 += -25;
        y1 = 0;
    }

    x = 1;
    y0 = 25*(N+1);
    float val = S_0;
    pt = n;
    while(x<N && PayOff(S_0,K,choix)!=(pt->get_valeur()).prix){
        float f = (float)rand()/(float)(RAND_MAX);
        if(f<p){
            drawLine(50*x,y0,50*(x+1),y0-25,GREEN,2);
            y0 = y0-25;
            pt = pt->fh();
        }
        else{
            drawLine(50*x,y0,50*(x+1),y0+25,GREEN,2);
            y0 = y0+25;
            pt = pt->fb();
        }
        x++;
    }
    drawCircle(50*x+10,y0-5,15,GREEN,2);

    click();
    closeWindow(w);

}


