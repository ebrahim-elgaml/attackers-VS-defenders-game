
/*
 * There is an extra defender which is the wall
 * you can create it by mouse or keyboard by typing w after selecting the cell.
 */
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
//g++ grapichsProject2.cpp -o gl  -std=c++11 -lGL -lGLU -lglut -lGLEW
using namespace std;
int getCurrentTime();
string trace = "Hello world";
float rotAng;
enum status { EMPTY, OCCUPIED, DESTROYED };
enum Shape { DEFENDER, RESOURCE_GATHERER, ATTACKER, WALL };
status boardStatus[9][5];
string word = "Save your house";
bool pause = false;
bool lanesStatus[5] = {false, false, false, false, false};
bool win = false;
bool lose = false;
double rotCamera = 0;
bool freeze = false;
int resources = 500;
int attackersKilled = 0;
int timeElapsed = 0;
int pausingTime = 0;
int savedTime = -1;
class Cell
{
	public:
		Cell() {} //constructor 1
		void insert(Shape s , int i ){
			occupiedBy = s;
			myStatus = OCCUPIED;
			shapeIndex = i ;
		}
		void setOCCupiedBy(Shape s ){
			occupiedBy = s;
		}
		void setMyStatus (status s){
			myStatus = s;
		}
		void setShapeIndex (int i ){
			shapeIndex = i;
		}
		status getMyStatus(){return myStatus;}
		Shape getOccupiedBy(){return occupiedBy;}
		int getShapeIndex(){return shapeIndex;}
		void empty(){
			myStatus = EMPTY;
			shapeIndex = -1;
		}
		bool isGenreatedResource(){return genrateResource;}
		void setGenerateResource(bool f){ genrateResource = f;}
		bool isSelected(){ return selected; }
		void setSelectedTrueIfApplicable(){
			if(myStatus == EMPTY){
				selected = true;
			}
		}
		void setSelected(bool f){ selected = f; }
	private:
		status myStatus = EMPTY;
		Shape occupiedBy = DEFENDER ;
		int shapeIndex = -1;
		bool genrateResource = false;
		bool selected = false;
};
Cell board[9][5];
class Attacker
{
	public:
		Attacker() {x = y = 0;health = 0; createdAt= getCurrentTime();} //constructor 1
		Attacker(float xx, float yy) {x=xx+0.5; y=yy+0.5;health = 0;createdAt= getCurrentTime();} //constructor 2
		void set(int xx, int yy) {x=xx; y=yy;}
		float getX() {return x;}
		float getY() {return y;}
		void draw(void)
		{
			if(!dead){
				glPushMatrix();
				glTranslated(x,y +0.1,0);
				glRotated(-90,0,0,1);
				glRotated(90,1,0,0);
				glScaled(0.2,0.2,0.2);
				glPushMatrix();
				glColor3f(1-health,0,0);
				glutSolidCube(2);
				drawWheels();
				glPushMatrix();
				glTranslated(-1,0.5,0.5);
				drawCones();
				glPopMatrix();
				drawHead();
				glPopMatrix();
				glPopMatrix();
			}

		}
		void drawCones(){

			glPushMatrix();
			glColor3f(206.0/255,144.0/255,212.0/255);
			glTranslated(1,0,0);
			for(int i = -1; i<=1; i++  ){
				glPushMatrix();
				glTranslated(0.5*i,-0.5,0);
				//glutSolidCone(0.2,2,10,10);
				glutSolidCone(0.6,4,10,10);
				glPopMatrix();
			}
			glPopMatrix();
		}
		void drawHead(){
			glPushMatrix();
			glColor3f(1-health,0,0);
			glTranslated(0,1.5,0);
			glScaled(0.5,0.5,0.5);
			glutSolidDodecahedron();
			glPopMatrix();
		}
		void drawWheels(){
			glPushMatrix();
			GLUquadricObj * qobj;
			qobj = gluNewQuadric();
			glPushMatrix();
			glColor3f(1-health,0,0);
			glTranslated(-1.5,-1,-1);
			glRotated(90,0,	1,0);
			gluQuadricDrawStyle(qobj,GLU_LINE);
			gluCylinder(qobj, 0.5, 0.5, 0.6, 100,100);
			gluDisk(qobj, 0.0,0.5,100,100);
			glPopMatrix();
			glPushMatrix();
			glColor3f(1-health,0,0);
			glTranslated(-1.5,-1,1);
			glRotated(90,0,	1,0);
			gluQuadricDrawStyle(qobj,GLU_LINE);
			gluCylinder(qobj, 0.5, 0.5, 0.6, 100,100);
			gluDisk(qobj, 0.0,0.5,100,100);
			glPopMatrix();
			glPushMatrix();
			glColor3f(1-health,0,0);
			glTranslated(1,-1,1);
			glRotated(90,0,	1,0);
			gluQuadricDrawStyle(qobj,GLU_LINE);
			gluCylinder(qobj, 0.5, 0.5, 0.6, 100,100);
			gluDisk(qobj, 0.0,0.5,100,100);
			glPopMatrix();
			glPushMatrix();
			glColor3f(1-health,0,0);
			glTranslated(1,-1,-1);
			glRotated(90,0,	1,0);
			gluQuadricDrawStyle(qobj,GLU_LINE);
			gluCylinder(qobj, 0.5, 0.5, 0.6, 100,100);
			gluDisk(qobj, 0.0,0.5,100,100);
			glPopMatrix();
			glPopMatrix();
		}
		void translate(){
			if(x<= -5){
				reached = true;
				int lane = int(y - 0.5) + 3;
				//word = to_string(lane) ;

				for(int i = 0 ; i < 9; i ++){
					board[i][lane].setMyStatus(DESTROYED);
				}
				//word = to_string(lane);
				lanesStatus[lane] = true;
			}else{
				x-=0.06;
			}

		}
		bool isDead(){return dead;}
		bool isReached(){return reached;}
		double getHealth(){return health;}
		void setHealth( double h ){
			health = h ;
			if(health>= 1   ){
				dead=true;
				resources +=20;
				if(attackersKilled<50){
					attackersKilled++;
				}
			}
		}
		void hit(){
			setHealth(health+0.1);
		}
		void hitByWall(){
			setHealth(health+0.6);
		}
		int getXCell(){
			//word = to_string((int)(x+0.5));
			return ((int)(x+5.5 -1));
		}
		int getYCell(){
			//word = to_string(((int)(y - 0.5)));
			return (int)(y - 0.5 + 3);
		}
		int getCreatedAt(){return createdAt;}
		string to_s(){
			return "x = "+to_string(x)+ ", y = "+to_string(y)+"reached : " + to_string(reached) +"dead : " + to_string(dead);
		}
	private:
		float x, y, health;
		bool dead = false;
		bool reached = false;
		int createdAt ;
};
int attackersSize = 0;
Attacker attackers[100000] = {} ;

class Bullet
{
	public:
		Bullet() {x = 0.5;} //constructor 1
		Bullet(float xx) {x=xx;} //constructor 2
		void set(float xx) {x=xx;}
		float getX() {return x;}
		void draw(void)
		{
			glPushMatrix();
			glTranslated(x,0,0.0); // sphere at (1,1,0)
			glColor3f(0, 0, 0);
			glutSolidSphere(0.05, 10, 10);
			glPopMatrix();
		}
	private:
		float x;
};
class LineEqu
{
	public:
		LineEqu() {x = y = 0; c = 0;} //constructor 1
		LineEqu(float x1, float y1, float x2, float y2) {
			float dy = y2-y1;
			float dx = x2 - x1;
			x = -1*dy;
			c = -1*y1*dx +x1*dy;
			y = dx;
		}
		float getX() {return x;}
		float getC() {return c;}
		float subs(float xx , float yy ){
			return y*yy + xx*x +c;
		}
	private:
		float x, c, y;
};
class Defender
{
	public:
		Defender() {x = y = 0;} //constructor 1
		Defender(int xx, int yy) {x=xx; y=yy;} //constructor 2
		void set(int xx, int yy) {x=xx; y=yy;}
		int getX() {return x;}
		int getY() {return y;}
		void draw(void)
		{
			glPushMatrix();
			//glScaled(1.5,1.5,1.5);

			glPushMatrix();
			glTranslated(x+0.5,y+0.5,0.0);
			//glRotatef(-45,1,0,0);
			//glScaled(1.5,1.5,1);
			//glRotatef(-45,1,0,0);
			glPushMatrix();
			//glTranslated(0,0.0,0);
			GLUquadricObj * qobj;
			qobj = gluNewQuadric();
			gluQuadricDrawStyle(qobj,GLU_LINE);
			glColor3f(0, 0, 1 - health);
			gluCylinder(qobj, 0.05, 0.05, 0.2, 100,100);
			glPopMatrix();
			glPushMatrix();
			glTranslated(0.0,0,0.2); // sphere at (1,1,0)
			glColor3f(51/255.0, 25/255.0, 0);
			glutSolidSphere(0.1, 10, 10);
			glPopMatrix();
			glPushMatrix();
			gluQuadricDrawStyle(qobj,GLU_LINE);
			glTranslated(0.05,0,0.2);
			glRotated(rotCamera,1,0,0);
			glRotatef(90, 0, 1, 0);
			gluQuadricDrawStyle(qobj,GLU_LINE);
			glColor3f(0, 0, 1);
			gluCylinder(qobj,  0.05, 0.05, 0.2, 100,100);
			glPopMatrix();

			glPopMatrix();
			glPushMatrix();
			glTranslated(x,y+0.5,0.2);

			//glScaled(1.5,1.5,1.5);
			b.draw();
			glPopMatrix();

			glPopMatrix();
		}
		void moveBullet(){
			if(getBulletLocationCell()<3.0){
				b.set(b.getX()+0.2);
			}else{
				b.set(0.5);
			}
			//b.set(0.500001 +1);
			//word = to_string(getBulletLocationCell() - 1);
		}
		int getBulletLocationCell(){
			return (int)(b.getX() + x -0.5);
		}
		void bulletCollideWithAttackerIndex(){

			for(int i = 0 ; i< attackersSize ; i++){
//				word = to_string(y) + ", "+ to_string(int(attackers[0].getY() - 0.5));
				if(!attackers[i].isDead() && !attackers[i].isReached() && y  == int(attackers[i].getY() - 0.5)){
					if( x + b.getX() >= attackers[i].getX() ){
						b.set(0.5);
						attackers[i].hit();
						//word = to_string(attackers[i].getHealth());
//						return i;
					}
				}
			}
//			return -1;
		}
		int getXCell(){
			return (int)(x)+5;
		}
		int getYCell(){
			return (int)(y) + 3;
		}
		float getHealth(){return health;}
		void setHealth(float h ){ health = h ; }
		void hit(){
			health+= 0.1;
			if(health>1){
				dead = true;
			}
		}
		bool isDead(){return dead;}
		int getCost(){return cost;}
		void drawIfApplicable(){
			if(resources > cost ){
				draw();
				resources -= cost;
			}
		}
		void setDead(bool f){ dead = f; }
	private:
		int x, y;
		Bullet b;
		float health = 0 ;
		bool dead  = false;
		int cost = 75;
};
class ResourceGatherer
{
	public:
		ResourceGatherer() {x = y = 0; rot=0; cellX = 0 ; cellY = 0;} //constructor 1
		//ResourceGatherer(int xx, int yy) {x=xx; y=yy;rot=0;}
		ResourceGatherer(int xx, int yy, int cx, int cy) {x=xx; y=yy;rot=0; cellX = cx ; cellY = cy;}//constructor 2
		void set(int xx, int yy) {x=xx; y=yy;}
		int getX() {return x;}
		int getY() {return y;}
		void setRot(float f) { rot = f;}
		void draw(void)

		{

			//glRotatef(-60,1,0,0);
			//glTranslated(x+0.5,y+0.5,0);
			glPushMatrix();

			//glScaled(2,2,2);

			glTranslated(x+0.5  ,y+0.5 ,0);

			glRotatef(-45,1,0,0);
			glRotatef(rot,0,0,1);
			glScaled(0.5,0.5,0.5);

			//glRotatef(-60,1,0,0);
			double t = 0;
			for(int i = 0; i<3600/4; i++){
				glPushMatrix();
				glTranslated(0,0,t);
				glRotatef(i, 0, 0, 1);
				t+=0.0007;
				glTranslatef(0.1, 0, 0);
				glColor3f(1 - health, 1- health, 0);
				glutWireSphere(0.05, 25, 25);
				glPopMatrix();
			}
			glPushMatrix();
			glTranslated(0.0,0,0.7);
			glScaled(0.1,0.1,0.1);
			glColor3f(1,165.0/255, 0);
			glutSolidDodecahedron();
			glPopMatrix();
			glPopMatrix();
		}
		void drawIfApplicable(){
			if(resources > cost ){
				draw();
				resources -= cost;
			}
		}
		int getXCell(){
			return (int)(x)+5;
		}
		int getYCell(){
			return (int)(y) + 3;
		}
		float getHealth(){return health;}
		void setHealth(int h ){ health = h ; }
		void hit(){
			health+= 0.1;
			if(health>1){
				dead = true;
			}
		}
		bool isDead(){return dead;}
		void generate(){
//			word = to_string(state);
			if(true){
				int diffTime = getCurrentTime() - startedAt;
				if(diffTime>=10){
					//word = to_string(diffTime);
					if(diffTime % 10 == 0){
						state = diffTime;
					}else {
						if( state != -1){
							//word = to_string(diffTime - state);
							if(state != -1 && diffTime - state < 4 ){
								//word = to_string((diffTime - state)%2==1);
								board[cellX][cellY].setGenerateResource((diffTime - state)%2==1);
							}else if(diffTime - state == 4){
								resources += 50;
								state = -1;
								board[cellX][cellY].setGenerateResource(false);

							}
						}

					}
				}
			}
		}
		int getCost(){return cost;}
		void setDead(bool f){ dead = f;}
	private:
		int x, y;
		float rot;
		float health = 0 ;
		bool dead = false;
		int startedAt = getCurrentTime();
		int state = -1;
		int cellX;
		int cellY;
		int cost = 50;
};
class Wall
{
	public:
		Wall() {x = y = 0;} //constructor 1
		Wall(int xx, int yy) {x=xx; y=yy;} //constructor 2
		void set(int xx, int yy) {x=xx; y=yy;}
		int getX() {return x;}
		int getY() {return y;}
		void draw(void)
		{
			glPushMatrix();
			glTranslated(x+0.5,y+0.5,0);
			glRotated(90,0,0,1);
			glColor3f(1 - health,1 - health,0);
			for(int i = -1; i<=1; i++  ){
				glPushMatrix();
				glTranslated(0.2*i,0,0);
				//glutSolidCone(0.2,2,10,10);
				glutSolidCone(0.2,0.7,5,5);
				glPopMatrix();
			}
			glPopMatrix();
		}
		double getCost(){
			return cost;
		}
		bool isDead(){return dead;}
		double getHealth(){ return health; }
		void setHealth(double h ){ health = h;}
		void hit(){
			health+=0.4;
			if(health>= 1){
				dead = true;
			}
		}
	private:
		int x, y;
		bool dead = false;
		double health = 0;
		double cost = 40;
};
int wallSize = 0;
Wall walls[1000000];
void drawGrid();
void axis(double);
void drawAxis();
void moveAllBullets();
void moveAllAttcakers();
void drawHeader();
void drawAt(int,int,Shape);
void spinAllGatherers();
void writeWord(int,int,int,string);
void setLight();
void writeWords();
void resize(int, int);
int selectedRow = -1;
int selectedColomn = -1;
int defendersSize = 0;
void removeSelection();
Defender defenders[100000];
int gathererSize = 0;
ResourceGatherer gatherers[100000] ;
int width  = 2390 ;
int height  = 768;
bool selected[3] = {false, false, false};
LineEqu l[10] = {LineEqu(228,606,494,280), LineEqu(319,612,530,280), LineEqu(411,613,570,280), LineEqu(500,608,605,280)
		,LineEqu(594,613,645,280), LineEqu(684,613,683,280), LineEqu(774,613,722,280), LineEqu(865,613,759,280)
		, LineEqu(950,613,799,278), LineEqu(1048,613,834,280)};
LineEqu lineHeaders[4] = {LineEqu(311,85,319,16), LineEqu(377,85,383,16), LineEqu(454,85,460,16), LineEqu(529,85,534,16)};
int gameLevel = 1;
bool view = false;;
double zView  = 3;
double dz = 0.01;
bool zDown = true;
void Display(void) {
	setLight();
	glMatrixMode(GL_PROJECTION); // set the view volume shape
	glLoadIdentity();
	glOrtho(-5, 5, -3.0, 3.0, -3, 3);
	glMatrixMode(GL_MODELVIEW); // position and aim the camera
	glLoadIdentity();
	gluPerspective(60.0f,1.3333333, 0.0f, 10.0f);
	if(view){
		gluLookAt(0,-1,zView, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}else{
		gluLookAt(0,-2,3, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	writeWords();
	// draw Sun.
	glPushMatrix();
	glTranslated(-18,8,2);
	glColor3f(1,1,0);
	glutSolidSphere(1,10,10);
	glPopMatrix();
	glPushMatrix();
	if(view){
		glRotatef(rotCamera, 0, 0, 1);
	}
	drawGrid();
	drawHeader();
	for(int i = 0; i<defendersSize ; i++){
		if(!defenders[i].isDead()){
			defenders[i].draw();
		}
	}
	for(int i = 0; i<gathererSize ; i++){
		if(!gatherers[i].isDead()){
			gatherers[i].draw();
		}
	}
	for(int i = 0 ; i<attackersSize;i++){
		attackers[i].draw();
	}
	for(int i = 0; i<wallSize ; i++){
		if(!walls[i].isDead()){
			walls[i].draw();
		}
	}
	glPopMatrix();
	glFlush();
}
void resize(int w, int h) {
    glutReshapeWindow( w, h);
}
void writeWords(){
	writeWord(2,3,3,"Time Elapsed In Seconds : " + to_string(getCurrentTime()));
	writeWord(2,5,2.8,"Attackers killed : " + to_string(attackersKilled));
	writeWord(2,3,2.5,"level : " + to_string(gameLevel));
	writeWord(-5,2,2,"Resources : " + to_string(resources));
	writeWord(-12,7,2.9,"75");
	writeWord(-9,7,2.9,"50");
	writeWord(-6,7,2.9,"40");
	writeWord(-3,4,0,word);
	//writeWord(-12,4,0,trace);
}
int getCurrentTime (){
	return timeElapsed - pausingTime;
}
void drawAxis(){
	glPushMatrix();
	glTranslated(-4.5,0,1);
	glColor3d(0,0,0); // draw black lines
	axis(0.5); // z-axis
	glPushMatrix();
	glRotated(90, 0,1.0, 0);
	axis(0.5); // y-axis
	glRotated(-90.0, 1, 0, 0);
	axis(0.5); // z-axis
	glPopMatrix();
	glPopMatrix();

}
void axis(double length){ // draw a z-axis, with cone at end
	 glPushMatrix();
	 glBegin(GL_LINES);
	 glVertex3d(0, 0, 0);
	 glVertex3d(0,0,length); // along the z-axis
	 glEnd();
	 glTranslated(0, 0,length -0.2);
	 glutWireCone(0.04, 0.2, 12, 9);
	 glPopMatrix();
}
void drawGrid(){
	glDepthRange (0.2, 1.0);
	for(int i = 0 ; i < 9 ; i++){
		for(int j = 0 ; j < 5 ; j ++){
			glBegin(GL_POLYGON);
				if(board[i][j].getMyStatus()== EMPTY or board[i][j].getMyStatus() == OCCUPIED ){
					if(board[i][j].isSelected()){
						glColor3f(1,1,1);
					}else{
						if(board[i][j].isGenreatedResource()){
							glColor3f(212.0/255,230.0/255,21.0/255);
						}else{
							glColor3f(0,1,0);
						}
					}
				}else if(board[i][j].getMyStatus() == DESTROYED){
					glColor3f(0,0,0);
				}

			glVertex3f(i-5,j-3,0);
			glVertex3f(i-4,j-3,0);
			glVertex3f(i-4,j-2,0);
			glVertex3f(i-5,j-2,0);
			glEnd();
		}
	}

	glDepthRange (0.1, 0.9);
	glLineWidth(8.0f);
	glBegin(GL_LINES);
	// Vertical lines
	for(int i = -3; i<=2; i++){
		glColor3f(.6,.3,.3);
		glVertex3f(-5,i,0);
		glVertex3f(4,i,0);
	}
	// Horizontal lines
	for(int i = -5; i<=4; i++){
		glColor3f(.6,.3,.3);
		glVertex3f(i,-3,0);
		glVertex3f(i,2,0);
	}
	glEnd();
	glLineWidth(1.0f);
	glDepthRange (0.0, 1);
}
void drawHeader(){
	glDepthRange (0.2, 1.0);
	glPushMatrix();
	glTranslated(0,0,1.7);
	glRotated(30, 1,0,0);
	glBegin(GL_POLYGON);
	selected[0]?glColor3f(122.0/255,1,136.0/255):glColor3f(1,1,1);
	glVertex3f(-4.9,2.1,0);
	glVertex3f(-4,2.1,0);
	glVertex3f(-4,3,0);
	glVertex3f(-4.9,3,0);
	glEnd();
	glBegin(GL_POLYGON);
	selected[1]?glColor3f(122.0/255,1,136.0/255):glColor3f(1,1,1);
	glVertex3f(-4,2.1,0);
	glVertex3f(-3,2.1,0);
	glVertex3f(-3,3,0);
	glVertex3f(-4,3,0);
	glEnd();
	glBegin(GL_POLYGON);
	selected[2]?glColor3f(122.0/255,1,136.0/255):glColor3f(1,1,1);
	glVertex3f(-3,2.1,0);
	glVertex3f(-2,2.1,0);
	glVertex3f(-2,3,0);
	glVertex3f(-3,3,0);
	glEnd();
	glDepthRange (0.0, 0.9);
	glLineWidth(4.0f);
	glBegin(GL_LINES);
	// Vertical lines
	glColor3f(0,0,0);
	glVertex3f(-4.9,2.1,0);
	glVertex3f(-4.9,3,0);
	glColor3f(0,0,0);
	glVertex3f(-4,2.1,0);
	glVertex3f(-4,3,0);
	glColor3f(0,0,0);
	glVertex3f(-3,2.1,0);
	glVertex3f(-3,3,0);
	glVertex3f(-2,2.1,0);
	glVertex3f(-2,3,0);
	// Horizontal lines
	glColor3f(0,0,0);
	glVertex3f(-4.9,2.1,0);
	glVertex3f(-2,2.1,0);
	glColor3f(0,0,0);
	glVertex3f(-4.9,3,0);
	glVertex3f(-2,3,0);
	glEnd();
	// Shapes.
	glLineWidth(1.0f);
	Defender a(-5,2);
	glPushMatrix();
	glTranslated(0,0,-0.1);
	a.draw();
	glPopMatrix();
	glPushMatrix();
	//
	glTranslated(0,0,-0.2);
	ResourceGatherer b(-4,2,0,0);
	b.draw();
	glPopMatrix();
	glPushMatrix();
	glScaled(0.5,0.5,0.5);
	glTranslated(-2,2,0);
	Wall w(-3,2);
	w.draw();
	glPopMatrix();
	glPopMatrix();
}

void drawAt(int x, int y, Shape s){
	if(s == ATTACKER){
		attackers[attackersSize] = Attacker(x,y - 3);
		attackersSize ++;
	}else{
		if(board[x][y].getMyStatus() == EMPTY){
			int index = -1;
			if(s == DEFENDER){
				index = defendersSize;
				Defender d (x-5,y-3);
				if(resources >= d.getCost()){
					defenders[defendersSize] = d;
					defendersSize ++;
					board[x][y].setMyStatus(OCCUPIED);
					board[x][y].setOCCupiedBy(s);
					board[x][y].setShapeIndex(index);
					resources -= d.getCost();
				}

			}else if(s == RESOURCE_GATHERER){
				index = gathererSize;
				ResourceGatherer r (x-5,y-3, x, y);
				if(resources >= r.getCost()){
					gatherers[gathererSize] = r;
					gathererSize ++;
					board[x][y].setMyStatus(OCCUPIED);
					board[x][y].setOCCupiedBy(s);
					board[x][y].setShapeIndex(index);
					resources -= r.getCost();
				}
			}else if(s == WALL){
				index = wallSize;
				Wall r (x-5,y-3);
				if(resources >= r.getCost()){
					walls[wallSize] = r;
					wallSize ++;
					board[x][y].setMyStatus(OCCUPIED);
					board[x][y].setOCCupiedBy(s);
					board[x][y].setShapeIndex(index);
					resources -= r.getCost();
				}
			}

		}
	}
	removeSelection();

}
void moveAllBullets(){
	for(int i = 0 ; i< defendersSize; i++){
		if(!defenders[i].isDead()){
			///word = to_string(defenders[i].getXCell())+ ", " + to_string(defenders[i].getYCell());
			defenders[i].moveBullet();
			defenders[i].bulletCollideWithAttackerIndex();
		}
	}
}
void generateAttackers(){
	if( getCurrentTime() != 0){
		int r = -1;
		if(gameLevel == 1){
			if(getCurrentTime() % 10 == 0){
				r = rand() % 5 ;
			}
		}else if (gameLevel == 2){
			if(getCurrentTime() % 5 == 0){
				r = rand() % 5 ;
			}
		}else if(gameLevel == 3){
			if(getCurrentTime() % 3 == 0){
				r = rand() % 5 ;
			}
		}else{
			r = -1;
		}
		//word = to_string(r);
		if(  r> -1 && r<5){
			if(board[0][r].getMyStatus() != DESTROYED){
				if(attackersSize == 0 ){

					if(r == 0 || r == 3){
						drawAt(3.5,r,ATTACKER);
					}else{
						drawAt(4,r,ATTACKER);
					}
				}else{
					if(attackers[attackersSize-1].getCreatedAt() != getCurrentTime()){
						if(r == 0 || r == 3){
							drawAt(3.5,r,ATTACKER);
						}else{
							drawAt(4,r,ATTACKER);
						}
					}
				}
			}
		}
	}
}
void moveAllAttcakers(){
	for(int i = 0 ; i< attackersSize; i++){
		if(!attackers[i].isDead() && !attackers[i].isReached() ){
			int xCell = attackers[i].getXCell();
			int yCell = attackers[i].getYCell();
			Cell c = board[xCell][yCell];
			//word = to_string(attackers[1].getXCell())+", " +to_string(attackers[1].getYCell());
			if(c.getMyStatus() == OCCUPIED){
				if(c.getOccupiedBy() == RESOURCE_GATHERER){
					//word = "here, "+to_string(c.getShapeIndex()+", "+ to_string(gatherers[c.getShapeIndex()].getHealth()));
					gatherers[c.getShapeIndex()].hit();
					if(gatherers[c.getShapeIndex()].isDead()){
						board[xCell][yCell].empty();
					}
				}else if(board[xCell][yCell].getOccupiedBy() == DEFENDER){
					defenders[c.getShapeIndex()].hit();
					if(defenders[c.getShapeIndex()].isDead()){
						board[xCell][yCell].empty();
					}
				}
				else if(board[xCell][yCell].getOccupiedBy() == WALL){
					walls[c.getShapeIndex()].hit();
					attackers[i].hitByWall();
					if(walls[c.getShapeIndex()].isDead()){
						board[xCell][yCell].empty();
					}
				}
			}else{
				attackers[i].translate();
			}

		}
	}
}
void spinAllGatherers(){
	for(int i = 0 ; i< gathererSize; i++){
		if(!gatherers[i].isDead()){
			gatherers[i].setRot(rotAng);
			gatherers[i].generate();
		}
	}
}
void writeWord(int x , int y , int z , string s){
	glPushMatrix();
	//glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos3i(x, y,z);
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	for(int i = 0; s[i]!='\0';i++){
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
	}
	glPopMatrix();
}
// return y level from 0 to 5.
// 0 is the last level from bottom.
// 5 is the first level for the selected items.
int getYLevel(double y){
	if(y>= 16 && y <=85){
		return 5;
	}else if(y>= 278 && y <= 310){
		return 4;
	}else if(y> 310 && y <=352){
		return 3;
	}else if(y> 352 && y <=405){
		return 2;
	}else if(y> 405 && y <=486){
		return 1;
	}else if(y> 486 && y <=610){
		return 0;
	}
	return -1;
}

int getXLevel(double x, double y){
	for(int i = 0 ; i<9; i++){
		if(l[i].subs(x,y) >= 0 &&l[i+1].subs(x,y) < 0){
			return i;
		}
	}
	for(int i = 0 ; i<3; i++){
		if(lineHeaders[i].subs(x,y) >= 0 && lineHeaders[i+1].subs(x,y) < 0){
			return i;
		}
	}
	return -1;
}
// Checks if a point in the field
bool validatepoint(double x , double y){
	int getX = getXLevel(x,y);
	int getY = getYLevel(y);
	if (getY == -1){
		return false;
	}else{
		if(getY == 5){
			return getX==1 || getX==0 || getX==2;
		}else{
			return getX>=0 && getX <=9;
		}
	}

}
void changeGameLevel(){
	if(attackersKilled >= 15 && attackersKilled < 40 ){
		gameLevel = 2;
	}else if(attackersKilled >= 40){
		gameLevel = 3;
	}
}
void setTimeElapsed(){
	timeElapsed = (int)(glutGet(GLUT_ELAPSED_TIME*1.0)/1000);
}
int totalLanesDestroyed(){
	int c = 0 ;
	for(int i = 0 ; i < 5; i++){
		if(lanesStatus[i]){
			c++;
		}
	}
	return c;
}
void checkFreeze(){
	if(attackersKilled >= 50){
		win = true;
	}
	if(win){
		freeze = true ;
		word = "Congratulation you won :)";
		return ;
	}
	if(totalLanesDestroyed() >= 3){
		lose = true;
	}
	if(lose){
		freeze = true ;
		word = "Zombies have destroyed your house";
		return ;
	}
}
void setLight(){
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);
	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);
	// Define Light source 0 ambient light
	GLfloat ambient[]={0.1f, 0.1f, 0.1, 1.0f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	// Define Light source 0 diffuse light
	GLfloat diffuse[]={0.7f, 0.7f, 0.7f, 1.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	// Define Light source 0 Specular light
	GLfloat specular[]={1.0f, 1.0f, 1.0f, 1.0f};
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	// Finally, define light source 0 position in World Space
	GLfloat light_position[]={100.0f, 1.0f, 1.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);
	// Set Material Properties which will be assigned by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	GLfloat shininess[] = {96.0f};
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}
void setCameraParams(){
	if(view){
		if(rotCamera >=360){
			rotCamera = 0;
			zView = 3;
			dz = -0.01;
			view = false;
		}else{
			rotCamera +=1;
			if(zDown){
				zView -= 0.01;
				if(zView<= 2){
					zDown = false;
				}
			}else{
				zView +=0.01;
				if(zView>= 3){
				 zDown = true;
				}
			}
		}
	}
}
void Anim() {
	if(!freeze && !pause){
		checkFreeze();
		rotAng += 15;
		setCameraParams();
		moveAllBullets();
		spinAllGatherers();
		moveAllAttcakers();
		generateAttackers();
		changeGameLevel();
		setTimeElapsed();

	}
	glutPostRedisplay();
}
void Mouse(int button, int state, int x, int y) {
	if(!freeze && !pause){
		if(validatepoint(x,y)){
			int getY = getYLevel(y);
			int getX = getXLevel(x, y);
			if(getY == 5){
				if(getX == 0){
					selected[0] = true;
					selected[1] = false;
					selected[2] = false;
				}else if(getX == 1){
					selected[0] = false;
					selected[1] = true;
					selected[2] = false;
				}
				else if(getX == 2){
					selected[0] = false;
					selected[1] = false;
					selected[2] = true;
				}
			}else{
				if(selected[0]){
					drawAt(getX, getY, DEFENDER);
				}else if(selected[1]){
					drawAt(getX, getY, RESOURCE_GATHERER);
				}else if(selected[2]){
					drawAt(getX, getY, WALL);
				}
				selected[0] = false;
				selected[1] = false;
				selected[2] = false;
			}
		}
	}
	glutPostRedisplay();
}
void removeSelection(){
	for(int i = 0 ; i < 9 ;i++){
		for(int j = 0 ; j < 5 ; j++){
			board[i][j].setSelected(false);
		}
	}
	selectedColomn = -1;
	selectedRow = -1;
}
void setRowOrColomn(int x){
	if(selectedRow == -1 ){
		if(x >=1 && x <=5){
			selectedRow = x;
			selectedColomn = -1;
		}
	}else if(selectedColomn == -1){
		if(x >=1 && x <=9){
			selectedColomn = x;
			board[selectedColomn-1][selectedRow-1].setSelected(true);
		}

	}else{
		removeSelection();
	}
}

void Key(unsigned char key, int a, int b) {
	if(!freeze){
		if(pause && key == 'p'){
			timeElapsed = (int)(glutGet(GLUT_ELAPSED_TIME*1.0)/1000);
			pausingTime += timeElapsed - savedTime;
			savedTime = -1;
			pause = false;
		}else{
			switch(key){
				case '1' : setRowOrColomn(1);break;
				case '2' : setRowOrColomn(2);break;
				case '3' : setRowOrColomn(3);break;
				case '4' : setRowOrColomn(4);break;
				case '5' : setRowOrColomn(5);break;
				case '6' : setRowOrColomn(6);break;
				case '7' : setRowOrColomn(7);break;
				case '8' : setRowOrColomn(8);break;
				case '9' : setRowOrColomn(9);break;
				case 'p' : pause = true; savedTime = timeElapsed; break;
				case 'd' :
					if( selectedColomn != -1 && selectedRow != -1){
						if(board[selectedColomn-1][selectedRow -1 ].getMyStatus() == EMPTY){
							drawAt(selectedColomn-1, selectedRow-1, DEFENDER);
						}
					}
					break;
				case 'r' :
					if( selectedColomn != -1 && selectedRow != -1){
						if(board[selectedColomn-1][selectedRow -1 ].getMyStatus() == EMPTY){
							drawAt(selectedColomn-1, selectedRow-1, RESOURCE_GATHERER);
						}
					}
					break;
				case 'w' :
					if( selectedColomn != -1 && selectedRow != -1){
						if(board[selectedColomn-1][selectedRow -1 ].getMyStatus() == EMPTY){
							drawAt(selectedColomn-1, selectedRow-1, WALL);
						}
					}
					break;
				case 'c' :
					if( selectedColomn != -1 && selectedRow != -1){
						if(board[selectedColomn-1][selectedRow -1 ].getMyStatus() == OCCUPIED){
							Cell c = board[selectedColomn-1][selectedRow -1 ];
							int index = c.getShapeIndex();
							if(c.getOccupiedBy()==DEFENDER){
								defenders[index].setDead(true);
							}else if(c.getOccupiedBy() == RESOURCE_GATHERER){
								gatherers[index].setDead(true);
							}
							board[selectedColomn -1 ][selectedRow -1 ].setShapeIndex(-1);
							board[selectedColomn -1 ][selectedRow -1 ].setMyStatus(EMPTY);
						}
					}
					removeSelection();
					break;
				case 'v' : view = !(view); break;
				case 'k' : zView++; break;
				case 'm' : zView--; break;
				default : removeSelection();
			}
		}
	}
	glutPostRedisplay();
}
void KeyUp(unsigned char key, int a, int b) {
  glutPostRedisplay();
}
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(150, 150);
	glutCreateWindow("OpenGL - 3D Template");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Key);
	glutKeyboardUpFunc(KeyUp);
	glutIdleFunc(Anim);
	glutMouseFunc(Mouse);

	glClearColor(1, 1, 1,0.0f); // background is white
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, 640, 480);
	glutMainLoop();
	return 0;
}
