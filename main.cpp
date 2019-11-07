#include "view.h"
#include "model.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include "controller.h"
#include "udplistener.h"
#include "poller.h"
#include <iostream>
#include <sstream>
#include "config.h"
#include <QFile>
#include "logging.h"
#include <QCommandLineParser>
#include <sched.h>
#include <csignal>
#include <QThread>
#include <QCursor>
#include <QFileSystemWatcher>

/*Redirect certain Debug messages to a File such that it can be displayed on the error/info-screen*/
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "%s: %s \n",context.category, localMsg.constData());
        break;
    case QtInfoMsg:{
        if (strcmp(context.category,"user")==0) {
            FILE * fp;
            fp = fopen("/root/.config/"+QCoreApplication::organizationName().toLocal8Bit()+"/log.txt", "a" );
            fprintf(fp, "Info: %s\n", localMsg.constData());
            fclose(fp);
        }
        fprintf(stderr, "%s: %s \n",context.category, localMsg.constData());
        break;}
    case QtWarningMsg:
        if (strcmp(context.category,"user")==0) {
            FILE * fp;
            fp = fopen("/root/.config/"+QCoreApplication::organizationName().toLocal8Bit()+"/log.txt", "a" );
            fprintf(fp, "Error: %s\n", localMsg.constData());
            fclose(fp);
        }
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}

void parseComandlineOptions(QCommandLineParser &parser, QGuiApplication &app){
    parser.setApplicationDescription("Logging options");
    parser.addHelpOption();     //add -h --help option
    parser.addVersionOption();  //add -v --version option

    /*add -a --all option to the parser to log everything*/
    QCommandLineOption logAll(QStringList() << "a" << "all", QCoreApplication::translate("main", "log everything"));
       parser.addOption(logAll);
    /*add -p --preset option to the parser to log preset traffic*/
    QCommandLineOption logPreset(QStringList() << "p" << "preset", QCoreApplication::translate("main", "log preset traffic"));
          parser.addOption(logPreset);
    /*add -x --xpt option to the parser to log xpt traffic*/
    QCommandLineOption logXpt(QStringList() << "x" << "xpt", QCoreApplication::translate("main", "log xpt traffic"));
         parser.addOption(logXpt);
    /*add -l --logic option to the parser to log logic traffic*/
    QCommandLineOption logLogic(QStringList() << "l" << "logic", QCoreApplication::translate("main", "log internal logic events"));
        parser.addOption(logLogic);
    /*add -e --extcont option to the parser to log traffic from adjacent rcp/ocp*/
    QCommandLineOption logExtRCP(QStringList() << "e" << "extcont", QCoreApplication::translate("main", "log traffic from adjacent rcp/ocp"));
        parser.addOption(logExtRCP);
    /*add -r --rx option to the parser to log traffic from camera heads in the net*/
    QCommandLineOption logRx(QStringList() << "r" << "rx", QCoreApplication::translate("main", "log incoming traffic from camera heads"));
        parser.addOption(logRx);
    /*add -t --tx option to the parser to log traffic from the programm itself*/
    QCommandLineOption logTx(QStringList() << "t" << "tx", QCoreApplication::translate("main", "log outgoing traffic from controller"));
        parser.addOption(logTx);
    /*add -w --watchdog option to the parser to log watchdog traffic*/
    QCommandLineOption logWatchdog(QStringList() << "w" << "watchdog", QCoreApplication::translate("main", "log watchdog traffic"));
        parser.addOption(logWatchdog);
    /*add -i --inquiry option to the parser to log inquiry traffic*/
    QCommandLineOption logInquiry(QStringList() << "i" << "inquiry", QCoreApplication::translate("main", "log inquiry traffic"));
        parser.addOption(logInquiry);
    parser.process(app);
        QByteArray filter;
        //filter.append("*.debug=false\n");   //disable any debugging
        filter.append("user=true\n");
        if (parser.isSet(logPreset)) {
            filter.append("preset.io=true\n"); //enable preset logging
        }
        if (parser.isSet(logXpt)) {
            filter.append("xpt.io=true\n"); //enable xpt logging
        }
        if (parser.isSet(logLogic)) {
            filter.append("logic.io=true\n"); //enable logic debugging
        }
        if (parser.isSet(logExtRCP)) {
            filter.append("rxRcp.io=true\n"); //enable adjacent rcp/ocp logging
        }
        if (parser.isSet(logRx)) {
            filter.append("rxHead.io=true\n"); //enable rx  logging
        }
        if (parser.isSet(logTx)) {
            filter.append("txHead.io=true\n"); //enable tx logging
        }
        if (parser.isSet(logWatchdog)) {
            filter.append("txWatchdog.io=true\n"); //enable tx Watchdocg logging
            filter.append("rxWatchdog.io=true\n"); //enable rx Watchdocg logging
        }
        if (parser.isSet(logInquiry)) {
            filter.append("request.io=true\n"); //enable inquirylogging
        }
        if (parser.isSet(logAll)) {
            filter.clear(); //override all
            filter.append("*.debug=false\n");   //disable any debugging
            filter.append("preset.io=true\n");  //enable preset logging
            filter.append("xpt.io=true\n");     //enable xpt logging
            filter.append("logic.io=true\n");   //enable logic debugging
            filter.append("rxRcp.io=true\n");   //enable adjacent rcp/ocp logging
            filter.append("rxHead.io=true\n");  //enable rx logging
            filter.append("txHead.io=true\n");  //enable tx logging
            filter.append("txWatchdog.io=true\n"); //enable tx Watchdocg logging
            filter.append("rxWatchdog.io=true\n"); //enable rx Watchdocg logging
            filter.append("request.io=true\n"); //enable inquirylogging
        }
        /*add filter to logger*/
        QLoggingCategory::setFilterRules(filter);
}

int main(int argc, char *argv[])
{

    QCoreApplication::setOrganizationName("BBMProductions");
    QCoreApplication::setApplicationName("BBMNetDualController");
    QCoreApplication::setApplicationVersion("5.1");

    static FILE * fp;
    fp = fopen("/root/.config/"+QCoreApplication::organizationName().toLocal8Bit()+"/log.txt", "w" );
    if(fp){
        fclose(fp);
    }
    qInstallMessageHandler(myMessageOutput);
    qRegisterMetaType<properties_t>("properties_t");
    qRegisterMetaType<flags_t>("flags_t");
    qRegisterMetaType<axis_t>("axis_t");
    qRegisterMetaType<control_t>("control_t");

    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setOverrideCursor(QCursor(Qt::BlankCursor));

    pthread_t this_thread = pthread_self ();
    struct sched_param params;
    params.sched_priority = 75;
    pthread_setschedparam (this_thread, SCHED_FIFO, &params);

    /*add a parser to grab commandline options for logging*/
    QCommandLineParser parser;
    parseComandlineOptions(parser, app);
    QFileSystemWatcher errorHanlder; //signal on new error
    errorHanlder.addPath("/root/.config/"+QCoreApplication::organizationName().toLocal8Bit()+"/log.txt");

    Model model;
    Controller controller(model);
    View view(model,controller);

    qmlRegisterType<CameraView>("io.qt.examples.backend", 1, 0, "CameraBackend");
    qmlRegisterType<Home>("io.qt.examples.backend", 1, 0, "HomeBackend");
    qmlRegisterType<XptControl>("io.qt.examples.backend", 1, 0, "XptBackend");
    qmlRegisterType<Others>("io.qt.examples.backend", 1, 0, "OthersBackend");
    qmlRegisterType<Controls>("io.qt.examples.backend", 1, 0, "ControlsBackend");
    qmlRegisterUncreatableMetaObject(Config::staticMetaObject,"com.bbm.config",1, 0,"Config","Error: only enums");

    engine.rootContext()->setContextProperty("cameraBackend",&view.cameraBackend);
    engine.rootContext()->setContextProperty("homeBackend",&view.homeBackend);
    engine.rootContext()->setContextProperty("xptBackend",&view.xptBackend);
    engine.rootContext()->setContextProperty("othersBackend",&view.othersBackend);
    engine.rootContext()->setContextProperty("controlsBackend",&view.controlsBackend);
    QQuickStyle::setStyle("Material");
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    engine.load(url);

    //view.setStyleSheet(StyleSheet);
    QObject::connect(&model, &Model::updateView,&view, &View::on_modelUpdate);
    QObject::connect(&model, &Model::updateViewProperty,&view, &View::on_modelUpdateProperty);
    QObject::connect(&model, &Model::updateViewFlag,&view, &View::on_modelUpdateFlag);
    QObject::connect(&model, &Model::updateServerConnectionStatus,&view, &View::on_serverConnectionStatusChanged);
    QObject::connect(&model, &Model::updateXptConnectionStatus,&view, &View::on_xptConnectionStatusChanged);
    QObject::connect(&errorHanlder, &QFileSystemWatcher::fileChanged,&view, &View::on_newError);

    Poller poller(controller);
    controller.setPoller(poller);
    UdpListener udpListener(controller);
    controller.startQueueProcessThread();
    poller.startListener();
    udpListener.startListener();

    app.exec();
    poller.stopListener();
    if(poller.t3.joinable()){ poller.t3.join();}
    controller.stopQueueProcessThread();
    if(controller.t1.joinable()){controller.t1.join();}
    udpListener.stopListener();
    //if(udpListener.t2.joinable()){udpListener.t2.join();}
    qCDebug(logicIo,"bye bye \n");
    exit(0);
}
