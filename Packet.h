#ifndef PACKET_H
#define PACKET_H

#include "Node.h"
#include "Link.h"
#include <GL/glut.h>
#include <string>

using namespace std;

enum class PacketType {
    NORMAL,
    DDOS,           // DDoS attack packet
    INTRUSION,      // Intrusion attempt
    ENCRYPTED,
    BLOCKED
};

struct SecurityStatus {
    bool isBlocked;
    bool isDetected;
    float threatLevel;    // 0.0 to 1.0
    int packetCount;      // For DDoS detection
};

class Packet {
private:
    Node* source;
    Node* destination;
    static const int MAX_PATH_LENGTH = 10;
    Link* path[MAX_PATH_LENGTH];
    int pathLength;
    float progress;          // 0.0 to 1.0
    float speed;            // Units per second
    PacketType type;
    float color[3];         // RGB color
    float size;            // Visual size
    bool isActive;
    unsigned int currentLinkIndex;
    unsigned long startTime;
    SecurityStatus securityStatus;
    float encryptedSizeMult = 1.0f; // Makes encrypted packets 30% smaller than default

public:
    Packet(Node* src, Node* dest, Link* pathLinks[], int numLinks,
        PacketType type = PacketType::NORMAL);

    void update();
    void draw();
    void start();
    void stop();
    void reset();

    void setSecurityStatus(bool blocked, bool detected, float threat);
    SecurityStatus getSecurityStatus() const { return securityStatus; }
    void updateSecurityStatus();

    bool isFinished() const;
    PacketType getType() const { return type; }
    bool getIsActive() const { return isActive; }
    Node* getSource() const { return source; }
    Node* getDestination() const { return destination; }
    int getPathLength() const { return pathLength; }
    Link* getPathLink(int index) const;

private:
    void setColor(float r, float g, float b);
    void updateColor();
    void calculatePosition(float& x, float& y, float& z);
    void drawSecurityEffect();
};

#endif