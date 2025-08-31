# HGIS (Heritage GIS) - 문화재 전용 독립 GIS 프로그램

## 프로젝트 개요

HGIS는 한국 문화재 전용 독립 GIS 애플리케이션입니다. QGIS 아키텍처를 참조하여 자체 구현한 완전한 독립 실행형 프로그램입니다.

## 핵심 특징

- **독립 실행형**: QGIS 플러그인이 아닌 완전한 독립 애플리케이션
- **문화재 특화**: 문화재청 표준 데이터 모델 지원
- **한국어 인터페이스**: 초보자 친화적 한국어 UI
- **하이브리드 아키텍처**: C++ 코어 + Python 확장

## 기술 스택

- **GUI**: Qt 5.15.2
- **공간 데이터**: GDAL 3.6+, PROJ 9.2+, GEOS 3.11+
- **Python 바인딩**: SIP 6.6+
- **빌드 시스템**: CMake
- **좌표계**: EPSG:5179 (Korea 2000), EPSG:5174 (Korea Central Belt)

## 프로젝트 구조

```
hgis/
├── src/                    # C++ 소스
│   ├── app/               # 메인 애플리케이션
│   ├── core/              # 코어 라이브러리
│   ├── gui/               # GUI 컴포넌트
│   ├── providers/         # 데이터 제공자
│   └── python/            # Python 바인딩
├── python/                # Python 모듈
│   └── hgis/              # HGIS Python 패키지
│       ├── heritage/      # 문화재 특화 기능
│       ├── coordinate/    # 좌표계 모듈
│       └── layout/        # 레이아웃 모듈
├── resources/             # 리소스 파일
├── tests/                 # 테스트
└── docs/                  # 문서
```

## 빌드 방법

### 의존성 설치 (Ubuntu)

```bash
sudo apt-get install -y \
    cmake build-essential \
    qt5-default libqt5widgets5-dev \
    libgdal-dev libproj-dev libgeos-dev \
    python3-dev python3-pip libsip-dev
```

### 빌드

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

## 개발 계획

12주간 단계별 개발 진행:

- **Phase 1 (1-3주)**: 기초 구축 및 기본 지도 캔버스
- **Phase 2 (4-6주)**: 공간 데이터 처리 및 레이어 시스템  
- **Phase 3 (7-9주)**: Python 통합 및 확장 시스템
- **Phase 4 (10-12주)**: 문화재 특화 기능 및 배포

## 현재 상태

- ✅ Phase 1: 기초 구축 완료
- ✅ Phase 2: 공간 데이터 처리 완료
- ✅ Phase 3: Python 통합 완료
- 🔄 Phase 4: 문화재 특화 기능 진행 중

## 라이선스

GPL v2+ (QGIS와 호환)