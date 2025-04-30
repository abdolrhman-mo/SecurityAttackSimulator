#include "Link.h"
#include <cmath>

Link::Link(Node* source, Node* target, bool directional, const string& label)
    : sourceNode(source), targetNode(target),
    isDirectional(directional), width(2.0f), state(LinkState::ACTIVE),
    isSelected(false), isHighlighted(false), label(label),
    bandwidth(100.0f), latency(0.0f) {
    setColor(0.5f, 0.5f, 0.5f);  // Default gray color
}

void Link::setState(LinkState newState) {
    state = newState;
    switch (state) {
    case LinkState::ACTIVE:
        setColor(0.5f, 0.5f, 0.5f);  // Gray
        break;
    case LinkState::INACTIVE:
        setColor(0.2f, 0.2f, 0.2f);  // Dark gray
        break;
    case LinkState::ATTACKED:
        setColor(1.0f, 0.0f, 0.0f);  // Red
        break;
    }
}

void Link::setColor(float r, float g, float b) {
    color[0] = r;
    color[1] = g;
    color[2] = b;
}

void Link::setWidth(float w) {
    width = w;
}

void Link::setSelected(bool selected) {
    isSelected = selected;
}

void Link::setHighlighted(bool highlighted) {
    isHighlighted = highlighted;
}

void Link::setLabel(const string& newLabel) {
    label = newLabel;
}

void Link::setBandwidth(float bw) {
    bandwidth = bw;
}

void Link::setLatency(float lat) {
    latency = lat;
}

void Link::draw() {
    if (!sourceNode || !targetNode) return;

    glPushMatrix();

    // Highlight if selected
    if (isSelected) {
        glColor3f(1.0f, 1.0f, 0.0f);  // Yellow
        glLineWidth(width + 2.0f);
    }
    // Highlight if needed
    else if (isHighlighted) {
        glColor3f(0.0f, 1.0f, 1.0f);  // Cyan
        glLineWidth(width + 1.0f);
    }
    else {
        glColor3f(color[0], color[1], color[2]);
        glLineWidth(width);
    }

    float x1 = sourceNode->getX();
    float y1 = sourceNode->getY();
    float z1 = sourceNode->getZ();
    float x2 = targetNode->getX();
    float y2 = targetNode->getY();
    float z2 = targetNode->getZ();

    // Draw main line
    glBegin(GL_LINES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glEnd();

    // Draw direction arrow if needed
    if (isDirectional) {
        drawArrow(x1, y1, z1, x2, y2, z2);
    }

    // Draw activity indicator
    if (state == LinkState::ACTIVE || state == LinkState::ATTACKED) {
        // Animate a small dot moving along the link
        float t = (glutGet(GLUT_ELAPSED_TIME) % 1000) / 1000.0f;
        float xt = x1 + (x2 - x1) * t;
        float yt = y1 + (y2 - y1) * t;
        float zt = z1 + (z2 - z1) * t;

        glPointSize(5.0f);
        glBegin(GL_POINTS);
        glVertex3f(xt, yt, zt);
        glEnd();
    }

    // Draw label if it exists
    if (!label.empty()) {
        float midX = (x1 + x2) / 2.0f;
        float midY = (y1 + y2) / 2.0f;
        float midZ = (z1 + z2) / 2.0f;

        glRasterPos3f(midX, midY + 0.2f, midZ);
        for (char c : label) {
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
        }
    }

    glPopMatrix();
}

void Link::drawArrow(float x1, float y1, float z1,
    float x2, float y2, float z2) {
    // Calculate direction vector
    float dx = x2 - x1;
    float dy = y2 - y1;
    float dz = z2 - z1;

    // Normalize
    float length = sqrt(dx * dx + dy * dy + dz * dz);
    dx /= length;
    dy /= length;
    dz /= length;

    // Calculate arrow position (80% along the line)
    float arrowPos = 0.8f;
    float ax = x1 + dx * length * arrowPos;
    float ay = y1 + dy * length * arrowPos;
    float az = z1 + dz * length * arrowPos;

    // Calculate perpendicular vectors for arrow head
    float px = -dy;
    float py = dx;
    float pz = 0;

    // Draw arrow head
    glBegin(GL_TRIANGLES);
    glVertex3f(ax + px * 0.1f, ay + py * 0.1f, az + pz * 0.1f);
    glVertex3f(ax - px * 0.1f, ay - py * 0.1f, az - pz * 0.1f);
    glVertex3f(x2, y2, z2);
    glEnd();
}