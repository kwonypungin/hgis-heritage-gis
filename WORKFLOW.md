# HGIS 독립 실행형 프로그램 개발 워크플로우

## 🎯 프로젝트 목표
QGIS 아키텍처를 참조하여 **완전히 독립적인 GIS 애플리케이션**을 개발

## 📋 현재 단계: Phase 1 - 기초 구축

### 1주차 체크리스트
- [x] 개발 환경 구축
- [x] HGIS_Plan.md 작성 (독립 프로그램 계획)
- [x] 프로젝트 구조 생성
- [ ] QGIS 소스 분석 (GitHub MCP)
- [ ] 기본 CMakeLists.txt 작성
- [ ] main.cpp 구현

## 🛠️ 개발 명령어

### 프로젝트 시작
```bash
# 개발 환경 확인
./scripts/check-environment.sh

# 프로젝트 빌드
mkdir build && cd build
cmake ..
make -j$(nproc)

# 실행
./hgis
```

### GitHub MCP로 QGIS 분석
```bash
# Claude에서 직접 실행
# mcp__smithery-ai-github__search_code로 QGIS 소스 분석
# 예: qgis/QGIS repo에서 main.cpp, qgsapplication.cpp 등 검색
```

### 개발 진행
```bash
# 현재 상태 동기화
./scripts/sync-context.sh

# 스냅샷 생성
./scripts/create-snapshot.sh "Phase1_Week1_Complete"

# 테스트 실행
cd build && ctest
python -m pytest tests/python/ -v
```

## 📁 프로젝트 구조 (독립 프로그램)

```
hgis/
├── src/
│   ├── app/                    # 메인 애플리케이션
│   │   ├── main.cpp            # 프로그램 진입점
│   │   ├── hgisapp.h/cpp       # 메인 애플리케이션 클래스
│   │   └── CMakeLists.txt
│   ├── core/                   # 코어 라이브러리
│   │   ├── hgisapplication.h/cpp
│   │   ├── hgismapcanvas.h/cpp
│   │   ├── hgisvectorlayer.h/cpp
│   │   └── CMakeLists.txt
│   ├── gui/                    # GUI 컴포넌트
│   │   ├── hgismainwindow.h/cpp
│   │   ├── hgislayertree.h/cpp
│   │   └── CMakeLists.txt
│   ├── providers/              # 데이터 제공자
│   │   ├── hgisgdalprovider.h/cpp
│   │   └── CMakeLists.txt
│   └── python/                 # Python 바인딩
│       ├── sipHGISAPI.sip
│       └── hgis/
├── python/                     # 순수 Python 모듈
│   ├── heritage/               # 문화재 특화
│   ├── coordinate/             # 좌표계
│   └── layout/                 # 레이아웃
├── resources/                  # 리소스
├── tests/                      # 테스트
└── CMakeLists.txt             # 최상위 빌드
```

## 🔄 일일 워크플로우

### 아침 시작
1. `./개발시작.sh` 실행
2. Todo 리스트 확인
3. GitHub MCP로 QGIS 소스 분석 (필요시)

### 작업 중
1. C++ 코어 기능 구현
2. Python 바인딩 추가
3. 테스트 작성
4. 문서 업데이트

### 작업 종료
1. `./scripts/sync-context.sh` 실행
2. 진행 상황 커밋
3. 스냅샷 생성 (주요 마일스톤)

## 📊 진행 상황 (12주 계획)

### Phase 1: 기초 구축 (3주)
- Week 1: [⏳] 환경 설정 및 QGIS 분석
  - [x] 개발 환경 구축
  - [x] 프로젝트 구조 생성
  - [ ] QGIS 소스 분석
  - [ ] CMake 설정
- Week 2: [ ] 최소 실행 가능 프로그램
  - [ ] main.cpp 작성
  - [ ] HGISApplication 구현
  - [ ] 기본 윈도우 표시
- Week 3: [ ] 기본 지도 캔버스
  - [ ] HGISMapCanvas 구현
  - [ ] 렌더링 시스템
  - [ ] 줌/패닝 기능

### Phase 2: 공간 데이터 처리 (3주)
- Week 4: [ ] GDAL 통합
- Week 5: [ ] 좌표계 시스템 (EPSG:5179)
- Week 6: [ ] 레이어 시스템

### Phase 3: Python 통합 (3주)
- Week 7: [ ] Python 바인딩 기초 (SIP6)
- Week 8: [ ] 핵심 API 바인딩
- Week 9: [ ] Python 확장 시스템

### Phase 4: 문화재 특화 (3주)
- Week 10: [ ] 문화재 데이터 모델
- Week 11: [ ] 출력 시스템
- Week 12: [ ] 최종 통합 및 배포

## 🔍 주요 참조 사항

### QGIS 소스 분석 대상 (GitHub MCP)
```
# 분석할 주요 파일들
qgis/QGIS:
├── src/app/main.cpp              # 애플리케이션 진입점
├── src/app/qgisapp.cpp           # 메인 애플리케이션
├── src/core/qgsapplication.cpp   # 코어 초기화
├── src/gui/qgsmapcanvas.cpp      # 맵 캔버스
└── src/python/qgispython.cpp     # Python 바인딩
```

### 직접 사용 라이브러리
- Qt 5.15.2 (GUI 프레임워크)
- GDAL 3.6+ (공간 데이터)
- PROJ 9.2+ (좌표 변환)
- Python 3.9+ (스크립팅)
- SIP 6.6+ (Python 바인딩)

## ⚠️ 중요 사항
1. **독립 실행형** - QGIS 플러그인이 아닌 독립 프로그램
2. **자체 구현** - QGIS 라이브러리 직접 사용 안함
3. **참조만** - QGIS 소스는 아키텍처 학습용
4. **단계별 검증** - 매주 실행 가능한 프로그램

## 🛠️ 빌드 및 테스트

### C++ 빌드
```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
```

### Python 바인딩 빌드
```bash
cd src/python
sip-build
```

### 테스트 실행
```bash
# C++ 테스트
cd build && ctest -V

# Python 테스트
python -m pytest tests/python/ -v
```

## 📝 관련 문서
- [HGIS_Plan.md](HGIS_Plan.md) - 전체 개발 계획
- [MCP_QUICK_REFERENCE.md](MCP_QUICK_REFERENCE.md) - GitHub MCP 사용법
- [docs/architecture.md](docs/architecture.md) - 아키텍처 문서

## 💡 개발 팁

### GitHub MCP 활용
```python
# QGIS 소스 검색 예시
mcp__smithery-ai-github__search_code(
    q="repo:qgis/QGIS path:src/app main.cpp",
    per_page=5
)

# 특정 파일 내용 보기
mcp__smithery-ai-github__get_file_contents(
    owner="qgis",
    repo="QGIS",
    path="src/app/main.cpp",
    mode="overview"
)
```

### 커밋 메시지 규칙
- feat: 새 기능 추가
- fix: 버그 수정
- docs: 문서 변경
- refactor: 코드 리팩토링
- test: 테스트 추가
- build: 빌드 시스템 변경

---
*독립 실행형 HGIS 프로그램 개발을 위한 워크플로우*