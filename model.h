#ifndef MODEL_H
#define MODEL_H

//#include "view.h" //nix gut, circular dependency -> forward declaration.
class View;         //Make sure each header can be included on its own.

class Model
{
public:
    Model(View& view);
    void setData(int data);
    int getData();
    void updateView();

private:
    int count;
    View* view;
};

#endif // MODEL_H
