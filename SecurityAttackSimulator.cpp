#define _CRT_SECURE_NO_WARNINGS

#include <GL/glut.h>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include "Node.h"
#include "Link.h"
#include "Packet.h"

using namespace std;

// Constants
const int MAX_NODES = 50;
const int MAX_LINKS = 100;
const int MAX_PACKETS = 200;
const float CAMERA_SPEED = 0.1f;

// Global variables
Node* nodes[MAX_NODES];
Link* links[MAX_LINKS];
Packet* packets[MAX_PACKETS];
int nodeCount = 0;
int linkCount = 0;
int packetCount = 0;

// Camera variables
float cameraX = 0.0f, cameraY = 10.0f, cameraZ = 13.0f;
float cameraRotX = 40.0f, cameraRotY = 0.0f;
bool mouseDown = false;
int lastMouseX = 0, lastMouseY = 0;

// UI State
bool showUI = true;
bool simulationPaused = false;
float ddosIntensity = 0.0f;
int selectedNode = -1;
bool showHelp = false;

// Security simulation variables
float ddosTimer = 0.0f;
float intrusionTimer = 0.0f;
bool firewallActive = true;
int attackCount = 0;
int blockedCount = 0;

void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

void drawUI() {
    if (!showUI) return;

    // Switch to orthographic projection for UI
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Disable depth testing for UI
    glDisable(GL_DEPTH_TEST);


    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(10, 10);
    glVertex2f(250, 10);
    glVertex2f(250, 200);
    glVertex2f(10, 200);
    glEnd();


    glColor3f(1.0f, 1.0f, 1.0f);
    char buffer[128];

    sprintf_s(buffer, sizeof(buffer), "Security Attack Simulator");
    drawText(20, 30, buffer);

    sprintf_s(buffer, sizeof(buffer), "Nodes: %d", nodeCount);
    drawText(20, 50, buffer);

    sprintf_s(buffer, sizeof(buffer), "Active Packets: %d", packetCount);
    drawText(20, 70, buffer);

    sprintf_s(buffer, sizeof(buffer), "Attacks: %d", attackCount);
    drawText(20, 90, buffer);

    sprintf_s(buffer, sizeof(buffer), "Blocked: %d", blockedCount);
    drawText(20, 110, buffer);

    sprintf_s(buffer, sizeof(buffer), "DDoS Intensity: %.1f%%", ddosIntensity * 100);
    drawText(20, 130, buffer);

    sprintf_s(buffer, sizeof(buffer), "Firewall: %s", firewallActive ? "Active" : "Disabled");
    drawText(20, 150, buffer);

    if (simulationPaused) {
        glColor3f(1.0f, 0.0f, 0.0f);
        drawText(20, 170, "SIMULATION PAUSED");
    }


    if (showHelp) {
        glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
        glBegin(GL_QUADS);
        glVertex2f(glutGet(GLUT_WINDOW_WIDTH) - 260, 10);
        glVertex2f(glutGet(GLUT_WINDOW_WIDTH) - 10, 10);
        glVertex2f(glutGet(GLUT_WINDOW_WIDTH) - 10, 200);
        glVertex2f(glutGet(GLUT_WINDOW_WIDTH) - 260, 200);
        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(glutGet(GLUT_WINDOW_WIDTH) - 250, 30, "Controls:");
        drawText(glutGet(GLUT_WINDOW_WIDTH) - 250, 50, "WASD - Move Camera");
        drawText(glutGet(GLUT_WINDOW_WIDTH) - 250, 70, "Mouse Drag - Rotate");
        drawText(glutGet(GLUT_WINDOW_WIDTH) - 250, 90, "Space - Pause/Resume");
        drawText(glutGet(GLUT_WINDOW_WIDTH) - 250, 110, "F - Toggle Firewall");
        drawText(glutGet(GLUT_WINDOW_WIDTH) - 250, 130, "H - Toggle Help");
        drawText(glutGet(GLUT_WINDOW_WIDTH) - 250, 150, "Tab - Toggle UI");
        drawText(glutGet(GLUT_WINDOW_WIDTH) - 250, 170, "Esc - Exit");
    }

    // --- BEGIN: Draw Legend Table (Top Right) ---
    int legendX = glutGet(GLUT_WINDOW_WIDTH) - 270;
    int legendY = 20;
    int boxSize = 24;
    int spacingY = 38;

    // Draw background for legend
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(legendX - 10, legendY - 10);
    glVertex2f(legendX + 240, legendY - 10);
    glVertex2f(legendX + 240, legendY + spacingY * 5 + 20);
    glVertex2f(legendX - 10, legendY + spacingY * 5 + 20);
    glEnd();

    // Draw colored boxes and labels
    // Normal (Regular safe traffic)
    glColor3f(0.0f, 1.0f, 0.0f); // Green
    glBegin(GL_QUADS);
    glVertex2f(legendX, legendY);
    glVertex2f(legendX + boxSize, legendY);
    glVertex2f(legendX + boxSize, legendY + boxSize);
    glVertex2f(legendX, legendY + boxSize);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(legendX + boxSize + 12, legendY + 16, "Normal (Regular safe traffic)");

    // DDoS Attack
    glColor3f(1.0f, 0.0f, 0.0f); // Red
    glBegin(GL_QUADS);
    glVertex2f(legendX, legendY + spacingY);
    glVertex2f(legendX + boxSize, legendY + spacingY);
    glVertex2f(legendX + boxSize, legendY + boxSize + spacingY);
    glVertex2f(legendX, legendY + boxSize + spacingY);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(legendX + boxSize + 12, legendY + spacingY + 16, "DDoS Attack");

    // Intrusion
    glColor3f(1.0f, 0.5f, 0.0f); // Orange
    glBegin(GL_QUADS);
    glVertex2f(legendX, legendY + spacingY * 2);
    glVertex2f(legendX + boxSize, legendY + spacingY * 2);
    glVertex2f(legendX + boxSize, legendY + boxSize + spacingY * 2);
    glVertex2f(legendX, legendY + boxSize + spacingY * 2);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(legendX + boxSize + 12, legendY + spacingY * 2 + 16, "Intrusion");

    // Encrypted (Secure communication)
    glColor3f(0.0f, 0.0f, 1.0f); // Blue
    glBegin(GL_QUADS);
    glVertex2f(legendX, legendY + spacingY * 3);
    glVertex2f(legendX + boxSize, legendY + spacingY * 3);
    glVertex2f(legendX + boxSize, legendY + boxSize + spacingY * 3);
    glVertex2f(legendX, legendY + boxSize + spacingY * 3);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(legendX + boxSize + 12, legendY + spacingY * 3 + 16, "Encrypted (Secure communication)");

    // Blocked (Stopped by firewall)
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow
    glBegin(GL_QUADS);
    glVertex2f(legendX, legendY + spacingY * 4);
    glVertex2f(legendX + boxSize, legendY + spacingY * 4);
    glVertex2f(legendX + boxSize, legendY + boxSize + spacingY * 4);
    glVertex2f(legendX, legendY + boxSize + spacingY * 4);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(legendX + boxSize + 12, legendY + spacingY * 4 + 16, "Blocked (Stopped by firewall)");
    // --- END: Draw Legend Table (Top Right) ---

    // Restore OpenGL state
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}


void createNewPacket() {
    if (packetCount >= MAX_PACKETS || simulationPaused) return;

    // Select random source and destination nodes
    int srcIndex = rand() % nodeCount;
    int destIndex;
    do {
        destIndex = rand() % nodeCount;
    } while (destIndex == srcIndex);

    Node* src = nodes[srcIndex];
    Node* dest = nodes[destIndex];

    // Determine packet type
    PacketType type = PacketType::NORMAL;
    float r = (float)rand() / RAND_MAX;

    if (r < ddosIntensity && src->getType() != NodeType::FIREWALL) {
        type = PacketType::DDOS;
    }
    else if (r < ddosIntensity + 0.1f && src->getType() != NodeType::FIREWALL) {
        type = PacketType::INTRUSION;
    }
    else if (r < ddosIntensity + 0.2f) {
        type = PacketType::ENCRYPTED;
    }

    // Create path for packet
    Link* path[MAX_LINKS];
    int pathLength = 0;

    // Simple path finding
    for (int i = 0; i < linkCount; i++) {
        if (links[i]->getSource() == src) {
            path[pathLength++] = links[i];
            break;
        }
    }

    if (pathLength > 0) {
        packets[packetCount] = new Packet(src, dest, path, pathLength, type);
        packets[packetCount]->start();
        packetCount++;

        if (type == PacketType::DDOS || type == PacketType::INTRUSION) {
            attackCount++;
        }
    }
}

void updateDDoS(float deltaTime) {
    ddosTimer += deltaTime;
    if (ddosTimer > 5.0f) {
        ddosTimer = 0.0f;
        ddosIntensity = (float)rand() / RAND_MAX * 0.3f;
    }
}

void updateIntrusion(float deltaTime) {
    intrusionTimer += deltaTime;
    if (intrusionTimer > 10.0f) {
        intrusionTimer = 0.0f;
        if ((float)rand() / RAND_MAX < 0.3f) {
            for (int i = 0; i < nodeCount; i++) {
                if (nodes[i]->getType() == NodeType::COMPUTER) {
                    createNewPacket();
                    break;
                }
            }
        }
    }
}

void update() {
    if (simulationPaused) return;

    static unsigned long lastTime = 0;
    unsigned long currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    updateDDoS(deltaTime);
    updateIntrusion(deltaTime);

    // The chance to create a packet each frame
    if ((float)rand() / RAND_MAX < 0.005f) {
        createNewPacket();
    }

    for (int i = 0; i < packetCount; i++) {
        if (packets[i] != nullptr) {
            packets[i]->update();

            if (packets[i]->isFinished()) {
                if (packets[i]->getType() == PacketType::BLOCKED) {
                    blockedCount++;
                }
                delete packets[i];
                packets[i] = packets[packetCount - 1];
                packets[packetCount - 1] = nullptr;
                packetCount--;
                i--;
            }
        }
    }

    glutPostRedisplay();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Apply camera rotation
    glRotatef(cameraRotX, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraRotY, 0.0f, 1.0f, 0.0f);

    // Apply camera position
    glTranslatef(-cameraX, -cameraY, -cameraZ);

    // Draw grid
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_LINES);
    for (float i = -5; i <= 5; i += 1.0f) {
        glVertex3f(i, 0, -5);
        glVertex3f(i, 0, 5);
        glVertex3f(-5, 0, i);
        glVertex3f(5, 0, i);
    }
    glEnd();

    // Draw links
    for (int i = 0; i < linkCount; i++) {
        links[i]->draw();
    }

    // Draw nodes
    for (int i = 0; i < nodeCount; i++) {
        nodes[i]->draw();
    }

    // Draw packets
    for (int i = 0; i < packetCount; i++) {
        packets[i]->draw();
    }

    drawUI();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w': cameraZ -= CAMERA_SPEED; break;
    case 's': cameraZ += CAMERA_SPEED; break;
    case 'a': cameraX -= CAMERA_SPEED; break;
    case 'd': cameraX += CAMERA_SPEED; break;
    case ' ': simulationPaused = !simulationPaused; break;
    case 'f': firewallActive = !firewallActive; break;
    case 'h': showHelp = !showHelp; break;
    case '\t': showUI = !showUI; break;
    case 27: exit(0); break;  // ESC key
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    mouseDown = (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN);
    lastMouseX = x;
    lastMouseY = y;
}

void mouseMotion(int x, int y) {
    if (mouseDown) {
        cameraRotX += (y - lastMouseY) * 0.5f;
        cameraRotY += (x - lastMouseX) * 0.5f;
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    }
}

void initNetwork() {
    // Create initial network nodes
    nodes[nodeCount++] = new Node(-2.0f, 0.0f, -2.0f, NodeType::COMPUTER, "PC1");
    nodes[nodeCount++] = new Node(2.0f, 0.0f, -2.0f, NodeType::COMPUTER, "PC2");
    nodes[nodeCount++] = new Node(0.0f, 0.0f, 0.0f, NodeType::ROUTER, "Router");
    nodes[nodeCount++] = new Node(0.0f, 0.0f, 2.0f, NodeType::FIREWALL, "Firewall");

    // Create network links
    links[linkCount++] = new Link(nodes[0], nodes[2]);  // PC1 to Router
    links[linkCount++] = new Link(nodes[1], nodes[2]);  // PC2 to Router
    links[linkCount++] = new Link(nodes[2], nodes[3], true);  // Router to Firewall
}

int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("Network Security Visualizer");

    // Set up OpenGL
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    // Set up lighting
    GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
    GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    // Initialize network
    initNetwork();

    // Set random seed
    srand(time(nullptr));

    // Set up callbacks
    glutDisplayFunc(display);
    glutIdleFunc(update);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);

    // Start main loop
    glutMainLoop();
    return 0;
}