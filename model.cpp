#include "model.h"
#include "view.h"

Model::Model(View& view)
{
    count = 0;
    this->view = &view;
}

void Model::setData(int data){
    count = data;
    emit updateView();
}

int Model::getData(void){
    return count;
}
