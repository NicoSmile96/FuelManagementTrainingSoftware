#ifndef PROJET_CARBURANT_AVION_AFFICHAGE_H
#define PROJET_CARBURANT_AVION_AFFICHAGE_H

#include <QWidget>
#include <QAbstractButton>
#include <QPainter>
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QPushButton>
#include "Log.h"
#include <map>


// CONSTANTES
    #define TANK_WIDTH 150
    #define TANK_HEIGHT 100
    #define PUMP_RAY 25
    #define VALVE_RAY 50
    #define ENGINE_WIDTH 200
    #define ENGINE_HEIGHT 100
    #define SPACING 110

    #define LOG_DIR "../Data/Log/"

// TYPES ETATS DU SYSTEME CARBURANT

    typedef enum{
        OFF = 0,
        ON,
        BROKEN
    } pumpState;

    class Log;

    class GenericTpev : public QAbstractButton{
        Q_OBJECT

        private:
            QString name;

        public:
            GenericTpev(){}
            ~GenericTpev(){}

            virtual short getState()=0;
            virtual void setState(short state)=0;
            virtual bool getEngine(){return true;};

        public slots:
            virtual void clickedSlot()=0;

        signals:
            void clickedLog();
            void clickedEval();
            void stateChanged(QString name);
    };

// TANK PUMP VALVE ENGINE
    class Pump;
    class Engine;
    // TANK
    class Tank : public GenericTpev{
        Q_OBJECT

        private:
            QString name;
            bool state;
            const Pump* primaryPump;
            const Pump* secondaryPump;
            void paintEvent(QPaintEvent*);

        public:
            Tank(QString name,Pump* primaryP,Pump* secondaryP);
            ~Tank(){}
            virtual short getState();
            virtual void setState(short state);

        public slots:
            virtual void clickedSlot();
            void setState(bool state);

        signals:
            void stateChanged(bool state);
    };

    // PUMP
    class Pump : public GenericTpev{
        Q_OBJECT

        private:
            QString name;
            pumpState state;
            bool engine;
            bool stateChangeable;
            Engine* supplyingEngine;

        public:
            Pump(QString name,Engine* supplyingE);
            ~Pump(){}
            virtual short getState();
            virtual void setState(short state);
            bool getEngine();
            void setEngine(bool engine);
            void paintEvent(QPaintEvent*);

        public slots:
            virtual void clickedSlot();

        signals:
            void stateChanged(short state);
    };

    // ENGINE
    class Engine : public GenericTpev{
        private:
            QString name;
            bool state;
            Pump* supplyingPump;
        public:
            Engine(QString name,Pump* supplyingP);
            ~Engine(){}
            virtual short getState();
            virtual void setState(short state);
            virtual void clickedSlot(){}
            void paintEvent(QPaintEvent*);
        };

    // VALVE
    class Valve : public GenericTpev{
        Q_OBJECT

        private:
            QString name;
            bool state;
            bool stateChangeable;
            unsigned short tankStateCount;

        public:
            Valve(const QString name);
            ~Valve(){}
            virtual short getState();
            virtual void setState(short state);
            void paintEvent(QPaintEvent*);

        public slots:
            virtual void clickedSlot();

        signals:
            void stateChanged(bool state);
    };

// SYSTEME CARBURANT
    class SystemeCarburant: public QWidget{
        private:
            QMap<QString, GenericTpev*> tpevMap;
            QMap<QString, qint32> lastLogEntry;
            Tank* tank1;
            Tank* tank2;
            Tank* tank3;
            Pump* pump11;
            Pump* pump12;
            Pump* pump21;
            Pump* pump22;
            Pump* pump31;
            Pump* pump32;
            Engine* engine1;
            Engine* engine2;
            Engine* engine3;
            Valve* vt12;
            Valve* vt23;
            Valve* v12;
            Valve* v13;
            Valve* v23;
            QPushButton *vtdb1;
            QPushButton *vtdb2;
            QPushButton *pdb1;
            QPushButton *pdb2;
            QPushButton *pdb3;
            QPushButton *vdb1;
            QPushButton *vdb2;
            QPushButton *vdb3;

        public:
            SystemeCarburant(){}
            SystemeCarburant(const SystemeCarburant& sc);
            SystemeCarburant(int width, int height);
            ~SystemeCarburant(){}

            void paintEvent(QPaintEvent*);
            void setMap(const QMap<QString, qint32>& logMap);
            QMap<QString, GenericTpev*>& getMap();
            void setLastEntry(const QMap<QString, qint32>& entry);
            QMap<QString, qint32>& getLastEntry();
    };

// MAIN WINDOW
    class MainWindow : public QMainWindow{
        Q_OBJECT

        private:
            Log* log;
            QMenu* fileMenu;
            QAction *saveAct;
            QAction *loadAct;
            QAction *simulAct;
            QAction *exerciceAct;
            QAction *exMakerAct;
        public:
            MainWindow();
            ~MainWindow(){}

            void createDockWindow(Log* log);
            void createMenus();
            void createActions();

        public slots:
            void saveLog();
            void loadLog();

        signals:
            void saveSignal(QString);
            void loadSignal(QString);
    };


#endif //PROJET_CARBURANT_AVION_AFFICHAGE_H
