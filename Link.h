#ifndef LINK_H
#define LINK_H

#include "Node.h"
#include <GL/glut.h>
#include <string>

using namespace std;

enum class LinkState {
    ACTIVE,
    INACTIVE,
    ATTACKED
};

class Link {
private:
    Node* sourceNode;
    Node* targetNode;
    float color[3];          // RGB color
    float width;             // Line width
    LinkState state;
    bool isDirectional;      // Whether the link shows direction
    bool isSelected;
    bool isHighlighted;
    string label;
    float bandwidth;
    float latency;

public:
    Link(Node* source, Node* target, bool directional = false, const string& label = "");

    // Getters and setters
    void setState(LinkState newState);
    void setColor(float r, float g, float b);
    void setWidth(float w);
    void setSelected(bool selected);
    void setHighlighted(bool highlighted);
    void setLabel(const string& label);
    void setBandwidth(float bw);
    void setLatency(float lat);

    // Render the link
    void draw();

    // Get link properties
    Node* getSource() const { return sourceNode; }
    Node* getTarget() const { return targetNode; }
    LinkState getState() const { return state; }
    const string& getLabel() const { return label; }
    float getBandwidth() const { return bandwidth; }
    float getLatency() const { return latency; }
    bool getIsSelected() const { return isSelected; }
    bool getIsHighlighted() const { return isHighlighted; }

private:
    void drawArrow(float x1, float y1, float z1,
        float x2, float y2, float z2);
};

#endif