//
//  main.cpp
//  Bees
//
//  Created by Airat K on 12/5/20.
//  Copyright © 2020 Airat K. All rights reserved.
//


#define GL_SILENCE_DEPRECATION

#include <iostream>  // TODO: Remove this line
#include <cmath>

#include <GLUT/glut.h>


// Window values
const float windowWidth = 1200.0;
const float windowHeight = 600.0;

const float windowRatio = windowWidth/windowHeight;


// View values
const float initialCenter = 0.0;
const float initialDistance = 17.5;  // Has to be greater than 0.0

float lookFromX = initialCenter;
float lookFromY = initialCenter;
float lookFromZ = initialCenter + initialDistance;

float lookAtX = initialCenter;
float lookAtY = initialCenter;
float lookAtZ = initialCenter;


// Movement & rotation values
const float initialStep = 0.2;
const float initialTurn = 0.01;

const float yStep = initialStep/5.0;

const float yTurn = initialTurn*20.0;
const float maxLookAtY = 12.5;

float xStep = 0.0;
float zStep = 0.0;

float yAngle = 0.0;

bool isPressedKey[256];


// Scene values
const float initialSize = 1.0;
const float sceneY = -4.0;

const float groundLength = 20.0;
const float groundHeight = 0.1;

const float lampBaseHeight = 0.2;
const float lampPostHeight = 4.0;
const float lampSize = 0.5;


// Lamp colors
const float lampAmbient[] = { 1.0, 1.0, 0.8, 1.0 };
const float lampDiffuse[] = { 0.8, 0.8, 0.2, 1.0 };
const float lampSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
float lampPosition[] = { 0.0, 0.0, 0.0, 1.0 };

const float lampAttenuation = 12.0;

// Lamp post colors
const float lampPostAmbient[] = { 0.1, 0.05, 0.025, 1.0 };
const float lampPostDiffuse[] = { 0.4, 0.2, 0.1, 1.0 };
const float lampPostSpecular[] = { 0.1, 0.05, 0.025, 1.0 };

// Ground colors
const float groundAmbient[] = { 0.0, 0.2, 0.0, 1.0 };
const float groundDiffuse[] = { 0.05, 0.5, 0.05, 1.0 };
const float groundSpecular[] = { 0.0, 0.5, 0.2, 1.0 };


void initialise(void);

void motion(int, int);
void keyboardDown(unsigned char, int, int);
void keyboardUp(unsigned char, int, int);
void useKeyboard();
void menu(int);

void reshape(int, int);

void display(void);

void drawGround(float, float, float);
void drawPath(float, float, float, float);
void drawLamp(GLenum, float, float, float);


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    
    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
    
    int windowPositionX = (screenWidth != 0) ? (screenWidth - windowWidth)/2 : 0;
    int windowPositionY = (screenHeight != 0) ? (screenHeight - windowHeight)/2 : 0;
    
    glutInitWindowPosition(windowPositionX, windowPositionY);
    
    glutCreateWindow("Bees");
    
    glutDisplayFunc(display);
    
    glutReshapeFunc(reshape);
    
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    
    glutCreateMenu(menu);
    glutAddMenuEntry("Turn off lamp", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    initialise();
    
    glutMainLoop();
    
    return 0;
}


void initialise(void) {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    gluPerspective(60.0, windowRatio, 1.0, 30.0);
}


void motion(int x, int y) {
    // TODO: Implement motion()
}

void keyboardDown(unsigned char key, int x, int y) {
    // Reseting values if 'q' is pressed
    if(key == 'q') {
        lookAtX = lookAtY = lookAtZ = initialCenter;
        lookFromX = lookFromY = lookFromZ = initialCenter;
        
        lookFromZ += initialDistance;
        
        yAngle = 0.0;
    } else {
        isPressedKey[key] = true;
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    isPressedKey[key] = false;
}

void useKeyboard() {
    // Calculating movement & rotation
    
    // Movement
    if(isPressedKey['e'] || isPressedKey['d'] || isPressedKey['s'] || isPressedKey['f']) {
        zStep = initialStep*cos(yAngle);
        xStep = initialStep*sin(yAngle);
    }
    
    // Rotation
    if(isPressedKey['j']) {
        yAngle += initialTurn;
    } else if(isPressedKey['l']) {
        yAngle -= initialTurn;
    }
    
    
    // Applying movement & rotation
    
    // Movement
    if(isPressedKey['e']) {
        lookFromZ -= zStep; lookAtZ -= zStep;
        lookFromX -= xStep; lookAtX -= xStep;
    } else if(isPressedKey['d']) {
        lookFromZ += zStep; lookAtZ += zStep;
        lookFromX += xStep; lookAtX += xStep;
    }
    
    if(isPressedKey['s']) {
        lookFromX -= zStep; lookAtX -= zStep;
        lookFromZ += xStep; lookAtZ += xStep;
    } else if(isPressedKey['f']) {
        lookFromX += zStep; lookAtX += zStep;
        lookFromZ -= xStep; lookAtZ -= xStep;
    }
    
    if(isPressedKey[';']) {
        lookFromY += yStep; lookAtY += yStep;
    } else if(isPressedKey['a']) {
        lookFromY -= yStep; lookAtY -= yStep;
    }
    
    // Rotation
    if(isPressedKey['i'] && (lookAtY <= maxLookAtY)) {
        lookAtY += yTurn;
    } else if(isPressedKey['k'] && (lookAtY >= -maxLookAtY)) {
        lookAtY -= yTurn;
    }
    
    if(isPressedKey['j'] || isPressedKey['l']) {
        lookAtX = lookFromX - initialDistance*sin(yAngle);
        lookAtZ = lookFromZ - initialDistance*cos(yAngle);
    }
}


void menu(int option) {
    switch(option) {
        case 0: {
            if(glIsEnabled(GL_LIGHT0)) {
                glDisable(GL_LIGHT0);
                
                glutChangeToMenuEntry(1, "Turn on lamp", 0);
            } else {
                glEnable(GL_LIGHT0);
                
                glutChangeToMenuEntry(1, "Turn off lamp", 0);
            }
        } break;
    }
}


void reshape(int width, int height) {
    (windowRatio > width/height) ? glViewport(0, 0, width, width/windowRatio) : glViewport(0, 0, height*windowRatio, height);
}


void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    useKeyboard();
    
    glPushMatrix();
        gluLookAt(
            lookFromX, lookFromY, lookFromZ,
            lookAtX, lookAtY, lookAtZ,
            0.0, 1.0, 0.0
        );
        
        drawGround(0.0, sceneY, 0.0);
        drawPath(6.0, 0.0, sceneY, 0.0);
        drawLamp(GL_LIGHT0, -9.0, sceneY + groundHeight, -9.0);
    glPopMatrix();
    
    glutSwapBuffers();
    glutPostRedisplay();
}


void drawGround(float x, float y, float z) {
    glMaterialfv(GL_FRONT, GL_AMBIENT, groundAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, groundDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, groundSpecular);
    
    glPushMatrix();
        glTranslatef(x, y + groundHeight + (initialSize - groundHeight)/2.0, z);
        glScalef(groundLength, groundHeight, groundLength);
        
        glutSolidCube(initialSize);
    glPopMatrix();
}

void drawPath(float width, float x, float y, float z) {
    y += groundHeight*2.0 + (initialSize - groundHeight)/2.0;
    
    float pathAmbient[] = { 0.1, 0.1, 0.1 };
    float pathDiffuse[] = { 0.6, 0.6, 0.6 };
    float pathSpecular[] = { 0.4, 0.4, 0.4 };
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, pathAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, pathDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, pathSpecular);
        
    glBegin(GL_QUADS);
        glVertex3f(x - width/2.0, y, z - groundLength/2.0);
        glVertex3f(x + width/2.0, y, z - groundLength/2.0);
        glVertex3f(x + width/2.0, y, z + groundLength/2.0);
        glVertex3f(x - width/2.0, y, z + groundLength/2.0);
    glEnd();
}

void drawLamp(GLenum light, float x, float y, float z) {
    glMaterialfv(GL_FRONT, GL_AMBIENT, lampPostAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, lampPostDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, lampPostSpecular);
    
    // Base
    glPushMatrix();
        glTranslatef(x, y + lampBaseHeight + (initialSize - lampBaseHeight)/2.0, z);
        glScalef(0.75, lampBaseHeight, 0.75);
        
        glutSolidCube(initialSize);
    glPopMatrix();
    
    // Post
    glPushMatrix();
        glTranslatef(x, y + lampBaseHeight + lampPostHeight + (initialSize - lampPostHeight)/2.0, z);
        glScalef(0.2, lampPostHeight, 0.2);
        
        glutSolidCube(initialSize);
    glPopMatrix();
    
    // Lamp
    lampPosition[0] = x;
    lampPosition[1] = y + lampBaseHeight + lampPostHeight + lampSize*2.0;
    lampPosition[2] = z;
    
    glLightfv(light, GL_AMBIENT, lampAmbient);
    glLightfv(light, GL_DIFFUSE, lampDiffuse);
    glLightfv(light, GL_SPECULAR, lampSpecular);
    glLightfv(light, GL_POSITION, lampPosition);
    glLightf(light, GL_QUADRATIC_ATTENUATION, lampAttenuation);
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, lampAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, lampDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, lampSpecular);
    
    glPushMatrix();
        glTranslatef(lampPosition[0], lampPosition[1], lampPosition[2]);
        
        glPushAttrib(GL_LIGHTING_BIT);
            glutSolidSphere(lampSize, 20, 20);
        glPopAttrib();
    glPopMatrix();
}
