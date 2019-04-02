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

private:
    int count;
};

#endif // MODEL_H
