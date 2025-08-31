/***************************************************************************
                            main.cpp  -  HGIS main
                              -------------------
              begin                : 2025-08-31  
              copyright            : (C) 2025 HGIS Project
              email                : support@hgis.org
 ***************************************************************************/

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QFont>
#include <QFontDatabase>
#include <QTextCodec>
#include <QMessageBox>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // UTF-8 인코딩 설정
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    
    // 애플리케이션 정보
    app.setOrganizationName("HGIS");
    app.setOrganizationDomain("hgis.org");
    app.setApplicationName("HGIS");
    app.setApplicationDisplayName("HGIS - Heritage GIS");
    app.setApplicationVersion("1.0.0");
    
    // 한국어 폰트 설정
    QFontDatabase fontDb;
    QStringList availableFonts = fontDb.families();
    
    QStringList preferredFonts = {
        "Noto Sans CJK KR",
        "Noto Serif CJK KR",
        "NanumGothic",
        "NanumBarunGothic",
        "Malgun Gothic"
    };
    
    QString selectedFont;
    for (const QString &font : preferredFonts) {
        if (availableFonts.contains(font)) {
            selectedFont = font;
            break;
        }
    }
    
    if (!selectedFont.isEmpty()) {
        QFont appFont = app.font();
        appFont.setFamily(selectedFont);
        appFont.setPointSize(10);
        app.setFont(appFont);
        qInfo() << "Korean font loaded:" << selectedFont;
    }
    
    // 메인 윈도우 생성
    QMainWindow mainWindow;
    mainWindow.setWindowTitle("HGIS v1.0.0 - 문화재 전용 GIS");
    mainWindow.setMinimumSize(1024, 768);
    
    // 중앙 위젯
    QWidget *centralWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    
    // 제목 라벨
    QLabel *titleLabel = new QLabel("🏛️ HGIS (Heritage GIS)");
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50; margin: 20px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    
    // 설명 라벨
    QLabel *descLabel = new QLabel("한국 문화재청 표준 GIS 애플리케이션");
    descLabel->setStyleSheet("font-size: 16px; color: #34495e; margin: 10px;");
    descLabel->setAlignment(Qt::AlignCenter);
    
    // 기능 버튼들
    QPushButton *newProjectBtn = new QPushButton("새 문화재 프로젝트");
    QPushButton *openProjectBtn = new QPushButton("프로젝트 열기");
    QPushButton *importDataBtn = new QPushButton("문화재 데이터 가져오기");
    QPushButton *aboutBtn = new QPushButton("정보");
    
    // 버튼 스타일링
    QString buttonStyle = "QPushButton { "
                         "background-color: #3498db; "
                         "color: white; "
                         "border: none; "
                         "padding: 10px 20px; "
                         "font-size: 14px; "
                         "border-radius: 5px; "
                         "margin: 5px; "
                         "} "
                         "QPushButton:hover { "
                         "background-color: #2980b9; "
                         "} "
                         "QPushButton:pressed { "
                         "background-color: #21618c; "
                         "}";
    
    newProjectBtn->setStyleSheet(buttonStyle);
    openProjectBtn->setStyleSheet(buttonStyle);
    importDataBtn->setStyleSheet(buttonStyle);
    aboutBtn->setStyleSheet(buttonStyle.replace("#3498db", "#27ae60").replace("#2980b9", "#229954").replace("#21618c", "#1e8449"));
    
    // 버튼 이벤트 연결
    QObject::connect(newProjectBtn, &QPushButton::clicked, [&]() {
        QMessageBox::information(&mainWindow, "새 프로젝트", "새 문화재 프로젝트 기능이 곧 추가될 예정입니다.");
    });
    
    QObject::connect(openProjectBtn, &QPushButton::clicked, [&]() {
        QMessageBox::information(&mainWindow, "프로젝트 열기", "프로젝트 열기 기능이 곧 추가될 예정입니다.");
    });
    
    QObject::connect(importDataBtn, &QPushButton::clicked, [&]() {
        QMessageBox::information(&mainWindow, "데이터 가져오기", "문화재 데이터 가져오기 기능이 곧 추가될 예정입니다.");
    });
    
    QObject::connect(aboutBtn, &QPushButton::clicked, [&]() {
        QString aboutText = "HGIS (Heritage GIS) v1.0.0\n\n"
                           "한국 문화재청 표준 GIS 애플리케이션\n\n"
                           "주요 기능:\n"
                           "• 문화재청 표준 데이터 모델 지원\n"
                           "• 한국 좌표계 (EPSG:5179, 5174) 지원\n"
                           "• 문화재 전용 레이아웃 템플릿\n"
                           "• 한국어 인터페이스\n\n"
                           "Copyright (C) 2025 HGIS Project";
        QMessageBox::about(&mainWindow, "HGIS 정보", aboutText);
    });
    
    // 레이아웃에 위젯 추가
    layout->addWidget(titleLabel);
    layout->addWidget(descLabel);
    layout->addSpacing(30);
    layout->addWidget(newProjectBtn);
    layout->addWidget(openProjectBtn);
    layout->addWidget(importDataBtn);
    layout->addSpacing(20);
    layout->addWidget(aboutBtn);
    layout->addStretch();
    
    // 중앙 위젯 설정
    mainWindow.setCentralWidget(centralWidget);
    
    // 메뉴바 생성
    QMenuBar *menuBar = mainWindow.menuBar();
    
    QMenu *fileMenu = menuBar->addMenu("파일(&F)");
    fileMenu->addAction("새 프로젝트(&N)", [&]() { newProjectBtn->click(); });
    fileMenu->addAction("열기(&O)", [&]() { openProjectBtn->click(); });
    fileMenu->addSeparator();
    fileMenu->addAction("종료(&X)", [&]() { app.quit(); });
    
    QMenu *dataMenu = menuBar->addMenu("데이터(&D)");
    dataMenu->addAction("가져오기(&I)", [&]() { importDataBtn->click(); });
    dataMenu->addAction("내보내기(&E)", [&]() { 
        QMessageBox::information(&mainWindow, "내보내기", "데이터 내보내기 기능이 곧 추가될 예정입니다.");
    });
    
    QMenu *helpMenu = menuBar->addMenu("도움말(&H)");
    helpMenu->addAction("정보(&A)", [&]() { aboutBtn->click(); });
    
    // 윈도우 표시
    mainWindow.show();
    
    qInfo() << "HGIS Heritage GIS started successfully";
    
    return app.exec();
}