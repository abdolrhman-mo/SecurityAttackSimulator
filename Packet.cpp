#include "Packet.h"
#include <cmath>

using namespace std;

void Packet::setColor(float r, float g, float b) {
    color[0] = r;
    color[1] = g;
    color[2] = b;
}

Packet::Packet(Node* src, Node* dest, Link* pathLinks[], int numLinks, PacketType type)
    : source(src), destination(dest), type(type),
    progress(0.0f), speed(1.0f), size(0.15f), isActive(false),
    currentLinkIndex(0), startTime(0), pathLength(0) {

    // Copy path
    for (int i = 0; i < numLinks && i < MAX_PATH_LENGTH; i++) {
        path[i] = pathLinks[i];
        pathLength++;
    }

    securityStatus = { false, false, 0.0f, 0 };

    // Set speed based on type
    /*switch (type) {
    case PacketType::DDOS: speed = 1.5f; break;
    case PacketType::INTRUSION: speed = 0.7f; break;
    default: speed = 1.0f; break;
    }*/
    switch (type) {
    case PacketType::DDOS: speed = 0.75f; break;
    case PacketType::INTRUSION: speed = 0.35f; break;
    default: speed = 0.5f; break;
    }

    updateColor();
}

void Packet::updateColor() {
    switch (type) {
    case PacketType::NORMAL:    setColor(0.0f, 1.0f, 0.0f); break; // Green
    case PacketType::DDOS:      setColor(1.0f, 0.0f, 0.0f); break; // Red
    case PacketType::INTRUSION: setColor(1.0f, 0.5f, 0.0f); break; // Orange
    case PacketType::ENCRYPTED: setColor(0.0f, 0.0f, 1.0f); break; // Blue
    case PacketType::BLOCKED:   setColor(1.0f, 1.0f, 0.0f); break; // Yellow
    }
}

void Packet::setSecurityStatus(bool blocked, bool detected, float threat) {
    securityStatus = { blocked, detected, threat, securityStatus.packetCount };
}

void Packet::updateSecurityStatus() {
    // Normal and encrypted packets pass
    if (type == PacketType::NORMAL || type == PacketType::ENCRYPTED) {
        securityStatus.isBlocked = false; // Never block normal/encrypted
        return;
    }

    if (type != PacketType::DDOS && type != PacketType::INTRUSION) return;

    // Increase threat level
    securityStatus.threatLevel += (type == PacketType::DDOS) ? 0.002f : 0.001f;
    if (securityStatus.threatLevel > 1.0f) securityStatus.threatLevel = 1.0f;

    // Check for detection
    float detectionChance = securityStatus.threatLevel * 0.5f;
    if (type == PacketType::DDOS) {
        securityStatus.packetCount++;
        detectionChance += securityStatus.packetCount * 0.01f;
    }

    if (!securityStatus.isDetected && ((float)rand() / RAND_MAX) < detectionChance) {
        securityStatus.isDetected = true;
        if (destination->getType() == NodeType::FIREWALL) {
            securityStatus.isBlocked = true;
            type = PacketType::BLOCKED;
            updateColor();
            stop();
        }
    }
}

void Packet::start() {
    isActive = true;
    startTime = glutGet(GLUT_ELAPSED_TIME);
}

void Packet::stop() {
    isActive = false;
}

void Packet::reset() {
    currentLinkIndex = 0;
    progress = 0.0f;
    isActive = false;
}

bool Packet::isFinished() const {
    return currentLinkIndex >= pathLength;
}

Link* Packet::getPathLink(int index) const {
    return (index >= 0 && index < pathLength) ? path[index] : nullptr;
}

void Packet::update() {
    if (!isActive || isFinished()) return;

    unsigned long currentTime = glutGet(GLUT_ELAPSED_TIME);
    progress = (currentTime - startTime) / 1000.0f * speed;

    updateSecurityStatus();

    while (progress >= 1.0f && currentLinkIndex < pathLength) {
        progress -= 1.0f;
        currentLinkIndex++;
        if (currentLinkIndex < pathLength) {
            startTime = currentTime - (unsigned long)(progress * 1000.0f / speed);
        }
    }

    if (currentLinkIndex >= pathLength) {
        isActive = false;
    }
}

void Packet::calculatePosition(float& x, float& y, float& z) {
    if (currentLinkIndex >= pathLength) {
        x = destination->getX();
        y = destination->getY();
        z = destination->getZ();
        return;
    }

    Link* currentLink = path[currentLinkIndex];
    Node* start = currentLink->getSource();
    Node* end = currentLink->getTarget();

    x = start->getX() + (end->getX() - start->getX()) * progress;
    y = start->getY() + (end->getY() - start->getY()) * progress;
    z = start->getZ() + (end->getZ() - start->getZ()) * progress;
}

void Packet::drawSecurityEffect() {
    if (!securityStatus.isDetected) return;

    float x, y, z;
    calculatePosition(x, y, z);

    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(glutGet(GLUT_ELAPSED_TIME) / 10.0f, 0.0f, 1.0f, 0.0f);

    if (securityStatus.isBlocked) {
        glColor4f(1.0f, 0.0f, 0.0f, 0.7f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex3f(-size, -size, 0); glVertex3f(size, size, 0);
        glVertex3f(-size, size, 0); glVertex3f(size, -size, 0);
        glEnd();
    }
    else {
        glColor4f(1.0f, 0.7f, 0.0f, 0.7f);
        glBegin(GL_TRIANGLES);
        glVertex3f(0, size * 1.5f, 0);
        glVertex3f(-size, -size, 0);
        glVertex3f(size, -size, 0);
        glEnd();
    }

    glPopMatrix();
}

void Packet::draw() {
    if (!isActive || isFinished()) return;

    float x, y, z;
    calculatePosition(x, y, z);

    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3f(color[0], color[1], color[2]);

    switch (type) {
    case PacketType::NORMAL:
        glutSolidSphere(size, 8, 8);
        break;
    case PacketType::DDOS:
        for (int i = 0; i < 3; i++) {
            glPushMatrix();
            glTranslatef(sin(i * 2.1f) * size, cos(i * 2.1f) * size, 0);
            glutSolidCube(size);
            glPopMatrix();
        }
        break;
    case PacketType::INTRUSION:
        glScalef(size, size, size);
        glBegin(GL_TRIANGLES);
        glVertex3f(0, 1, 0); glVertex3f(-1, 0, 0); glVertex3f(1, 0, 0);
        glVertex3f(0, -1, 0); glVertex3f(-1, 0, 0); glVertex3f(1, 0, 0);
        glEnd();
        break;
    case PacketType::ENCRYPTED:
        glPushMatrix();
        glTranslatef(x, y, z);
        glScalef(size * encryptedSizeMult,
            size * encryptedSizeMult,
            size * encryptedSizeMult);
        glutSolidOctahedron();
        glPopMatrix();
        break;
    case PacketType::BLOCKED:
        glutSolidTetrahedron();
        break;
    }

    glPopMatrix();
    drawSecurityEffect();

    // Draw trail
    if (currentLinkIndex < pathLength) {
        Link* currentLink = path[currentLinkIndex];
        Node* start = currentLink->getSource();
        Node* end = currentLink->getTarget();

        glBegin(GL_LINE_STRIP);
        glColor4f(color[0], color[1], color[2], 0.5f);
        for (float t = 0.0f; t < 0.5f; t += 0.1f) {
            glVertex3f(x - (end->getX() - start->getX()) * t,
                y - (end->getY() - start->getY()) * t,
                z - (end->getZ() - start->getZ()) * t);
        }
        glEnd();
    }
}