#include "model.h"
#include "view.h"
#include "config.h"
#include <QString>

Model::Model()
{
    count = 0;
    x = 0;
    y = 0;
    activeCamera=0;
    memset(cameras,0,sizeof (cameras));


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

void Model::setUsedPreset(int presetNr)
{
    cameras[activeCamera].usedPresets|=1<<presetNr;
     //cameras[activeCamera].usedPresets[presetNr]=1;
}

int Model::getUsedPreset()
{
    return cameras[activeCamera].usedPresets;
}

void Model::setActivePreset(unsigned char actPreset)
{
    cameras[activeCamera].activePreset=actPreset;
}

unsigned char Model::getActivePreset()
{
    return cameras[activeCamera].activePreset;
}



void Model::setActiveCamera(unsigned char camNr)
{
    activeCamera=camNr;
}

unsigned char Model::getActiveCamera()
{
    return activeCamera;
}

QStringList* Model::getErrorList(){
    return &errorList;
}

void Model::setCamType(unsigned char type)
{
    switch (type) {
    case 1:
        for(int i= 0;i<ROW_ENTRIES;i++){
            for(int j=0;j<COLUM_ENTRIES;j++){cameras[activeCamera].values[i][j]=c1Values[i][j];}
        }
        cameras[activeCamera].camType=type;
        break;
    case 2:
        for(int i= 0;i<ROW_ENTRIES;i++){
            for(int j=0;j<COLUM_ENTRIES;j++){cameras[activeCamera].values[i][j]=c2Values[i][j];}
        }
        cameras[activeCamera].camType=type;
        break;
    case 6:
        for(int i= 0;i<ROW_ENTRIES;i++){
            for(int j=0;j<COLUM_ENTRIES;j++){cameras[activeCamera].values[i][j]=rValues[i][j];}
        }
        cameras[activeCamera].camType=type;
        break;
    case 7:
        for(int i= 0;i<ROW_ENTRIES;i++){
            for(int j=0;j<COLUM_ENTRIES;j++){cameras[activeCamera].values[i][j]=rValues[i][j];}
        }
        cameras[activeCamera].values[V_ND_FILTER][0]=0;
        cameras[activeCamera].values[V_ND_FILTER][1]=0;
        cameras[activeCamera].values[V_ND_FILTER][2]=3;
        cameras[activeCamera].camType=type;
        break;

    }
}

unsigned char Model::getCamtype()
{
    return cameras[activeCamera].camType;
}

void Model::setValue(int property, int value)
{
    cameras[activeCamera].values[property][0]=value;
}

int Model::getValue(int property)
{
    switch (cameras[activeCamera].values[property][3]) {
    case NORMAL:
        return cameras[activeCamera].values[property][0];
    case CENTER:
        return cameras[activeCamera].values[property][0]-(cameras[activeCamera].values[property][2]-cameras[activeCamera].values[property][1])/2;
    case NAN:
        return -2048;
    case TEXT:
        return -2049;

    }
    return 0;

}

int Model::getMin(int property)
{
    return cameras[activeCamera].values[property][1];
}

int Model::getMax(int property)
{
    return cameras[activeCamera].values[property][1];
}

void Model::setCamFlag(int flag,bool value)
{
   cameras[activeCamera].flags[flag]=value;
}

bool Model::getCamFlag(int flag)
{
    return cameras[activeCamera].flags[flag];
}
