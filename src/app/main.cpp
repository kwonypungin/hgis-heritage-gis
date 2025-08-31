/***************************************************************************
                            main.cpp  -  HGIS main
                              -------------------
              begin                : 2025-08-31  
              copyright            : (C) 2025 HGIS Project
              email                : support@hgis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// Qt includes
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QFontDatabase>
#include <QLocale>
#include <QSplashScreen>
#include <QString>
#include <QStringList>
#include <QTranslator>
#include <QTextCodec>
#include <QPixmap>
#include <QMessageBox>
#include <QDebug>
#include <cstdio>
#include <cstdlib>

// QGIS includes
#include "qgsapplication.h"
#include "qgssettings.h"
#include "qgsmessagelog.h"
#include "qgslogger.h"

// HGIS includes  
#include "hgisapp.h"

int main( int argc, char *argv[] )
{
  // Initialize QgsApplication instead of QApplication for QGIS integration
  QgsApplication::init();
  QgsApplication app( argc, argv, true );

  // Set organization info for HGIS
  app.setOrganizationName( QStringLiteral( "HGIS" ) );
  app.setOrganizationDomain( QStringLiteral( "hgis.org" ) );
  app.setApplicationName( QStringLiteral( "HGIS" ) );
  app.setApplicationDisplayName( QStringLiteral( "HGIS - Heritage GIS" ) );
  app.setApplicationVersion( QStringLiteral( "1.0.0" ) );

  // Setup Korean font support
  QFontDatabase fontDb;
  QStringList availableFonts = fontDb.families();
  
  QStringList preferredKoreanFonts = {
    "Noto Sans CJK KR",
    "Noto Serif CJK KR", 
    "NanumGothic",
    "NanumBarunGothic",
    "Malgun Gothic",
    "맑은 고딕"
  };

  QString selectedFont;
  for ( const QString &fontName : preferredKoreanFonts ) {
    if ( availableFonts.contains( fontName ) ) {
      selectedFont = fontName;
      break;
    }
  }

  if ( !selectedFont.isEmpty() ) {
    QFont appFont = app.font();
    appFont.setFamily( selectedFont );
    appFont.setPointSize( 10 );
    app.setFont( appFont );
    qInfo() << "Korean font set:" << selectedFont;
  } else {
    qWarning() << "Korean fonts not found. Install Noto CJK fonts for better Korean support.";
    qDebug() << "Available Korean fonts:" << availableFonts.filter( QRegExp( ".*CJK.*|.*Noto.*|.*Nanum.*|.*Gothic.*" ) );
  }

  // Set UTF-8 encoding for Korean text
  QTextCodec::setCodecForLocale( QTextCodec::codecForName( "UTF-8" ) );

  // Setup Korean translation
  QTranslator translator;
  QLocale locale = QLocale::system();
  
  if ( locale.language() == QLocale::Korean ) {
    QString translationPath = app.applicationDirPath() + "/../resources/translations/hgis_ko";
    if ( translator.load( translationPath ) ) {
      app.installTranslator( &translator );
      qInfo() << "Korean translation loaded successfully";
    } else {
      qWarning() << "Failed to load Korean translation from:" << translationPath;
    }
  }

  // Initialize QGIS application
  app.initQgis();
  
  // Create and show splash screen
  QPixmap splashPix( ":/images/splash/hgis_splash.png" );
  if ( splashPix.isNull() ) {
    // Use default QGIS splash if HGIS splash not found
    splashPix = QPixmap( ":/images/splash/splash.png" );
  }
  
  QSplashScreen *splash = nullptr;
  if ( !splashPix.isNull() ) {
    splash = new QSplashScreen( splashPix );
    splash->show();
    splash->showMessage( QObject::tr( "HGIS - Heritage GIS 시작 중..." ), 
                         Qt::AlignBottom | Qt::AlignCenter, Qt::white );
    app.processEvents();
  }

  // Initialize HGIS main application
  HGISApp *hgisApp = new HGISApp( splash, true, false );
  
  // Set window properties
  hgisApp->setWindowTitle( QStringLiteral( "HGIS v1.0.0 - 문화재 전용 GIS" ) );
  hgisApp->setWindowIcon( QIcon( ":/images/icons/hgis.png" ) );
  
  // Show main window
  hgisApp->show();
  
  // Hide splash screen
  if ( splash ) {
    splash->finish( hgisApp );
    delete splash;
  }

  // Log startup
  QgsMessageLog::logMessage( QStringLiteral( "HGIS Heritage GIS started successfully" ), 
                            QStringLiteral( "HGIS" ), Qgis::Info );
  qInfo() << "HGIS Heritage GIS application started";

  // Run application event loop
  int result = app.exec();

  // Cleanup
  delete hgisApp;
  app.exitQgis();

  return result;
}