#include <QLayout>
#include <QPainter>
#include <QPushButton>
#include <QMouseEvent>
#include <QDockWidget>
#include <QListWidget>
#include <QSizePolicy>
#include "affichage.h"
#include "Log.h"
#include <iostream>

// WIDGETS TO DRAW
// TANKS
    Tank::Tank(QString name,Pump* primaryP,Pump* secondaryP)     {
        this->name = name;
        this->primaryPump = primaryP;
        this->secondaryPump = secondaryP;
        state = true;
        setFixedWidth(TANK_WIDTH);
        setFixedHeight(TANK_HEIGHT);
        setEnabled(true);
    }

    void Tank::paintEvent(QPaintEvent*) {
        QPainter p(this);

        // rect
        if(state){
            p.setBrush(QBrush(Qt::green));
        } else{
            p.setBrush(QBrush(Qt::red));
        }
        p.drawRect(0, 0, TANK_WIDTH-10, TANK_HEIGHT-10);

        // texte
        p.setBrush(QBrush(Qt::black));
        QFont font("Arial", 12);
        font.setWeight(QFont::Bold);
        p.setFont(font);
        p.drawText(rect(), Qt::AlignHCenter, name);
    }

    void Tank::mousePressEvent(QMouseEvent*){
        if(state) state = false;
        else state = true;

        emit tankStateChanged(state);
        update();
    }

    void Tank::setState(bool state) {
        if(state && !this->state){
            this->state = true;
            emit tankStateChanged(this->state);
            update();
        }
    }
// PUMPS

    Pump::Pump(QString name,Engine* supplyingE) {
        this->name = name;
        this->supplyingEngine = supplyingE;
        setFixedWidth(PUMP_RAY*2);
        setFixedHeight(PUMP_RAY*2);

        if(engine) state = ON;
        else state = OFF;
    }

    pumpState Pump::getState(){
        return state;
    }

    bool Pump::getEngine(){
        return engine;
    }

    void Pump::setEngine(bool engine){
        this->engine = engine;
    }

    void Pump::paintEvent(QPaintEvent*){
        QPainter p(this);
        if(state == OFF)
            p.setBrush(QBrush(Qt::red));
        else if(state == ON)
            p.setBrush(QBrush(Qt::green));
        else
            p.setBrush(QBrush(Qt::gray));

        // contour
        p.drawEllipse(rect().topLeft().rx(), rect().topLeft().ry(), PUMP_RAY*2 - 2, PUMP_RAY*2 -2);

        // texte
        p.drawText(rect(), Qt::AlignCenter, name);
    }

    void Pump::mousePressEvent(QMouseEvent*){
        short emitState = 0;

        switch(state){
            case ON :
                emitState--;
                state = OFF;
                break;
            case OFF :
                emitState += 2;
                state = BROKEN;
                break;
            case BROKEN :
                emitState--;
                state = ON;
                break;
            default:
                std::cerr << "unrecognized pump state" << std::endl;
                break;
        }

        emit stateChanged(emitState);
        update();
    }
    void Pump::stateChangedSlot(){
        short emitState = 0;

        switch(state){
            case ON :
                emitState--;
                state = OFF;
                break;
            case OFF :
                emitState += 1;
                state = ON;
                break;
            case BROKEN :
                break;
            default:
                std::cerr << "unrecognized pump state" << std::endl;
                break;
        }

        emit stateChanged(emitState);
        update();
    }
    

// ENGINES
    Engine::Engine(QString name,Pump* supplyingP){
        this->name = name;
        this->supplyingPump = supplyingP;
        state = true;
        setFixedWidth(TANK_WIDTH);
        setFixedHeight(TANK_HEIGHT);
    }

    void Engine::paintEvent(QPaintEvent *) {
        QPainter p(this);

        // rect
        p.setBrush(QBrush(Qt::gray));
        p.drawRect(0, 0, TANK_WIDTH-10, TANK_HEIGHT-10);

        // texte
        p.setBrush(QBrush(Qt::black));
        QFont font("Arial", 12);
        font.setWeight(QFont::Bold);
        p.setFont(font);
        p.drawText(rect(), Qt::AlignCenter, name);
    }

// VALVES
    Valve::Valve(const QString name) {
        this->name = name;
        tankStateCount = 2;
        state = false;
        stateChangeable = true;
        setFixedWidth(VALVE_RAY*2);
        setFixedHeight(VALVE_RAY*2);
    }

    void Valve::paintEvent(QPaintEvent *) {
        QPainter p(this);

        // texte
        QFont font("Arial", 12);
        font.setWeight(QFont::Bold);
        p.setFont(font);
        p.drawText(rect(), Qt::AlignTop | Qt::AlignHCenter, name);

        // circle
        p.setBrush(QBrush(Qt::black));
        QPoint center(rect().center().rx(), rect().center().ry());
        p.drawEllipse(center, VALVE_RAY/2, VALVE_RAY/2);

        // line
        if(!state){
            p.setBrush(QBrush(Qt::white));
            p.drawRect(rect().center().rx()-3, rect().center().ry() - VALVE_RAY/2, 6,VALVE_RAY);
        } else{
            p.setBrush(QBrush(Qt::white));
            p.drawRect(rect().center().rx()-VALVE_RAY/2, rect().center().ry() - 3, VALVE_RAY, 6);
        }
    }

    void Valve::mousePressEvent(QMouseEvent *) {
        if(stateChangeable){
            (!state) ? state = true : state = false;
            emit valveStateChanged(state);
            update();
        }
    }
    void Valve::stateChanged(){
        if(stateChangeable){
            (!state) ? state = true : state = false;
            emit valveStateChanged(state);
            update();
        }
    }

// MAIN WINDOW
    MainWindow::MainWindow() {
        SystemeCarburant* systemeC = new SystemeCarburant(700,700);
        systemeC->setParent(this);
        this->setCentralWidget(systemeC);
//        Log* log = new Log(this);
        createDockWindow();
    }

    void MainWindow::createDockWindow(){
        QDockWidget* dock = new QDockWidget(tr("Log"), this);
        dock->setAllowedAreas(Qt::RightDockWidgetArea);

        QListWidget* widgetList = new QListWidget(dock);
//        LogItem* logi = new LogItem("VT12");
//        logi->addAction("VT12", 1);

//        widgetList->addItems(QStringList() << logi->getName());
        dock->setWidget(widgetList);
        addDockWidget(Qt::RightDockWidgetArea, dock);
    }

// SYSTEME CARBURANT WINDOW
    SystemeCarburant::SystemeCarburant(int width, int height) {
        // Params mains window
        this->setMinimumWidth(width);
        this->setMinimumHeight(height);

        // Widgets to draw
        tank1 = new Tank("Tank 1",pump11,pump12);
        tank2 = new Tank("Tank 2",pump21,pump22);
        tank3 = new Tank("Tank 3",pump31,pump32);
        pump11 = new Pump("P11",engine1);
        pump12 = new Pump("P12",nullptr);
        pump21 = new Pump( "P21",engine2);
        pump22 = new Pump("P22",nullptr);
        pump31 = new Pump("P31",engine3);
        pump32 = new Pump("P32",nullptr);
        engine1 = new Engine("Engine 1",pump11);
        engine2 = new Engine("Engine 2",pump21);
        engine3 = new Engine("Engine 3",pump31);
        vt12 = new Valve("VT12");
        vt23 = new Valve("VT23");
        v12 = new Valve("V12");
        v13 = new Valve("V13");
        v23 = new Valve("V23");

        // DashBoard buttons
        QPushButton *vtdb1 = new QPushButton("VT12");
        QPushButton *vtdb2 = new QPushButton("VT23");
        QPushButton *pdb1 = new QPushButton("P12");
        QPushButton *pdb2 = new QPushButton("P22");
        QPushButton *pdb3 = new QPushButton("P32");
        QPushButton *vdb1 = new QPushButton("V12");
        QPushButton *vdb2 = new QPushButton("V13");
        QPushButton *vdb3 = new QPushButton("V23");

        // Layouts
        QVBoxLayout* mainLayout = new QVBoxLayout;
        QHBoxLayout* tankLayout = new QHBoxLayout;
        QHBoxLayout* pumpLayout1 = new QHBoxLayout;
        QHBoxLayout* pumpLayout2 = new QHBoxLayout;
        QHBoxLayout* pumpLayout3 = new QHBoxLayout;
        QHBoxLayout* engineLayout = new QHBoxLayout;
        QVBoxLayout* mainDashBoardLayout = new QVBoxLayout;
        QHBoxLayout* vtDashBoardLayout = new QHBoxLayout;
        QHBoxLayout* pumpDashBoardLayout = new QHBoxLayout;
        QHBoxLayout* valveDashBoardLayout = new QHBoxLayout;

        // add layouts to layouts
        tank1->setLayout(pumpLayout1);
        tank2->setLayout(pumpLayout2);
        tank3->setLayout(pumpLayout3);
        this->setLayout(mainLayout);
        mainLayout->addLayout(tankLayout);
        mainLayout->addLayout(engineLayout);
        mainLayout-> addLayout(mainDashBoardLayout);
        mainDashBoardLayout -> addLayout(vtDashBoardLayout);
        mainDashBoardLayout -> addLayout(pumpDashBoardLayout);
        mainDashBoardLayout -> addLayout(valveDashBoardLayout);

        // add widgets to layouts
        v12->setParent(this);
        v13->setParent(this);
        v23->setParent(this);
        tankLayout->addWidget(tank1);
        tankLayout->addWidget(vt12);
        tankLayout->addWidget(tank2);
        tankLayout->addWidget(vt23);
        tankLayout->addWidget(tank3);
        pumpLayout1->addWidget(pump11);
        pumpLayout1->addWidget(pump12);
        pumpLayout2->addWidget(pump21);
        pumpLayout2->addWidget(pump22);
        pumpLayout3->addWidget(pump31);
        pumpLayout3->addWidget(pump32);
        engineLayout->addWidget(engine1);
        engineLayout->addWidget(engine2);
        engineLayout->addWidget(engine3);
        vtDashBoardLayout->addWidget(vtdb1);
        vtDashBoardLayout->addWidget(vtdb2);
        pumpDashBoardLayout->addWidget(pdb1);
        pumpDashBoardLayout->addWidget(pdb2);
        pumpDashBoardLayout->addWidget(pdb3);
        valveDashBoardLayout->addWidget(vdb1);
        valveDashBoardLayout->addWidget(vdb2);
        valveDashBoardLayout->addWidget(vdb3);

        // Layout params spacing and alignment
        mainLayout->setAlignment(tankLayout, Qt::AlignTop | Qt::AlignHCenter);
        mainLayout->setAlignment(engineLayout, Qt::AlignBottom | Qt::AlignHCenter);
        engineLayout->setSpacing(SPACING);

        // WidgetClicked signals
       /*  QObject::connect(tank1, SIGNAL(tankStateChanged(bool)), vt12, SLOT(setChangeable(bool)));
        QObject::connect(tank2, SIGNAL(tankStateChanged(bool)), vt12, SLOT(setChangeable(bool)));
        QObject::connect(tank2, SIGNAL(tankStateChanged(bool)), vt23, SLOT(setChangeable(bool)));
        QObject::connect(tank3, SIGNAL(tankStateChanged(bool)), vt23, SLOT(setChangeable(bool))); */

        QObject::connect(vt12, SIGNAL(valveStateChanged(bool)), tank1, SLOT(setState(bool)));
        QObject::connect(vt12, SIGNAL(valveStateChanged(bool)), tank2, SLOT(setState(bool)));
        QObject::connect(vt23, SIGNAL(valveStateChanged(bool)), tank2, SLOT(setState(bool)));
        QObject::connect(vt23, SIGNAL(valveStateChanged(bool)), tank3, SLOT(setState(bool)));

        //Dashboard signals
        QObject::connect(vtdb1, SIGNAL(clicked()), vt12, SLOT(stateChanged()));
        QObject::connect(vtdb2, SIGNAL(clicked()), vt23, SLOT(stateChanged()));
        QObject::connect(pdb1, SIGNAL(clicked()), pump12, SLOT(stateChangedSlot()));
        QObject::connect(pdb2, SIGNAL(clicked()), pump22, SLOT(stateChangedSlot()));
        QObject::connect(pdb3, SIGNAL(clicked()), pump32, SLOT(stateChangedSlot()));
        QObject::connect(vdb1, SIGNAL(clicked()), v12, SLOT(stateChanged()));
        QObject::connect(vdb2, SIGNAL(clicked()), v13, SLOT(stateChanged()));
        QObject::connect(vdb3, SIGNAL(clicked()), v23, SLOT(stateChanged()));

    }

// PAINT EVENT WINDOW
    void SystemeCarburant::paintEvent(QPaintEvent *) {
        QPainter p(this);

        // Lines
        int xleft, xcenter, xright;
        xcenter = width()/2;
        xleft = xcenter - TANK_WIDTH - 2*VALVE_RAY;
        xright = xcenter + TANK_WIDTH + 2*VALVE_RAY;

        // Tanks and engines
        p.drawLine(xleft, TANK_HEIGHT/2 + 10, xright, TANK_HEIGHT/2 + 10);  // between tanks
        p.drawLine(xleft, 3*TANK_HEIGHT, xleft - 20, 3*TANK_HEIGHT); // t1 e1 horizontal
        p.drawLine(xleft - 20, 3*TANK_HEIGHT, xleft - 20, height() - 2*TANK_HEIGHT);   // t1 e1 vertical
        p.drawLine(xcenter, TANK_HEIGHT/2 + 10, xcenter, height() - 2*TANK_HEIGHT);     // t2 e2

        // Valve lines
        v12->setGeometry(width()/2 - TANK_WIDTH, 3.5*TANK_HEIGHT, VALVE_RAY*2, VALVE_RAY*2);
        v13->setGeometry(width()/2 + TANK_WIDTH/2, 3*TANK_HEIGHT/2, VALVE_RAY*2, VALVE_RAY*2);
        v23->setGeometry(width()/2 + TANK_WIDTH/2, 4*TANK_HEIGHT, VALVE_RAY*2, VALVE_RAY*2);

        p.drawLine(xleft, TANK_HEIGHT/2 + 10, xleft, 4*TANK_HEIGHT);    // v12 vertical
        p.drawLine(xleft, 4*TANK_HEIGHT, xcenter, 4*TANK_HEIGHT);   // v12 horizontal
        p.drawLine(xright, 4.5*TANK_HEIGHT, xcenter, 4.5*TANK_HEIGHT);  // v23 horizontal
        p.drawLine(xright, TANK_HEIGHT/2 + 10, xright, 4.5*TANK_HEIGHT);   // v23 vertical
        p.drawLine(xleft, 2*TANK_HEIGHT, xright + 20, 2*TANK_HEIGHT);    // v13 horizontal
        p.drawLine(xright + 20, 2*TANK_HEIGHT, xright + 20, height() - 2*TANK_HEIGHT);   // v13 vertical
    }
