//
//  Created by Yufeng Wu on 3/2/22.
//
#include "ECGraphicViewImp.h"

#include "ShapeMaker.h"

// Test graphical view code
int real_main(int argc, char **argv)
{
  // checks if a file is passed through
  bool hasFile = false;
  if (argc == 2)
    hasFile = true;

  const int widthWin = 800, heightWin = 800;
  ECGraphicViewImp view(widthWin, heightWin);

  // makes view and observers. Then attach and detach them to the view
  ShapeMaker model(view);
  ECObserver* mouse = new MouseObserver(view, model.GetCtrl());
  ECObserver* board = new KeyBoardObserver(view, model.GetCtrl());

  // loads shapes from file
  if (hasFile)
    model.LoadShapes(argv[1]);

  view.Attach(mouse);
  view.Attach(board);

  view.Show();
  
  view.Detach(mouse); 
  view.Detach(board);

  // saves shapes to file
  if (hasFile)
    model.SaveShapes(argv[1]);

  delete mouse;
  delete board;

  return 0;
}

int main(int argc, char **argv)
{
    return real_main(argc, argv);
    //return al_run_main(argc, argv, real_main);
}

