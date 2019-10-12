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



int main(int argc, char *argv[])
{
   qRegisterMetaType<properties_t>("properties_t");
   qRegisterMetaType<flags_t>("flags_t");

    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    //a.quit();
     pthread_t this_thread = pthread_self ();
     struct sched_param params;
     params.sched_priority = 75;
     pthread_setschedparam (this_thread, SCHED_FIFO, &params);


    QCoreApplication::setApplicationName("BBMNet RCP/OCP Dual Controller");
    QCoreApplication::setApplicationVersion("5.1");

    /*add a parser to grab commandline options for logging*/
    QCommandLineParser parser;
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
    filter.append("*.debug=false\n");   //disable any debugging

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


    Model model;
    Controller controller(model);
    View view(model,controller);

    qmlRegisterType<CameraView>("io.qt.examples.backend", 1, 0, "CameraBackend");
    qmlRegisterType<Home>("io.qt.examples.backend", 1, 0, "HomeBackend");
    qmlRegisterType<XptControl>("io.qt.examples.backend", 1, 0, "XptBackend");
    qmlRegisterType<Others>("io.qt.examples.backend", 1, 0, "OthersBackend");


    qmlRegisterUncreatableMetaObject(Config::staticMetaObject, // static meta object
                                     "com.bbm.config",          // import statement
                                     1, 0,                         // major and minor version of the import
                                     "Config",                 // name in QML
                                     "Error: only enums");
    engine.rootContext()->setContextProperty("cameraBackend",&view.cameraBackend);
    engine.rootContext()->setContextProperty("homeBackend",&view.homeBackend);
    engine.rootContext()->setContextProperty("xptBackend",&view.xptBackend);
    engine.rootContext()->setContextProperty("othersBackend",&view.othersBackend);

    QQuickStyle::setStyle("Material");
    const QUrl url(QStringLiteral("qrc:/main.qml"));
//    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
//                     &app, [url](QObject *obj, const QUrl &objUrl) {
//        if (!obj && url == objUrl)
//            QCoreApplication::exit(-1);
//    }, Qt::QueuedConnection);
    engine.load(url);

    //Q_INIT_RESOURCE(styles);
    //QFile File(":/stylesheet.qss");
    //File.open(QFile::ReadOnly);
    //QString StyleSheet = QLatin1String(File.readAll());

    //view.setStyleSheet(StyleSheet);
    QObject::connect(&model, &Model::updateView,            // model signal mit view slot verbinden
                     &view, &View::on_modelUpdate);
//    QObject::connect(&model, SIGNAL(updateViewProperty(properties_t)),
//                &view, SLOT(on_modelUpdateProperty(properties_t)));
    QObject::connect(&model, &Model::updateViewProperty,            // model signal mit view slot verbinden
                     &view, &View::on_modelUpdateProperty);
    QObject::connect(&model, &Model::updateViewFlag,            // model signal mit view slot verbinden
                     &view, &View::on_modelUpdateFlag);
    QObject::connect(&model, &Model::setUpView,            // model signal mit view slot verbinden
                     &view, &View::on_modelSetUp);
    QObject::connect(&model, &Model::updateServerConnectionStatus,
                     &view, &View::on_serverConnectionStatusChanged);
    QObject::connect(&model, &Model::updateCameraConnectionStatus,
                     &view, &View::on_cameraConnectionStatusChanged);
    QObject::connect(&model, &Model::updateXptConnectionStatus,
                     &view, &View::on_xptConnectionStatusChanged);
    QObject::connect(&model, &Model::updateXptEnableStatus,
                     &view, &View::on_xptEnableStatusChanged);
    QObject::connect(&model, &Model::newSignalReceived,
                     &view, &View::on_newReceive);
    QObject::connect(&model, &Model::receiveAllNew,
                     &view, &View::on_newRequest);



    QObject::connect(&controller, &Controller::clearLoadButon,
                     &view, &View::on_loadButtonCleared);

//    QObject::connect(&a, &QApplication::aboutToQuit,
//                     &controller, &Controller::onAppQuit);

    //view.setModelController(model, controller);
    //view.show();

    Poller poller(controller);
    controller.setPoller(poller);
    UdpListener udpListener(controller);
    controller.startQueueProcessThread();
    poller.startListener();
    udpListener.startListener();
    //QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));

    app.exec();
    poller.stopListener();
    if(poller.t3.joinable()){ poller.t3.join();}
    controller.stopQueueProcessThread();
    if(controller.t1.joinable()){controller.t1.join();}
    udpListener.stopListener();
    //if(udpListener.t2.joinable()){udpListener.t2.join();}
    printf("bye bye \n");
    exit(0);
}
