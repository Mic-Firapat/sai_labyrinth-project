#include <stdio.h>
#include <stdlib.h>
#include "GL/gl.h"
#include "GL/glut.h"
#include <time.h>
#include <math.h>

#define WIDTH 720
#define LENGTH 720
#define WORK_WIDTH 20
#define WORK_LENGTH 20

#define NB_ETAGE 3
#define W_ETAGE 15
#define L_ETAGE 15
#define H_ETAGE 10

//Minimum 6 et si possible pair
#define W_CASE 30
#define L_CASE 30

#define V_DEP 2
#define V_ROTAT 3


int ***batiment;// Tableau de grilles, représente le batiment
//0 : libre, 1 : Mur complet , 2 : Téléporteur montant, 3 : Téléporteur descendant
int *murs_complexes;
//Tableau utilisé pour garder la taille des murs complexes (horizontaux et verticaux)
float angle = 270;
float posx = 5,
    posy = 5,
    posz = 15;
float lookx = -15,
    looky = 0,
    lookz = -15;

float *listecubes;
int nbcubes;
//Affiche le cube décrit par les deux sommets opposés (x1,y1,z1) et (x2,y2,z2)
void affichecube(float x1, float y1, float z1, float x2, float y2, float z2){
    glBegin(GL_QUADS);
    //Face décrite par z = z1
    glColor3f(1,0,0);
    glVertex3f(x1,y1,z1);
    glVertex3f(x1,y2,z1);
    glVertex3f(x2,y2,z1);
    glVertex3f(x2,y1,z1);

    //Face décrite par z = z2
    glColor3f(0,1,0);
    glVertex3f(x1,y1,z2);
    glVertex3f(x1,y2,z2);
    glVertex3f(x2,y2,z2);
    glVertex3f(x2,y1,z2);

    //Face décrite par x = x1
    glColor3f(0,1,1);
    glVertex3f(x1,y1,z1);
    glVertex3f(x1,y2,z1);
    glVertex3f(x1,y2,z2);
    glVertex3f(x1,y1,z2);


    //Face décrite par x = x2
    glColor3f(1,1,0);
    glVertex3f(x2,y1,z1);
    glVertex3f(x2,y2,z1);
    glVertex3f(x2,y2,z2);
    glVertex3f(x2,y1,z2);

    //Face décrite par y = y1
    glColor3f(0.9, 0.64,0);
    glVertex3f(x1,y1,z1);
    glVertex3f(x1,y1,z2);
    glVertex3f(x2,y1,z2);
    glVertex3f(x2,y1,z1);

    //Face décrite par y = y2
    glColor3f(0.9, 0.64,0);
    glVertex3f(x1,y2,z1);
    glVertex3f(x1,y2,z2);
    glVertex3f(x2,y2,z2);
    glVertex3f(x2,y2,z1);
    
    glEnd();
}


void GererClavier(unsigned char key, int x, int y){
    //printf("Touche : %c\nSouris : %d %d\n",key,x,y);
    if (key == 'z') {
        float angler = M_PI / 180 * angle;
        posx += V_DEP*cos(angler);
        posz +=V_DEP*sin(angler);
    }
    if (key == 's') {
        float angler = M_PI / 180 * angle;
        posx += -V_DEP*cos(angler);
        posz += -V_DEP*sin(angler);
    }
    if (key == 'o') { posy +=1;}
    if (key == 'l') { posy -= 1;}
    if (key == 'p') {looky += 1;}
    if (key == 'm') {looky -= 1;}
    if (key == 'q'){
        angle = (angle >= V_ROTAT) ? angle - V_ROTAT : angle + 360 - V_ROTAT; 
    }
    if (key == 'd'){
        angle = (angle <360 - V_ROTAT) ? angle + V_ROTAT : angle - 360 + V_ROTAT;
    }
}

void affichage(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.1,0.1,-0.1,0.1,0.1,500);
    float angler;
    angler = angle * M_PI / 180;
    lookx = cos(angler)*10;
    lookz = sin(angler)*10;
    gluLookAt(posx,posy,posz,posx + lookx,posy + looky,posz + lookz,0,1,0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    nbcubes = 0;
    for (int i = 0; i < NB_ETAGE; i++){
        glBegin(GL_QUADS);
        glColor3f(0.9, 0.64,0);
        glVertex3f(0,i * H_ETAGE,0);
        glVertex3f(L_ETAGE*L_CASE,i* H_ETAGE,0);
        glVertex3f(L_ETAGE*L_CASE, i *H_ETAGE, W_ETAGE * W_CASE);
        glVertex3f(0, i *H_ETAGE, W_ETAGE * W_CASE);
        glEnd();
        for (int j = 0; j < W_ETAGE; j++){
            
            for (int k = 0; k < L_ETAGE; k++){
                if (batiment[i][j][k] != 2 && batiment[i][j][k] != 3){
                    glBegin(GL_QUADS);
                    glColor3f(0.9, 0.64,0);
                    glVertex3f(k*L_CASE,i * H_ETAGE,j*W_CASE);
                    glVertex3f((k+1)*L_CASE,i* H_ETAGE,j*W_CASE);
                    glVertex3f((k+1)*L_CASE, i *H_ETAGE, (j+1)*W_CASE);
                    glVertex3f(k*L_CASE, i *H_ETAGE, (j+1)*W_CASE);
                    glEnd();
                }
                if (batiment[i][j][k] == 0){
                    //Zone vide où on peut se déplacer
                    nbcubes += 0;
                }
                else if (batiment[i][j][k] == 1){
                    //Mur complet (gros cube)
                    listecubes[nbcubes*6] = k*L_CASE;
                    listecubes[nbcubes*6 + 1] = i * H_ETAGE;
                    listecubes[nbcubes*6 + 2] = j * W_CASE;
                    listecubes[nbcubes*6 + 3] = (k+1) * L_CASE;
                    listecubes[nbcubes*6 + 4] = (i+1) * H_ETAGE;
                    listecubes[nbcubes*6 + 5] = (j+1) * W_CASE;
                    nbcubes ++;
                }
                else if (batiment[i][j][k] == 2){
                    //Téléporteur ascendant ou rampe à l'avenir
                    nbcubes += 0;
                }
                else if (batiment[i][j][k] == 3){
                    //Téléporteur descendant ou juste rien à l'avenir
                    nbcubes += 0;
                }
                else if (batiment[i][j][k] == 4){
                    //Mur vertical et horizontal
                    //Partie verticale
                    int aux = i * L_ETAGE * W_ETAGE + j*L_ETAGE + k;
                    int tmp = k*L_CASE + 0.5 * L_CASE;
                    int tmp2 = murs_complexes[aux] / 2;
                    listecubes[nbcubes*6] = tmp - tmp2;
                    listecubes[nbcubes*6 + 1] = i * H_ETAGE;
                    listecubes[nbcubes*6 + 2] = j * W_CASE;
                    listecubes[nbcubes*6 + 3] = tmp + tmp2;
                    listecubes[nbcubes*6 + 4] = (i+1) * H_ETAGE;
                    listecubes[nbcubes*6 + 5] = (j+1) * W_CASE;
                    nbcubes ++;
                    //Partie horizontale
                    tmp = j*W_CASE + 0.5*W_CASE;
                    tmp2 = murs_complexes[aux + 1] / 2;
                    listecubes[nbcubes*6] = k*L_CASE;
                    listecubes[nbcubes*6 + 1] = i * H_ETAGE;
                    listecubes[nbcubes*6 + 2] = tmp - tmp2;
                    listecubes[nbcubes*6 + 3] = (k+1) * L_CASE;
                    listecubes[nbcubes*6 + 4] = (i+1) * H_ETAGE;
                    listecubes[nbcubes*6 + 5] = tmp + tmp2;
                    nbcubes ++;
                }
                else if (batiment[i][j][k] % 2 == 0){
                    //Mur vertical uniquement
                    int tmp = k*L_CASE + 0.5 * L_CASE;
                    int tmp2 = batiment[i][j][k] / 2;
                    listecubes[nbcubes*6] = tmp - tmp2;
                    listecubes[nbcubes*6 + 1] = i * H_ETAGE;
                    listecubes[nbcubes*6 + 2] = j * W_CASE;
                    listecubes[nbcubes*6 + 3] = tmp + tmp2;
                    listecubes[nbcubes*6 + 4] = (i+1) * H_ETAGE;
                    listecubes[nbcubes*6 + 5] = (j+1) * W_CASE;
                    nbcubes ++;
                }
                else {
                    //Mur horizontal
                    int tmp = j*W_CASE + 0.5*W_CASE;
                    int tmp2 = (batiment[i][j][k] - 1) / 2;
                    listecubes[nbcubes*6] = k*L_CASE;
                    listecubes[nbcubes*6 + 1] = i * H_ETAGE;
                    listecubes[nbcubes*6 + 2] = tmp - tmp2;
                    listecubes[nbcubes*6 + 3] = (k+1) * L_CASE;
                    listecubes[nbcubes*6 + 4] = (i+1) * H_ETAGE;
                    listecubes[nbcubes*6 + 5] = tmp + tmp2;
                    nbcubes ++;
                }
                
                
            }
        }
    }
    for (int i = 0; i < nbcubes; i++){
        affichecube(listecubes[i*6],listecubes[i*6+1],listecubes[i*6+2],listecubes[i*6+3],listecubes[i*6+4],listecubes[i*6+5]);
    }
    

    affichecube(-0.1,0,-0.1,0.1,1000,0.1);
    affichecube(0,-0.1,-0.1,1000,0.1,0.1);
    
    glFlush();
}

void Animer(){
    glutPostRedisplay();
}


void lire_liste_cubes(char *nom_fichier){
    //Lit la liste des cubes dans le fichier
    FILE *fichier = NULL;

    //Ouverture
    fichier = fopen(nom_fichier, "r");
    if (fichier == NULL){
        fprintf(stderr, "Ouverture du fichier ratée\n");
        return;
    }
    //Lecture de la première ligne : nombre de cubes
    if (fscanf(fichier, "%d\n",&nbcubes) != 1){
        fprintf(stderr,"Lecture de la première ligne ratée\n");
        return;
    }

    //Remplissage de la liste de cubes
    listecubes = malloc(sizeof(float)*nbcubes*6);
    for (int i = 0; i < nbcubes; i++){
        if (fscanf(fichier,"%f %f %f_%f %f %f\n",&(listecubes[i*6]),&(listecubes[i*6+1]),&(listecubes[i*6+2]), &(listecubes[i*6+3]), &(listecubes[i*6+4]), &(listecubes[i*6+5])) != 6){
            fprintf(stderr, "Erreur de lecture sur la ligne de données numéro %d\n",i);
            return;
        }
    }

    //Fermeture du fichier et renvoi du tableau
    fclose(fichier);
}


/*Affiche en terminal le contenu des grilles*/
void affiche_grilles(){
    for (int i = 0; i < NB_ETAGE;i++){
        for (int j = 0; j < W_ETAGE; j++){
            for (int k = 0; k <L_ETAGE; k++){
                printf("%d ", batiment[i][j][k]);
            }
            printf("\n");
        }
        printf("-------\n");
    }
}

int main(int argc, char **argv){


    srand(time(NULL));
    listecubes = malloc(sizeof(float) * 6 * NB_ETAGE*W_ETAGE*L_ETAGE * 2);
    batiment = malloc(sizeof(int)*NB_ETAGE*W_ETAGE*L_ETAGE);
    murs_complexes = malloc(sizeof(int)* NB_ETAGE*W_ETAGE*L_ETAGE*2);
    for (int i = 0; i < NB_ETAGE;i++){
        batiment[i] = malloc(sizeof(int) * W_ETAGE * L_ETAGE);
        for (int j = 0; j < W_ETAGE; j++){
            batiment[i][j] = malloc(sizeof(int) * L_ETAGE);
            for (int k = 0; k <L_ETAGE; k++){
                batiment[i][j][k] = 3;
            }
        }
    }
    //On remplit les bords par des cubes/murs pleins
    for (int i = 0; i < NB_ETAGE; i++){
        for (int k = 0; k < L_ETAGE; k++){
            batiment[i][0][k] = 1;
            batiment[i][W_ETAGE - 1][k] = 1;
        }
        for (int j = 1; j < W_ETAGE - 1; j++){
            batiment[i][j][0] = 1;
            batiment[i][j][L_ETAGE - 1] = 1;
        }
    }


    //Création du labyrinthe
    int aux_voisins[4][2] = {
        {-2,0},
        {2,0},
        {0,-2},
        {0,2}
    };
    int **historique = malloc(sizeof(int)*W_ETAGE*L_ETAGE*2);
    for (int aux = 0; aux < W_ETAGE*L_ETAGE;aux++){
        historique[aux] = malloc(sizeof(int)*2);
        historique[aux][0] = -1;
        historique[aux][1] = -1;
    }
    for (int i = 0; i < NB_ETAGE;i ++){
        for (int aux = 0; aux < W_ETAGE*L_ETAGE;aux++){
            historique[aux][0] = -1;
            historique[aux][1] = -1;
        }
        int courant = 0;
        historique[courant][0] = 1;
        historique[courant][1] = 1;
        batiment[i][historique[courant][0]][historique[courant][1]] = 2;
        do {
            //Considération des voisins
            int voisins[4][4];
            int nbvoisins_possibles = 0;
            for (int aux = 0; aux < 4; aux ++){
                int tmpx = historique[courant][0] + aux_voisins[aux][0];
                int tmpz = historique[courant][1] + aux_voisins[aux][1];
                int tmpx2 = historique[courant][0] + aux_voisins[aux][0] / 2;
                int tmpz2 = historique[courant][1] + aux_voisins[aux][1] / 2;
                if (tmpx > 0 && tmpx < (L_ETAGE - 1) && tmpz > 0 && tmpz < (W_ETAGE - 1) && tmpx2 > 0 && tmpx2 < (L_ETAGE - 1) && tmpz2 > 0 && tmpz2 < (W_ETAGE - 1) && (batiment[i][tmpx][tmpz] == 0 || batiment[i][tmpx][tmpz] == 3) && (batiment[i][tmpx2][tmpz2] != 1)){
                    //Voisin envisageable
                   
                    voisins[nbvoisins_possibles][0] = tmpx;
                    voisins[nbvoisins_possibles][1] = tmpz;
                    voisins[nbvoisins_possibles][2] = tmpx2;
                    voisins[nbvoisins_possibles][3] = tmpz2;
                    nbvoisins_possibles++;
                }
            }
            if (nbvoisins_possibles == 0 && courant != 0){
                //Si pas de voisins on revient en arrière si on peut
                courant -= 1;
            }
            else if (nbvoisins_possibles != 0){
                //On choisit un voisin au pif
                int choix = rand() % nbvoisins_possibles;
                courant++;
                historique[courant][0] = voisins[choix][0];
                historique[courant][1] = voisins[choix][1];
                batiment[i][historique[courant][0]][historique[courant][1]] = 2;
                //On casse le mur entre les deux
                batiment[i][voisins[choix][2]][voisins[choix][3]] = 0;
            }
        } while (courant != 0);
        //On a fini cet étage on passe au suivant
    }

    //Les murs à tomber restants deviennent des vrais murs et on remet des 0 à la place des 2
    for (int i = 0; i < NB_ETAGE; i++){
        for (int j = 0; j < W_ETAGE; j++){
            for (int k = 0; k < L_ETAGE; k++){
                if (batiment[i][j][k] == 3){
                    switch(rand()%3){
                    case 0:
                        //Double mur
                        batiment[i][j][k] = 4;
                        break;
                    case 1:
                        //Mur vertical 
                        batiment[i][j][k] = rand() % ((L_CASE + 6)/2 - 6) + 6;
                        if (batiment[i][j][k] % 2 == 1) { batiment[i][j][k] += -1;}
                        break;
                    default :
                        //Mur horizontal
                        batiment[i][j][k] = rand() % ((W_CASE + 6)/2 - 6) + 6;
                        if (batiment[i][j][k] % 2 == 0) { batiment[i][j][k] += 1;}
                    }
                }
                else if (batiment[i][j][k] == 2){
                    batiment[i][j][k] = 0;
                }
            }
        }
    }

    //Déterminer la taille des murs complexes (verticaux et horizontaux)
    for (int i = 0; i < NB_ETAGE; i++){
        for (int j = 0; j < W_ETAGE; j++){
            for (int k = 0; k < L_ETAGE; k++){
                if (batiment[i][j][k] == 4){
                    int aux = i * L_ETAGE * W_ETAGE + j*L_ETAGE + k;
                    murs_complexes[aux] = rand() % ((L_CASE + 6)/2) + 4;
                    if (murs_complexes[aux] % 2 == 1){
                        murs_complexes[aux] += -1;
                    }
                    murs_complexes[aux + 1] = rand() % ((W_CASE + 6)/2) + 4;
                    
                    if (murs_complexes[aux + 1] % 2 == 1){
                        murs_complexes[aux + 1] += -1;
                    }
                    printf("%d %d\n",murs_complexes[aux],murs_complexes[aux + 1]);
                }
            }
        }
    }
    affiche_grilles();
    //lire_liste_cubes("listecubes");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA |GLUT_SINGLE | GLUT_DEPTH);

    glutInitWindowSize(WIDTH,LENGTH);
    glutInitWindowPosition(42,42);

    glutCreateWindow("Trwa dai");
    glEnable(GL_DEPTH_TEST);
    glutDisplayFunc(affichage);
    glutKeyboardFunc(GererClavier);
    glutIdleFunc(Animer);
    glutMainLoop();
    exit(0);
}
