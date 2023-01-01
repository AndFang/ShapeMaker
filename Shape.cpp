#include "Shape.h"
#include <algorithm>

Rectangle::Rectangle(const EC2DPoint& tl, const EC2DPoint& br, bool fill) : _fill(fill)
{
	_tl = tl;
	_br = br;
}

bool Rectangle::IsPointInside(const EC2DPoint& pt)
{
	bool x = std::min(_tl.GetX(), _br.GetX()) <= pt.GetX() && pt.GetX() <= std::max(_tl.GetX(), _br.GetX());
	bool y = std::min(_tl.GetY(), _br.GetY()) <= pt.GetY() && pt.GetY() <= std::max(_tl.GetY(), _br.GetY());

	return x && y;
}

void Rectangle::SetTL(const EC2DPoint& tl)
{
	_tl = tl;
}

void Rectangle::SetBR(const EC2DPoint& br)
{
	_br = br;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ellipse::Ellipse(const EC2DPoint& tl, const EC2DPoint& br, bool fill) : _fill(fill)
{
	_tl = tl;
	_br = br;

	int _cx = (_tl.GetX() + _br.GetX()) / 2;
	int _cy = (_tl.GetY() + _br.GetY()) / 2;
	_center = EC2DPoint(_cx, _cy);
	_rx = _cx - tl.GetX();
	_ry = _cy - tl.GetY();
}

bool Ellipse::IsPointInside(const EC2DPoint& pt)
{
	double x = _center.GetX() - pt.GetX();
	double y = _center.GetY() - pt.GetY();

	double a = (x * x) / (_rx * _rx);
	double b = (y * y) / (_ry * _ry);

	return a + b <= 1;
}

void Ellipse::SetTL(const EC2DPoint& tl)
{
	_tl = tl;
	int _cx = (_tl.GetX() + _br.GetX()) / 2;
	int _cy = (_tl.GetY() + _br.GetY()) / 2;
	_center = EC2DPoint(_cx, _cy);
}

void Ellipse::SetBR(const EC2DPoint& br)
{
	_br = br;
	int _cx = (_tl.GetX() + _br.GetX()) / 2;
	int _cy = (_tl.GetY() + _br.GetY()) / 2;
	_center = EC2DPoint(_cx, _cy);
}

EC2DPoint Ellipse::GetCenter()
{
	return _center;
}

int Ellipse::GetRadiusX()
{
	return _rx;
}

int Ellipse::GetRadiusY()
{
	return _ry;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Composite::Composite(const std::set<ECAbstract2DShape*>& shapes)
{
	int tlx = 1e6;
	int tly = 1e6;
	int brx = -1e6;
	int bry = -1e6;

	for (auto i : shapes)
	{
		AddChild(i);
		EC2DPoint tl = i->GetTL();
		EC2DPoint br = i->GetBR();

		tlx = std::min(tlx, tl.GetX());
		tly = std::min(tly, tl.GetY());
		brx = std::max(brx, br.GetX());
		bry = std::max(bry, br.GetY());
	}

	_tl = EC2DPoint(tlx, tly);
	_br = EC2DPoint(brx, bry);
}

bool Composite::IsPointInside(const EC2DPoint& pt)
{
	for (auto i : GetChildren())
		if (i->IsPointInside(pt))
			return true;
	return false;
}

void Composite::SetTL(const EC2DPoint& tl)
{
	int x = tl.GetX() - _tl.GetX();
	int y = tl.GetY() - _tl.GetY();

	for (auto i : GetChildren())
	{
		EC2DPoint curtl = i->GetTL();
		EC2DPoint shift(curtl.GetX() + x, curtl.GetY() + y);
		i->SetTL(shift);
	}

	_tl = tl;
}

void Composite::SetBR(const EC2DPoint& br)
{
	int x = br.GetX() - _br.GetX();
	int y = br.GetY() - _br.GetY();

	for (auto i : GetChildren())
	{
		EC2DPoint curbr = i->GetBR();
		EC2DPoint shift(curbr.GetX() + x, curbr.GetY() + y);
		i->SetBR(shift);
	}

	_br = br;
}