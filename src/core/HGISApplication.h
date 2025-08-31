#ifndef HGISAPPLICATION_H
#define HGISAPPLICATION_H

#include <QApplication>
#include <QString>
#include <memory>

#ifdef HGIS_CORE_EXPORT
  #define CORE_EXPORT Q_DECL_EXPORT
#else
  #define CORE_EXPORT Q_DECL_IMPORT
#endif

class CORE_EXPORT HGISApplication : public QApplication
{
    Q_OBJECT

public:
    HGISApplication(int &argc, char **argv);
    virtual ~HGISApplication();
    
    bool init();
    
    static HGISApplication* instance();
    
    QString dataPath() const;
    QString pluginPath() const;
    QString configPath() const;
    
    void setDataPath(const QString &path);
    void setPluginPath(const QString &path);
    void setConfigPath(const QString &path);
    
signals:
    void initialized();
    void aboutToTerminate();
    
private:
    void initPaths();
    void initLibraries();
    void loadSettings();
    void saveSettings();
    
    class Private;
    std::unique_ptr<Private> d;
    
    static HGISApplication* sInstance;
};

#endif // HGISAPPLICATION_H