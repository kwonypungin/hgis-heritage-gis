#ifndef HGISCRSSELECTIONDIALOG_H
#define HGISCRSSELECTIONDIALOG_H

#include <QDialog>
#include <memory>
#include "core/HGISCoordinateReferenceSystem.h"

class QListWidget;
class QLineEdit;
class QTextEdit;
class QPushButton;

class HGISCrsSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HGISCrsSelectionDialog(QWidget *parent = nullptr);
    ~HGISCrsSelectionDialog();
    
    // 선택된 CRS 가져오기
    HGISCoordinateReferenceSystem selectedCrs() const;
    
    // 현재 CRS 설정
    void setCurrentCrs(const HGISCoordinateReferenceSystem &crs);
    
private slots:
    void onSearchTextChanged(const QString &text);
    void onCrsSelectionChanged();
    void onKoreaCrsButtonClicked();
    
private:
    void setupUi();
    void populateCrsList();
    void addCrsItem(int epsg, const QString &name, const QString &description);
    
    class Private;
    std::unique_ptr<Private> d;
};

#endif // HGISCRSSELECTIONDIALOG_H