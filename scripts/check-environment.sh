#!/bin/bash
# HGIS 독립 프로그램 개발 환경 검증 스크립트

set -e

# 색상 정의
BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${BLUE}=== HGIS 독립 프로그램 개발 환경 검증 ===${NC}"
echo ""

ERROR_COUNT=0
WARNING_COUNT=0

# 1. Python 환경 확인
echo -e "${BLUE}[1/7] Python 환경 확인${NC}"
if command -v python3 &> /dev/null; then
    PYTHON_VERSION=$(python3 --version | cut -d' ' -f2)
    PYTHON_MAJOR=$(echo $PYTHON_VERSION | cut -d'.' -f1)
    PYTHON_MINOR=$(echo $PYTHON_VERSION | cut -d'.' -f2)
    
    if [ "$PYTHON_MAJOR" -ge 3 ] && [ "$PYTHON_MINOR" -ge 9 ]; then
        echo -e "${GREEN}✅ Python $PYTHON_VERSION (>= 3.9)${NC}"
    else
        echo -e "${RED}❌ Python $PYTHON_VERSION (3.9+ 필요)${NC}"
        ((ERROR_COUNT++))
    fi
else
    echo -e "${RED}❌ Python3 없음${NC}"
    ((ERROR_COUNT++))
fi

# 2. Qt5 개발 환경 확인
echo -e "${BLUE}[2/7] Qt5 개발 환경 확인${NC}"
if command -v qmake &> /dev/null; then
    QT_VERSION=$(qmake -query QT_VERSION 2>/dev/null || echo "unknown")
    echo -e "${GREEN}✅ Qt $QT_VERSION${NC}"
else
    echo -e "${RED}❌ Qt5 개발 도구 없음${NC}"
    echo "   설치: sudo apt-get install qt5-default qtbase5-dev"
    ((ERROR_COUNT++))
fi

# 3. C++ 컴파일러 확인
echo -e "${BLUE}[3/7] C++ 컴파일러 확인${NC}"
if command -v g++ &> /dev/null; then
    GCC_VERSION=$(g++ --version | head -n1)
    echo -e "${GREEN}✅ $GCC_VERSION${NC}"
else
    echo -e "${RED}❌ g++ 컴파일러 없음${NC}"
    echo "   설치: sudo apt-get install build-essential"
    ((ERROR_COUNT++))
fi

# 4. CMake 빌드 시스템 확인
echo -e "${BLUE}[4/7] CMake 빌드 시스템 확인${NC}"
if command -v cmake &> /dev/null; then
    CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
    CMAKE_MAJOR=$(echo $CMAKE_VERSION | cut -d'.' -f1)
    CMAKE_MINOR=$(echo $CMAKE_VERSION | cut -d'.' -f2)
    
    if [ "$CMAKE_MAJOR" -ge 3 ] && [ "$CMAKE_MINOR" -ge 16 ]; then
        echo -e "${GREEN}✅ CMake $CMAKE_VERSION (>= 3.16)${NC}"
    else
        echo -e "${YELLOW}⚠️  CMake $CMAKE_VERSION (3.16+ 권장)${NC}"
        ((WARNING_COUNT++))
    fi
else
    echo -e "${RED}❌ CMake 없음${NC}"
    echo "   설치: sudo apt-get install cmake"
    ((ERROR_COUNT++))
fi

# 5. GDAL 라이브러리 확인
echo -e "${BLUE}[5/7] GDAL 공간 데이터 라이브러리 확인${NC}"
if command -v gdal-config &> /dev/null; then
    GDAL_VERSION=$(gdal-config --version)
    echo -e "${GREEN}✅ GDAL $GDAL_VERSION${NC}"
else
    echo -e "${RED}❌ GDAL 없음${NC}"
    echo "   설치: sudo apt-get install gdal-bin libgdal-dev"
    ((ERROR_COUNT++))
fi

# 6. PROJ 라이브러리 확인
echo -e "${BLUE}[6/7] PROJ 좌표 변환 라이브러리 확인${NC}"
if command -v proj &> /dev/null; then
    echo -e "${GREEN}✅ PROJ 설치됨${NC}"
else
    echo -e "${YELLOW}⚠️  PROJ 명령줄 도구 없음${NC}"
    echo "   설치: sudo apt-get install proj-bin libproj-dev"
    ((WARNING_COUNT++))
fi

# 7. Git 설정 확인
echo -e "${BLUE}[7/7] Git 설정 확인${NC}"
if command -v git &> /dev/null; then
    GIT_USER=$(git config --global user.name 2>/dev/null || echo "")
    GIT_EMAIL=$(git config --global user.email 2>/dev/null || echo "")
    
    if [ -n "$GIT_USER" ] && [ -n "$GIT_EMAIL" ]; then
        echo -e "${GREEN}✅ Git 설정 완료 ($GIT_USER <$GIT_EMAIL>)${NC}"
    else
        echo -e "${YELLOW}⚠️  Git 사용자 정보 미설정${NC}"
        echo "   설정: git config --global user.name \"Your Name\""
        echo "        git config --global user.email \"your@email.com\""
        ((WARNING_COUNT++))
    fi
else
    echo -e "${RED}❌ Git 없음${NC}"
    ((ERROR_COUNT++))
fi

# 결과 요약
echo ""
echo -e "${BLUE}=== 환경 검증 결과 ===${NC}"

if [ $ERROR_COUNT -eq 0 ]; then
    if [ $WARNING_COUNT -eq 0 ]; then
        echo -e "${GREEN}✅ 모든 환경이 준비되었습니다!${NC}"
        echo -e "${GREEN}   독립 실행형 HGIS 프로그램 개발을 시작할 수 있습니다.${NC}"
        exit 0
    else
        echo -e "${YELLOW}⚠️  기본 환경은 준비되었지만 ${WARNING_COUNT}개의 권장사항이 있습니다.${NC}"
        echo -e "${YELLOW}   프로젝트는 시작 가능하지만 일부 기능이 제한될 수 있습니다.${NC}"
        exit 0
    fi
else
    echo -e "${RED}❌ ${ERROR_COUNT}개의 필수 구성요소가 누락되었습니다.${NC}"
    echo -e "${RED}   위의 설치 명령을 참고하여 필수 도구를 설치하세요.${NC}"
    exit 1
fi

echo ""
echo -e "${BLUE}💡 다음 단계:${NC}"
echo "1. ./scripts/init-project.sh 실행하여 프로젝트 구조 생성"
echo "2. CMakeLists.txt 작성"
echo "3. main.cpp 및 HGISApplication 구현"