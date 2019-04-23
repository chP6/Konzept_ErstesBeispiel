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
    cameras[activeCamera].busedPresets|=1<<presetNr;
     //cameras[activeCamera].usedPresets[presetNr]=1;
}

int Model::getUsedPreset()
{
    return cameras[activeCamera].busedPresets;
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

void Model::setPresetInStore(bool presetInStore)
{
    cameras[activeCamera].presetInStore=presetInStore;
}

bool Model::getPresetInStore()
{
    return cameras[activeCamera].presetInStore;
}

QStringList* Model::getErrorList(){
    return &errorList;
}
