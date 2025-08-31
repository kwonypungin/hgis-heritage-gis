#!/bin/bash
# HGIS 독립 실행형 프로그램 개발 시작 스크립트
# 독립 GIS 애플리케이션 개발 환경 초기화

set -e

# 색상 정의
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
BOLD='\033[1m'
NC='\033[0m'

clear

# 대형 배너
echo -e "${CYAN}"
echo "██╗  ██╗ ██████╗ ██╗███████╗"
echo "██║  ██║██╔════╝ ██║██╔════╝"
echo "███████║██║  ███╗██║███████╗"
echo "██╔══██║██║   ██║██║╚════██║"
echo "██║  ██║╚██████╔╝██║███████║"
echo "╚═╝  ╚═╝ ╚═════╝ ╚═╝╚══════╝"
echo -e "${NC}"
echo -e "${BOLD}Heritage GIS - 독립 실행형 GIS 프로그램${NC}"
echo -e "${YELLOW}QGIS 아키텍처 참조 기반 자체 구현${NC}"
echo ""
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""

# 시작 시간 기록
START_TIME=$(date +%s)
echo -e "${MAGENTA}🚀 개발 시작: $(date '+%Y-%m-%d %H:%M:%S')${NC}"
echo ""

# =====================================
# 1단계: 환경 검증
# =====================================
echo -e "${BOLD}${BLUE}[1/6] 개발 환경 검증${NC}"
echo -e "${YELLOW}→ Qt, GDAL, PROJ, Python 확인${NC}"

if [ -f "./scripts/check-environment.sh" ]; then
    ./scripts/check-environment.sh
    if [ $? -ne 0 ]; then
        echo -e "${RED}❌ 환경 검증 실패. 필수 도구를 설치하세요.${NC}"
        echo -e "${YELLOW}필요한 라이브러리:${NC}"
        echo "  - Qt 5.15.2+"
        echo "  - GDAL 3.6+"
        echo "  - PROJ 9.2+"
        echo "  - Python 3.9+"
        echo "  - CMake 3.16+"
        exit 1
    fi
else
    echo -e "${YELLOW}⚠️  환경 검증 스크립트 없음. 계속 진행...${NC}"
fi
echo ""

# =====================================
# 2단계: Git 상태 확인
# =====================================
echo -e "${BOLD}${BLUE}[2/6] Git 저장소 상태 확인${NC}"

if [ ! -d ".git" ]; then
    echo -e "${YELLOW}Git 저장소 초기화 중...${NC}"
    git init
    git add .
    git commit -m "🎉 HGIS 독립 프로그램 프로젝트 시작" || true
fi

CURRENT_BRANCH=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "none")
echo -e "${GREEN}✅ 현재 브랜치: ${CURRENT_BRANCH}${NC}"
echo ""

# =====================================
# 3단계: 프로젝트 구조 생성
# =====================================
echo -e "${BOLD}${BLUE}[3/6] 독립 프로그램 구조 초기화${NC}"
echo -e "${YELLOW}→ C++ 코어 + Python 바인딩 구조 생성${NC}"

# 독립 프로그램 구조 생성
DIRS_TO_CREATE=(
    "src/app"
    "src/core"
    "src/gui"
    "src/providers"
    "src/python/hgis"
    "python/heritage"
    "python/coordinate"
    "python/layout"
    "resources/icons"
    "resources/styles"
    "resources/translations"
    "tests/cpp"
    "tests/python"
    "docs/api"
    "docs/user_guide"
    "build"
    "snapshots"
)

for dir in "${DIRS_TO_CREATE[@]}"; do
    if [ ! -d "$dir" ]; then
        mkdir -p "$dir"
        echo -e "  ${GREEN}✓${NC} $dir 생성"
    fi
done

echo -e "${GREEN}✅ 프로젝트 구조 생성 완료${NC}"
echo ""

# =====================================
# 4단계: 컨텍스트 초기화
# =====================================
echo -e "${BOLD}${BLUE}[4/6] 컨텍스트 관리 시스템 활성화${NC}"

# 컨텍스트 동기화
if [ -f "./scripts/sync-context.sh" ]; then
    ./scripts/sync-context.sh
fi

# 초기 스냅샷 생성
if [ -f "./scripts/create-snapshot.sh" ]; then
    echo -e "${YELLOW}스냅샷 생성 중...${NC}"
    ./scripts/create-snapshot.sh "dev_start_$(date +%Y%m%d)"
fi
echo ""

# =====================================
# 5단계: Phase별 작업 시작
# =====================================
echo -e "${BOLD}${BLUE}[5/6] Phase 1 작업 준비${NC}"
echo -e "${YELLOW}→ 기초 구축: 독립 실행형 프로그램 기반 작업${NC}"
echo ""

# Phase 1 체크리스트 생성
cat > PHASE1_CHECKLIST.md << 'EOF'
# Phase 1: 기초 구축 (독립 프로그램)

## Week 1: 환경 설정 및 QGIS 분석
- [ ] 개발 환경 설정 (15시간)
  - [ ] Qt Creator 설치 및 설정
  - [ ] CMake 빌드 시스템 구성
  - [ ] GDAL/PROJ 라이브러리 설치
- [ ] QGIS 소스 분석 (20시간) - GitHub MCP 사용
  - [ ] src/app/main.cpp 진입점 분석
  - [ ] QgisApp 클래스 구조 파악
  - [ ] QgsApplication 초기화 과정
- [ ] 프로젝트 초기 설정 (15시간)
  - [ ] 기본 CMakeLists.txt 작성
  - [ ] 의존성 관리 설정

## Week 2: 최소 실행 가능 프로그램
- [ ] main.cpp 구현
  - [ ] HGISApplication 클래스 작성
  - [ ] Qt 메인 윈도우 생성
  - [ ] 기본 메뉴 구성
- [ ] CMake 빌드 설정
  - [ ] Qt5 찾기
  - [ ] 실행 파일 생성

## Week 3: 기본 지도 캔버스
- [ ] HGISMapCanvas 구현
- [ ] 기본 렌더링 시스템
- [ ] 줌/패닝 기능
- [ ] 마우스 이벤트 처리

## MCP 도구 활용 (QGIS 소스 분석용)
- mcp__smithery-ai-github__get_file_contents: QGIS 소스 파일 분석
- mcp__smithery-ai-github__search_code: 특정 클래스/함수 검색
EOF

echo -e "${GREEN}✅ Phase 1 체크리스트 생성 완료${NC}"
echo ""

# =====================================
# 6단계: 빌드 시스템 준비
# =====================================
echo -e "${BOLD}${BLUE}[6/6] 빌드 시스템 초기화${NC}"
echo ""

# 기본 CMakeLists.txt 생성 (없는 경우)
if [ ! -f "CMakeLists.txt" ]; then
    echo -e "${YELLOW}CMakeLists.txt 생성 중...${NC}"
    cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.16)
project(HGIS VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

# Qt5 찾기
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# 메시지 출력
message(STATUS "HGIS 독립 실행형 프로그램 빌드 설정")
message(STATUS "Qt5 버전: ${Qt5_VERSION}")

# 추후 추가될 서브디렉토리
# add_subdirectory(src/core)
# add_subdirectory(src/gui)
# add_subdirectory(src/app)
EOF
    echo -e "${GREEN}✅ CMakeLists.txt 생성 완료${NC}"
fi

echo -e "${CYAN}📚 QGIS 소스 분석 대상 (GitHub MCP 사용):${NC}"
echo "Repository: qgis/QGIS"
echo "주요 파일:"
echo "  - src/app/main.cpp (애플리케이션 진입점)"
echo "  - src/app/qgisapp.cpp (메인 애플리케이션)"
echo "  - src/core/qgsapplication.cpp (코어 초기화)"
echo "  - src/gui/qgsmapcanvas.cpp (맵 캔버스)"
echo ""

# =====================================
# 완료 메시지
# =====================================
END_TIME=$(date +%s)
ELAPSED=$((END_TIME - START_TIME))

echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""
echo -e "${GREEN}${BOLD}✅ 독립 프로그램 개발 환경 준비 완료! (소요시간: ${ELAPSED}초)${NC}"
echo ""
echo -e "${YELLOW}${BOLD}📋 다음 작업:${NC}"
echo -e "${YELLOW}1. CMakeLists.txt 완성${NC}"
echo -e "${YELLOW}2. main.cpp 및 HGISApplication 구현${NC}"
echo -e "${YELLOW}3. GitHub MCP로 QGIS 소스 분석${NC}"
echo -e "${YELLOW}4. 매일 작업 종료시: ./scripts/sync-context.sh${NC}"
echo ""
echo -e "${MAGENTA}${BOLD}💪 독립 실행형 HGIS 프로그램 개발 시작!${NC}"
echo ""

# 자동으로 VS Code 또는 에디터 실행 (선택사항)
if command -v code &> /dev/null; then
    read -p "VS Code를 실행하시겠습니까? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        code .
    fi
fi

exit 0