//
//  main.cpp
//  Bees
//
//  Created by Airat K on 12/5/20.
//  Copyright © 2020 Airat K. All rights reserved.
//


#define GL_SILENCE_DEPRECATION


#include <GLUT/glut.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159
#endif


// Window values
const float windowWidth = 1200.0;
const float windowHeight = 600.0;

const float windowRatio = windowWidth/windowHeight;


// View values
const float initialCenter = 0.0;
const float initialDistance = 17.5;  // 0.0 < initialDistance

float lookFromX = initialCenter;
float lookFromY = initialCenter;
float lookFromZ = initialCenter + initialDistance;

float lookAtX = initialCenter;
float lookAtY = initialCenter;
float lookAtZ = initialCenter;


// Movement & rotation values
const float initialStep = 0.15;
const float initialTurn = 1.0;

const float yStep = initialStep/3.0;

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

const float curbHeight = 0.2;

const float lampBaseHeight = 0.2;
const float lampPostHeight = 4.0;
const float lampSize = 0.5;

enum class Wing { Right, Left };

float lampPosition[] = { 0.0, 0.0, 0.0, 1.0 };


// Animation values
float flapper = 0.0;  // 0.0 <= flapper <= 360.0
float mover = 0.0;  // 0.0 <= mover <= 360.0

const float flappingVelocity = 40.0;
const float movingVelocity = 1.0;


// Colors
const float lampAmbient[] = { 1.0, 1.0, 0.7, 1.0 };
const float lampDiffuse[] = { 0.8, 0.8, 0.4, 1.0 };
const float lampSpecular[] = { 1.0, 1.0, 0.7, 1.0 };
const float lampAttenuation = 12.0;

const float lampPostAmbient[] = { 0.1, 0.05, 0.025, 1.0 };
const float lampPostDiffuse[] = { 0.4, 0.2, 0.1, 1.0 };
const float lampPostSpecular[] = { 0.1, 0.05, 0.025, 1.0 };

const float groundAmbient[] = { 0.0, 0.2, 0.0, 1.0 };
const float groundDiffuse[] = { 0.05, 0.5, 0.05, 1.0 };
const float groundSpecular[] = { 0.0, 0.5, 0.2, 1.0 };

const float pathAmbient[] = { 0.1, 0.1, 0.1 };
const float pathDiffuse[] = { 0.6, 0.6, 0.6 };
const float pathSpecular[] = { 0.4, 0.4, 0.4 };

const float beeBlackAmbient[] = { 0.0, 0.0, 0.0 };
const float beeBlackDiffuse[] = { 0.25, 0.25, 0.25 };
const float beeBlackSpecular[] = { 0.3, 0.3, 0.3 };

const float beeYellowAmbient[] = { 0.4, 0.4, 0.0 };
const float beeYellowDiffuse[] = { 0.8, 0.8, 0.0 };
const float beeYellowSpecular[] = { 1.0, 1.0, 0.0 };

const float beeWhiteAmbient[] = { 0.3, 0.3, 0.3 };
const float beeWhiteDiffuse[] = { 0.6, 0.6, 0.6 };
const float beeWhiteSpecular[] = { 0.8, 0.8, 0.8 };

const float beeWingsAmbient[] = { 0.4, 0.6, 1.0 };
const float beeWingsDiffuse[] = { 0.4, 0.6, 1.0 };
const float beeWingsSpecular[] = { 0.6, 1.0, 1.0 };


void initialise(void);

void keyboardDown(unsigned char, int, int);
void keyboardUp(unsigned char, int, int);
void useKeyboard();
void menu(int);

void reshape(int, int);

void display(void);

void drawGround(float, float, float);
void drawSidewalk(float, float, float, float);
void drawLamp(GLenum, float, float, float);
void drawBee(void);
void drawWing(Wing);

float radians(float);


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
    
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    
    glutCreateMenu(menu);
    glutAddMenuEntry("Turn off lamps", 0);
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
    glEnable(GL_LIGHT1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    gluPerspective(60.0, windowRatio, 1.0, 30.0);
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
        zStep = initialStep*cos(radians(yAngle));
        xStep = initialStep*sin(radians(yAngle));
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
        lookAtY += radians(yTurn);
    } else if(isPressedKey['k'] && (lookAtY >= -maxLookAtY)) {
        lookAtY -= radians(yTurn);
    }
    
    if(isPressedKey['j'] || isPressedKey['l']) {
        lookAtX = lookFromX - initialDistance*sin(radians(yAngle));
        lookAtZ = lookFromZ - initialDistance*cos(radians(yAngle));
    }
}


void menu(int option) {
    switch(option) {
        case 0: {
            if(glIsEnabled(GL_LIGHT0) || glIsEnabled(GL_LIGHT1)) {
                glDisable(GL_LIGHT0);
                glDisable(GL_LIGHT1);
                
                glutChangeToMenuEntry(1, "Turn on lamps", 0);
            } else {
                glEnable(GL_LIGHT0);
                glEnable(GL_LIGHT1);
                
                glutChangeToMenuEntry(1, "Turn off lamps", 0);
            }
        } break;
        
        default: {}
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
        
        // Ground
        drawGround(0.0, sceneY, 0.0);
        drawSidewalk(6.0, 0.0, sceneY, 0.0);
        
        // Lamps
        drawLamp(GL_LIGHT0, -4.0, sceneY + groundHeight, -8.0);
        drawLamp(GL_LIGHT1, 4.0, sceneY + groundHeight, 8.0);
        
        // Bees
        (mover >= 360.0) ? mover = 0.0 : mover += movingVelocity;
        
        glPushMatrix();
            glTranslatef(5.0*cos(radians(mover)) - 4.0, 2.0, 5.0*sin(radians(mover)) + 2.0);
            glRotatef(-mover, 0.0, 1.0, 0.0);
            
            drawBee();
        glPopMatrix();
        
        glPushMatrix();
            glTranslatef(-5.0*cos(radians(mover)) + 4.0, -1.0, -5.0*sin(radians(mover)) - 2.0);
            glRotatef(-mover - 180.0, 0.0, 1.0, 0.0);
            
            drawBee();
        glPopMatrix();
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

void drawSidewalk(float width, float x, float y, float z) {
    y += curbHeight + (initialSize - curbHeight)/2.0;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, pathAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, pathDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, pathSpecular);
    
    // Sidewalk
    glBegin(GL_QUADS);
        glVertex3f(x - width/2.0, y + 0.01, z - groundLength/2.0);
        glVertex3f(x + width/2.0, y + 0.01, z - groundLength/2.0);
        glVertex3f(x + width/2.0, y + 0.01, z + groundLength/2.0);
        glVertex3f(x - width/2.0, y + 0.01, z + groundLength/2.0);
    glEnd();
    
    // Left curb
    glPushMatrix();
        glTranslatef(x - width/2.0, y + curbHeight/2.0, z);
        glScalef(0.75, curbHeight, groundLength);
        
        glutSolidCube(initialSize);
    glPopMatrix();
    
    // Right curb
    glPushMatrix();
        glTranslatef(x + width/2.0, y + curbHeight/2.0, z);
        glScalef(0.75, curbHeight, groundLength);
        
        glutSolidCube(initialSize);
    glPopMatrix();
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
        
        glutSolidSphere(lampSize, 20, 20);
    glPopMatrix();
}

void drawBee(void) {
    // Face
    glMaterialfv(GL_FRONT, GL_AMBIENT, beeWhiteAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, beeWhiteDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, beeWhiteSpecular);
    
    glPushMatrix();
        glTranslatef(0.0, 0.0, 0.3);
        
        glutSolidSphere(0.5, 20, 20);
    glPopMatrix();
    
    // Head
    glMaterialfv(GL_FRONT, GL_AMBIENT, beeBlackAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, beeBlackDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, beeBlackSpecular);
    
    glPushMatrix();
        glRotatef(90.0, 0.0, 0.0, 1.0);
        glScalef(0.4, 0.45, 0.4);
        
        glutSolidDodecahedron();
    glPopMatrix();
    
    // Body
    glPushMatrix();
        glRotatef(-10.0, 1.0, 0.0, 0.0);
        
        int bodyUnitCounter = 0;
        bool isBodyUnitYellow = true;
        float bodyUnitZ = -0.6;
        float bodyUnitRadius = 0.3;
        
        for(; bodyUnitCounter < 4; bodyUnitCounter++, isBodyUnitYellow = !isBodyUnitYellow, bodyUnitZ -= 0.3, bodyUnitRadius -= 0.05) {
            if(isBodyUnitYellow) {
                glMaterialfv(GL_FRONT, GL_AMBIENT, beeYellowAmbient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, beeYellowDiffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, beeYellowSpecular);
            } else {
                glMaterialfv(GL_FRONT, GL_AMBIENT, beeBlackAmbient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, beeBlackDiffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, beeBlackSpecular);
            }
            
            glPushMatrix();
                glTranslatef(0.0, 0.0, bodyUnitZ);
                
                glutSolidTorus(0.2, bodyUnitRadius, 8, 20);
            glPopMatrix();
        }
    glPopMatrix();
    
    // Sting
    glMaterialfv(GL_FRONT, GL_AMBIENT, beeWhiteAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, beeWhiteDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, beeWhiteSpecular);
    
    glPushMatrix();
        glTranslatef(0.0, -0.3, -1.6);
        glRotatef(180.0, 1.0, 0.0, 0.0);
        
        glutSolidCone(0.2, 0.6, 10, 10);
    glPopMatrix();
    
    // Wings
    drawWing(Wing::Right);
    drawWing(Wing::Left);
}

void drawWing(Wing wing) {
    float inverter = 0.0;
    
    switch(wing) {
        case Wing::Right: inverter = -1.0; break;
        case Wing::Left: inverter = 1.0; break;
        
        default: {}
    }
    
    (flapper >= 360.0) ? flapper = 0.0 : flapper += flappingVelocity;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, beeWingsAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, beeWingsDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, beeWingsSpecular);
    
    glPushMatrix();
        glRotatef(10.0, 1.0, 0.0, 0.0);  // Forward tilt
        glRotatef(15.0*sin(radians(flapper))*inverter, 0.0, 0.0, 1.0);  // Flapping animation
        glRotatef(-75.0*inverter, 0.0, 1.0, 0.0);  // Opened wings
        
        glBegin(GL_POLYGON);
            for(float t = -90.0; t <= 90.0; t += 3.0) {
                glVertex3f((0.65*cos(radians(t)) - 0.15)*inverter, 0.45, sin(radians(t)) - 1.0);
            }
        glEnd();
    glPopMatrix();
}

float radians(float degrees) {
    return (degrees/180.0)*M_PI;
}
