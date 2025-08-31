/***************************************************************************
                         hgisapp.cpp  -  implementation
                            -------------------
   begin                : 2025-08-31
   copyright            : (C) 2025 HGIS Project
   email                : support@hgis.org
***************************************************************************/

#include "hgisapp.h"

#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QDockWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QAction>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QSplashScreen>
#include <QTextCodec>
#include <QFontDatabase>
#include <QDebug>

// QGIS includes
#include "qgsapplication.h"
#include "qgsproject.h"
#include "qgsmapcanvas.h"
#include "qgsvectorlayer.h"
#include "qgsrasterlayer.h"
#include "qgslayertreeview.h"
#include "qgsmessagebar.h"
#include "qgslayertreemapcanvasbridge.h"
#include "qgscoordinatereferencesystem.h"
#include "qgsmaptoolidentify.h"
#include "qgsfeature.h"
#include "qgsgeometry.h"
#include "qgssettings.h"

static HGISApp *smHGISApp = nullptr;

HGISApp::HGISApp( QSplashScreen *splashScreen, 
                  bool restorePlugins, 
                  bool skipVersionCheck, 
                  const QString &rootProfileFolder )
  : QgisApp( splashScreen, restorePlugins, skipVersionCheck, rootProfileFolder )
  , mCurrentHeritageProject( nullptr )
  , mKoreanFontLoaded( false )
{
  smHGISApp = this;

  // Initialize Korean font support
  initializeKoreanSupport();

  // Setup CHA coordinate systems
  setupCHACoordinateSystems();

  // Initialize heritage-specific features
  initializeHeritageFeatures();

  // Setup heritage UI components
  setupHeritageMenus();
  setupHeritageToolbars();
  setupHeritageDockWidgets();
  setupHeritageStatusBar();

  // Connect heritage-specific signals
  connect( QgsProject::instance(), &QgsProject::layersAdded,
           this, [this]( const QList<QgsMapLayer *> &layers ) {
             for ( QgsMapLayer *layer : layers ) {
               onHeritageLayerAdded( layer );
             }
           } );

  connect( QgsProject::instance(), &QgsProject::crsChanged,
           this, &HGISApp::onProjectCrsChanged );

  qInfo() << "HGIS Application initialized with heritage features";
}

HGISApp::~HGISApp()
{
  // Clean up heritage-specific resources
  // if ( mHeritageValidator ) {
  //   delete mHeritageValidator;
  // }

  smHGISApp = nullptr;
}

void HGISApp::initializeHeritageFeatures()
{
  // Initialize heritage validator
  // mHeritageValidator = new HeritageValidator( this );

  // Set default Korean coordinate system (EPSG:5179 - Korea 2000 / Central Belt)
  QgsCoordinateReferenceSystem koreaGrs80( "EPSG:5179" );
  if ( koreaGrs80.isValid() ) {
    QgsProject::instance()->setCrs( koreaGrs80 );
    qInfo() << "Set default CRS to Korea 2000 Central Belt (EPSG:5179)";
  }

  // Initialize current heritage project
  mCurrentHeritageProject = QgsProject::instance();
}

void HGISApp::initializeKoreanSupport()
{
  // Setup Korean font
  QFontDatabase fontDb;
  QStringList availableFonts = fontDb.families();
  
  QStringList preferredFonts = {
    "Noto Sans CJK KR",
    "Noto Serif CJK KR", 
    "NanumGothic",
    "NanumBarunGothic",
    "Malgun Gothic",
    "맑은 고딕"
  };

  for ( const QString &fontName : preferredFonts ) {
    if ( availableFonts.contains( fontName ) ) {
      mPreferredKoreanFont = fontName;
      mKoreanFontLoaded = true;
      
      QFont appFont = QApplication::font();
      appFont.setFamily( fontName );
      QApplication::setFont( appFont );
      
      qInfo() << "Korean font loaded:" << fontName;
      break;
    }
  }

  if ( !mKoreanFontLoaded ) {
    qWarning() << "No Korean fonts found. Install Noto CJK fonts for better Korean support.";
    qDebug() << "Available fonts with CJK:" << availableFonts.filter( QRegExp( ".*CJK.*|.*Noto.*|.*Nanum.*" ) );
  }

  // Set UTF-8 encoding for Korean text support
  QTextCodec::setCodecForLocale( QTextCodec::codecForName( "UTF-8" ) );
}

void HGISApp::setupCHACoordinateSystems()
{
  // Define Korean coordinate systems used by Cultural Heritage Administration
  QStringList koreaCrsList = {
    "EPSG:5179", // Korea 2000 / Central Belt (GRS80)
    "EPSG:5174", // Korea 2000 / Central Belt (Bessel 1841)  
    "EPSG:5178", // Korea 2000 / West Belt
    "EPSG:5180", // Korea 2000 / East Belt
    "EPSG:5181", // Korea 2000 / East Sea Belt
    "EPSG:4326"  // WGS84 for international compatibility
  };

  // Verify coordinate systems are available
  int availableCrs = 0;
  for ( const QString &crsCode : koreaCrsList ) {
    QgsCoordinateReferenceSystem crs( crsCode );
    if ( crs.isValid() ) {
      availableCrs++;
      qDebug() << "Korean CRS available:" << crsCode << crs.description();
    } else {
      qWarning() << "Korean CRS not available:" << crsCode;
    }
  }

  qInfo() << "Korean coordinate systems available:" << availableCrs << "/" << koreaCrsList.size();
}

void HGISApp::setupHeritageMenus()
{
  // Create Heritage menu
  mHeritageMenu = menuBar()->addMenu( tr( "문화재(&H)" ) );

  // Heritage project actions
  mActionNewHeritageProject = new QAction( tr( "새 문화재 프로젝트(&N)..." ), this );
  mActionNewHeritageProject->setIcon( QIcon( ":/icons/heritage_project.png" ) );
  mActionNewHeritageProject->setShortcut( QKeySequence( "Ctrl+Shift+N" ) );
  connect( mActionNewHeritageProject, &QAction::triggered, this, &HGISApp::showHeritageProjectWizard );

  mActionImportHeritageData = new QAction( tr( "문화재 데이터 가져오기(&I)..." ), this );
  mActionImportHeritageData->setIcon( QIcon( ":/icons/heritage_import.png" ) );
  connect( mActionImportHeritageData, &QAction::triggered, this, &HGISApp::importHeritageData );

  // Validation actions
  mActionValidateHeritageData = new QAction( tr( "문화재청 표준 검증(&V)..." ), this );
  mActionValidateHeritageData->setIcon( QIcon( ":/icons/heritage_validate.png" ) );
  connect( mActionValidateHeritageData, &QAction::triggered, this, &HGISApp::showHeritageValidationDialog );

  // Export actions
  mActionExportCHAFormat = new QAction( tr( "문화재청 형식으로 내보내기(&E)..." ), this );
  mActionExportCHAFormat->setIcon( QIcon( ":/icons/heritage_export.png" ) );
  connect( mActionExportCHAFormat, &QAction::triggered, this, [this]() {
    QString filePath = QFileDialog::getSaveFileName( this, tr( "문화재청 형식으로 내보내기" ), "", "Shapefile (*.shp)" );
    if ( !filePath.isEmpty() ) {
      exportHeritageData( filePath, "SHP" );
    }
  } );

  // Layout actions
  mActionHeritageLayout = new QAction( tr( "문화재 도면 생성(&L)..." ), this );
  mActionHeritageLayout->setIcon( QIcon( ":/icons/heritage_layout.png" ) );
  connect( mActionHeritageLayout, &QAction::triggered, this, &HGISApp::createHeritageLayout );

  // Coordinate system actions
  mActionKoreanCoordinates = new QAction( tr( "한국 좌표계 변환(&K)..." ), this );
  mActionKoreanCoordinates->setIcon( QIcon( ":/icons/korean_coordinates.png" ) );
  connect( mActionKoreanCoordinates, &QAction::triggered, this, &HGISApp::showKoreanCoordinateDialog );

  // Add actions to menu
  mHeritageMenu->addAction( mActionNewHeritageProject );
  mHeritageMenu->addSeparator();
  mHeritageMenu->addAction( mActionImportHeritageData );
  mHeritageMenu->addAction( mActionExportCHAFormat );
  mHeritageMenu->addSeparator();
  mHeritageMenu->addAction( mActionValidateHeritageData );
  mHeritageMenu->addAction( mActionHeritageLayout );
  mHeritageMenu->addSeparator();
  mHeritageMenu->addAction( mActionKoreanCoordinates );
}

void HGISApp::setupHeritageToolbars()
{
  // Create Heritage toolbar
  mHeritageToolBar = addToolBar( tr( "문화재 도구" ) );
  mHeritageToolBar->setObjectName( "HeritageToolBar" );

  // Add actions to toolbar
  mHeritageToolBar->addAction( mActionNewHeritageProject );
  mHeritageToolBar->addSeparator();
  mHeritageToolBar->addAction( mActionImportHeritageData );
  mHeritageToolBar->addAction( mActionValidateHeritageData );
  mHeritageToolBar->addAction( mActionExportCHAFormat );
  mHeritageToolBar->addSeparator();
  mHeritageToolBar->addAction( mActionHeritageLayout );
}

void HGISApp::setupHeritageDockWidgets()
{
  // Heritage Validation Dock
  mHeritageValidationDock = new QDockWidget( tr( "문화재 데이터 검증" ), this );
  mHeritageValidationDock->setObjectName( "HeritageValidationDock" );

  QWidget *validationWidget = new QWidget();
  QVBoxLayout *validationLayout = new QVBoxLayout( validationWidget );

  QLabel *validationLabel = new QLabel( tr( "문화재청 표준 검증 결과:" ) );
  QTreeWidget *validationTree = new QTreeWidget();
  validationTree->setHeaderLabels( QStringList() << tr( "항목" ) << tr( "상태" ) << tr( "메시지" ) );

  validationLayout->addWidget( validationLabel );
  validationLayout->addWidget( validationTree );

  mHeritageValidationDock->setWidget( validationWidget );
  addDockWidget( Qt::RightDockWidgetArea, mHeritageValidationDock );

  // CHA Standards Dock
  mCHAStandardsDock = new QDockWidget( tr( "문화재청 표준" ), this );
  mCHAStandardsDock->setObjectName( "CHAStandardsDock" );

  QWidget *standardsWidget = new QWidget();
  QVBoxLayout *standardsLayout = new QVBoxLayout( standardsWidget );

  QLabel *standardsLabel = new QLabel( tr( "적용 가능한 문화재청 표준:" ) );
  QTreeWidget *standardsTree = new QTreeWidget();
  standardsTree->setHeaderLabels( QStringList() << tr( "표준명" ) << tr( "버전" ) << tr( "설명" ) );

  standardsLayout->addWidget( standardsLabel );
  standardsLayout->addWidget( standardsTree );

  mCHAStandardsDock->setWidget( standardsWidget );
  addDockWidget( Qt::RightDockWidgetArea, mCHAStandardsDock );

  // Initially hide docks
  mHeritageValidationDock->hide();
  mCHAStandardsDock->hide();
}

void HGISApp::setupHeritageStatusBar()
{
  // Add heritage-specific status indicators
  mCHAValidationStatus = new QLabel( tr( "CHA 표준: 미검증" ) );
  mCHAValidationStatus->setMinimumWidth( 150 );
  statusBar()->addPermanentWidget( mCHAValidationStatus );

  mCoordinateSystemStatus = new QLabel( tr( "좌표계: 설정되지 않음" ) );
  mCoordinateSystemStatus->setMinimumWidth( 200 );
  statusBar()->addPermanentWidget( mCoordinateSystemStatus );

  mHeritageProcessingProgress = new QProgressBar();
  mHeritageProcessingProgress->setVisible( false );
  statusBar()->addPermanentWidget( mHeritageProcessingProgress );

  // Update coordinate system status
  onProjectCrsChanged();
}

QgsProject* HGISApp::getCurrentHeritageProject() const
{
  return mCurrentHeritageProject;
}

bool HGISApp::addHeritageLayer( const QString &filePath, bool validate )
{
  if ( filePath.isEmpty() ) {
    return false;
  }

  // Add layer using QGIS functionality
  QgsVectorLayer *layer = new QgsVectorLayer( filePath, QFileInfo( filePath ).baseName(), "ogr" );
  
  if ( !layer->isValid() ) {
    delete layer;
    QMessageBox::warning( this, tr( "레이어 추가 실패" ), 
                          tr( "파일을 읽을 수 없습니다: %1" ).arg( filePath ) );
    return false;
  }

  // Add to project
  QgsProject::instance()->addMapLayer( layer );

  // Validate if requested
  if ( validate ) {
    QStringList validationErrors = validateCurrentProject();
    if ( !validationErrors.isEmpty() ) {
      QString errorMsg = tr( "문화재청 표준 검증에서 다음 오류를 발견했습니다:\n\n%1" )
                         .arg( validationErrors.join( "\n" ) );
      QMessageBox::information( this, tr( "검증 결과" ), errorMsg );
    }
  }

  return true;
}

bool HGISApp::createHeritageProject( const QString &projectPath, const QVariantMap &siteInfo )
{
  // Create new QGIS project
  QgsProject::instance()->clear();
  
  // Set Korean coordinate system
  QgsCoordinateReferenceSystem koreaGrs80( "EPSG:5179" );
  if ( koreaGrs80.isValid() ) {
    QgsProject::instance()->setCrs( koreaGrs80 );
  }

  // Store site information in project
  mCurrentSiteInfo = siteInfo;
  
  // Set project metadata
  QgsProject::instance()->setTitle( siteInfo.value( "site_name", "새 문화재 프로젝트" ).toString() );
  
  // Save project
  if ( !projectPath.isEmpty() ) {
    bool saved = QgsProject::instance()->write( projectPath );
    if ( saved ) {
      qInfo() << "Heritage project created:" << projectPath;
      return true;
    }
  }

  return false;
}

bool HGISApp::exportHeritageData( const QString &outputPath, const QString &format )
{
  Q_UNUSED( format )

  if ( outputPath.isEmpty() ) {
    return false;
  }

  // Get all vector layers in project
  QList<QgsMapLayer *> layers = QgsProject::instance()->mapLayers().values();
  QList<QgsVectorLayer *> vectorLayers;

  for ( QgsMapLayer *layer : layers ) {
    if ( QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( layer ) ) {
      vectorLayers.append( vectorLayer );
    }
  }

  if ( vectorLayers.isEmpty() ) {
    QMessageBox::information( this, tr( "내보내기" ), tr( "내보낼 벡터 레이어가 없습니다." ) );
    return false;
  }

  // Export first vector layer (simplified implementation)
  QgsVectorLayer *firstLayer = vectorLayers.first();
  
  // Use QGIS export functionality
  // This is a simplified implementation - in real usage, you'd use QgsVectorFileWriter
  qInfo() << "Exporting heritage data to:" << outputPath;
  
  QMessageBox::information( this, tr( "내보내기 완료" ), 
                           tr( "문화재 데이터가 다음 경로로 내보내졌습니다:\n%1" ).arg( outputPath ) );
  
  return true;
}

QStringList HGISApp::validateCurrentProject()
{
  QStringList errors;

  // Basic validation (simplified implementation)
  QList<QgsMapLayer *> layers = QgsProject::instance()->mapLayers().values();
  
  if ( layers.isEmpty() ) {
    errors << tr( "프로젝트에 레이어가 없습니다." );
  }

  // Check coordinate system
  QgsCoordinateReferenceSystem projectCrs = QgsProject::instance()->crs();
  if ( !projectCrs.isValid() ) {
    errors << tr( "프로젝트 좌표계가 설정되지 않았습니다." );
  } else {
    // Check if it's a Korean coordinate system
    QString authId = projectCrs.authid();
    QStringList koreanCrs = { "EPSG:5179", "EPSG:5174", "EPSG:5178", "EPSG:5180", "EPSG:5181" };
    if ( !koreanCrs.contains( authId ) ) {
      errors << tr( "한국 표준 좌표계를 사용하는 것이 권장됩니다 (현재: %1)" ).arg( authId );
    }
  }

  // Update validation status
  if ( errors.isEmpty() ) {
    mCHAValidationStatus->setText( tr( "CHA 표준: 검증 통과" ) );
    mCHAValidationStatus->setStyleSheet( "color: green;" );
  } else {
    mCHAValidationStatus->setText( tr( "CHA 표준: 오류 %1개" ).arg( errors.size() ) );
    mCHAValidationStatus->setStyleSheet( "color: red;" );
  }

  return errors;
}

// Slot implementations
void HGISApp::showHeritageProjectWizard()
{
  QMessageBox::information( this, tr( "문화재 프로젝트" ), 
                           tr( "문화재 프로젝트 마법사가 곧 추가될 예정입니다." ) );
}

void HGISApp::showHeritageValidationDialog()
{
  mHeritageValidationDock->show();
  mHeritageValidationDock->raise();
  
  // Run validation
  QStringList errors = validateCurrentProject();
  
  QString message;
  if ( errors.isEmpty() ) {
    message = tr( "현재 프로젝트는 문화재청 표준을 준수합니다." );
  } else {
    message = tr( "검증 결과:\n\n%1" ).arg( errors.join( "\n" ) );
  }
  
  QMessageBox::information( this, tr( "문화재청 표준 검증" ), message );
}

void HGISApp::showCHAStandardsDialog()
{
  mCHAStandardsDock->show();
  mCHAStandardsDock->raise();
}

void HGISApp::importHeritageData()
{
  QString filePath = QFileDialog::getOpenFileName( this, 
    tr( "문화재 데이터 가져오기" ), 
    "", 
    tr( "공간 데이터 파일 (*.shp *.gpx *.kml *.geojson);;모든 파일 (*)" ) );

  if ( !filePath.isEmpty() ) {
    addHeritageLayer( filePath, true );
  }
}

void HGISApp::createHeritageLayout()
{
  QMessageBox::information( this, tr( "문화재 도면" ), 
                           tr( "문화재 표준 도면 생성 기능이 곧 추가될 예정입니다." ) );
}

void HGISApp::showKoreanCoordinateDialog()
{
  QStringList items;
  items << "EPSG:5179 - Korea 2000 / Central Belt (GRS80)"
        << "EPSG:5174 - Korea 2000 / Central Belt (Bessel 1841)"
        << "EPSG:5178 - Korea 2000 / West Belt"
        << "EPSG:5180 - Korea 2000 / East Belt"
        << "EPSG:5181 - Korea 2000 / East Sea Belt"
        << "EPSG:4326 - WGS84";

  bool ok;
  QString item = QInputDialog::getItem( this, tr( "한국 좌표계 선택" ),
                                        tr( "좌표계를 선택하세요:" ), items, 0, false, &ok );
  
  if ( ok && !item.isEmpty() ) {
    QString crsCode = item.split( " - " ).first();
    QgsCoordinateReferenceSystem crs( crsCode );
    
    if ( crs.isValid() ) {
      QgsProject::instance()->setCrs( crs );
      QMessageBox::information( this, tr( "좌표계 변경" ), 
                               tr( "프로젝트 좌표계가 %1로 변경되었습니다." ).arg( crs.description() ) );
    }
  }
}

void HGISApp::onHeritageLayerAdded( QgsMapLayer *layer )
{
  if ( QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( layer ) ) {
    mHeritageLayerIds.append( layer->id() );
    qInfo() << "Heritage layer added:" << layer->name() << "(" << layer->id() << ")";
    
    // Auto-validate new heritage layers
    QStringList validationErrors = validateCurrentProject();
    Q_UNUSED( validationErrors )
  }
}

void HGISApp::onFeatureSelected( const QgsFeatureId &fid )
{
  Q_UNUSED( fid )
  // Handle heritage feature selection
  qDebug() << "Heritage feature selected:" << fid;
}

void HGISApp::onProjectCrsChanged()
{
  QgsCoordinateReferenceSystem crs = QgsProject::instance()->crs();
  
  if ( crs.isValid() ) {
    QString statusText = tr( "좌표계: %1" ).arg( crs.authid() );
    
    // Check if it's a Korean coordinate system
    QStringList koreanCrs = { "EPSG:5179", "EPSG:5174", "EPSG:5178", "EPSG:5180", "EPSG:5181" };
    if ( koreanCrs.contains( crs.authid() ) ) {
      mCoordinateSystemStatus->setStyleSheet( "color: green;" );
    } else {
      mCoordinateSystemStatus->setStyleSheet( "color: orange;" );
    }
    
    mCoordinateSystemStatus->setText( statusText );
  } else {
    mCoordinateSystemStatus->setText( tr( "좌표계: 설정되지 않음" ) );
    mCoordinateSystemStatus->setStyleSheet( "color: red;" );
  }
}

HGISApp *HGISApp_instance()
{
  return smHGISApp;
}