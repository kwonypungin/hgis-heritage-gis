# HGIS (Heritage GIS) 독립 실행형 프로그램 개발 계획서
## QGIS 아키텍처 참조 기반 자체 GIS 구현

---

## ⚠️ 핵심 원칙

### 필수 준수 사항
1. **독립 실행형 프로그램** - QGIS 플러그인이 아닌 완전한 독립 애플리케이션
2. **🔥 QGIS 오픈소스 직접 사용** - QGIS 소스코드를 기반으로 문화재 전용 기능 추가
3. **QGIS 코어 라이브러리 기반** - QgsApplication, QgsMapCanvas, QgsVectorLayer 등 QGIS 클래스 직접 활용
4. **문화재청 표준 데이터 모델 통합** - QGIS QgsFeature 기반으로 문화재 속성 확장
5. **단계적 개발** - 각 단계에서 실행 가능한 프로그램 완성

### ⚠️ 중요 변경사항 (2025-08-31)
- **기존 방식:** QGIS 아키텍처 참조 후 자체 구현
- **새로운 방식:** QGIS 오픈소스를 직접 사용하여 문화재 전용 GIS 구현
- **구현 전략:** QGIS 소스코드를 포크하여 문화재 특화 기능 추가

---

## 1. 프로젝트 개요

### 1.1 목적
- **HGIS (Heritage GIS) = QGIS를 고고학/문화재 전용으로 변형**
- **QGIS 오픈소스 100% 기반 개발**
- **고고학/문화재 발굴조사 전용 워크플로우**
- **문화재청 표준 데이터 모델 내장**
- **초보자 친화적 한국어 인터페이스**

### 1.2 핵심 전략
```
QGIS 기반 독립 애플리케이션 아키텍처:
├── QGIS Core Libraries (직접 사용)
│   ├── QgsApplication (메인 애플리케이션)
│   ├── QgsMapCanvas (지도 렌더링)
│   ├── QgsVectorLayer (벡터 데이터)
│   ├── QgsProject (프로젝트 관리)
│   ├── QgsLayout (레이아웃 시스템)
│   └── QgsFeature (공간 피처)
├── Heritage Extensions (문화재 특화)
│   ├── HeritageDataModel (문화재 데이터 모델)
│   ├── HeritageValidator (데이터 검증)
│   ├── CulturalPropertyStandards (문화재청 표준)
│   └── HeritageLayoutTemplates (문화재 도면 템플릿)
└── Korean Localization (한국어 지원)
    ├── UI Translation (인터페이스 번역)
    ├── Korean Coordinate Systems (한국 좌표계)
    └── Cultural Property Workflows (문화재 워크플로우)
```

---

## 2. 기술 아키텍처

### 2.1 QGIS 오픈소스 직접 활용 (GitHub MCP 활용)
**QGIS 소스코드 직접 사용:**
- `qgis/QGIS` 저장소를 포크하여 HGIS 개발
- `src/app/main.cpp` - QGIS 메인 애플리케이션을 HGIS 전용으로 수정
- `src/core/` - QGIS 코어 라이브러리 (QgsApplication, QgsMapCanvas 등) 직접 사용
- `src/gui/` - QGIS GUI 컴포넌트 직접 활용하여 문화재 UI 구현
- `src/python/` - QGIS Python 바인딩을 통해 문화재 스크립트 개발

**직접 사용 라이브러리:**
```yaml
dependencies:
  qt5-base: "5.15.2"         # GUI 프레임워크
  qt5-widgets: "5.15.2"      # 위젯 라이브러리
  gdal: "3.6+"               # 공간 데이터 처리
  proj: "9.2+"               # 좌표 변환
  geos: "3.11+"              # 공간 연산
  python3-dev: "3.9+"        # Python 임베딩
  sip6: "6.6+"               # Python 바인딩
```

### 2.2 프로젝트 구조
```
hgis/
├── src/
│   ├── app/                    # 메인 애플리케이션
│   │   ├── main.cpp            # 프로그램 진입점
│   │   ├── hgisapp.h           # 메인 애플리케이션 클래스
│   │   ├── hgisapp.cpp
│   │   └── CMakeLists.txt
│   ├── core/                   # 코어 라이브러리
│   │   ├── hgisapplication.h   # Qt 애플리케이션 확장
│   │   ├── hgisapplication.cpp
│   │   ├── hgismapcanvas.h     # 지도 캔버스
│   │   ├── hgismapcanvas.cpp
│   │   ├── hgisvectorlayer.h   # 벡터 레이어
│   │   ├── hgisvectorlayer.cpp
│   │   ├── hgisproject.h       # 프로젝트 관리
│   │   ├── hgisproject.cpp
│   │   └── CMakeLists.txt
│   ├── gui/                    # GUI 컴포넌트
│   │   ├── hgismainwindow.h    # 메인 윈도우
│   │   ├── hgismainwindow.cpp
│   │   ├── hgislayertree.h     # 레이어 트리
│   │   ├── hgislayertree.cpp
│   │   ├── hgismaptool.h       # 지도 도구
│   │   ├── hgismaptool.cpp
│   │   └── CMakeLists.txt
│   ├── providers/              # 데이터 제공자
│   │   ├── hgisgdalprovider.h  # GDAL 제공자
│   │   ├── hgisgdalprovider.cpp
│   │   └── CMakeLists.txt
│   └── python/                 # Python 바인딩
│       ├── CMakeLists.txt
│       ├── sipHGISAPI.sip      # SIP 정의 파일
│       ├── hgiscore.sip        # core 모듈 바인딩
│       ├── hgisgui.sip         # gui 모듈 바인딩
│       └── hgis/               # Python 패키지
│           ├── __init__.py
│           ├── core.py
│           ├── gui.py
│           └── utils.py
├── python/                     # 순수 Python 모듈
│   ├── heritage/               # 문화재 특화 기능
│   │   ├── __init__.py
│   │   ├── data_model.py      # 문화재 데이터 모델
│   │   └── validator.py       # 데이터 검증
│   ├── coordinate/             # 좌표계 모듈
│   │   ├── __init__.py
│   │   └── transformer.py     # 좌표 변환
│   └── layout/                 # 레이아웃 모듈
│       ├── __init__.py
│       └── manager.py          # 레이아웃 관리
├── resources/                  # 리소스 파일
│   ├── icons/                  # 아이콘
│   ├── styles/                 # 스타일시트
│   └── translations/           # 번역 파일
├── tests/                      # 테스트
│   ├── cpp/                    # C++ 테스트
│   └── python/                 # Python 테스트
├── docs/                       # 문서
│   ├── architecture.md         # 아키텍처 문서
│   ├── api/                    # API 문서
│   └── user_guide/             # 사용자 가이드
├── CMakeLists.txt             # 최상위 빌드 설정
├── requirements.txt           # Python 의존성
└── setup.py                   # Python 패키지 설정
```

---

## 3. 세분화된 개발 단계 (12주)

### Phase 1: 기초 구축 (3주)

#### 1주차: 환경 설정 및 QGIS 분석
**목표:** 개발 환경 구축 및 QGIS 아키텍처 이해

**작업 내역:**
1. **개발 환경 설정 (15시간)**
   - Qt Creator 설치 및 설정
   - CMake 빌드 시스템 구성
   - GDAL/PROJ 라이브러리 설치
   - Python 개발 환경 구축

2. **QGIS 소스 분석 (20시간)**
   - GitHub MCP로 qgis/QGIS 저장소 분석
   - src/app/main.cpp 진입점 분석
   - QgisApp 클래스 구조 파악
   - QgsApplication 초기화 과정 이해

3. **프로젝트 초기 설정 (15시간)**
   - Git 저장소 구성
   - 디렉토리 구조 생성
   - 기본 CMakeLists.txt 작성
   - 의존성 관리 설정

#### 2주차: 최소 실행 가능 프로그램
**목표:** "Hello World" 수준의 독립 실행 프로그램

**구현 내용:**
```cpp
// src/app/main.cpp
#include <QApplication>
#include <QMainWindow>
#include "hgisapplication.h"

int main(int argc, char *argv[])
{
    HGISApplication app(argc, argv);
    
    QMainWindow window;
    window.setWindowTitle("HGIS - Heritage GIS");
    window.resize(1024, 768);
    window.show();
    
    return app.exec();
}
```

```cpp
// src/core/hgisapplication.h
class HGISApplication : public QApplication
{
    Q_OBJECT
public:
    HGISApplication(int &argc, char **argv);
    static HGISApplication* instance();
    void initializeCore();
};
```

#### 3주차: 기본 지도 캔버스
**목표:** 간단한 지도 표시 기능

**구현 내용:**
- HGISMapCanvas 클래스 구현
- 기본 렌더링 시스템
- 줌/패닝 기능
- 마우스 이벤트 처리

### Phase 2: 공간 데이터 처리 (3주)

#### 4주차: GDAL 통합
**목표:** Shapefile 읽기 기능

**구현 내용:**
```cpp
// src/providers/hgisgdalprovider.cpp
class HGISGdalProvider
{
public:
    bool open(const QString& path);
    QList<HGISFeature> readFeatures();
    QString getProjection();
};
```

#### 5주차: 좌표계 시스템
**목표:** 한국 좌표계 지원

**구현 내용:**
- PROJ 라이브러리 통합
- EPSG:5179 (GRS80) 지원
- EPSG:5174 (Bessel) 지원
- 좌표 변환 기능

#### 6주차: 레이어 시스템
**목표:** 다중 레이어 관리

**구현 내용:**
- HGISVectorLayer 구현
- HGISLayerManager 구현
- 레이어 트리 UI
- 속성 테이블 기초

### Phase 3: Python 통합 (3주)

#### 7주차: Python 바인딩 기초
**목표:** SIP6를 이용한 기본 바인딩

**SIP 정의:**
```sip
// src/python/sipHGISAPI.sip
%Module(name=hgis.core)

class HGISApplication : QApplication
{
%TypeHeaderCode
#include <hgisapplication.h>
%End

public:
    HGISApplication(int &argc, char **argv);
    static HGISApplication* instance();
};
```

#### 8주차: 핵심 API 바인딩
**목표:** core/gui 모듈 Python 노출

**Python 사용 예:**
```python
# Python에서 HGIS 사용
from hgis.core import HGISApplication, HGISVectorLayer
from hgis.gui import HGISMapCanvas

app = HGISApplication([])
canvas = HGISMapCanvas()
layer = HGISVectorLayer("path/to/shapefile.shp")
canvas.addLayer(layer)
canvas.show()
app.exec_()
```

#### 9주차: Python 확장 시스템
**목표:** Python 모듈 로딩 시스템

**구현 내용:**
- 모듈 동적 로딩
- Python 스크립팅 엔진
- 이벤트 시스템 연동
- 사용자 스크립트 실행

### Phase 4: 문화재 특화 기능 (3주)

#### 10주차: 문화재 데이터 모델
**목표:** 문화재청 표준 지원

**Python 구현:**
```python
# python/heritage/data_model.py
class HeritageDataModel:
    REQUIRED_FIELDS = {
        'site_name': str,      # 유적명
        'site_code': str,      # 유적코드
        'period': str,         # 시대
        'area_m2': float,      # 면적
        'coordinate_x': float, # X좌표
        'coordinate_y': float  # Y좌표
    }
    
    def validate(self, feature):
        """문화재 데이터 검증"""
        pass
```

#### 11주차: 출력 시스템
**목표:** 문화재 도면 생성

**구현 내용:**
- 레이아웃 관리자
- A3/A4 템플릿
- PDF/PNG 출력
- 축척 관리
- 범례 생성

#### 12주차: 최종 통합 및 배포
**목표:** 완성된 프로그램 배포

**작업 내역:**
1. **통합 테스트 (20시간)**
   - 기능 테스트
   - 성능 테스트
   - 사용성 테스트

2. **문서화 (15시간)**
   - 사용자 매뉴얼
   - API 문서
   - 설치 가이드

3. **패키징 (15시간)**
   - Windows 인스톨러 (NSIS)
   - Linux AppImage
   - 의존성 번들링

---

## 4. 빌드 시스템

### 4.1 최상위 CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.16)
project(HGIS VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

# Qt5 찾기
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# GDAL/PROJ 찾기
find_package(GDAL REQUIRED)
find_package(PROJ REQUIRED)

# Python 찾기
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# 서브디렉토리 추가
add_subdirectory(src/core)
add_subdirectory(src/gui)
add_subdirectory(src/app)
add_subdirectory(src/providers)
add_subdirectory(src/python)

# 실행 파일 생성
add_executable(hgis src/app/main.cpp)
target_link_libraries(hgis 
    hgis_core 
    hgis_gui
    Qt5::Core 
    Qt5::Gui 
    Qt5::Widgets
    ${GDAL_LIBRARIES}
    ${PROJ_LIBRARIES}
)
```

---

## 5. 테스트 전략

### 5.1 C++ 테스트 (Qt Test)
```cpp
// tests/cpp/test_vectorlayer.cpp
class TestVectorLayer : public QObject
{
    Q_OBJECT
private slots:
    void testLoadShapefile();
    void testCoordinateTransform();
    void testAttributeTable();
};
```

### 5.2 Python 테스트 (pytest)
```python
# tests/python/test_heritage.py
def test_heritage_validation():
    model = HeritageDataModel()
    assert model.validate(test_feature) == True
```

---

## 6. 위험 관리

### 6.1 기술적 위험
| 위험 요소 | 완화 방안 |
|---------|----------|
| Qt/GDAL 버전 호환성 | 고정 버전 사용, Docker 환경 |
| Python 바인딩 복잡도 | SIP 예제 코드 활용, 단계적 구현 |
| 크로스 플랫폼 이슈 | CI/CD로 지속적 테스트 |
| 한글 인코딩 문제 | UTF-8 일관성 유지 |

### 6.2 일정 위험
- 주간 마일스톤 설정
- 2주 단위 스프린트
- 핵심 기능 우선순위화
- 버퍼 시간 확보 (각 Phase별 20%)

---

## 7. 성공 지표

### 7.1 기술적 목표
- ✅ 독립 실행 가능한 프로그램
- ✅ GDAL/PROJ 직접 통합
- ✅ Python 스크립팅 지원
- ✅ 문화재청 표준 Shapefile 생성
- ✅ EPSG:5179 좌표계 지원
- ✅ 크로스 플랫폼 동작

### 7.2 사용성 목표
- ✅ 초보자 30분 내 기본 작업
- ✅ 한국어 인터페이스
- ✅ 단일 실행 파일 배포
- ✅ 5초 내 프로그램 시작

---

## 8. 다음 단계

### 8.1 즉시 시작 작업
```bash
# 프로젝트 디렉토리 구조 생성
mkdir -p src/{app,core,gui,providers,python}
mkdir -p python/{heritage,coordinate,layout}
mkdir -p resources/{icons,styles,translations}
mkdir -p tests/{cpp,python}
mkdir -p docs/{api,user_guide}

# 기본 CMakeLists.txt 생성
touch CMakeLists.txt
touch src/{app,core,gui,providers,python}/CMakeLists.txt

# main.cpp 작성 시작
touch src/app/main.cpp
touch src/core/hgisapplication.{h,cpp}
```

### 8.2 첫 번째 마일스톤 (1주차 완료)
1. ✅ 개발 환경 완전 구축
2. ✅ QGIS 소스 분석 완료
3. ✅ 프로젝트 구조 생성
4. ✅ 기본 빌드 시스템 동작

---

**⚠️ 핵심 원칙 재강조:**
- **독립 실행형 프로그램** - QGIS 플러그인 아님
- **QGIS 소스 참조만** - 직접 의존 없음
- **단계별 검증** - 매주 실행 가능한 프로그램
- **문화재 특화** - 한국 문화재 전용 기능

---

*최종 수정: 2025-08-30*
*참조: QGIS 3.34 소스코드 (GitHub MCP 분석)*