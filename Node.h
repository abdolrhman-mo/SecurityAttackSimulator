#ifndef NODE_H
#define NODE_H

#include <GL/glut.h>
#include <string>

using namespace std;

enum class NodeType {
    ROUTER,
    COMPUTER,
    FIREWALL
};

class Node {
private:
    float x, y, z;           // Position
    float size;              // Node size
    float color[3];          // RGB color
    string label;            // Node label
    NodeType type;           // Type of node
    bool isSelected;         // Selection state
    bool isHighlighted;      // Highlight state
    int connectionCount;     // Number of connections

public:
    Node(float x, float y, float z, NodeType type, const string& label = "");

    // Getters and setters
    void setPosition(float x, float y, float z);
    void setColor(float r, float g, float b);
    void setSelected(bool selected);
    void setHighlighted(bool highlighted);
    void setType(NodeType type);
    void setLabel(const string& label);

    // Render the node
    void draw();
    void drawComputer();

    // Check if a point is inside the node (for selection)
    bool containsPoint(float px, float py, float pz);

    // Get node properties
    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }
    NodeType getType() const { return type; }
    const string& getLabel() const { return label; }
    bool getIsSelected() const { return isSelected; }
    bool getIsHighlighted() const { return isHighlighted; }
};

#endif