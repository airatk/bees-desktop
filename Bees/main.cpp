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


// Значения окна: ширина, высота, отношение сторон
const float windowWidth = 1200.0;
const float windowHeight = 600.0;

const float windowRatio = windowWidth/windowHeight;


// Значения перспективной проекции
const float initialCenter = 0.0;
const float initialDistance = 17.5;  // 0.0 < initialDistance

float lookFromX = initialCenter;
float lookFromY = initialCenter;
float lookFromZ = initialCenter + initialDistance;

float lookAtX = initialCenter;
float lookAtY = initialCenter;
float lookAtZ = initialCenter;


// Значения перемещения и поворота камеры
const float initialStep = 0.15;
const float initialTurn = 1.0;

const float yStep = initialStep/3.0;

const float yTurn = initialTurn*20.0;
const float maxLookAtY = 12.5;

float xStep = 0.0;
float zStep = 0.0;

float yAngle = 0.0;

bool isPressedKey[256];


// Значения объектов сцены
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


// Значения анимации
float flapper = 0.0;  // 0.0 <= flapper <= 360.0
float mover = 0.0;  // 0.0 <= mover <= 360.0

const float flappingVelocity = 4.0;
const float movingVelocity = 0.1;


// Параметры материалов
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


// Объявления функций
void initialise(void);

void keyboardDown(unsigned char, int, int);
void keyboardUp(unsigned char, int, int);
void useKeyboard();
void menu(int);

void display(void);

void drawGround(float, float, float);
void drawSidewalk(float, float, float, float);
void drawLamp(GLenum, float, float, float);
void drawBee(void);
void drawWing(Wing);

float radians(float);


// Главная функция
int main(int argc, char** argv) {
    // Инициализация GLUT
    glutInit(&argc, argv);
    
    // Инициализация режима дисплея
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    // Инициализация рамера окна
    glutInitWindowSize(windowWidth, windowHeight);
    
    // Расположение окна в центре экрана
    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
    
    int windowPositionX = (screenWidth != 0) ? (screenWidth - windowWidth)/2 : 0;
    int windowPositionY = (screenHeight != 0) ? (screenHeight - windowHeight)/2 : 0;
    
    glutInitWindowPosition(windowPositionX, windowPositionY);
    
    // Создание окна
    glutCreateWindow("Пчёлы");
    
    // Инициализация функции перерисовки
    glutDisplayFunc(display);
    
    // Инициализация функций обработки нажатия клавиш
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    
    // Инициализация меню
    glutCreateMenu(menu);
    // Добавление в меню одного элемента: "Выключить лампы"
    glutAddMenuEntry("Выключить лампы", 0);
    // Добавление поддержки вызова меню по нажатии ПКМ
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    // Вызов функции инициализации сцены
    initialise();
    
    // Запуск основного цикла GLUT
    glutMainLoop();
    
    // Выход из программы с кодом 0
    return 0;
}


void initialise(void) {
    // Инициализация значений отчистки буферов цветов
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    // Включение теста глубины
    glEnable(GL_DEPTH_TEST);
    // Включение освещения
    glEnable(GL_LIGHTING);
    // Включение источников света GL_LIGHT0, GL_LIGHT1
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    
    // Задание режима матрицы GL_MODELVIEW
    glMatrixMode(GL_MODELVIEW);
    // Установка единичной матрицы
    glLoadIdentity();
    
    // Установка матрицы перспективной проекции
    gluPerspective(60.0, windowRatio, 1.0, 30.0);
}


// Функция обработки нажатия клавиши
void keyboardDown(unsigned char key, int x, int y) {
    // Возврат камеры в исходное положение в случае нажатия клавиши 'q'
    if(key == 'q') {
        lookAtX = lookAtY = lookAtZ = initialCenter;
        lookFromX = lookFromY = lookFromZ = initialCenter;
        
        lookFromZ += initialDistance;
        
        yAngle = 0.0;
    } else {
        isPressedKey[key] = true;
    }
}

// Функция обработки отжатия клавиши
void keyboardUp(unsigned char key, int x, int y) {
    isPressedKey[key] = false;
}

// Функция поддержки нажатия нескольких клавиш одновременно
void useKeyboard() {
    // Расчёт перемещения и поворота камеры
    
    // Перемещение
    if(isPressedKey['e'] || isPressedKey['d'] || isPressedKey['s'] || isPressedKey['f']) {
        zStep = initialStep*cos(radians(yAngle));
        xStep = initialStep*sin(radians(yAngle));
    }
    
    // Вращение
    if(isPressedKey['j']) {
        yAngle += initialTurn;
    } else if(isPressedKey['l']) {
        yAngle -= initialTurn;
    }
    
    
    // Применение расчётов перемещения и поворота камеры
    
    // Перемещение
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
    
    // Поворот
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


// Функция обработки нажатия элементов меню
void menu(int option) {
    switch(option) {
        case 0: {
            if(glIsEnabled(GL_LIGHT0) || glIsEnabled(GL_LIGHT1)) {
                // Выключение ламп в случае нажатия элемента меню "Выключить лампы"
                glDisable(GL_LIGHT0);
                glDisable(GL_LIGHT1);
                
                // Замена элемента меню "Выключить лампы" на "Включить лампы"
                glutChangeToMenuEntry(1, "Включить лампы", 0);
            } else {
                // Включение ламп в случае нажатия элемента меню "Включить лампы"
                glEnable(GL_LIGHT0);
                glEnable(GL_LIGHT1);
                
                // Замена элемента меню "Включить лампы" на "Выключить лампы"
                glutChangeToMenuEntry(1, "Выключить лампы", 0);
            }
        } break;
        
        default: {}
    }
}


// Функция перерисовки
void display(void) {
    // Очистка буферов к начальным значениям
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Вызов функции поддержки нажатия нескольких клавиш одновременно
    useKeyboard();
    
    glPushMatrix();
        // Определение положения и просмотра камеры
        gluLookAt(
            lookFromX, lookFromY, lookFromZ,
            lookAtX, lookAtY, lookAtZ,
            0.0, 1.0, 0.0
        );
        
        // Вызов функции отрисовки земли
        drawGround(0.0, sceneY, 0.0);
        // Вызов функции отрисовки дорожки
        drawSidewalk(6.0, 0.0, sceneY, 0.0);
        
        // Вызов функции отрисовки лампы для источников света GL_LIGHT0, GL_LIGHT1
        drawLamp(GL_LIGHT0, -4.0, sceneY + groundHeight, -8.0);
        drawLamp(GL_LIGHT1, 4.0, sceneY + groundHeight, 8.0);
        
        // Изменение переменной движения пчёл
        (mover >= 360.0) ? mover = 0.0 : mover += movingVelocity;
        
        glPushMatrix();
            // Анимация перемещения пчелы 1
            glTranslatef(5.0*cos(radians(mover)) - 4.0, 2.0, 5.0*sin(radians(mover)) + 2.0);
            // Анимация поворота пчелы 1
            glRotatef(-mover, 0.0, 1.0, 0.0);
            
            // Вызов функции отрисовки пчелы для пчелы 1
            drawBee();
        glPopMatrix();
        
        glPushMatrix();
            // Анимация перемещения пчелы 2
            glTranslatef(-5.0*cos(radians(mover)) + 4.0, -1.0, -5.0*sin(radians(mover)) - 2.0);
            // Анимация поворота пчелы 2
            glRotatef(-mover - 180.0, 0.0, 1.0, 0.0);
            
            // Вызов функции отрисовки пчелы для пчелы 2
            drawBee();
        glPopMatrix();
    glPopMatrix();
    
    // Перестановка буферов
    glutSwapBuffers();
    // Принудительная перерисовка окна
    glutPostRedisplay();
}


// Функция вызова отрисовки земли, реализованной кубом
void drawGround(float x, float y, float z) {
    // Наложение материала на куб
    glMaterialfv(GL_FRONT, GL_AMBIENT, groundAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, groundDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, groundSpecular);
    
    glPushMatrix();
        // Перемещение куба
        glTranslatef(x, y + groundHeight + (initialSize - groundHeight)/2.0, z);
        // Изменение размера куба
        glScalef(groundLength, groundHeight, groundLength);
        
        // Куб
        glutSolidCube(initialSize);
    glPopMatrix();
}

// Функция отрисовки дорожки, реализованной 2D-примитивом четырёхугольник
void drawSidewalk(float width, float x, float y, float z) {
    y += curbHeight + (initialSize - curbHeight)/2.0;
    
    // Наложение материала на четырёхугольник
    glMaterialfv(GL_FRONT, GL_AMBIENT, pathAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, pathDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, pathSpecular);
    
    // Четырёхугольник, реализованный 4 точками в пространстве
    glBegin(GL_QUADS);
        glVertex3f(x - width/2.0, y + 0.01, z - groundLength/2.0);
        glVertex3f(x + width/2.0, y + 0.01, z - groundLength/2.0);
        glVertex3f(x + width/2.0, y + 0.01, z + groundLength/2.0);
        glVertex3f(x - width/2.0, y + 0.01, z + groundLength/2.0);
    glEnd();
    
    // Левый бордюр, реализованный кубом
    glPushMatrix();
        // Перемещение куба
        glTranslatef(x - width/2.0, y + curbHeight/2.0, z);
        // Изменение размера куба
        glScalef(0.75, curbHeight, groundLength);
        
        // Куб
        glutSolidCube(initialSize);
    glPopMatrix();
    
    // Правый бордюр, реализованный кубом
    glPushMatrix();
        // Перемещение куба
        glTranslatef(x + width/2.0, y + curbHeight/2.0, z);
        // Изменение размера куба
        glScalef(0.75, curbHeight, groundLength);
        
        // Куб
        glutSolidCube(initialSize);
    glPopMatrix();
}

// Функция отрисовки лампы, реализованной сферой и кубами
void drawLamp(GLenum light, float x, float y, float z) {
    // Наложение материала на кубы
    glMaterialfv(GL_FRONT, GL_AMBIENT, lampPostAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, lampPostDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, lampPostSpecular);
    
    // Основа ламы, реализованная кубом
    glPushMatrix();
        // Перемещение куба
        glTranslatef(x, y + lampBaseHeight + (initialSize - lampBaseHeight)/2.0, z);
        // Изменение размера куба
        glScalef(0.75, lampBaseHeight, 0.75);
        
        // Куб
        glutSolidCube(initialSize);
    glPopMatrix();
    
    // Столб лампы, реализованный кубом
    glPushMatrix();
        // Перемещение куба
        glTranslatef(x, y + lampBaseHeight + lampPostHeight + (initialSize - lampPostHeight)/2.0, z);
        // Изменение размера куба
        glScalef(0.2, lampPostHeight, 0.2);
        
        // Куб
        glutSolidCube(initialSize);
    glPopMatrix();
    
    // Задание положения лампы
    lampPosition[0] = x;
    lampPosition[1] = y + lampBaseHeight + lampPostHeight + lampSize*2.0;
    lampPosition[2] = z;
    
    // Задание параметров источника света в лампе
    glLightfv(light, GL_AMBIENT, lampAmbient);
    glLightfv(light, GL_DIFFUSE, lampDiffuse);
    glLightfv(light, GL_SPECULAR, lampSpecular);
    glLightfv(light, GL_POSITION, lampPosition);
    glLightf(light, GL_QUADRATIC_ATTENUATION, lampAttenuation);
    
    // Наложение материала на сферу
    glMaterialfv(GL_FRONT, GL_AMBIENT, lampAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, lampDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, lampSpecular);
    
    // Лампа, реализованная сферой
    glPushMatrix();
        // Перемещение сферы
        glTranslatef(lampPosition[0], lampPosition[1], lampPosition[2]);
        
        // Сфера
        glutSolidSphere(lampSize, 20, 20);
    glPopMatrix();
}

// Функция отрисовки пчелы, реализованной сферой, додекаэдром, торами, конусом и 2D-примитивами полигонами
void drawBee(void) {
    // Наложение материала на сферу
    glMaterialfv(GL_FRONT, GL_AMBIENT, beeWhiteAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, beeWhiteDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, beeWhiteSpecular);
    
    // Голова пчелы, реализованная сферой
    glPushMatrix();
        // Перемещение сферы
        glTranslatef(0.0, 0.0, 0.3);
        
        // Сфера
        glutSolidSphere(0.5, 20, 20);
    glPopMatrix();
    
    // Наложение материала на додекаэдр
    glMaterialfv(GL_FRONT, GL_AMBIENT, beeBlackAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, beeBlackDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, beeBlackSpecular);
    
    // Связующее звено между головой и телом пчелы, реализованное додекаэдром
    glPushMatrix();
        // Перемещение додекаэдра
        glRotatef(90.0, 0.0, 0.0, 1.0);
        // Изменения размера додекаэдра
        glScalef(0.4, 0.45, 0.4);
        
        // Додекаэдр
        glutSolidDodecahedron();
    glPopMatrix();
    
    // Тело пчелы, реализованное торами
    glPushMatrix();
        // Поворот торов
        glRotatef(-10.0, 1.0, 0.0, 0.0);
        
        int bodyUnitCounter = 0;
        bool isBodyUnitYellow = true;
        float bodyUnitZ = -0.6;
        float bodyUnitRadius = 0.3;
        
        // Отрисовка торов циклом с постепенным уменьшением размера торов
        for(; bodyUnitCounter < 4; bodyUnitCounter++, isBodyUnitYellow = !isBodyUnitYellow, bodyUnitZ -= 0.3, bodyUnitRadius -= 0.05) {
            // Попеременное наложение жёлтого и чёрного материалов на торы
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
                // Перемещение тора
                glTranslatef(0.0, 0.0, bodyUnitZ);
                
                // Тор
                glutSolidTorus(0.2, bodyUnitRadius, 8, 20);
            glPopMatrix();
        }
    glPopMatrix();
    
    // Наложение материала на конус
    glMaterialfv(GL_FRONT, GL_AMBIENT, beeWhiteAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, beeWhiteDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, beeWhiteSpecular);
    
    // Жало пчелы, реализованное конусом
    glPushMatrix();
        // Перемещение конуса
        glTranslatef(0.0, -0.3, -1.6);
        // Поворот конуса
        glRotatef(180.0, 1.0, 0.0, 0.0);
        
        // Конус
        glutSolidCone(0.2, 0.6, 10, 10);
    glPopMatrix();
    
    // Вызов функции отрисовки крыла для левого крыла и правого крыла
    drawWing(Wing::Right);
    drawWing(Wing::Left);
}

// Функция отрисовки крыла
void drawWing(Wing wing) {
    float inverter = 0.0;
    
    // Выбор крыла
    switch(wing) {
        case Wing::Right: inverter = -1.0; break;
        case Wing::Left: inverter = 1.0; break;
        
        default: {}
    }
    
    // Изменение переменной движения крыльев
    (flapper >= 360.0) ? flapper = 0.0 : flapper += flappingVelocity;
    
    // Наложение материалов на 2D-примитив полигон
    glMaterialfv(GL_FRONT, GL_AMBIENT, beeWingsAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, beeWingsDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, beeWingsSpecular);
    
    // Крыло, реализованное 2D-примитивом полигон в форме полукруга
    glPushMatrix();
        // Наклон полигона
        glRotatef(10.0, 1.0, 0.0, 0.0);
        // Анимация взмахов
        glRotatef(15.0*sin(radians(flapper))*inverter, 0.0, 0.0, 1.0);
        // Раскрытие крыла
        glRotatef(-75.0*inverter, 0.0, 1.0, 0.0);
        
        // Полигон из точек в пространстве, отрисованных циклом по полукругу
        glBegin(GL_POLYGON);
            for(float t = -90.0; t <= 90.0; t += 3.0) {
                glVertex3f((0.65*cos(radians(t)) - 0.15)*inverter, 0.45, sin(radians(t)) - 1.0);
            }
        glEnd();
    glPopMatrix();
}

// Функция перевода градусов в радианы
float radians(float degrees) {
    return (degrees/180.0)*M_PI;
}
