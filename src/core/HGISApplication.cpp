#include "HGISApplication.h"
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QSettings>

#ifdef GDAL_VERSION_NUM
#include <gdal.h>
#include <ogr_api.h>
#endif

#ifdef PROJ_VERSION_MAJOR
#include <proj.h>
#endif

HGISApplication* HGISApplication::sInstance = nullptr;

class HGISApplication::Private
{
public:
    QString dataPath;
    QString pluginPath;
    QString configPath;
    QSettings* settings = nullptr;
    
    Private()
    {
        settings = new QSettings("HGIS", "HGIS");
    }
    
    ~Private()
    {
        delete settings;
    }
};

HGISApplication::HGISApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , d(std::make_unique<Private>())
{
    sInstance = this;
}

HGISApplication::~HGISApplication()
{
    emit aboutToTerminate();
    saveSettings();
    sInstance = nullptr;
}

bool HGISApplication::init()
{
    qInfo() << "Initializing HGIS Application...";
    
    initPaths();
    initLibraries();
    loadSettings();
    
    emit initialized();
    
    qInfo() << "HGIS Application initialized successfully";
    qInfo() << "Data path:" << d->dataPath;
    qInfo() << "Plugin path:" << d->pluginPath;
    qInfo() << "Config path:" << d->configPath;
    
    return true;
}

HGISApplication* HGISApplication::instance()
{
    return sInstance;
}

void HGISApplication::initPaths()
{
    QString appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    
    d->dataPath = appDataLocation + "/data";
    d->pluginPath = appDataLocation + "/plugins";
    d->configPath = appDataLocation + "/config";
    
    QDir().mkpath(d->dataPath);
    QDir().mkpath(d->pluginPath);
    QDir().mkpath(d->configPath);
}

void HGISApplication::initLibraries()
{
#ifdef GDAL_VERSION_NUM
    GDALAllRegister();
    OGRRegisterAll();
    qInfo() << "GDAL initialized, version:" << GDALVersionInfo("RELEASE_NAME");
#else
    qWarning() << "GDAL not available - spatial data support limited";
#endif

#ifdef PROJ_VERSION_MAJOR
    PJ_INFO info = proj_info();
    qInfo() << "PROJ initialized, version:" << info.release;
#else
    qWarning() << "PROJ not available - coordinate transformation support limited";
#endif
}

void HGISApplication::loadSettings()
{
    if (d->settings) {
        d->settings->beginGroup("Paths");
        if (d->settings->contains("DataPath")) {
            d->dataPath = d->settings->value("DataPath").toString();
        }
        if (d->settings->contains("PluginPath")) {
            d->pluginPath = d->settings->value("PluginPath").toString();
        }
        if (d->settings->contains("ConfigPath")) {
            d->configPath = d->settings->value("ConfigPath").toString();
        }
        d->settings->endGroup();
    }
}

void HGISApplication::saveSettings()
{
    if (d->settings) {
        d->settings->beginGroup("Paths");
        d->settings->setValue("DataPath", d->dataPath);
        d->settings->setValue("PluginPath", d->pluginPath);
        d->settings->setValue("ConfigPath", d->configPath);
        d->settings->endGroup();
        d->settings->sync();
    }
}

QString HGISApplication::dataPath() const
{
    return d->dataPath;
}

QString HGISApplication::pluginPath() const
{
    return d->pluginPath;
}

QString HGISApplication::configPath() const
{
    return d->configPath;
}

void HGISApplication::setDataPath(const QString &path)
{
    d->dataPath = path;
    QDir().mkpath(path);
}

void HGISApplication::setPluginPath(const QString &path)
{
    d->pluginPath = path;
    QDir().mkpath(path);
}

void HGISApplication::setConfigPath(const QString &path)
{
    d->configPath = path;
    QDir().mkpath(path);
}