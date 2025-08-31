#include "HGISMainWindow.h"
#include "HGISCrsSelectionDialog.h"
#include "HGISMapCanvas.h"
#include "core/HGISLayerManager.h"
#include "core/HGISVectorLayer.h"
#include "core/HGISCoordinateReferenceSystem.h"
#include "core/HGISCoordinateTransform.h"
#include "providers/HGISGdalProvider.h"
#include <QApplication>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QTreeWidget>
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QFile>
#include <QSplitter>
#include <QGraphicsPolygonItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsPathItem>
#include <QPainterPath>

class HGISMainWindow::Private
{
public:
    // 핵심 컴포넌트
    HGISLayerManager *layerManager = nullptr;
    HGISMapCanvas *mapCanvas = nullptr;
    
    QMenu *fileMenu = nullptr;
    QMenu *editMenu = nullptr;
    QMenu *viewMenu = nullptr;
    QMenu *layerMenu = nullptr;
    QMenu *settingsMenu = nullptr;
    QMenu *helpMenu = nullptr;
    
    QToolBar *fileToolBar = nullptr;
    QToolBar *editToolBar = nullptr;
    QToolBar *navigationToolBar = nullptr;
    
    QAction *newProjectAct = nullptr;
    QAction *openProjectAct = nullptr;
    QAction *saveProjectAct = nullptr;
    QAction *saveProjectAsAct = nullptr;
    QAction *exitAct = nullptr;
    QAction *openShapefileAct = nullptr;
    QAction *aboutAct = nullptr;
    QAction *aboutQtAct = nullptr;
    
    // 줌 액션들
    QAction *zoomInAct = nullptr;
    QAction *zoomOutAct = nullptr;
    QAction *zoomFullAct = nullptr;
    QAction *panAct = nullptr;
    
    // 좌표계 액션
    QAction *selectCrsAct = nullptr;
    
    QDockWidget *layersDock = nullptr;
    QDockWidget *browserDock = nullptr;
    QDockWidget *propertiesDock = nullptr;
    
    QTreeWidget *layersTree = nullptr;
    QListWidget *browserList = nullptr;
    QTextEdit *propertiesEdit = nullptr;
    
    QLabel *coordinateLabel = nullptr;
    QLabel *scaleLabel = nullptr;
    QLabel *projectionLabel = nullptr;
    
    // 현재 프로젝트 CRS
    HGISCoordinateReferenceSystem projectCrs;
};

HGISMainWindow::HGISMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , d(std::make_unique<Private>())
{
    setupUi();
    readSettings();
}

HGISMainWindow::~HGISMainWindow()
{
}

void HGISMainWindow::setupUi()
{
    // 스타일시트 로드
    QFile styleFile(":/styles/dark_theme.qss");
    if (!styleFile.exists()) {
        styleFile.setFileName(QApplication::applicationDirPath() + "/../resources/styles/dark_theme.qss");
    }
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        qApp->setStyleSheet(styleSheet);
        styleFile.close();
    }
    
    // 레이어 매니저 생성
    d->layerManager = new HGISLayerManager(this);
    
    // 기본 프로젝트 CRS 설정 (Korea 2000 Central Belt)
    d->projectCrs = HGISCoordinateReferenceSystem::korea2000Central();
    d->layerManager->setProjectCrs(d->projectCrs);
    
    // 지도 캔버스를 중앙 위젯으로 설정
    d->mapCanvas = new HGISMapCanvas(this);
    d->mapCanvas->setLayerManager(d->layerManager);
    setCentralWidget(d->mapCanvas);
    
    // 지도 캔버스 시그널 연결
    connect(d->mapCanvas, &HGISMapCanvas::xyCoordinates, this, [this](const QPointF &point) {
        d->coordinateLabel->setText(QString("좌표: X: %1 Y: %2")
            .arg(point.x(), 0, 'f', 2)
            .arg(point.y(), 0, 'f', 2));
    });
    
    connect(d->mapCanvas, &HGISMapCanvas::scaleChanged, this, [this](double scale) {
        d->scaleLabel->setText(QString("축척: 1:%1")
            .arg(static_cast<int>(1.0 / scale)));
    });
    
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDockWindows();
    
    setWindowIcon(QIcon(":/icons/hgis.png"));
}

void HGISMainWindow::createActions()
{
    d->newProjectAct = new QAction("새 프로젝트(&N)...", this);
    d->newProjectAct->setShortcuts(QKeySequence::New);
    d->newProjectAct->setStatusTip("새 프로젝트를 생성합니다");
    connect(d->newProjectAct, &QAction::triggered, this, &HGISMainWindow::newProject);
    
    d->openProjectAct = new QAction("프로젝트 열기(&O)...", this);
    d->openProjectAct->setShortcuts(QKeySequence::Open);
    d->openProjectAct->setStatusTip("기존 프로젝트를 엽니다");
    connect(d->openProjectAct, &QAction::triggered, this, &HGISMainWindow::openProject);
    
    d->saveProjectAct = new QAction("프로젝트 저장(&S)", this);
    d->saveProjectAct->setShortcuts(QKeySequence::Save);
    d->saveProjectAct->setStatusTip("현재 프로젝트를 저장합니다");
    connect(d->saveProjectAct, &QAction::triggered, this, &HGISMainWindow::saveProject);
    
    d->saveProjectAsAct = new QAction("다른 이름으로 저장(&A)...", this);
    d->saveProjectAsAct->setShortcuts(QKeySequence::SaveAs);
    d->saveProjectAsAct->setStatusTip("프로젝트를 새 이름으로 저장합니다");
    connect(d->saveProjectAsAct, &QAction::triggered, this, &HGISMainWindow::saveProjectAs);
    
    d->exitAct = new QAction("종료(&X)", this);
    d->exitAct->setShortcuts(QKeySequence::Quit);
    d->exitAct->setStatusTip("프로그램을 종료합니다");
    connect(d->exitAct, &QAction::triggered, this, &QWidget::close);
    
    // Shapefile 열기 액션
    d->openShapefileAct = new QAction("Shapefile 열기(&S)...", this);
    d->openShapefileAct->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_O);
    d->openShapefileAct->setStatusTip("Shapefile을 불러옵니다");
    connect(d->openShapefileAct, &QAction::triggered, this, &HGISMainWindow::openShapefile);
    
    // 줌 액션들
    d->zoomInAct = new QAction("확대(&+)", this);
    d->zoomInAct->setShortcut(QKeySequence::ZoomIn);
    d->zoomInAct->setStatusTip("지도를 확대합니다");
    connect(d->zoomInAct, &QAction::triggered, d->mapCanvas, &HGISMapCanvas::zoomIn);
    
    d->zoomOutAct = new QAction("축소(&-)", this);
    d->zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    d->zoomOutAct->setStatusTip("지도를 축소합니다");
    connect(d->zoomOutAct, &QAction::triggered, d->mapCanvas, &HGISMapCanvas::zoomOut);
    
    d->zoomFullAct = new QAction("전체 범위(&F)", this);
    d->zoomFullAct->setShortcut(Qt::Key_F);
    d->zoomFullAct->setStatusTip("전체 범위로 확대/축소합니다");
    connect(d->zoomFullAct, &QAction::triggered, d->mapCanvas, &HGISMapCanvas::zoomToFullExtent);
    
    // 좌표계 선택 액션
    d->selectCrsAct = new QAction("프로젝트 좌표계(&C)...", this);
    d->selectCrsAct->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_P);
    d->selectCrsAct->setStatusTip("프로젝트 좌표계를 선택합니다");
    connect(d->selectCrsAct, &QAction::triggered, this, &HGISMainWindow::selectProjectCrs);
    
    d->aboutAct = new QAction("HGIS 정보(&A)", this);
    d->aboutAct->setStatusTip("HGIS에 대한 정보를 표시합니다");
    connect(d->aboutAct, &QAction::triggered, this, &HGISMainWindow::about);
    
    d->aboutQtAct = new QAction("Qt 정보(&Q)", this);
    d->aboutQtAct->setStatusTip("Qt 라이브러리 정보를 표시합니다");
    connect(d->aboutQtAct, &QAction::triggered, this, &HGISMainWindow::aboutQt);
}

void HGISMainWindow::createMenus()
{
    d->fileMenu = menuBar()->addMenu("파일(&F)");
    d->fileMenu->addAction(d->newProjectAct);
    d->fileMenu->addAction(d->openProjectAct);
    d->fileMenu->addAction(d->openShapefileAct);
    d->fileMenu->addSeparator();
    d->fileMenu->addAction(d->saveProjectAct);
    d->fileMenu->addAction(d->saveProjectAsAct);
    d->fileMenu->addSeparator();
    d->fileMenu->addAction(d->exitAct);
    
    d->editMenu = menuBar()->addMenu("편집(&E)");
    
    d->viewMenu = menuBar()->addMenu("보기(&V)");
    d->viewMenu->addAction(d->zoomInAct);
    d->viewMenu->addAction(d->zoomOutAct);
    d->viewMenu->addAction(d->zoomFullAct);
    d->viewMenu->addSeparator();
    
    d->layerMenu = menuBar()->addMenu("레이어(&L)");
    
    d->settingsMenu = menuBar()->addMenu("설정(&S)");
    d->settingsMenu->addAction(d->selectCrsAct);
    
    d->helpMenu = menuBar()->addMenu("도움말(&H)");
    d->helpMenu->addAction(d->aboutAct);
    d->helpMenu->addAction(d->aboutQtAct);
}

void HGISMainWindow::createToolBars()
{
    d->fileToolBar = addToolBar("파일");
    d->fileToolBar->setMovable(false);
    d->fileToolBar->addAction(d->newProjectAct);
    d->fileToolBar->addAction(d->openProjectAct);
    d->fileToolBar->addAction(d->saveProjectAct);
    
    d->editToolBar = addToolBar("편집");
    d->editToolBar->setMovable(false);
    
    d->navigationToolBar = addToolBar("탐색");
    d->navigationToolBar->setMovable(false);
    d->navigationToolBar->addAction(d->zoomInAct);
    d->navigationToolBar->addAction(d->zoomOutAct);
    d->navigationToolBar->addAction(d->zoomFullAct);
}

void HGISMainWindow::createStatusBar()
{
    statusBar()->showMessage("준비 완료");
    
    d->coordinateLabel = new QLabel("좌표: X: 0.00 Y: 0.00");
    d->coordinateLabel->setMinimumWidth(200);
    statusBar()->addPermanentWidget(d->coordinateLabel);
    
    d->scaleLabel = new QLabel("축척: 1:1,000");
    d->scaleLabel->setMinimumWidth(120);
    statusBar()->addPermanentWidget(d->scaleLabel);
    
    d->projectionLabel = new QLabel(QString("좌표계: EPSG:%1 (%2)")
        .arg(d->projectCrs.epsgCode())
        .arg(d->projectCrs.description()));
    d->projectionLabel->setMinimumWidth(200);
    statusBar()->addPermanentWidget(d->projectionLabel);
}

void HGISMainWindow::createDockWindows()
{
    // Layer panel
    d->layersDock = new QDockWidget("레이어", this);
    d->layersDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    d->layersTree = new QTreeWidget();
    d->layersTree->setHeaderLabel("레이어 목록");
    d->layersTree->setAlternatingRowColors(true);
    d->layersTree->setRootIsDecorated(true);
    
    // Add sample layers
    QTreeWidgetItem *baseLayer = new QTreeWidgetItem(d->layersTree);
    baseLayer->setText(0, "기본 지도");
    baseLayer->setCheckState(0, Qt::Checked);
    
    QTreeWidgetItem *vectorLayer = new QTreeWidgetItem(d->layersTree);
    vectorLayer->setText(0, "벡터 레이어");
    vectorLayer->setCheckState(0, Qt::Unchecked);
    
    d->layersDock->setWidget(d->layersTree);
    addDockWidget(Qt::LeftDockWidgetArea, d->layersDock);
    d->viewMenu->addAction(d->layersDock->toggleViewAction());
    
    // Browser panel
    d->browserDock = new QDockWidget("데이터 브라우저", this);
    d->browserDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    d->browserList = new QListWidget();
    
    // Add sample data sources
    d->browserList->addItem("프로젝트 홈");
    d->browserList->addItem("SHP 파일");
    d->browserList->addItem("GeoTIFF");
    d->browserList->addItem("WMS 서비스");
    d->browserList->addItem("PostGIS 데이터베이스");
    
    d->browserDock->setWidget(d->browserList);
    addDockWidget(Qt::LeftDockWidgetArea, d->browserDock);
    d->viewMenu->addAction(d->browserDock->toggleViewAction());
    
    // Properties panel
    d->propertiesDock = new QDockWidget("속성", this);
    d->propertiesDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    d->propertiesEdit = new QTextEdit();
    d->propertiesEdit->setReadOnly(true);
    d->propertiesEdit->setPlainText("선택된 항목의 속성이\n여기에 표시됩니다.");
    d->propertiesDock->setWidget(d->propertiesEdit);
    addDockWidget(Qt::RightDockWidgetArea, d->propertiesDock);
    d->viewMenu->addAction(d->propertiesDock->toggleViewAction());
    
    // Organize as tabs
    tabifyDockWidget(d->layersDock, d->browserDock);
    d->layersDock->raise();
}

void HGISMainWindow::readSettings()
{
    QSettings settings("HGIS", "HGIS");
    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(1024, 768)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    restoreState(settings.value("state").toByteArray());
    settings.endGroup();
}

void HGISMainWindow::writeSettings()
{
    QSettings settings("HGIS", "HGIS");
    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("state", saveState());
    settings.endGroup();
}

void HGISMainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void HGISMainWindow::newProject()
{
    qInfo() << "새 프로젝트 생성 중...";
    statusBar()->showMessage("새 프로젝트가 생성되었습니다", 2000);
}

void HGISMainWindow::openProject()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "프로젝트 열기", "", "HGIS 프로젝트 파일 (*.hgis)");
    
    if (!fileName.isEmpty()) {
        qInfo() << "프로젝트 열기:" << fileName;
        statusBar()->showMessage(QString("프로젝트를 열었습니다: %1").arg(fileName), 2000);
    }
}

void HGISMainWindow::saveProject()
{
    qInfo() << "프로젝트 저장 중...";
    statusBar()->showMessage("프로젝트가 저장되었습니다", 2000);
}

void HGISMainWindow::saveProjectAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "다른 이름으로 저장", "", "HGIS 프로젝트 파일 (*.hgis)");
    
    if (!fileName.isEmpty()) {
        qInfo() << "다른 이름으로 저장:" << fileName;
        statusBar()->showMessage(QString("프로젝트가 저장되었습니다: %1").arg(fileName), 2000);
    }
}

void HGISMainWindow::about()
{
    QMessageBox::about(this, "HGIS 정보",
        "<h2>HGIS 1.0.0</h2>"
        "<p><b>고급 공간정보 시스템</b></p>"
        "<p>Qt, GDAL, PROJ 기반으로 구축</p>"
        "<p>공간 데이터 분석 및 시각화를 위한<br>"
        "전문 GIS 애플리케이션입니다.</p>"
        "<p style='color: #007ACC;'>© 2025 HGIS 개발팀</p>");
}

void HGISMainWindow::aboutQt()
{
    QApplication::aboutQt();
}

void HGISMainWindow::selectProjectCrs()
{
    HGISCrsSelectionDialog dialog(this);
    dialog.setCurrentCrs(d->projectCrs);
    
    if (dialog.exec() == QDialog::Accepted) {
        HGISCoordinateReferenceSystem newCrs = dialog.selectedCrs();
        if (newCrs.isValid() && newCrs != d->projectCrs) {
            d->projectCrs = newCrs;
            
            // 상태바 업데이트
            QString crsText = QString("좌표계: EPSG:%1 (%2)")
                .arg(d->projectCrs.epsgCode())
                .arg(d->projectCrs.description());
            d->projectionLabel->setText(crsText);
            
            qInfo() << "프로젝트 좌표계 변경:" << crsText;
            statusBar()->showMessage(QString("프로젝트 좌표계가 변경되었습니다: EPSG:%1")
                .arg(d->projectCrs.epsgCode()), 3000);
        }
    }
}

void HGISMainWindow::openShapefile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Shapefile 열기", "", "Shapefile (*.shp);;모든 파일 (*.*)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    // 벡터 레이어 생성 및 로드
    HGISVectorLayer *layer = new HGISVectorLayer();
    layer->setName(QFileInfo(fileName).baseName());
    
    if (!layer->loadFromFile(fileName)) {
        QMessageBox::critical(this, "오류", 
            QString("Shapefile을 열 수 없습니다:\n%1").arg(fileName));
        delete layer;
        return;
    }
    
    qInfo() << "Shapefile 열기 성공:" << fileName;
    qInfo() << "  레이어:" << layer->name();
    qInfo() << "  피처 수:" << layer->featureCount();
    qInfo() << "  지오메트리 타입:" << layer->geometryTypeAsString();
    qInfo() << "  EPSG:" << layer->crs().epsgCode();
    
    // 심볼 설정 (지오메트리 타입에 따라)
    HGISSymbol symbol;
    switch (layer->geometryType()) {
        case HGISGeometryType::Point:
        case HGISGeometryType::MultiPoint:
            symbol.pointSymbolType = HGISSymbol::Circle;
            symbol.fillColor = Qt::blue;
            symbol.strokeColor = Qt::darkBlue;
            symbol.pointSize = 5.0;
            break;
            
        case HGISGeometryType::LineString:
        case HGISGeometryType::MultiLineString:
            symbol.strokeColor = Qt::darkGreen;
            symbol.strokeWidth = 2.0;
            break;
            
        case HGISGeometryType::Polygon:
        case HGISGeometryType::MultiPolygon:
            symbol.fillColor = QColor(100, 150, 200, 100);
            symbol.strokeColor = Qt::darkBlue;
            symbol.strokeWidth = 1.5;
            break;
            
        default:
            break;
    }
    layer->setSymbol(symbol);
    
    // 레이어 매니저에 추가
    d->layerManager->addLayer(layer);
    
    // 전체 범위로 확대/축소
    d->mapCanvas->zoomToFullExtent();
    
    // 레이어 트리에 추가
    QTreeWidgetItem *shpLayer = new QTreeWidgetItem(d->layersTree);
    shpLayer->setText(0, QFileInfo(fileName).baseName());
    shpLayer->setCheckState(0, Qt::Checked);
    shpLayer->setIcon(0, QIcon(":/icons/layer-vector.png"));
    
    // 속성 패널 업데이트
    QRectF extent = layer->extent();
    QString properties = QString("레이어 정보\n"
                                "================\n"
                                "이름: %1\n"
                                "파일: %2\n"
                                "피처 수: %3\n"
                                "지오메트리: %4\n"
                                "좌표계: EPSG:%5\n"
                                "범위:\n"
                                "  X: %6 - %7\n"
                                "  Y: %8 - %9")
        .arg(layer->name())
        .arg(QFileInfo(fileName).fileName())
        .arg(layer->featureCount())
        .arg(layer->geometryTypeAsString())
        .arg(layer->crs().epsgCode())
        .arg(extent.left(), 0, 'f', 2)
        .arg(extent.right(), 0, 'f', 2)
        .arg(extent.top(), 0, 'f', 2)
        .arg(extent.bottom(), 0, 'f', 2);
    
    d->propertiesEdit->setPlainText(properties);
    
    statusBar()->showMessage(QString("Shapefile을 열었습니다: %1 (%2개 피처)")
        .arg(QFileInfo(fileName).fileName())
        .arg(layer->featureCount()), 3000);
}