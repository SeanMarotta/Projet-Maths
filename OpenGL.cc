/*********************************************************/
/*                    openGL.cc                          */
/*********************************************************/
/*                                                       */
/*********************************************************/ 

/* inclusion des fichiers d'en-tete Glut */
#include <GL/glut.h>
#include <math.h>
#include <string.h>
#include <fstream>
#include <iostream>
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <jerror.h>
#define Pi 3.141592654

double Scal=36;

double trX=0.0,trY=0.0,dist=0.;//,trZ=0.0
char presse;
int anglex,angley,x,y,xold,yold;


/****************************************************************
 **                                                            **
 **                    Mes structures		                   **
 **                                                            **
 ****************************************************************/

// Structure Point
struct Point{
	double x;
	double y;
	double poids;
};

// Structure Fonction
struct Fonction{
	double a, b, c, d, e, f, g; // (ax³+bx²+cx+d) / (ex²+fx+g)
};

//creation de fonction
Fonction create_fonction();


//------------------------------------------------------------------
/* Prototype des fonctions */
void affichage();//  procedure a modifier en fonction de la scene
void clavier(unsigned char touche,int x,int y); 
void reshape(int x,int y);
void idle();
void mouse(int bouton,int etat,int x,int y);
void mousemotion(int x,int y);

//-************************************************************
//
//  Procedure avec mise en file des sommets des primitives
//
//-***********************************************************
void init(Fonction f, FILE* fichier);

int main(int argc,char **argv)
{
	/*Création du fichier*/
	FILE* fichier = NULL;
	char nomFichier[100];
	cout << "Nom de votre fichier texte\n";
	cin >> nomFichier;
	fichier = fopen(nomFichier,"w+");

	/*Création de la fonction entrée par l'utilisateur*/
	Fonction f = create_fonction();

	/* initialisation de glut et creation de la fenetre */
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(1000, 1000);
	glutCreateWindow("RQBC et conique.");
	/* Initialisation d'OpenGL */
	glClearColor(1.0,1.0,1.0,0.0);
	glColor3f(0.0,0.0,0.0);
	glPointSize(2.0);
	glEnable(GL_DEPTH_TEST);

	glColor3f(0.0,0.0,0.0);
	//glEdgeFlag(GL_FALSE);
	glEdgeFlag(GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
	// glEnable(GL_LIGHTING);
	glDisable(GL_LIGHTING);

	/* enregistrement des fonctions de rappel */
	init(f, fichier);
	glutDisplayFunc(affichage);
	glutKeyboardFunc(clavier);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mousemotion);

	/* Entree dans la boucle principale glut */
	glutMainLoop();
	fclose(fichier);
	return 0;

}

void clavier(unsigned char touche,int x,int y)
{
	switch (touche)
	{
		case 'q' : /*la touche 'q' permet de quitter le programme */
			exit(0);
		case '+' :
			dist+=0.5;
			Scal=Scal+0.5;
			glutPostRedisplay();
		break;
		case '-' :
			dist-=0.5;
			Scal=Scal-0.5;
			glutPostRedisplay();
		break;
		case '4' : trX-=0.25;  glutPostRedisplay();  break;
		case '6' : trX+=0.25;  glutPostRedisplay();  break;
		case '2' : trY+=0.25;  glutPostRedisplay();  break;
		case '8' : trY-=0.25;  glutPostRedisplay();  break;
	}
}

void reshape(int x,int y)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//taille de la scene
	double  Ortho=-150;
	glOrtho(-Ortho,Ortho,-Ortho,Ortho,-Ortho,Ortho);// fenetre
	glMatrixMode(GL_MODELVIEW);
	glViewport(0,0,x,y);
}

void mouse(int button, int state,int x,int y)
{
	/* si on appuie sur le bouton gauche */
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
	{
		presse = 1; /* le booleen presse passe a 1 (vrai) */
		xold = x; /* on sauvegarde la position de la souris */
		yold=y;
	}
	/* si on relache le bouton gauche */
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) presse=0; /* le booleen presse passe a 0 (faux) */
}

void mousemotion(int x,int y)
{
	if (presse) /* si le bouton gauche est presse */
	{
		/* on modifie les angles de rotation de l'objet
		en fonction de la position actuelle de la souris et de la derniere
		position sauvegardee */
		anglex=anglex+(x-xold); 
		angley=angley+(y-yold);
		glutPostRedisplay(); /* on demande un rafraichissement de l'affichage */
	}

	xold=x; /* sauvegarde des valeurs courante de le position de la souris */
	yold=y;
}

/****************************************************************
 **                                                            **
 **                    Affichage de  la scene                  **
 **                                                            **
 ****************************************************************/

void openGL(double x, double y, double r0,double g0, double b0, double size)
{
    glColor3f(r0,g0,b0);	//initialisation de la couleur
	glPointSize(size);	// initialisation de la taille
	glBegin(GL_POINTS);	// on trace un point
  	glVertex2f(x,y);	// coordonnees du point
	glEnd();	// fin de glBegin
}

void trace_segment(double x0, double y0,double x1, double y1, double red, double green, double blue, double size)
{
    glColor3f(red,green,blue);//initialisation de la couleur
	glLineWidth(size); // initialisation de la taille
	glBegin(GL_LINES); // on trace un segment
  	glVertex2f(x0,y0); // coordonnees du premier point
  	glVertex2f(x1,y1); // coordonnees du dernier point 
	glEnd(); // fin de glBegin
}


/****************************************************************
 **                                                            **
 **                    Mes fonctions		                   **
 **                                                            **
 ****************************************************************/

// Fonction qui renvoie une fonction entrée par l'utilisateur
Fonction create_fonction(){
	Fonction f;
	
	// Entrée des coefficients par l'utilisateur
	cout << "\n(ax³+bx²+cx+d) / (ex²+fx+g)\nVeuillez entrer vos coefficient.\n";
	cout << "\n(ax³) a = ";
	cin >> f.a;
	cout << "(bx²) b = ";
	cin >> f.b;
	cout << "(cx) c = ";
	cin >> f.c;
	cout << "(d) d = ";
	cin >> f.d;
	cout << "(ex²) e = ";
	cin >> f.e;
	cout << "(fx) f = ";
	cin >> f.f;
	cout << "(g) g = ";
	cin >> f.g;
	return f;
}

// Fonction qui dessine le quadrillage
void fonction_grille(double borneMin, double borneMax){
	for (double i = borneMin; i < borneMax; i=i+0.1)
	{
		for (double j = borneMin; j < borneMax; j=j+0.1)
		{
			openGL(j,i,0.,0.,0.,1.0);
		}
	}
	trace_segment(-100,0,100,0,0.,0.,0.,1.); // axe des abscisses
	trace_segment(0,-100,0,100,0.,0.,0.,1.); // axe des ordonnées
}

// Fonction qui trace les asymptotes
void trace_asymptote(Fonction fonction){

	double t = -100;
	double y;

	while(t < 100){
		// Formule générale du calcul des asymptotes  
		if(fonction.a == 0 && fonction.e == 0){
			y = ((fonction.b/fonction.f)*t) + ((fonction.c - ((fonction.g*fonction.b)/fonction.f))/fonction.f); //Degré 2
		}else {
			y = ((fonction.a/fonction.e)*t) + ((fonction.b - ((fonction.a*fonction.f)/fonction.e))/fonction.e); //Degré 3
		}

		openGL(t,y,1.,0.,3.,2.);
		t = t + 0.05;
	}
}

void trace_asymptote_verticale(Fonction fonction){

	double t = -100;
	double delta = fonction.f*fonction.f - 4*fonction.e*fonction.g;
	double x1,x2;

	if (fonction.e != 0){// Dénominateur second degré ex²+fx+g

		if (delta > 0 ){

			x2 = (-fonction.f + sqrt(delta)) / (2*fonction.a);
			x1 = (-fonction.f - sqrt(delta)) / (2*fonction.a);

			while (t < 100){
				openGL(x1,t,1.,0.,1.,2.);
				openGL(x2,t,1.,0.,1.,2.);
				t = t + 0.09;
			}	
		}else if (delta == 0){

			x1 = -fonction.b / (2*fonction.a);

			while (t < 100){
				openGL(x1,t,1.,0.,1.,2.);
				t = t + 0.09;
			}
		}
	}else if (fonction.e == 0){ // Dénominateur affine fx+g

		x1 = -fonction.g / fonction.f;

		while (t < 100){
			openGL(x1,t,1.,0.,1.,2.);
			t = t + 0.09;
		}
	}
}

// Courbe de Bezier de degré 3
void trace_Bezier3(Point a, Point b, Point c, Point d){

	double t = -100;
	double xAncien, yAncien;
	int existe = 0;

	// Pour faciliter les calculs (m0x --> coordonnée x du point A)
	double m0x, m1x, m2x, m3x;
	double m0y, m1y, m2y, m3y;

	while (t < 100){

		//double h;
		//h = (A*(1-t)+B*t)/(C*(1-t)+D*t);

		//Bernstein
		double b0 = (1-t)*(1-t)*(1-t);
		double b1 = 3*t*(1-t)*(1-t);
		double b2 = 3*t*t*(1-t);
		double b3 = t*t*t;

		/*Poids et coordonnées des points de controle*/
		//-------------------------------------------------------
		double w0 = a.poids;
		if(w0 == 0){
			m0x = b0*a.x;
			m0y = b0*a.y;
		}
		else{
			m0x = b0*w0*a.x;
			m0y = b0*w0*a.y;
		} 
		//-------------------------------------------------------
		double w1 =	b.poids;
		if(w1 == 0){
			m1x = b1*b.x;
			m1y = b1*b.y;
		}
		else{
			m1x = b1*w1*b.x;
			m1y = b1*w1*b.y;
		} 
		//-------------------------------------------------------
		double w2 = c.poids;
		if(w2 == 0){
			m2x = b2*c.x;
			m2y = b2*c.y;
		}
		else{
			m2x = b2*w2*c.x;
			m2y = b2*w2*c.y;
		} 
		//-------------------------------------------------------
		double w3 = d.poids;
		if(w3 == 0){
			m3x = b3*d.x;
			m3y = b3*d.y;
		}
		else{
			m3x = b3*w3*d.x;
			m3y = b3*w3*d.y;
		} 

		// Valeurs de x(t) et y(t), équation paramètrique
		double x = (m0x+m1x+m2x+m3x) / (b0*w0 + b1*w1 + b2*w2 + b3*w3);
		double y = (m0y+m1y+m2y+m3y) / (b0*w0 + b1*w1 + b2*w2 + b3*w3);

		// Traçage du point
		openGL(x, y, 1.0,0.,1.,1.);

		// Lien entre les points
		if(existe != 0){
			trace_segment(x, y, xAncien, yAncien, 1.,0.,0.,2.);
		}
		xAncien = x;
		yAncien = y;
		existe = 1;

		// On incrémente t
		t = t + 0.01;
	}
}

// Courbe de Bézier à partir d'une fonction
void trace_BezierFonction(Fonction fonction, FILE* fichier){

	// Point de contrôle
	Point a1;
	Point b1;
	Point c1;
	Point d1;

	// Pour facilter les calculs
	double A,B,C,D,E,F,G;
	A = fonction.a;
	B = fonction.b;
	C = fonction.c;
	D = fonction.d;
	E = fonction.e;
	F = fonction.f;
	G = fonction.g;

	// Calculs des coordonnées et des poids des points

	/*Point de controle A--------------------*/
	a1.x = 0;
	a1.y = D/G;
	a1.poids = G;
	//Si dénominateur nul
	if(G == 0 ){
		a1.x = 0;
		a1.y = D;
	}

	/*Point de controle B--------------------*/
	b1.x = G/(F + 3*G);
	b1.y = (C+3*D) / (F + 3*G);
	b1.poids = F/3 + G;
	//Si dénominateur nul
	if((F + 3*G) == 0){
		d1.x = G;
		d1.y = C + 3*D;
	}

	/*Point de controle C--------------------*/
	c1.x = (F + 2*G) / (E + 2*F + 3*G);
	c1.y = (B + 2*C + 3*D) / (E + 2*F + 3*G);
	c1.poids = ((E + 2*F) / 3) + G;
	//Si dénominateur nul
	if((E + 2*F + 3*G) == 0){
		d1.x = (F + 2*G);
		d1.y = (B + 2*C + 3*D);
	}

	/*Point de controle D--------------------*/
	d1.x = (E + F + G) / (E + F + G);
	d1.y = (A + B + C + D) / (E + F + G);
	d1.poids = E + F + G;
	//Si dénominateur nul
	if((E + F + G)== 0){
		d1.y = A + B + C + D;
		d1.x = E + F + G;
	}

	/* C'est pour verifier le bon emplacement des points de controle
	openGL(a1.x,a1.y,0.,2.,1.,20.);
	openGL(b1.x,b1.y,0.,2.,1.,20.);
	openGL(c1.x,c1.y,0.,2.,1.,20.);
	openGL(d1.x,d1.y,0.,2.,1.,20.);
	*/

	// On entre les informations dans le fichier
	fprintf(fichier, "Point A(%f ; %f) de poids %f.\n", a1.x, a1.y, a1.poids);
	fprintf(fichier, "Point B(%f ; %f) de poids %f.\n", b1.x, b1.y, b1.poids);
	fprintf(fichier, "Point C(%f ; %f) de poids %f.\n", c1.x, c1.y, c1.poids);
	fprintf(fichier, "Point D(%f ; %f) de poids %f.\n", d1.x, d1.y, d1.poids);

	//On trace la courbe de Bézier
	trace_Bezier3(a1,b1,c1,d1);
}

//fonction ou les objets sont a definir
void init(Fonction f, FILE* fichier)
{ 
	double xO=0.,yO=0.,xI=1.,yI=0.,xJ=0.,yJ=1.;

	glNewList(1,GL_COMPILE_AND_EXECUTE); //liste numero 1
	openGL(xO,yO,1.0,0.,0.,15.);
	openGL(xI,yI,0.0,1.,0.,10.);
	openGL(xJ,yJ,0.0,0.,1.,10.);
	glEndList();

	glNewList(2,GL_COMPILE_AND_EXECUTE);  //liste numero 2
	trace_segment(xO,yO,xI,yI,1.0,0.0,1.0,2.0); // on trace [OI]
	trace_segment(xO,yO,xJ,yJ,1.0,0.50,0.0,2.0);// on trace [OJ]
	glEndList();

	glNewList(3,GL_COMPILE_AND_EXECUTE);  //liste numero 4
	trace_BezierFonction(f, fichier); // On trace la courbe de Bézier
	glEndList();

	glNewList(4,GL_COMPILE_AND_EXECUTE);  //liste numero 5
	fonction_grille(-10,10); // On trace le quadrillage
	glEndList();

	glNewList(5,GL_COMPILE_AND_EXECUTE); //liste numero 6
	trace_asymptote(f); // On trace les asymptotes obliques et horizontales
	trace_asymptote_verticale(f); // On trace les asymptotes verticales
	glEndList();


	cout<<"\n Voila, c'est fini"<<endl;

}   
   
// fonction permettant d'afficher les objets en utilisant des listes   
void affichage() 
{
	/* effacement de l'image avec la couleur de fond */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(0.0,0.0,dist);
	// Pour la 3D
	// glRotatef(-angley,1.0,0.0,0.0);
	//  glRotatef(-anglex,0.0,1.0,0.0);
	// Pour la 2D    
	glRotatef(-anglex+angley,0.0,0.0,1.0);
	glScalef(Scal,Scal,Scal); // diminution de la vue de la scene
	glRotatef(180,0.0,1.0,0.0);
	glRotatef(180,1.0,0.0,0.0);
	glTranslatef(-trX,trY,0.);
	glCallList(1); // appel de la liste numero 1
	glCallList(2);   // appel de la liste numero 2
	
	glCallList(3);
	glCallList(4);
	glCallList(5);

	glFlush(); 
	// On echange les buffers
	glutSwapBuffers();
}

