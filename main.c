#include <stdio.h>
#include <stdlib.h>
#include "GL/gl.h"
#include "GL/glut.h"
#include <math.h>

#define WIDTH 1280
#define LENGTH 720
#define WORK_WIDTH 20
#define WORK_LENGTH 20

#define NB_ETAGE 3
#define W_ETAGE 10
#define L_ETAGE 10
#define H_ETAGE 10
#define W_CASE 5
#define L_CASE 5

#define V_DEP 1
#define V_ROTAT 3


int ***batiment;// Tableau de grilles, représente le batiment
//0 : libre, 1 : Mur , 2 : Téléporteur montant, 3 : Téléporteur descendant
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
    glFrustum(-0.2,0.2,-0.2,0.2,0.2,500);
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
                if (batiment[i][j][k] == 1){
                    //Mur
                    listecubes[nbcubes*6] = k*L_CASE;
                    listecubes[nbcubes*6 + 1] = i * H_ETAGE;
                    listecubes[nbcubes*6 + 2] = j * W_CASE;
                    listecubes[nbcubes*6 + 3] = (k+1) * L_CASE;
                    listecubes[nbcubes*6 + 4] = (i+1) * H_ETAGE;
                    listecubes[nbcubes*6 + 5] = (j+1) * W_CASE;
                    nbcubes ++;
                }
                else {
                    if (batiment[i][j][k] == 2){
                        //Téléporteur ascendant
                        nbcubes += 0;
                    }
                    else if (batiment[i][j][k] == 3){
                        //Téléporteur descendant
                        nbcubes += 0;
                    }
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


    listecubes = malloc(sizeof(float) * 6 * NB_ETAGE*W_ETAGE*L_ETAGE);
    batiment = malloc(sizeof(int)*NB_ETAGE*W_ETAGE*L_ETAGE);
    for (int i = 0; i < NB_ETAGE;i++){
        batiment[i] = malloc(sizeof(int) * W_ETAGE * L_ETAGE);
        for (int j = 0; j < W_ETAGE; j++){
            batiment[i][j] = malloc(sizeof(int) * L_ETAGE);
            for (int k = 0; k <L_ETAGE; k++){
                batiment[i][j][k] = 0;
            }
        }
    }
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
