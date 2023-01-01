#ifndef EC_ABSTRACT_2D_SHAPE_H
#define EC_ABSTRACT_2D_SHAPE_H

#include <vector>

// point used to store coorinate location
class EC2DPoint 
{
public:
    EC2DPoint() {}
    EC2DPoint(int x, int y) : _pointx(x), _pointy(y) {}
    int GetX() const { return _pointx; }
    int GetY() const { return _pointy; }
    EC2DPoint& operator=(const EC2DPoint &rhs) { _pointx = rhs.GetX(); _pointy = rhs.GetY(); return *this; } 
private:
    int _pointx;
    int _pointy;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// abstract class that defines attributes of shapes
class ECAbstract2DShape
{
public:
    virtual ~ECAbstract2DShape() {}
    virtual bool IsPointInside(const EC2DPoint & pt) = 0;
    virtual void SetTL(const EC2DPoint& tl) = 0;
    virtual void SetBR(const EC2DPoint& br) = 0;

    EC2DPoint GetTL() { return _tl; };
    EC2DPoint GetBR() { return _br; };
    void AddChild(ECAbstract2DShape* add) { _children.push_back(add); };
    std::vector<ECAbstract2DShape*> GetChildren() const { return _children; };
protected:
    EC2DPoint _tl;
    EC2DPoint _br;
private:
    std::vector<ECAbstract2DShape*> _children;
};

#endif