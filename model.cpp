#include "model.h"
#include "view.h"

Model::Model()
{
    count = 0;
}

void Model::setData(int data){
    count = data;
    emit updateView();
}

int Model::getData(void){
    return count;
}
