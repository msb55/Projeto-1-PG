#include <iostream>
using std::cerr;
using std::endl;
#include <stdlib.h>
#include <GL/glut.h> 
#include <windows.h>
#include <cmath>
#include <gl/Gl.h>
#include <gl/Glu.h>
#include <vector>

//Leitura de arquivo
#include <fstream>
#include <string>

using namespace std;


int SCREEN_HEIGHT = 480;
int NUMPOINTS = 0;
int numvetor = 0;
bool showControllerPoints = true;
bool showControlPoligonal = true;
bool showCurveDots = true;
bool bezierCurveControl = true;

int contadora = 0;
int cont = 0;
float largura = 7;//DEFAULT
float avaliacoes = 1000;//DEFAULT
bool plota = false;



class Point {
public:
	double x, y, z;
	void setxy(double x2, double y2) { x = x2; y = y2; }

};

Point lastPoint;
vector<Point> points;
vector<Point> deltaOnePontos;
vector<Point> arc;

void myInit() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 0.0, 0.0);
	glPointSize(4.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 640.0, 0.0, 480.0);
	lastPoint.setxy(0.0, 0.0);

	string slargura, savaliacoes;
	ifstream myfile("in.txt");//arquivo deve estar em \bin do projeto :)
	if (myfile.is_open())
	{
		getline(myfile, slargura);
		getline(myfile, savaliacoes);

		largura = atof(slargura.c_str());
		avaliacoes = atof(savaliacoes.c_str());

		myfile.close();
	}
}

void drawDot(int x, int y) {
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
	glFlush();
}

void drawLine(Point p1, Point p2) {
	glBegin(GL_LINES);
	glVertex2f(p1.x, p1.y);
	glVertex2f(p2.x, p2.y);

	glEnd();
	glFlush();
}


Point Casteljau(vector<Point> pontos1, int n, double t) {//CASTELJÃO DA MASSA 2.1

	vector<Point> pontos = pontos1;
	int qtd = n;
	int limite = n;
	int inicio = 0;
	Point P; int i;

	do {
		for (i = inicio; i < limite - 1; i++) {
			P.x = (1 - t)*pontos[i].x + t*pontos[i + 1].x;
			P.y = (1 - t)*pontos[i].y + t*pontos[i + 1].y;
			pontos.push_back(P);
		}
		qtd--;
		limite = limite + qtd;
		inicio = i + 1;

	} while (qtd != 1);

	return pontos[inicio];
}

void DrawCircle(float cx, float cy, float x0, float y0, float r, int num_segments)
{
	float theta = 1 * 3.1415926 / float(num_segments);
	float c = cosf(theta);
	float s = sinf(theta);
	float t;
	float x = x0;
	float y = y0;

	Point P0;
	Point P1;
	P0.setxy(x + cx, y + cy);
	glColor3f(0.0, 0.0, 1.0);

	for (int ii = 1; ii < num_segments; ii++) {
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
		P1.setxy(x + cx, y + cy);
		drawLine(P0, P1);


		P0 = P1;
	}

	glFlush();
}

Point metodo(Point vetorTangente, Point pontoAcostamento) {

	if (!((abs(vetorTangente.y) <= 0.163 && abs(vetorTangente.x) <= 0.163))) {

		plota = false;
		Point Pn, Pc;

		Pn.setxy(pontoAcostamento.x - lastPoint.x, pontoAcostamento.y - lastPoint.y);
		if (lastPoint.x == 0.0 && lastPoint.y == 0.0) return vetorTangente;


		Pn.x = Pn.x / (sqrt(pow(Pn.x, 2) + pow(Pn.y, 2)));
		Pn.y = Pn.y / (sqrt(pow(Pn.x, 2) + pow(Pn.y, 2)));


		Pc.x = 0.5*lastPoint.x + 0.5*pontoAcostamento.x;
		Pc.y = 0.5*lastPoint.y + 0.5*pontoAcostamento.y;


		if ((abs(lastPoint.x - pontoAcostamento.x) > 0.1) && (abs(lastPoint.y - pontoAcostamento.y) > 0.1)) {
			DrawCircle(Pc.x, Pc.y, (pontoAcostamento.x - Pc.x), (pontoAcostamento.y - Pc.y), largura / 2, 50);
		}
	}

	return vetorTangente;
}

Point acostamento(Point pontoReferencia, Point pontoReferencia2, float largura) {
	Point vetorTangente, vetorNormal, pontoAcostamento;
	float aux, x, y;
	// VETOR TANGENTE
	vetorTangente.x = pontoReferencia.x - pontoReferencia2.x;
	vetorTangente.y = pontoReferencia.y - pontoReferencia2.y;

	x = vetorTangente.x;
	y = vetorTangente.y;

	Point vT = vetorTangente;
	//NORMALIZA VETOR TANGENTE
	vetorTangente.x = vetorTangente.x / (sqrt(pow(vetorTangente.x, 2) + pow(vetorTangente.y, 2)));
	vetorTangente.y = vetorTangente.y / (sqrt(pow(vetorTangente.x, 2) + pow(vetorTangente.y, 2)));
	//CALCULA NORMAL
	vetorNormal.x = (-1) * vetorTangente.y;
	vetorNormal.y = vetorTangente.x;
	//PONTO DO ACOSTAMENTO
	pontoAcostamento.x = pontoReferencia.x + vetorNormal.x * largura;
	pontoAcostamento.y = pontoReferencia.y + vetorNormal.y * largura;

	if (((abs(y) <= 0.163 && abs(x) <= 0.163) || plota) && cont != 0) {
		plota = true;
		metodo(vT, pontoAcostamento);
	}
	else {
		lastPoint = pontoAcostamento;
	}

	return pontoAcostamento;
}

void draw() { //DESENHA OBJETOS NA TELA

	cont = 0;
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();

	if (showControllerPoints) {
		for (int i = 0; i < NUMPOINTS; i++) {
			drawDot(points[i].x, points[i].y);//PONTOS DE CONTROLE
		}
	}



	if (NUMPOINTS >= 3) { //DESENHA CURVA A PARTIR DE 3 PONTOS

		if (showControlPoligonal) {
			glColor3f(1.0, 1.0, 1.0);
			for (int i = 0; i < NUMPOINTS - 1; i++) {
				drawLine(points[i], points[i + 1]); //POLIGONAL DE CONTROLE
			}
		}


		glColor3f(0.5, 0.5, 1.0);
		glPointSize(2.0);


		Point POld = points[0];
		Point PCOld = points[0];
		float x = 1 / avaliacoes;
		for (double t = 0.0; t <= 1.0; t += x) {
			Point P1 = Casteljau(points, NUMPOINTS, t);//CALCULA PONTO NA CURVA DE BEZIER
			Point P2 = acostamento(P1, POld, largura); //CALCULA PONTO DO ACOSTAMENTO
			if (bezierCurveControl) {//DESENHA CURVA DE BEZIER
				glColor3f(1.0, 0.0, 0.0);
				drawLine(POld, P1);
			}
			cont++;
			if (!plota) {

				glColor3f(0.0, 0.0, 1.0); //DESENHA ACOSTAMENTO
				if (bezierCurveControl) drawLine(PCOld, P2);
			}
			PCOld = P2;
			POld = P1;

		}
		glColor3f(1.0, 0.0, 0.0);
	}
}


void myMouse(int button, int state, int x, int y) {//GUARDA COORDENADAS DO MOUSE	

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		Point p;
		p.setxy((double)x, (double)(SCREEN_HEIGHT - y));
		points.push_back(p);
		NUMPOINTS++;
		draw();
	}
}

void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}

void hadleSpecialKeyboard(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_F1: //SHOW POLIGONAL DE CONTROLE
		showControlPoligonal = !showControlPoligonal;
		draw();
		break;
	case GLUT_KEY_F2://SHOW PONTOS DE CONTROLE
		showControllerPoints = !showControllerPoints;
		draw();
		break;
	case GLUT_KEY_F3: //SHOW PONTOS DA CURVA
		showCurveDots = !showCurveDots;
		draw();
		break;
	case GLUT_KEY_F4: //SHOW CURVA DE BEZIER
		bezierCurveControl = !bezierCurveControl;
		draw();
		break;
	}
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Estrada de Bezier");

	glutMouseFunc(myMouse);
	glutDisplayFunc(myDisplay);
	glutSpecialUpFunc(hadleSpecialKeyboard);

	myInit();
	glutMainLoop();

	return 0;
}