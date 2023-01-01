#ifndef SHAPEMAKER_H
#define SHAPEMAKER_H

#include "ECGraphicViewImp.h"
#include "ECCommand.h"
#include "Shape.h"
#include <string>
#include <fstream>

using namespace std;

class ShapeMaker;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Command for insertion

class InsertShapeCmd : public ECCommand
{
public:
	InsertShapeCmd(ShapeMaker& maker, int sx, int sy, int ex, int ey, int shape, int fill);
	~InsertShapeCmd();
	virtual void Execute();
	virtual void UnExecute();
private:
	ShapeMaker& _maker;
	EC2DPoint _tl;
	EC2DPoint _br;
	ECAbstract2DShape* _ptr;
	int _sh;
	int _fill;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Command for change position

class ChangePositionCmd : public ECCommand
{
public:
	ChangePositionCmd(ShapeMaker& maker, set<ECAbstract2DShape*>& shapes, int shiftx, int shifty);
	~ChangePositionCmd();
	virtual void Execute();
	virtual void UnExecute();
private:
	ShapeMaker& _maker;
	set<ECAbstract2DShape*> _shapes;
	int _x;
	int _y;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Command for insertion

class DeleteShapeCmd : public ECCommand
{
public:
	DeleteShapeCmd(ShapeMaker& maker, set<ECAbstract2DShape*>& shapes, bool comp);
	~DeleteShapeCmd();
	virtual void Execute();
	virtual void UnExecute();
private:
	ShapeMaker& _maker;
	set<ECAbstract2DShape*> _shapes;
	bool _comp;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Command for composite

class CompositeShapeCmd : public ECCommand
{
public:
	CompositeShapeCmd(ShapeMaker& maker, set<ECAbstract2DShape*>& shapes);
	~CompositeShapeCmd();
	virtual void Execute();
	virtual void UnExecute();
private:
	ShapeMaker& _maker;
	set<ECAbstract2DShape*> _shapes;
	ECAbstract2DShape* _ptr;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Controller for shapes

class ShapeMakerCtrl
{
public:
	ShapeMakerCtrl(ECGraphicViewImp& gui, ShapeMaker& maker);
	ShapeMaker& GetMaker();
	ECGraphicViewImp& GetView();

	void Click();
	void Release();

	void ChangeMode();
	bool Undo();
	bool Redo();
	void DeleteShape();
	void PressG();
	void ChangeFill();
	void MoveShape(int x, int y);
	void DownCtrl();
	void UpCtrl();

	void Populate();
private:
	void Reset();
	void Insert(int cx, int cy);
	void Select(vector<ECAbstract2DShape*> shapes, int x, int y, ECGVColor color);

	ShapeMaker& _maker;
	ECGraphicViewImp& _view;
	ECCommandHistory _histCmds;
	int _mode; // 0 = Insert, 1 = Edit
	int _shape; // 0 = rectangle, 1 = ellipse
	int _fill; // 0 = nofill, 1 = fill

	bool _click;
	bool _group;

	EC2DPoint _start;

	set<ECAbstract2DShape*> _selected;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Model for shapes

class ShapeMaker
{
public:
	ShapeMaker(ECGraphicViewImp& gui);
	~ShapeMaker();
	ShapeMakerCtrl& GetCtrl();
	void AddShapeAt(const EC2DPoint& tl, const EC2DPoint& br, int shape, int fill, ECAbstract2DShape*& ptr);
	void AddShapeAt(const set<ECAbstract2DShape*>& shapes, ECAbstract2DShape*& ptr);
	void MoveShape(ECAbstract2DShape* ptr, int x, int y);
	void RemoveShape(ECAbstract2DShape* ptr, bool comp);
	ECAbstract2DShape* GetShapeAt(const EC2DPoint& point);
	void Draw(const vector<ECAbstract2DShape*>& shapes);

	void LoadShapes(const string& fileName);
	void SaveShapes(const string& fileName);
	vector<ECAbstract2DShape*> GetShapes() const { return _shapes; };
private:
	ECAbstract2DShape* AddShape(ifstream& file, const string& data);
	void WriteShape(map<ECAbstract2DShape*, string>& output, ECAbstract2DShape* ptr);

	ECGraphicViewImp& _view;
	ShapeMakerCtrl _ctrl;
	vector<ECAbstract2DShape*> _shapes;
	map<ECAbstract2DShape*, bool> _hide;
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class MouseObserver : public ECObserver
{
public:
	MouseObserver(ECGraphicViewImp& view, ShapeMakerCtrl& ctrl);
	virtual void Update();
private:
	ECGraphicViewImp& _view;
	ShapeMakerCtrl& _ctrl;
};

class KeyBoardObserver : public ECObserver
{
public:
	KeyBoardObserver(ECGraphicViewImp& view, ShapeMakerCtrl& ctrl);
	virtual void Update();
private:
	ECGraphicViewImp& _view;
	ShapeMakerCtrl& _ctrl;
};
#endif