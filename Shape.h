#ifndef SHAPE_H
#define SHAPE_H

#include "ECAbstract2DShape.h"
#include <set>

class Rectangle : public ECAbstract2DShape
{
public:
	Rectangle(const EC2DPoint& tl, const EC2DPoint& br, bool fill);
	virtual bool IsPointInside(const EC2DPoint& pt);
	virtual void SetTL(const EC2DPoint& tl);
	virtual void SetBR(const EC2DPoint& br);
	
	bool Fill() const { return _fill; };
private:
	bool _fill;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class Ellipse : public ECAbstract2DShape
{
public:
	Ellipse(const EC2DPoint& tl, const EC2DPoint& br, bool fill);
	virtual bool IsPointInside(const EC2DPoint& pt);
	virtual void SetTL(const EC2DPoint& tl);
	virtual void SetBR(const EC2DPoint& br);

	bool Fill() const { return _fill; };
	EC2DPoint GetCenter();
	int GetRadiusX();
	int GetRadiusY();
private:
	bool _fill;
	EC2DPoint _center;
	int _rx;
	int _ry;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class Composite : public ECAbstract2DShape
{
public:
	Composite(const std::set<ECAbstract2DShape*>& shapes);
	virtual bool IsPointInside(const EC2DPoint& pt);
	virtual void SetTL(const EC2DPoint& tl);
	virtual void SetBR(const EC2DPoint& br);
};
#endif