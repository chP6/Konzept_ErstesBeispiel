#include "model.h"
#include "view.h"
#include <QString>

Model::Model()
{
    count = 0;
    x = 0;
    y = 0;
}

void Model::setData(int data){
    count = data;
    emit updateView();
}

int Model::getData(void){
    return count;
}

void Model::setAxis(int x, int y){
    this->x = x;
    this->y = y;
    emit updateView();
}

void Model::getAxis(int &x, int &y){
    x = this->x;
    y = this->y;
}

void Model::addError(std::string str){
    errorList.prepend(QString::fromStdString(str));
    emit updateView();
}

void Model::clearErrors(){
    errorList.clear();
    emit updateView();
}

QStringList* Model::getErrorList(){
    return &errorList;
}
