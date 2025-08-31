/***************************************************************************
                         hgisapp.h  -  description
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

#ifndef HGISAPP_H
#define HGISAPP_H

#include <QMainWindow>
#include <QTimer>
#include <QProgressBar>
#include <QLabel>
#include <QSplashScreen>

// QGIS includes
#include "qgisapp.h"
#include "qgsapplication.h"
#include "qgsmapcanvas.h"
#include "qgsproject.h"
#include "qgsvectorlayer.h"
#include "qgsrasterlayer.h"
#include "qgslayertreeview.h"
#include "qgsmessagebar.h"

// HGIS Heritage includes
#include "../../python/hgis/heritage/data_model.h"
#include "../../python/hgis/heritage/validator.h"
#include "../../python/hgis/heritage/standards.h"

class QAction;
class QMenu;
class QToolBar;
class QDockWidget;
class QTreeWidget;
class QComboBox;
class QSpinBox;
class QCheckBox;

class QgsLayerTreeMapCanvasBridge;
class QgsMapOverviewCanvas;
class QgsGeometry;
class QgsFeature;
class QgsMapToolIdentify;

/**
 * \class HGISApp
 * \brief HGIS main application window - extends QgisApp for heritage/archaeological features
 * 
 * This class extends the standard QGIS application with specialized features for
 * cultural heritage management and archaeological data processing according to
 * Korean Cultural Heritage Administration (CHA) standards.
 */
class HGISApp : public QgisApp
{
    Q_OBJECT

  public:
    /**
     * Constructor
     * \param splashScreen splash screen widget
     * \param restorePlugins whether to restore plugin state
     * \param skipVersionCheck whether to skip version check
     * \param rootProfileFolder root profile folder
     */
    HGISApp( QSplashScreen *splashScreen = nullptr, 
             bool restorePlugins = true, 
             bool skipVersionCheck = false, 
             const QString &rootProfileFolder = QString() );

    ~HGISApp() override;

    /**
     * Initialize HGIS-specific features
     */
    void initializeHeritageFeatures();

    /**
     * Get current heritage project
     */
    QgsProject* getCurrentHeritageProject() const;

    /**
     * Add heritage layer with CHA standards validation
     * \param filePath path to heritage data file
     * \param validate whether to validate against CHA standards
     */
    bool addHeritageLayer( const QString &filePath, bool validate = true );

    /**
     * Create new heritage project with CHA template
     * \param projectPath path for new project
     * \param siteInfo heritage site information
     */
    bool createHeritageProject( const QString &projectPath, const QVariantMap &siteInfo );

    /**
     * Export heritage data to CHA standard format
     * \param outputPath output file path
     * \param format export format (SHP, GPX, etc.)
     */
    bool exportHeritageData( const QString &outputPath, const QString &format = "SHP" );

    /**
     * Validate current project against CHA standards
     */
    QStringList validateCurrentProject();

    /**
     * Get heritage data validator
     */
    // HeritageValidator* getHeritageValidator() const;

  public slots:
    /**
     * Show heritage project wizard
     */
    void showHeritageProjectWizard();

    /**
     * Show heritage data validation dialog
     */
    void showHeritageValidationDialog();

    /**
     * Show CHA standards dialog
     */
    void showCHAStandardsDialog();

    /**
     * Import heritage data from external formats
     */
    void importHeritageData();

    /**
     * Create heritage layout with Korean standards
     */
    void createHeritageLayout();

    /**
     * Show coordinate transformation dialog (Korean coordinate systems)
     */
    void showKoreanCoordinateDialog();

  protected slots:
    /**
     * Called when heritage layer is added
     */
    void onHeritageLayerAdded( QgsMapLayer *layer );

    /**
     * Called when feature is selected for heritage validation
     */
    void onFeatureSelected( const QgsFeatureId &fid );

    /**
     * Called when project CRS changes - ensure Korean CRS compatibility
     */
    void onProjectCrsChanged();

  protected:
    void setupHeritageMenus();
    void setupHeritageToolbars();
    void setupHeritageDockWidgets();
    void setupHeritageStatusBar();
    
    /**
     * Initialize Korean fonts and localization
     */
    void initializeKoreanSupport();

    /**
     * Setup CHA standard coordinate systems
     */
    void setupCHACoordinateSystems();

  private:
    // Heritage-specific UI elements
    QMenu *mHeritageMenu = nullptr;
    QToolBar *mHeritageToolBar = nullptr;
    QDockWidget *mHeritageValidationDock = nullptr;
    QDockWidget *mCHAStandardsDock = nullptr;

    // Heritage actions
    QAction *mActionNewHeritageProject = nullptr;
    QAction *mActionImportHeritageData = nullptr;
    QAction *mActionValidateHeritageData = nullptr;
    QAction *mActionExportCHAFormat = nullptr;
    QAction *mActionHeritageLayout = nullptr;
    QAction *mActionKoreanCoordinates = nullptr;

    // Heritage processing
    // HeritageValidator *mHeritageValidator = nullptr;
    QgsProject *mCurrentHeritageProject = nullptr;
    
    // Korean localization
    bool mKoreanFontLoaded = false;
    QString mPreferredKoreanFont;

    // Status indicators
    QLabel *mCHAValidationStatus = nullptr;
    QLabel *mCoordinateSystemStatus = nullptr;
    QProgressBar *mHeritageProcessingProgress = nullptr;

    // Heritage feature tracking
    QStringList mHeritageLayerIds;
    QVariantMap mCurrentSiteInfo;

    Q_DISABLE_COPY( HGISApp )
};

/**
 * Singleton access to the main HGIS application instance
 */
HGISApp *HGISApp_instance();

#endif // HGISAPP_H