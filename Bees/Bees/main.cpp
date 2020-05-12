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

const float yTurn = initialTurn*20.0;
const float maxLookAtY = 12.5;

float stepPseudoX = 0.0;
float stepPseudoY = 0.0;

float xAngle = 0.0;

bool isPressedKey[256];


// Scene values
const float initialSize = 1.0;
const float sceneY = -4.0;

const float groundHeight = 0.1;

const float lampBaseHeight = 0.2;
const float lampPostHeight = 4.0;
const float lampSize = 0.5;


// Lamp values
float lampAmbient[] = { 1.0, 1.0, 0.8, 1.0 };
float lampDiffuse[] = { 0.8, 0.8, 0.2, 1.0 };
float lampSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
float lampPosition[] = { 0.0, 0.0, 0.0, 1.0 };


void initialise(void);

void motion(int, int);
void keyboardDown(unsigned char, int, int);
void keyboardUp(unsigned char, int, int);
void useKeyboard();
void menu(int);

void reshape(int, int);

void display(void);

void drawLamp(float, float, float);
void drawGround(float, float, float);


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
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, lampAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lampDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lampSpecular);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 12.0);
    
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
        
        xAngle = 0.0;
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
        stepPseudoY = initialStep*cos(xAngle);
        stepPseudoX = initialStep*sin(xAngle);
    }
    
    // Rotation
    if(isPressedKey['j']) {
        xAngle += initialTurn;
    } else if(isPressedKey['l']) {
        xAngle -= initialTurn;
    }
    
    
    // Applying movement & rotation
    
    // Movement
    if(isPressedKey['e']) {
        lookFromZ -= stepPseudoY; lookAtZ -= stepPseudoY;
        lookFromX -= stepPseudoX; lookAtX -= stepPseudoX;
    } else if(isPressedKey['d']) {
        lookFromZ += stepPseudoY; lookAtZ += stepPseudoY;
        lookFromX += stepPseudoX; lookAtX += stepPseudoX;
    }
    
    if(isPressedKey['s']) {
        lookFromX -= stepPseudoY; lookAtX -= stepPseudoY;
        lookFromZ += stepPseudoX; lookAtZ += stepPseudoX;
    } else if(isPressedKey['f']) {
        lookFromX += stepPseudoY; lookAtX += stepPseudoY;
        lookFromZ -= stepPseudoX; lookAtZ -= stepPseudoX;
    }
    
    // Rotation
    if(isPressedKey['i'] && (lookAtY <= maxLookAtY)) {
        lookAtY += yTurn;
    } else if(isPressedKey['k'] && (lookAtY >= -maxLookAtY)) {
        lookAtY -= yTurn;
    }
    
    if(isPressedKey['j'] || isPressedKey['l']) {
        lookAtX = lookFromX - initialDistance*sin(xAngle);
        lookAtZ = lookFromZ - initialDistance*cos(xAngle);
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
        drawLamp(-9.0, sceneY + groundHeight, -9.0);
    glPopMatrix();
    
    glutSwapBuffers();
    glutPostRedisplay();
}


void drawGround(float x, float y, float z) {
    GLfloat groundAmbient[] = { 0.5, 1.0, 0.5, 0.5 };
    GLfloat groundDiffuse[] = { 0.5, 1.0, 0.5, 0.1 };
    
    glPushMatrix();
//        glMaterialfv(GL_FRONT, GL_AMBIENT, groundAmbient);
//        glMaterialfv(GL_FRONT, GL_DIFFUSE, groundDiffuse);
        
        glTranslatef(x, y + groundHeight + (initialSize - groundHeight)/2.0, z);
        
        glScalef(20.0, groundHeight, 20.0);
        
        glutSolidCube(initialSize);
    glPopMatrix();
}

void drawLamp(float x, float y, float z) {
    // Base
    glPushMatrix();
        glTranslatef(x, y + lampBaseHeight + (initialSize - lampBaseHeight)/2.0, z);
        
        glScalef(0.75, lampBaseHeight, 0.75);
        
        glutSolidCube(initialSize);
    glPopMatrix();
    
    // Post
    glPushMatrix();
        glTranslatef(x, y + lampBaseHeight + lampPostHeight + (initialSize - lampPostHeight)/2.0, z);
        
        glScalef(0.225, lampPostHeight, 0.225);

        glutSolidCube(initialSize);
    glPopMatrix();
    
    // Lamp
    lampPosition[0] = x;
    lampPosition[1] = y + lampBaseHeight + lampPostHeight + lampSize*2.0;
    lampPosition[2] = z;
    
    glLightfv(GL_LIGHT0, GL_POSITION, lampPosition);
    
    glPushMatrix();
        glPushAttrib(GL_LIGHTING_BIT);
            glMaterialfv(GL_FRONT, GL_AMBIENT, lampAmbient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, lampDiffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, lampSpecular);
            
            glTranslatef(lampPosition[0], lampPosition[1], lampPosition[2]);
            
            glutSolidSphere(lampSize, 20, 20);
        glPopAttrib();
    glPopMatrix();
}
