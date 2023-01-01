//
//  ECCommand.cpp
//  
//
//  Created by Yufeng Wu on 2/26/20.
//
//

#include "ECCommand.h"

// ******************************************************
// Implement command history


ECCommandHistory :: ECCommandHistory() 
{

}

ECCommandHistory :: ~ECCommandHistory()
{
    for (ECCommand* i : _u)
        delete i;
    for (ECCommand* i : _r)
        delete i;
    _u.clear();
    _r.clear();
}

bool ECCommandHistory::Undo()
{
    // checks if there are commands on the undo stack
    if (_u.size())
    {
        // removes the latest command from undo stack, unexecute it, and add it to the redo stack
        ECCommand* cur = _u.back();
        _u.pop_back();
        cur->UnExecute();
        _r.push_back(cur);
        return true;
    }
    return false;
}

bool ECCommandHistory::Redo()
{
    // checks if there are commands on the redo stack
    if (_r.size())
    {
        // removes the latest command from redo stack, execute it, and add it to the undo stack
        ECCommand* cur = _r.back();
        _r.pop_back();
        cur->Execute();
        _u.push_back(cur);
        return true;
    }
    return false;
}

void ECCommandHistory::ExecuteCmd( ECCommand *pCmd )
{
    // executes the command and add it to the undo stack
    pCmd->Execute();
    _u.push_back(pCmd);
    // clears out all commands from the redo stack
    _r.clear();
}