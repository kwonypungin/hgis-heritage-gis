#ifndef HGISMAINWINDOW_H
#define HGISMAINWINDOW_H

#include <QMainWindow>
#include <memory>

#ifdef HGIS_GUI_EXPORT
  #define GUI_EXPORT Q_DECL_EXPORT
#else
  #define GUI_EXPORT Q_DECL_IMPORT
#endif

class QAction;
class QMenu;
class QToolBar;
class QStatusBar;
class QDockWidget;

class GUI_EXPORT HGISMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HGISMainWindow(QWidget *parent = nullptr);
    virtual ~HGISMainWindow();
    
protected:
    void closeEvent(QCloseEvent *event) override;
    
private slots:
    void newProject();
    void openProject();
    void saveProject();
    void saveProjectAs();
    void openShapefile();
    void selectProjectCrs();
    void about();
    void aboutQt();
    
private:
    void setupUi();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();
    void readSettings();
    void writeSettings();
    
    class Private;
    std::unique_ptr<Private> d;
};

#endif // HGISMAINWINDOW_H