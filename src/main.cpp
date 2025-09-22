#include <GL/freeglut.h>
#include <cmath>
#include <vector>

#include <iostream>
#include <string>
#include <stdio.h>


constexpr float PI = 3.14159265358979323846f;
constexpr int window_width = 800;
constexpr int window_height = 800;

float sweep_angle = 0.0f;       // Current sweep angle in degrees
float sweep_speed = 60.0f;      // Degrees per second
float detection_tolerance = 5.0f; // Degrees tolerance for target detection

struct Target {
    float angle;
    float radius;
    bool detected;
};

std::vector<Target> targets = {
    {30.0f, 0.6f, false},
    {90.0f, 0.75f, false},
    {135.0f, 0.5f, false},
    {210.0f, 0.85f, false},
    {300.0f, 0.65f, false}
};

inline float deg2rad(float deg) {
    return deg * PI / 180.0f;
}

void drawCircle(float radius, int segments) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * PI * i / segments;
        glVertex2f(radius * cos(angle), radius * sin(angle));
    }
    glEnd();
}

void drawRadarGrid() {
    glColor3f(0.0f, 0.4f, 0.0f);
    int rings = 5;
    for (int i = 1; i <= rings; i++)
        drawCircle(i * 0.2f, 100);

    glBegin(GL_LINES);
    for (int i = 0; i < 12; i++) {
        float angle = deg2rad(i * 30.0f);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(cos(angle), sin(angle));
    }
    glEnd();
}

void drawRadarSweep() {
    int segments = 100;
    float sweep_width_deg = 5.0f;
    float start_angle = sweep_angle - sweep_width_deg / 2.0f;
    float end_angle = sweep_angle + sweep_width_deg / 2.0f;

    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.0f, 1.0f, 0.0f, 0.4f);
    glVertex2f(0.0f, 0.0f);
    for (int i = 0; i <= segments; i++) {
        float angle = deg2rad(start_angle + (end_angle - start_angle) * i / segments);
        float alpha = 0.4f * (1.0f - (float)i / segments);
        glColor4f(0.0f, 1.0f, 0.0f, alpha);
        glVertex2f(cos(angle), sin(angle));
    }
    glEnd();
}

void drawTargets() {
    float point_size = 10.0f;
    glPointSize(point_size);
    glBegin(GL_POINTS);
    for (auto& t : targets) {
        if (t.detected) glColor3f(1.0f, 1.0f, 0.0f);
        else glColor3f(0.0f, 1.0f, 0.0f);
        float rad = deg2rad(t.angle);
        glVertex2f(cos(rad) * t.radius, sin(rad) * t.radius);
    }
    glEnd();
}

void updateDetection() {
    for (auto& t : targets) {
        float diff = fabs(t.angle - sweep_angle);
        if (diff > 180.0f) diff = 360.0f - diff;
        t.detected = (diff <= detection_tolerance);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawRadarGrid();
    drawRadarSweep();
    drawTargets();

    glutSwapBuffers();
}

void timer(int) {
    sweep_angle -= sweep_speed * 0.016f;
    // printf("%f\n", sweep_angle);
    if (sweep_angle >= 360.0f) sweep_angle -= 360.0f;

    updateDetection();

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        gluOrtho2D(-1.0, 1.0, -1.0 * (GLfloat)h / w, 1.0 * (GLfloat)h / w);
    else
        gluOrtho2D(-1.0 * (GLfloat)w / h, 1.0 * (GLfloat)w / h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("Radar PPI Display - OpenGL");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);

    initGL();

    glutMainLoop();
    return 0;
}
