#include "HGISCrsSelectionDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QListWidgetItem>

class HGISCrsSelectionDialog::Private
{
public:
    QListWidget *crsList = nullptr;
    QLineEdit *searchEdit = nullptr;
    QTextEdit *detailsEdit = nullptr;
    QPushButton *korea2000Btn = nullptr;
    QPushButton *koreaBessel1987Btn = nullptr;
    QPushButton *wgs84Btn = nullptr;
    
    HGISCoordinateReferenceSystem selectedCrs;
    
    struct CrsInfo {
        int epsg;
        QString name;
        QString description;
    };
    
    QList<CrsInfo> allCrsList;
};

HGISCrsSelectionDialog::HGISCrsSelectionDialog(QWidget *parent)
    : QDialog(parent)
    , d(std::make_unique<Private>())
{
    setupUi();
    populateCrsList();
}

HGISCrsSelectionDialog::~HGISCrsSelectionDialog() = default;

void HGISCrsSelectionDialog::setupUi()
{
    setWindowTitle("좌표계 선택");
    setMinimumSize(800, 600);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 빠른 선택 버튼 그룹
    QGroupBox *quickSelectGroup = new QGroupBox("빠른 선택 - 한국 좌표계");
    QHBoxLayout *quickLayout = new QHBoxLayout(quickSelectGroup);
    
    d->korea2000Btn = new QPushButton("Korea 2000 / Central Belt\n중부원점 TM (EPSG:5186)");
    d->korea2000Btn->setMinimumHeight(60);
    quickLayout->addWidget(d->korea2000Btn);
    
    d->koreaBessel1987Btn = new QPushButton("Korean 1985 중부원점\n(EPSG:5174)");
    d->koreaBessel1987Btn->setMinimumHeight(60);
    quickLayout->addWidget(d->koreaBessel1987Btn);
    
    d->wgs84Btn = new QPushButton("WGS 84\n(EPSG:4326)");
    d->wgs84Btn->setMinimumHeight(60);
    quickLayout->addWidget(d->wgs84Btn);
    
    mainLayout->addWidget(quickSelectGroup);
    
    // 검색 필드
    QHBoxLayout *searchLayout = new QHBoxLayout();
    QLabel *searchLabel = new QLabel("검색:");
    d->searchEdit = new QLineEdit();
    d->searchEdit->setPlaceholderText("좌표계 이름 또는 EPSG 코드 입력...");
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(d->searchEdit);
    mainLayout->addLayout(searchLayout);
    
    // CRS 목록과 상세 정보
    QHBoxLayout *contentLayout = new QHBoxLayout();
    
    // CRS 목록
    QVBoxLayout *listLayout = new QVBoxLayout();
    QLabel *listLabel = new QLabel("좌표계 목록:");
    d->crsList = new QListWidget();
    listLayout->addWidget(listLabel);
    listLayout->addWidget(d->crsList);
    contentLayout->addLayout(listLayout, 2);
    
    // 상세 정보
    QVBoxLayout *detailsLayout = new QVBoxLayout();
    QLabel *detailsLabel = new QLabel("상세 정보:");
    d->detailsEdit = new QTextEdit();
    d->detailsEdit->setReadOnly(true);
    detailsLayout->addWidget(detailsLabel);
    detailsLayout->addWidget(d->detailsEdit);
    contentLayout->addLayout(detailsLayout, 1);
    
    mainLayout->addLayout(contentLayout);
    
    // 버튼
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);
    
    // 시그널 연결
    connect(d->searchEdit, &QLineEdit::textChanged,
            this, &HGISCrsSelectionDialog::onSearchTextChanged);
    connect(d->crsList, &QListWidget::itemSelectionChanged,
            this, &HGISCrsSelectionDialog::onCrsSelectionChanged);
    connect(d->korea2000Btn, &QPushButton::clicked,
            this, [this]() {
                d->selectedCrs = HGISCoordinateReferenceSystem::korea2000Central();
                accept();
            });
    connect(d->koreaBessel1987Btn, &QPushButton::clicked,
            this, [this]() {
                d->selectedCrs = HGISCoordinateReferenceSystem::koreaBessel1987Central();
                accept();
            });
    connect(d->wgs84Btn, &QPushButton::clicked,
            this, [this]() {
                d->selectedCrs = HGISCoordinateReferenceSystem::wgs84();
                accept();
            });
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void HGISCrsSelectionDialog::populateCrsList()
{
    // 주요 좌표계 목록
    d->allCrsList.clear();
    
    // 전세계 좌표계
    d->allCrsList.append({4326, "WGS 84", "World Geodetic System 1984"});
    d->allCrsList.append({3857, "WGS 84 / Pseudo-Mercator", "Web Mercator projection"});
    
    // 한국 좌표계 - Korea 2000 TM (현재 표준)
    d->allCrsList.append({5186, "Korea 2000 / Central Belt", "Korea 2000 / Central Belt 중부원점 TM"});
    d->allCrsList.append({5185, "Korea 2000 / West Belt", "Korea 2000 / West Belt 서부원점 TM"});
    d->allCrsList.append({5187, "Korea 2000 / East Belt", "Korea 2000 / East Belt 동부원점 TM"});
    d->allCrsList.append({5188, "Korea 2000 / East Sea Belt", "Korea 2000 / East Sea Belt 동해(울릉)원점 TM"});
    d->allCrsList.append({5179, "Korea 2000 / Unified CS (UTM-K)", "Korea 2000 통일원점 (네이버 지도)"});
    
    // 한국 좌표계 - Korean 1985 (Bessel)
    d->allCrsList.append({5174, "Korean 1985 / Central Belt", "Korean 1985 중부원점 (Bessel)"});
    d->allCrsList.append({5175, "Korean 1985 / West Belt", "Korean 1985 서부원점 (Bessel)"});
    d->allCrsList.append({5176, "Korean 1985 / East Belt", "Korean 1985 동부원점 (Bessel)"});
    d->allCrsList.append({5177, "Korean 1985 / East Sea Belt", "Korean 1985 동해원점 (Bessel)"});
    d->allCrsList.append({5178, "Korean 1985 / Unified CS", "Korean 1985 통일원점 (Bessel)"});
    
    // 한국 좌표계 - Korea 2000 지리좌표계
    d->allCrsList.append({4737, "Korea 2000", "Korea 2000 지리좌표계 (GRS80)"});
    d->allCrsList.append({4162, "Korean 1985", "Korean 1985 지리좌표계 (Bessel)"});
    
    // UTM 좌표계 (한국 지역)
    d->allCrsList.append({32651, "WGS 84 / UTM zone 51N", "한국 서부 지역 UTM"});
    d->allCrsList.append({32652, "WGS 84 / UTM zone 52N", "한국 동부 지역 UTM"});
    
    // 목록에 추가
    for (const auto &crs : d->allCrsList) {
        addCrsItem(crs.epsg, crs.name, crs.description);
    }
}

void HGISCrsSelectionDialog::addCrsItem(int epsg, const QString &name, const QString &description)
{
    QListWidgetItem *item = new QListWidgetItem(d->crsList);
    item->setText(QString("EPSG:%1 - %2").arg(epsg).arg(name));
    item->setData(Qt::UserRole, epsg);
    item->setData(Qt::UserRole + 1, name);
    item->setData(Qt::UserRole + 2, description);
}

void HGISCrsSelectionDialog::onSearchTextChanged(const QString &text)
{
    for (int i = 0; i < d->crsList->count(); ++i) {
        QListWidgetItem *item = d->crsList->item(i);
        bool visible = text.isEmpty() || 
                      item->text().contains(text, Qt::CaseInsensitive);
        item->setHidden(!visible);
    }
}

void HGISCrsSelectionDialog::onCrsSelectionChanged()
{
    QListWidgetItem *current = d->crsList->currentItem();
    if (!current) {
        d->detailsEdit->clear();
        return;
    }
    
    int epsg = current->data(Qt::UserRole).toInt();
    QString name = current->data(Qt::UserRole + 1).toString();
    QString description = current->data(Qt::UserRole + 2).toString();
    
    // CRS 생성 및 상세 정보 표시
    HGISCoordinateReferenceSystem crs(epsg);
    if (crs.isValid()) {
        d->selectedCrs = crs;
        
        QString details = QString("EPSG 코드: %1\n"
                                 "이름: %2\n"
                                 "설명: %3\n"
                                 "타입: %4\n"
                                 "단위: %5\n\n"
                                 "PROJ 문자열:\n%6")
            .arg(epsg)
            .arg(name)
            .arg(description)
            .arg(crs.isGeographic() ? "지리좌표계" : "투영좌표계")
            .arg(crs.mapUnits())
            .arg(crs.toProj());
        
        d->detailsEdit->setText(details);
    } else {
        d->detailsEdit->setText("좌표계 정보를 가져올 수 없습니다.");
    }
}

HGISCoordinateReferenceSystem HGISCrsSelectionDialog::selectedCrs() const
{
    return d->selectedCrs;
}

void HGISCrsSelectionDialog::setCurrentCrs(const HGISCoordinateReferenceSystem &crs)
{
    d->selectedCrs = crs;
    
    // 목록에서 해당 CRS 선택
    int epsg = crs.epsgCode();
    for (int i = 0; i < d->crsList->count(); ++i) {
        QListWidgetItem *item = d->crsList->item(i);
        if (item->data(Qt::UserRole).toInt() == epsg) {
            d->crsList->setCurrentItem(item);
            break;
        }
    }
}

void HGISCrsSelectionDialog::onKoreaCrsButtonClicked()
{
    // 한국 좌표계만 필터링
    d->searchEdit->setText("Korea");
}