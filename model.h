#ifndef MODEL_H
#define MODEL_H

#include <QWidget>
//#include "view.h" //nix gut, circular dependency -> forward declaration.
//class View;         //Make sure each header can be included on its own.

class Model : public QObject
{
    Q_OBJECT        //qmake vor erstem kompilieren laufen lassen!

signals:
    void updateView();

public:
    Model();
    void setData(int data);
    int getData();
    void setAxis(int x, int y);
    void getAxis(int &x, int &y);

private:
    int count;
    int x;
    int y;
};

#endif // MODEL_H
