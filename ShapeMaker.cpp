#include "ShapeMaker.h"

InsertShapeCmd::InsertShapeCmd(ShapeMaker& maker, int sx, int sy, int ex, int ey, int sh, int fill) : _maker(maker), _ptr(nullptr), _sh(sh), _fill(fill)
{
	int tlx = min(sx,ex);
	int tly = min(sy,ey);
	int brx = max(sx,ex);
	int bry = max(sy,ey);
	_tl = EC2DPoint(tlx, tly);
	_br = EC2DPoint(brx, bry);
}

InsertShapeCmd::~InsertShapeCmd()
{

}

void InsertShapeCmd::Execute()
{
	_maker.AddShapeAt(_tl, _br, _sh, _fill, _ptr);
}

void InsertShapeCmd::UnExecute()
{
	_maker.RemoveShape(_ptr, false);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ChangePositionCmd::	ChangePositionCmd(ShapeMaker& maker, set<ECAbstract2DShape*>& shapes, int shiftx, int shifty) : _maker(maker), _shapes(shapes), _x(shiftx), _y(shifty)
{

}

ChangePositionCmd::~ChangePositionCmd()
{
	_shapes.clear();
}

void ChangePositionCmd::Execute()
{
	for (auto shape : _shapes)
		_maker.MoveShape(shape, _x, _y);
}

void ChangePositionCmd::UnExecute()
{
	for (auto shape : _shapes)
		_maker.MoveShape(shape, -_x, -_y);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

DeleteShapeCmd::DeleteShapeCmd(ShapeMaker& maker, set<ECAbstract2DShape*>& shapes, bool comp) : _maker(maker), _shapes(shapes), _comp(comp)
{

}

DeleteShapeCmd::~DeleteShapeCmd()
{
	_shapes.clear();
}

void DeleteShapeCmd::Execute()
{
	for (auto shape : _shapes)
		_maker.RemoveShape(shape, _comp);
}

void DeleteShapeCmd::UnExecute()
{
	for (auto shape : _shapes)
	{
		EC2DPoint tl = shape->GetTL();
		EC2DPoint br = shape->GetBR();
		_maker.AddShapeAt(tl, br, -1, -1, shape);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

CompositeShapeCmd::CompositeShapeCmd(ShapeMaker& maker, set<ECAbstract2DShape*>& shapes) : _maker(maker), _shapes(shapes), _ptr(nullptr)
{

}

CompositeShapeCmd::~CompositeShapeCmd()
{
	_shapes.clear();
}

void CompositeShapeCmd::Execute()
{
	_maker.AddShapeAt(_shapes, _ptr);
}

void CompositeShapeCmd::UnExecute()
{
	_maker.RemoveShape(_ptr, true);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ShapeMakerCtrl::ShapeMakerCtrl(ECGraphicViewImp& gui, ShapeMaker& maker) : _maker(maker), _view(gui), _mode(0), _shape(0), _fill(0), _click(false)
{

}

ShapeMaker& ShapeMakerCtrl::GetMaker()
{
	return _maker;
}

ECGraphicViewImp& ShapeMakerCtrl::GetView()
{
	return _view;
}

void ShapeMakerCtrl::Click()
{
	int cx = -1;
	int cy = -1;
	_view.GetCursorPosition(cx, cy);

	_start = EC2DPoint(cx, cy);

	_click = true;
	if (_mode == 0)
	{
		if (!_group)
			_selected.clear();

		ECAbstract2DShape* shape = _maker.GetShapeAt(_start);

		if (shape != nullptr)
			_selected.insert(shape);
		else
			_click = false;
	}
}

void ShapeMakerCtrl::Release()
{
	int cx = -1;
	int cy = -1;
	_view.GetCursorPosition(cx, cy);

	if (_click)
	{
		if (_mode == 1)
		{
			ECCommand* cmd = new InsertShapeCmd(GetMaker(), _start.GetX(), _start.GetY(), cx, cy, _shape, _fill);
			_histCmds.ExecuteCmd(cmd);
		}
		else if (_start.GetX() != cx || _start.GetY() != cy)
		{
			int x = cx - _start.GetX();
			int y = cy - _start.GetY();
			ECCommand* cmd = new ChangePositionCmd(GetMaker(), _selected, x, y);
    		_histCmds.ExecuteCmd(cmd);
		}

	}

	_view.SetRedraw(true);
	_click = false;
}

void ShapeMakerCtrl::ChangeMode()
{
	_mode = (_mode + 1) % 2;
	_shape = 0;
	_fill = 0;
	Reset();
}

bool ShapeMakerCtrl::Undo()
{
	bool res = _histCmds.Undo();
	Reset();
	return res;
}

bool ShapeMakerCtrl::Redo()
{
	bool res = _histCmds.Redo();
	Reset();
	return res;
}

void ShapeMakerCtrl::DeleteShape()
{
	if (_mode == 0 && _selected.size())
	{
		ECCommand* cmd = new DeleteShapeCmd(GetMaker(), _selected, false);
		_histCmds.ExecuteCmd(cmd);
		Reset();
	}
}

void ShapeMakerCtrl::PressG()
{
	if (_mode == 1)
		_shape = (_shape + 1) % 2;
	else
	{
		if (_selected.size() > 1)
		{
			ECCommand* cmd = new CompositeShapeCmd(GetMaker(), _selected);
			_histCmds.ExecuteCmd(cmd);
		}
		if (_selected.size() == 1 && dynamic_cast<Composite*>(*_selected.begin()) != nullptr)
		{
			ECCommand* cmd = new DeleteShapeCmd(GetMaker(), _selected, true);
			_histCmds.ExecuteCmd(cmd);
		}
		Reset();
	}
}

void ShapeMakerCtrl::ChangeFill()
{
	_fill = (_fill + 1) % 2;
}

void ShapeMakerCtrl::MoveShape(int x, int y)
{
	if (_selected.size())
	{
		ECCommand* cmd = new ChangePositionCmd(GetMaker(), _selected, x, y);
		_histCmds.ExecuteCmd(cmd);
		_view.SetRedraw(true);
	}
}

void ShapeMakerCtrl::DownCtrl()
{
	_group = true;
}

void ShapeMakerCtrl::UpCtrl()
{
	_group = false;
}

void ShapeMakerCtrl::Populate()
{
	_maker.Draw(_maker.GetShapes());

	int cx = -1;
	int cy = -1;
	_view.GetCursorPosition(cx, cy);

	if (_click)
	{
		if (_mode == 1)
			Insert(cx, cy);
		else
		{
			vector<ECAbstract2DShape*> shapes(_selected.begin(), _selected.end());
			Select(shapes, cx - _start.GetX(), cy - _start.GetY(), ECGV_CYAN);
		}
		_view.SetRedraw(true);
	}

	if (_selected.size())
	{
		vector<ECAbstract2DShape*> shapes(_selected.begin(), _selected.end());
		Select(shapes, 0, 0, ECGV_BLUE);
		_view.SetRedraw(true);
	}
}

void ShapeMakerCtrl::Reset()
{
	_click = false;
	_selected.clear();
	_view.SetRedraw(true);
}

void ShapeMakerCtrl::Insert(int cx, int cy)
{
	if (_shape == 0)
	{
		if (_fill == 0)
			_view.DrawRectangle(_start.GetX(), _start.GetY(), cx, cy, 3, ECGV_PURPLE);
		else
			_view.DrawFilledRectangle(_start.GetX(), _start.GetY(), cx, cy, ECGV_PURPLE);
	}
	if (_shape == 1)
	{
		int cenx = (_start.GetX() + cx) / 2;
		int ceny = (_start.GetY() + cy) / 2;
		int rx = abs(cenx - _start.GetX());
		int ry = abs(ceny - _start.GetY());

		if (_fill == 0)
			_view.DrawEllipse(cenx, ceny, rx, ry, 3, ECGV_PURPLE);
		else
			_view.DrawFilledEllipse(cenx, ceny, rx, ry, ECGV_PURPLE);
	}
}

void ShapeMakerCtrl::Select(vector<ECAbstract2DShape*> shapes, int x, int y, ECGVColor color)
{
	for (auto shape : shapes)
	{

		Rectangle* rect = dynamic_cast<Rectangle*>(shape);
		Ellipse* elli = dynamic_cast<Ellipse*>(shape);
		Composite* comp = dynamic_cast<Composite*>(shape);

		if (rect != nullptr)
		{
			EC2DPoint tl = shape->GetTL();
			EC2DPoint br = shape->GetBR();

			if (rect->Fill())
				_view.DrawFilledRectangle(tl.GetX() + x, tl.GetY() + y, br.GetX() + x, br.GetY() + y, color);
			else
				_view.DrawRectangle(tl.GetX() + x, tl.GetY() + y, br.GetX() + x, br.GetY() + y, 3, color);
		}
		if (elli != nullptr)
		{
			EC2DPoint cen = elli->GetCenter();
			int rx = elli->GetRadiusX();
			int ry = elli->GetRadiusY();

			if (elli->Fill())
				_view.DrawFilledEllipse(cen.GetX() + x, cen.GetY() + y, rx, ry, color);
			else
				_view.DrawEllipse(cen.GetX() + x, cen.GetY() + y, rx, ry, 3, color);
		}
		if (comp != nullptr)
		{
			Select(comp->GetChildren(), x, y, color);
		}
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ShapeMaker::ShapeMaker(ECGraphicViewImp& gui) : _view(gui), _ctrl(gui, *this)
{

}

ShapeMaker::~ShapeMaker()
{
	for (auto i : _shapes)
		delete i;
	_shapes.clear();
}

ShapeMakerCtrl& ShapeMaker::GetCtrl()
{
    return _ctrl;
}

void ShapeMaker::AddShapeAt(const EC2DPoint& tl, const EC2DPoint& br, int shape, int fill, ECAbstract2DShape*& ptr)
{
	_hide[ptr] = false;
	if (ptr != nullptr)
	{
		for (auto i : ptr->GetChildren())
		{
			EC2DPoint temp(-1, -1);
			AddShapeAt(temp, temp, -1, -1, i);
		}
		return;
	}

	if (shape == 0)
		ptr = new Rectangle(tl, br, (bool)fill);
	if (shape == 1)
		ptr = new Ellipse(tl, br, (bool)fill);

	_shapes.push_back(ptr);
}

void ShapeMaker::AddShapeAt(const set<ECAbstract2DShape*>& shapes, ECAbstract2DShape*& ptr)
{
	_hide[ptr] = false;
	if (ptr != nullptr)	
		return;	
	
	ptr = new Composite(shapes);
	_shapes.push_back(ptr);
}

void ShapeMaker::MoveShape(ECAbstract2DShape* ptr, int x, int y) 
{
	EC2DPoint pretl = ptr->GetTL();
	EC2DPoint prebr = ptr->GetBR();
	EC2DPoint curtl = EC2DPoint(pretl.GetX() + x, pretl.GetY() + y);
	EC2DPoint curbr = EC2DPoint(prebr.GetX() + x, prebr.GetY() + y);

	ptr->SetTL(curtl);
	ptr->SetBR(curbr);
}

void ShapeMaker::RemoveShape(ECAbstract2DShape* ptr, bool comp)
{
	_hide[ptr] = true;
	if (!comp)
		for (auto i : ptr->GetChildren())
			RemoveShape(i, comp);
}

ECAbstract2DShape* ShapeMaker::GetShapeAt(const EC2DPoint& point)
{
	for (auto i = _shapes.rbegin(); i != _shapes.rend(); i++)
	{
		auto shape = *i;

		if (_hide[shape])
			continue;

		if (shape->IsPointInside(point))
			return shape;
	}

	return nullptr;
}

void ShapeMaker::Draw(const vector<ECAbstract2DShape*>& shapes)
{
	for (auto shape : shapes)
	{
		if (_hide[shape])
			continue;

		Rectangle* rect = dynamic_cast<Rectangle*>(shape);
		Ellipse* elli = dynamic_cast<Ellipse*>(shape);
		Composite* comp = dynamic_cast<Composite*>(shape);

		if (rect != nullptr)
		{
			if (rect->Fill())
				_view.DrawFilledRectangle(shape->GetTL().GetX(), shape->GetTL().GetY(), shape->GetBR().GetX(), shape->GetBR().GetY());
			else
				_view.DrawRectangle(shape->GetTL().GetX(), shape->GetTL().GetY(), shape->GetBR().GetX(), shape->GetBR().GetY());	
		}
		if (elli != nullptr)
		{
			EC2DPoint center = elli->GetCenter();
			int rx = elli->GetRadiusX();
			int ry = elli->GetRadiusY();

			if (elli->Fill())
				_view.DrawFilledEllipse(center.GetX(), center.GetY(), rx, ry);
			else
				_view.DrawEllipse(center.GetX(), center.GetY(), rx, ry);	
		}
		if (comp != nullptr)
			Draw(shape->GetChildren());
	}
}

void ShapeMaker::LoadShapes(const string& fileName)
{
	try
	{
		ifstream file;
		file.open(fileName);
		if (file.is_open())
		{
			string line;
			getline(file, line);
			int n = stoi(line);

			while (n)
			{
				getline(file, line);
				AddShape(file, line);
				n -= 1;
			}
			file.close();
		}

		_view.SetRedraw(true);
	}
	catch(...)
	{
		_shapes.clear();
		cout << "Cannot load from file\n";
	}
}

void ShapeMaker::SaveShapes(const string& fileName)
{
	ofstream file;
	file.open(fileName);
	if (file.is_open())
  	{
  		map<ECAbstract2DShape*, string> output;
  		set<ECAbstract2DShape*> use;

    	for (auto i : _shapes)
    		if (!_hide[i])
    		{
    			if (output.find(i) != output.end())
    				continue;
    			WriteShape(output, i);

    			use.insert(i);
    			for (auto child : i->GetChildren())
    			{
    				auto place = use.find(child);
    				if (place != use.end())
    					use.erase(place);
    			}
    		}

    	file << to_string(use.size());
    	for (auto i : use)
    		file << endl << output[i];
    	
    	file.close();
  	}
}

ECAbstract2DShape* ShapeMaker::AddShape(ifstream& file, const string& data)
{
	vector<int> info;

	int start = 0;
	int end = -1;
	do {
		start = end + 1;
		end = data.find(" ", start);
		info.push_back(stoi(data.substr(start, end - start)));
	} while (end != -1);

	int shape = info[0];

	int sh = shape % 2;
	int fill = shape < 2 ? 0 : 1;

	if (shape == 4)
	{
		ECAbstract2DShape* added = nullptr;
		set<ECAbstract2DShape*> children;

		string line;
		int n = info[1];

		while (n)
		{
			getline(file, line);
			ECAbstract2DShape* child = AddShape(file, line);
			children.insert(child);
			n -= 1;
		}

		AddShapeAt(children, added);

		return added;
	}

	int tlx = 1e6;
	int tly = 1e6;
	int brx = -1e6;
	int bry = -1e6;
	if (sh == 0)
	{
		for (auto i = info.begin() + 2; i != info.end() - 1; i += 2)
		{
			tlx = min(tlx, *i);
			tly = min(tly, *(i + 1));
			brx = max(brx, *i);
			bry = max(bry, *(i + 1));
		}
	}
	else
	{
		tlx = info[1] - info[3];
		tly = info[2] - info[4];
		brx = info[1] + info[3];
		bry = info[2] + info[4];
	}

	EC2DPoint tl(tlx, tly);
	EC2DPoint br(brx, bry);

	ECAbstract2DShape* added = nullptr;
	AddShapeAt(tl, br, sh, fill, added);

	return added;
}

void ShapeMaker::WriteShape(map<ECAbstract2DShape*, string>& output, ECAbstract2DShape* ptr)
{
	Rectangle* rect = dynamic_cast<Rectangle*>(ptr);
	Ellipse* elli = dynamic_cast<Ellipse*>(ptr);
	Composite* comp = dynamic_cast<Composite*>(ptr);

	string format = "";
	if (rect != nullptr)
	{
		int sh = 0;
		if (rect->Fill())
			sh += 2;
		EC2DPoint tl = rect->GetTL();
		EC2DPoint br = rect->GetBR();

		format += to_string(sh) + " " + to_string(4) + " "; 

		vector<int> x = {tl.GetX(), br.GetX()};
		vector<int> y = {tl.GetY(), br.GetY()};

		for (int i = 0; i < x.size(); i++)
			for (int j = 0; j < y.size(); j++)
				format += to_string(x[i]) + " " + to_string(y[j]) + " ";

		format += "0";

	}
	if (elli != nullptr)
	{
		int sh = 1;
		if (elli->Fill())
			sh += 2;
		EC2DPoint cen = elli->GetCenter();
		format += to_string(sh) + " " + to_string(cen.GetX()) + " " + to_string(cen.GetY()) + " " + to_string(elli->GetRadiusX()) + " " + to_string(elli->GetRadiusY()) + " 0";
	}
	if (comp != nullptr)
	{
		format += "4 " + to_string(comp->GetChildren().size());
		for (auto i : comp->GetChildren())
		{
			WriteShape(output, i);
			format += "\n" + output[i];
		}
	}

	output[ptr] = format;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

MouseObserver::MouseObserver(ECGraphicViewImp& view, ShapeMakerCtrl& ctrl) : _view(view), _ctrl(ctrl)
{

}

void MouseObserver::Update()
{
	ECGVEventType action = _view.GetCurrEvent();

	switch (action)
	{
		case ECGV_EV_MOUSE_BUTTON_DOWN:
			_ctrl.Click();
			break;
		case ECGV_EV_MOUSE_BUTTON_UP:
			_ctrl.Release();
			break;
	}

	_ctrl.Populate();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

KeyBoardObserver::KeyBoardObserver(ECGraphicViewImp& view, ShapeMakerCtrl& ctrl) : _view(view), _ctrl(ctrl)
{

}

void KeyBoardObserver::Update()
{
	ECGVEventType action = _view.GetCurrEvent();

	switch (action)
	{
		case ECGV_EV_KEY_DOWN_SPACE:
			_ctrl.ChangeMode();
			return;
		case ECGV_EV_KEY_DOWN_Z:
			_ctrl.Undo();
			return;
		case ECGV_EV_KEY_DOWN_Y:
			_ctrl.Redo();
			return;
		case ECGV_EV_KEY_DOWN_D:
			_ctrl.DeleteShape();
			return;
		case ECGV_EV_KEY_DOWN_G:
			_ctrl.PressG();
			return;
		case ECGV_EV_KEY_DOWN_F:
			_ctrl.ChangeFill();
			return;
		case ECGV_EV_KEY_DOWN_UP:
			_ctrl.MoveShape(0,-10);
			return;
		case ECGV_EV_KEY_DOWN_DOWN:
			_ctrl.MoveShape(0,10);
			return;
		case ECGV_EV_KEY_DOWN_LEFT:
			_ctrl.MoveShape(-10,0);
			return;
		case ECGV_EV_KEY_DOWN_RIGHT:
			_ctrl.MoveShape(10,0);
			return; 
		case ECGV_EV_KEY_DOWN_CTRL:
			_ctrl.DownCtrl();
			return;
		case ECGV_EV_KEY_UP_CTRL:
			_ctrl.UpCtrl();
			return;
	}
}