#include "Node.h"
#include <cmath>

using namespace std;

Node::Node(float x, float y, float z, NodeType type, const string& label)
    : x(x), y(y), z(z), type(type), label(label), isSelected(false),
    size(0.6f), connectionCount(0) {
    // Set default colors based on node type
    switch (type) {
    case NodeType::COMPUTER:
        setColor(0.2f, 0.6f, 1.0f);  // Blue
        break;
    case NodeType::ROUTER:
        setColor(0.8f, 0.4f, 0.0f);  // Orange
        break;
    case NodeType::FIREWALL:
        setColor(1.0f, 0.2f, 0.2f);  // Red
        break;
    }
}

void Node::setPosition(float newX, float newY, float newZ) {
    x = newX;
    y = newY;
    z = newZ;
}

void Node::setColor(float r, float g, float b) {
    color[0] = r;
    color[1] = g;
    color[2] = b;
}

void Node::setSelected(bool selected) {
    isSelected = selected;
}

void Node::draw() {
    glPushMatrix();
    glTranslatef(x, y, z);

    // If selected, draw highlight
    if (isSelected) {
        glPushMatrix();
        glColor3f(1.0f, 1.0f, 0.0f);  // Yellow highlight
        glutWireCube(size * 1.1f);
        glPopMatrix();
    }

    // Draw node based on type
    glColor3f(color[0], color[1], color[2]);
    switch (type) {
    case NodeType::COMPUTER:
        drawComputer();
        break;
    case NodeType::ROUTER:
        glutSolidSphere(size / 2, 16, 16);
        break;
    case NodeType::FIREWALL:
        glBegin(GL_TRIANGLES);
        glVertex3f(-size / 2, -size / 2, 0);
        glVertex3f(size / 2, -size / 2, 0);
        glVertex3f(0, size / 2, 0);
        glEnd();
        break;
    }

    // Draw label
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos3f(0, size + 0.1f, 0);
    for (char c : label) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    }

    glPopMatrix();
}

void Node::drawComputer() {
    glutSolidCube(size);
}

bool Node::containsPoint(float px, float py, float pz) {
    float dx = px - x;
    float dy = py - y;
    float dz = pz - z;
    return (dx * dx + dy * dy + dz * dz) <= (size * size);
}