#ifndef MODEL_H
#define MODEL_H

#include <QWidget>
#include <QStringList>
//#include "view.h" //nix gut, circular dependency -> forward declaration.
//class View;         //Make sure each header can be included on its own.

struct Camera_s{
  unsigned char activePreset;
  bool usedPresets[6];
  bool presetInStore;
  unsigned char busedPresets;

};

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
    void addError(std::string str);
    void clearErrors();
    void setUsedPreset(int presetNr);
    int getUsedPreset();

    void setActivePreset(unsigned char actPreset);
    unsigned char getActivePreset();
    void setActiveCamera(unsigned char camNr);
    unsigned char getActiveCamera();
    void setPresetInStore(bool presetInStore);
    bool getPresetInStore();
    QStringList* getErrorList();

private:
    QStringList errorList;
    int answerStack;
    int count;
    int x;
    int y;
    struct Camera_s cameras[6];
    unsigned char activeCamera;

};

#endif // MODEL_H
