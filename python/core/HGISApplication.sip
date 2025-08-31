// HGISApplication 클래스 바인딩

class HGISApplication : QApplication
{
%TypeHeaderCode
#include "HGISApplication.h"
%End

public:
    HGISApplication(int &argc, char **argv);
    virtual ~HGISApplication();
    
    static HGISApplication* instance();
    
    // 초기화
    void initialize();
    bool isInitialized() const;
    
    // 경로 정보
    QString dataPath() const;
    QString pluginPath() const;
    QString configPath() const;
    
    // 플러그인 관리
    void loadPlugins();
    void loadPlugin(const QString &path);
    QStringList availablePlugins() const;
    
    // 메타데이터
    static QString version();
    static int versionInt();
    static QString releaseName();
    
signals:
    void initialized();
    void pluginLoaded(const QString &name);
    void criticalMessage(const QString &message);
};