#include <stdio.h>
#include <stdlib.h>
#include "GL/gl.h"
#include "GL/glut.h"
#include <time.h>
#include <math.h>
#include <string.h>

#define WIDTH 800
#define LENGTH 800
#define WORK_WIDTH 20
#define WORK_LENGTH 20

#define NB_ETAGE 3
#define W_ETAGE 5
#define L_ETAGE 5
#define H_ETAGE 10

//Minimum 6 et si possible pair
#define W_CASE 30
#define L_CASE 30

#define V_DEP 2
#define V_ROTAT 5
#define H_VIEW 5 //Hauteur de vue du perso

int ***batiment;// Tableau de grilles, représente le batiment
//0 : libre, 1 : Mur complet , 2 : Téléporteur montant, 3 : Téléporteur descendant
int *murs_complexes;
//Tableau utilisé pour garder la taille des murs complexes (horizontaux et verticaux)
float angle = 90;
float posx = L_CASE * 3/2,
    posy = H_VIEW,
    posz = W_CASE * 3/2;
float lookx = -15,
    looky = 0,
    lookz = -15;

int mouseIn = 0,
    mousex = 0,
    mousey = 0;


float *listecubes;
int nbcubes;


int affiche_texte = 0, xray=0, hitbox=0, ghosting = 0;
int nbObjet = 0;

//Texture
GLuint texture;
GLuint mur;



GLuint ChargeTexture(GLuint tex, const char * fichier, int w, int h){

  unsigned char * data;
  FILE * file;
  size_t warning;

  // Lecture fichier image

  file = fopen(fichier, "rb");
  if ( file == NULL) return 0;
  data = (unsigned char *)malloc(w*h*3);
  warning = fread(data, w*h*3,1,file);
  warning+=0;
  fclose(file);

  // Gentexture
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


  // Paramètrage texture
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );


  // Mipmaps des textures
  gluBuild2DMipmaps( GL_TEXTURE_2D, 3, w, h,GL_RGB, GL_UNSIGNED_BYTE, data );
  free(data);

  return tex;
}


// Libère la mémoire de texture
void FreeTexture(GLuint texture){
  glDeleteTextures(1, &texture);
}

//Donne la position dans la grille du joueur actuellement (uniquement utilisée pour certains trucs
int *posDansGrille(){
    int *pos = malloc(sizeof(int)*3);
    int i = (posy - H_VIEW) / (H_ETAGE);
    int j = posz / W_CASE;
    int k = posx / L_CASE;
    pos[0] = i; pos[1] = j; pos[2] = k;
    return pos;
    
}
//Affiche le cube décrit par les deux sommets opposés (x1,y1,z1) et (x2,y2,z2)
void affichecube(float x1, float y1, float z1, float x2, float y2, float z2){

  if(!hitbox){
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, mur);
    glBegin(GL_QUADS);
    //Face décrite par z = z1
    glColor3f(1,0,0);
    glTexCoord2f(0.0,0.0);glVertex3f(x1,y1,z1);
    glTexCoord2f(1,0.0);glVertex3f(x1,y2,z1);
    glTexCoord2f(1,1);glVertex3f(x2,y2,z1);
    glTexCoord2f(0.0,1);glVertex3f(x2,y1,z1);

    //Face décrite par z = z2
    glColor3f(0,1,0);
    glTexCoord2f(0.0,0.0);glVertex3f(x1,y1,z2);
    glTexCoord2f(1,0.0);glVertex3f(x1,y2,z2);
    glTexCoord2f(1,1);glVertex3f(x2,y2,z2);
    glTexCoord2f(0.0,1);glVertex3f(x2,y1,z2);

    //Face décrite par x = x1
    glColor3f(0,1,1);
    glTexCoord2f(0.0,0.0);glVertex3f(x1,y1,z1);
    glTexCoord2f(1,0.0);glVertex3f(x1,y2,z1);
    glTexCoord2f(1,1);glVertex3f(x1,y2,z2);
    glTexCoord2f(0.0,1);glVertex3f(x1,y1,z2);


    //Face décrite par x = x2
    glColor3f(1,1,0);
    glTexCoord2f(0.0,0.0);glVertex3f(x2,y1,z1);
    glTexCoord2f(1,0.0);glVertex3f(x2,y2,z1);
    glTexCoord2f(1,1);glVertex3f(x2,y2,z2);
    glTexCoord2f(0.0,1);glVertex3f(x2,y1,z2);

    glBindTexture(GL_TEXTURE_2D, texture);

    //Face décrite par y = y1
    //glColor3f(0.9, 0.64,0);
    
    glTexCoord2f(0.0,0.0);glVertex3f(x1+0.01,y1+0.01,z1+0.01);
    glTexCoord2f(1,0.0);glVertex3f(x1+0.01,y1+0.01,z2-0.01);
    glTexCoord2f(1,1);glVertex3f(x2-0.01,y1+0.01,z2-0.01);
    glTexCoord2f(0.0,1);glVertex3f(x2-0.01,y1+0.01,z1+0.01);

    //Face décrite par y = y2
    //glColor3f(0.9, 0.64,0);
    glTexCoord2f(0.0,0.0);glVertex3f(x1+0.01,y2-0.01,z1+0.01);
    glTexCoord2f(1,0.0);glVertex3f(x1+0.01,y2-0.01,z2-0.01);
    glTexCoord2f(1,1);glVertex3f(x2-0.01,y2-0.01,z2-0.01);
    glTexCoord2f(0.0,1);glVertex3f(x2-0.01,y2-0.01,z1+0.01);
    glDisable(GL_TEXTURE_2D);
    glEnd();
  }

  else {
    glLineWidth(7);
    glBegin(GL_LINE_LOOP);
    glColor3f(0,1,0);
    //Face décrite par z = z1
    glVertex3f(x1,y1,z1);
    glVertex3f(x1,y2,z1);
    glVertex3f(x2,y2,z1);
    glVertex3f(x2,y1,z1);

    //Face décrite par z = z2
    glVertex3f(x1,y1,z2);
    glVertex3f(x1,y2,z2);
    glVertex3f(x2,y2,z2);
    glVertex3f(x2,y1,z2);

    //Face décrite par x = x1
    glVertex3f(x1,y1,z1);
    glVertex3f(x1,y2,z1);
    glVertex3f(x1,y2,z2);
    glVertex3f(x1,y1,z2);


    //Face décrite par x = x2
    glVertex3f(x2,y1,z1);
    glVertex3f(x2,y2,z1);
    glVertex3f(x2,y2,z2);
    glVertex3f(x2,y1,z2);

    //Face décrite par y = y1
    glVertex3f(x1+0.01,y1+0.01,z1+0.01);
    glVertex3f(x1+0.01,y1+0.01,z2-0.01);
    glVertex3f(x2-0.01,y1+0.01,z2-0.01);
    glVertex3f(x2-0.01,y1+0.01,z1+0.01);

    //Face décrite par y = y2
    glVertex3f(x1+0.01,y2-0.01,z1+0.01);
    glVertex3f(x1+0.01,y2-0.01,z2-0.01);
    glVertex3f(x2-0.01,y2-0.01,z2-0.01);
    glVertex3f(x2-0.01,y2-0.01,z1+0.01);
    glEnd();
  }

}


void afficheObjet(float x1, float y1, float z1, float x2, float y2, float z2){
  glBegin(GL_QUADS);
  //Face décrite par z = z1
  glColor3f(1,0.5,1);
  glVertex3f(x1+W_CASE/2.3,y1+3,z1+W_CASE/2.3);
  glVertex3f(x1+W_CASE/2.3,y2-4,z1+W_CASE/2.3);
  glVertex3f(x2-W_CASE/2.3,y2-4,z1+W_CASE/2.3);
  glVertex3f(x2-W_CASE/2.3,y1+3,z1+W_CASE/2.3);

  //Face décrite par z = z2
  glVertex3f(x1+W_CASE/2.3,y1+3,z2-W_CASE/2.3);
  glVertex3f(x1+W_CASE/2.3,y2-4,z2-W_CASE/2.3);
  glVertex3f(x2-W_CASE/2.3,y2-4,z2-W_CASE/2.3);
  glVertex3f(x2-W_CASE/2.3,y1+3,z2-W_CASE/2.3);

  //Face décrite par x = x1
  glVertex3f(x1+W_CASE/2.3,y1+3,z1+W_CASE/2.3);
  glVertex3f(x1+W_CASE/2.3,y2-4,z1+W_CASE/2.3);
  glVertex3f(x1+W_CASE/2.3,y2-4,z2-W_CASE/2.3);
  glVertex3f(x1+W_CASE/2.3,y1+3,z2-W_CASE/2.3);


  //Face décrite par x = x2
  glVertex3f(x2-W_CASE/2.3,y1+3,z1+W_CASE/2.3);
  glVertex3f(x2-W_CASE/2.3,y2-4,z1+W_CASE/2.3);
  glVertex3f(x2-W_CASE/2.3,y2-4,z2-W_CASE/2.3);
  glVertex3f(x2-W_CASE/2.3,y1+3,z2-W_CASE/2.3);

  //Face décrite par y = y1
  glVertex3f(x1+W_CASE/2.3,y1+3,z1+W_CASE/2.3);
  glVertex3f(x1+W_CASE/2.3,y1+3,z2-W_CASE/2.3);
  glVertex3f(x2-W_CASE/2.3,y1+3,z2-W_CASE/2.3);
  glVertex3f(x2-W_CASE/2.3,y1+3,z1+W_CASE/2.3);

  //Face décrite par y = y2
  glVertex3f(x1+W_CASE/2.3,y2-4,z1+W_CASE/2.3);
  glVertex3f(x1+W_CASE/2.3,y2-4,z2-W_CASE/2.3);
  glVertex3f(x2-W_CASE/2.3,y2-4,z2-W_CASE/2.3);
  glVertex3f(x2-W_CASE/2.3,y2-4,z1+W_CASE/2.3);
  glEnd();
}



// Gestion des évènements clic de la souris
void GererCliqueSouris(int button, int state, int x, int y){
  state+=0;
  if( ( (x >= 0) && (x <= WIDTH) ) && ( (y >= 0) && (y <= LENGTH) ) ){
    if(button == GLUT_LEFT_BUTTON){
      mouseIn = 1;
      glutSetCursor(GLUT_CURSOR_NONE);
      glutWarpPointer(WIDTH/2, LENGTH/2);
      mousex = WIDTH/2;
      mousey = LENGTH/2;
    }
    else if (button == GLUT_RIGHT_BUTTON){
      mouseIn = 0;
      glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
    }
  }
}

// Gestion des évènements mouvement de la souris
void GererMouvementSouris(int x, int y){
  int dx = mousex - x, dy = mousey - y;
  
  if(mouseIn){

    if( dx > 0) {angle -= V_ROTAT/2;}
    if( dx < 0) {angle += V_ROTAT/2;}

    if( dy > 0) {looky += 0.5;}
    if( dy < 0) {looky -= 0.5;}


    if ( (x < WIDTH - 50  && x > 20) && (y < LENGTH -50 && y > 20) ){
      mousex = x;
      mousey = y;
    }
    else {
      mousex = WIDTH/2;
      mousey = LENGTH/2;
      glutWarpPointer(WIDTH/2, LENGTH/2);
    }
  }
  
}

void collisions(float *x, float *y, float *z){
    //Change les valeurs en paramètres pour qu'elles correspondent à l'endroit où va se déplacer le personnage
    int non = 0;
    int i = (*y - H_VIEW) / (H_ETAGE);
    int j = *z / W_CASE;
    int k = *x / L_CASE;
    if (batiment[i][j][k] != 2 && batiment[i][j][k] != 3){
        int aux = (*y - H_VIEW);
        if (aux % H_ETAGE != 0){
            if ((int)(*y) % H_ETAGE < H_VIEW){
                non = 1;
            }
            else {
                *y = i*H_ETAGE + H_VIEW;
                non = 0;
            }
        }
        
    }
    if (batiment[i][j][k] == 1){
        //Juste pas le droit de se déplacer là
        non = 1;
    }
    else if (batiment[i][j][k] == 4){
        //On vérifie les colisions avec les deux murs
        //Mur vertical et horizontal
        //Partie verticale
        int aux = i * L_ETAGE * W_ETAGE + j*L_ETAGE + k;
        int tmp = k*L_CASE + 0.5 * L_CASE;
        int tmp2 = murs_complexes[aux] / 2;
        if (
            (*x >= tmp - tmp2) &&
            (*x <= tmp + tmp2) &&
            (*z >= j * W_CASE) &&
            (*z <= (j+1) * W_CASE)){
            //Dans la partie verticale donc pas possible
            non = 1;
        }
        tmp = j*W_CASE + 0.5*W_CASE;
        tmp2 = murs_complexes[aux + 1] / 2;
        if (
            (*x >= k*L_CASE) &&
            (*x <= (k+1)*L_CASE) &&
            (*z >= tmp - tmp2) &&
            (*z <= tmp + tmp2)
            ) {
            //Partie horizontale
            //Pas possible non plus
            non = 1;
        }        
    }
    else if( batiment[i][j][k] >= 6 &&  (batiment[i][j][k] % 2 == 0)){
        //Mur vertical
        int tmp = k*L_CASE + 0.5 * L_CASE;
        int tmp2 = batiment[i][j][k] / 2;
         if (
            (*x >= tmp - tmp2) &&
            (*x <= tmp + tmp2) &&
            (*z >= j * W_CASE) &&
            (*z <= (j+1) * W_CASE)){
            //Dans la partie verticale donc pas possible
            non = 1;
         }
    }
     else if( batiment[i][j][k] >= 6 &&  (batiment[i][j][k] % 2 == 1)){
        //Mur horiontal
        int tmp = j*W_CASE + 0.5*W_CASE;
        int tmp2 = batiment[i][j][k] / 2;
        if (
            (*x >= k*L_CASE) &&
            (*x <= (k+1)*L_CASE) &&
            (*z >= tmp - tmp2) &&
            (*z <= tmp + tmp2)
            ) {
            //Partie horizontale
            //Pas possible non plus
            non = 1;
        }        
    }
    if (non){
        *x = posx, *y = posy, *z = posz;
    }
}

void GererClavier(unsigned char key, int x, int y){
  x+=0; y+=0;
    //printf("Touche : %c\nSouris : %d %d\n",key,x,y);
    float tmpx = posx, tmpy = posy, tmpz = posz;

    if (key == 'x') {
      if(xray == 0) {xray ++;}
      else {xray--;}
    }

    if( key == 'b'){
        if(hitbox == 0) {hitbox++;}
        else {hitbox--;}
    }

    if(key == 'g'){
        ghosting = 1 - ghosting;
    }
    
    if (key == 'z') {
        float angler = M_PI / 180 * angle;
        tmpx += V_DEP*cos(angler);
        tmpz +=V_DEP*sin(angler);
    }
    if (key == 's') {
        float angler = M_PI / 180 * angle;
        tmpx += -V_DEP*cos(angler);
        tmpz += -V_DEP*sin(angler);
    }
    if( key == 'q') {
      float angler =  M_PI / 180 * angle;
      tmpx += V_DEP*sin(angler);
      tmpz += -V_DEP*cos(angler);
    }
    if ( key == 'd') {
      float angler = M_PI / 180 * angle;
      tmpx += -V_DEP*sin(angler);
      tmpz += V_DEP*cos(angler);
    }
    if (key == 'o') {
      int *pos = posDansGrille();
      pos[0] = (pos[0] >= NB_ETAGE) ? NB_ETAGE - 1 : pos[0];
      if (((batiment[pos[0]][pos[1]][pos[2]] == 2) && (pos[0] + nbObjet < NB_ETAGE)) || ghosting == 1){	
	tmpy +=1;
      }
    }
    if (key == 'l') {
        int *pos = posDansGrille();
        pos[0] = (pos[0] >= NB_ETAGE) ? NB_ETAGE - 1 : pos[0];
        if ( batiment[pos[0]][pos[1]][pos[2]] == 3 || ghosting == 1){
            tmpy -=1;
        }
        else if (batiment[pos[0]][pos[1]][pos[2]] == 2 ){
            if (tmpy - pos[0]*H_ETAGE > H_VIEW){
                tmpy -=1;
            }
        }
    }
    if (key == 'p') {looky += 1;}
    if (key == 'm') {looky -= 1;}
    if (key == 'a'){
        angle = (angle >= V_ROTAT) ? angle - V_ROTAT : angle + 360 - V_ROTAT; 
    }
    if (key == 'e'){
        angle = (angle <360 - V_ROTAT) ? angle + V_ROTAT : angle - 360 + V_ROTAT;
    }

    if (key == 't'){
        int *pos = posDansGrille();
        if (batiment[pos[0]][pos[1]][pos[2]] == 5){
            nbObjet --;
            batiment[pos[0]][pos[1]][pos[2]] = 0;
        }
        
    }


    if ( key == 27){
      exit(0);
    }

    if (ghosting == 0){
        collisions(&tmpx,&tmpy,&tmpz);
    }
    posx = tmpx, posy = tmpy, posz = tmpz;
    
}

void affichage(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    int *pos = posDansGrille();

    //glDisable(GL_TEXTURE_2D);
    glColor3f(1,0,1);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0,WIDTH,0.0,LENGTH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    if(batiment[pos[0]][pos[1]][pos[2]] == 5){
      char *s = "Rammasser l'objet avec 't'";
      void * font = GLUT_BITMAP_TIMES_ROMAN_24;
      glRasterPos2i(WIDTH/2 - strlen(s)*5, LENGTH/1.9);
      for(int i = 0; i < (int)strlen(s); i++){
	glutBitmapCharacter(font,s[i]);
      }
    }
    if(pos[0] + nbObjet >= NB_ETAGE){
      char *s = "Vous devez trouver l'objet de cet etage";
      void * font = GLUT_BITMAP_TIMES_ROMAN_24;
      glRasterPos2i(WIDTH/2 - strlen(s)*10, LENGTH/6);
      for(int i = 0; i < (int)strlen(s); i++){
	glutBitmapCharacter(font,s[i]);
      }
    }
    else {
      char *s = "Vous pouvez prendre l'ascenseur";
      void * font = GLUT_BITMAP_TIMES_ROMAN_24;
      glRasterPos2i(WIDTH/2 - strlen(s)*10, LENGTH/6);
      for(int i = 0; i < (int)strlen(s); i++){
	glutBitmapCharacter(font,s[i]);
      }
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    //glEnable(GL_TEXTURE_2D);

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
        for (int j = 0; j < W_ETAGE; j++){
            
            for (int k = 0; k < L_ETAGE; k++){
                if (batiment[i][j][k] != 2 && batiment[i][j][k] != 3){
		  glEnable(GL_TEXTURE_2D);
		  glBindTexture(GL_TEXTURE_2D, texture);
		  // Affichage du sol
		  if(!xray){
		    glBegin(GL_QUADS);
		    glColor3f(1,1,1);
                    glTexCoord2f(0.0,0.0);glVertex3f(k*L_CASE,i * H_ETAGE,j*W_CASE);
                    glTexCoord2f(0.5,0.0);glVertex3f((k+1)*L_CASE,i* H_ETAGE,j*W_CASE);
                    glTexCoord2f(0.5,0.5);glVertex3f((k+1)*L_CASE, i *H_ETAGE, (j+1)*W_CASE);
                    glTexCoord2f(0.0,0.5);glVertex3f(k*L_CASE, i *H_ETAGE, (j+1)*W_CASE);
		    glBindTexture(GL_TEXTURE_2D, 0);
		    glDisable(GL_TEXTURE_2D);
		    glEnd();
		  
		  }
		  else{
		    glLineWidth(10);
		    glBegin(GL_LINE_LOOP);
		    glColor3f(1,0,0);
		    glVertex3f(k*L_CASE,i * H_ETAGE,j*W_CASE);
		    glVertex3f((k+1)*L_CASE,i* H_ETAGE,j*W_CASE);
		    glVertex3f((k+1)*L_CASE, i *H_ETAGE, (j+1)*W_CASE);
		    glVertex3f(k*L_CASE, i *H_ETAGE, (j+1)*W_CASE);
		    glEnd();
		  }
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
                    //Téléporteur ascendant
		  glEnable(GL_TEXTURE_2D);
		  glBegin(GL_QUADS);
		  glColor3f(1,1,1);
		  glTexCoord2f(0.55,0.55);glVertex3f(k*L_CASE,i * H_ETAGE,j*W_CASE);
		  glTexCoord2f(1,0.55);glVertex3f((k+1)*L_CASE,i* H_ETAGE,j*W_CASE);
		  glTexCoord2f(1,1);glVertex3f((k+1)*L_CASE, i *H_ETAGE, (j+1)*W_CASE);
		  glTexCoord2f(0.55,1);glVertex3f(k*L_CASE, i *H_ETAGE, (j+1)*W_CASE);
		  glEnd();
		  glDisable(GL_TEXTURE_2D);
                    nbcubes += 0;
                }
                else if (batiment[i][j][k] == 3){
                    //Téléporteur descendant
                    //On affiche rien c'est un trou
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
		else if(batiment[i][j][k] == 5){
		  // Objet à ramasser
		  afficheObjet(k*L_CASE,i * H_ETAGE,j * W_CASE,(k+1) * L_CASE,(i+1) * H_ETAGE,(j+1) * W_CASE);
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
    glColor3f(0.9,0.9,0.9);
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

void construire_labyrinthe(){
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
    int rampe[2] = {-1,-1};
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
        if (i == 0){
            historique[courant][0] = 1;
            historique[courant][1] = 1;
            batiment[i][historique[courant][0]][historique[courant][1]] = 2;
        }
        else {
            historique[courant][0] = rampe[0];
            historique[courant][1] = rampe[1];
            batiment[i][rampe[0]][rampe[1]] = -3;
        }
        rampe[0] = -1; rampe[1] = -1;
        
        do {
            //Considération des voisins
            int voisins[4][4];
            int nbvoisins_possibles = 0;
            for (int aux = 0; aux < 4; aux ++){
                int tmpx, tmpz, tmpx2, tmpz2;
                tmpx = historique[courant][0] + aux_voisins[aux][0];
                tmpz = historique[courant][1] + aux_voisins[aux][1];
                tmpx2 = historique[courant][0] + aux_voisins[aux][0] / 2;
                tmpz2 = historique[courant][1] + aux_voisins[aux][1] / 2;
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
                if (rampe[0] == -1){
                    batiment[i][historique[courant][0]][historique[courant][1]] = -2;
                    rampe[0] = historique[courant][0];
                    rampe[1] = historique[courant][1];
                }
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
                else if (batiment[i][j][k] < 0){
                    batiment[i][j][k] = batiment[i][j][k] * (-1);
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
                }
            }
        }
    }

    //On va intégrer un objet dans chaque étage
    for (int i = 0; i < NB_ETAGE; i++){
      int j = 1, k = 1; 
      while ( nbObjet <= i){
	if (j == W_ETAGE-1){
	  j = 1;
	  k = 1;
	}
	else if(k == L_ETAGE-1){
	  k = 1;
	  j++;
	}
	else if (batiment[i][j][k] == 0){
	  int rg = rand() % 100;
	  if(rg < 10){
	    batiment[i][j][k] = 5;
	    nbObjet++;
	  }
	}
	k++;    
      }
    }
   
    
}
int main(int argc, char **argv){


    srand(time(NULL));
    listecubes = malloc(sizeof(float) * 6 * NB_ETAGE*W_ETAGE*L_ETAGE * 2);

    construire_labyrinthe();
    affiche_grilles();
    //lire_liste_cubes("listecubes");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA |GLUT_SINGLE | GLUT_DEPTH);

    glutInitWindowSize(WIDTH,LENGTH);
    glutInitWindowPosition(42,42);

    

    glutCreateWindow("Trwa dai");
    glEnable(GL_DEPTH_TEST);
    texture = ChargeTexture(texture, "Texture/Sol_2.bmp",632,632);
    mur = ChargeTexture(mur, "Texture/Mur.bmp",632,632);
    //teleporteur = ChargeTexture(teleporteur, "Texture/Tp.bmp",632,632);
    glutDisplayFunc(affichage);
    glutKeyboardFunc(GererClavier);
    glutMouseFunc(GererCliqueSouris);
    glutPassiveMotionFunc(GererMouvementSouris);
    glutIdleFunc(Animer);
    glutMainLoop();
    FreeTexture(texture);
    FreeTexture(mur);
    exit(0);
}


