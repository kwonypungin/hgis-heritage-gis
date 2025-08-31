#!/bin/bash
# HGIS 컨텍스트 동기화 스크립트
# 독립 프로그램 개발 상태를 추적하고 동기화

set -e

# 색상 정의
BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${BLUE}=== HGIS 프로젝트 컨텍스트 동기화 ===${NC}"

# 현재 브랜치 확인
CURRENT_BRANCH=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "main")
echo -e "${YELLOW}현재 브랜치: ${CURRENT_BRANCH}${NC}"

# CLAUDE.md 파일 생성/업데이트
cat > CLAUDE.md << 'EOF'
# HGIS 독립 실행형 프로그램 개발 컨텍스트

## 프로젝트 개요
- **목표**: QGIS 아키텍처를 참조한 독립 GIS 애플리케이션
- **구조**: C++ 코어 + Python 바인딩 (SIP6)
- **상태**: Phase 1 - 기초 구축 중

## 현재 진행 상황
- Phase 1 Week 1: 환경 설정 및 QGIS 분석
- [x] 프로젝트 구조 생성
- [x] 개발 계획 수립 (HGIS_Plan.md)
- [ ] CMakeLists.txt 작성
- [ ] main.cpp 구현
- [ ] HGISApplication 클래스 구현

## 기술 스택
- Qt 5.15.2 (GUI 프레임워크)
- GDAL 3.6+ (공간 데이터)
- PROJ 9.2+ (좌표 변환)
- Python 3.9+ (스크립팅)
- SIP 6.6+ (Python 바인딩)

## 프로젝트 구조
```
hgis/
├── src/
│   ├── app/         # 메인 애플리케이션
│   ├── core/        # 코어 라이브러리
│   ├── gui/         # GUI 컴포넌트
│   ├── providers/   # 데이터 제공자
│   └── python/      # Python 바인딩
├── python/          # Python 모듈
├── resources/       # 리소스
├── tests/          # 테스트
└── CMakeLists.txt  # 빌드 설정
```

## QGIS 소스 분석 대상 (GitHub MCP)
- qgis/QGIS 저장소
- src/app/main.cpp
- src/core/qgsapplication.cpp
- src/gui/qgsmapcanvas.cpp

## 다음 작업
1. CMakeLists.txt 완성
2. main.cpp 및 HGISApplication 구현
3. 기본 Qt 윈도우 표시
4. GitHub MCP로 QGIS 아키텍처 분석

## 참조 문서
- [HGIS_Plan.md](HGIS_Plan.md) - 전체 개발 계획
- [WORKFLOW.md](WORKFLOW.md) - 개발 워크플로우
- [MCP_QUICK_REFERENCE.md](MCP_QUICK_REFERENCE.md) - GitHub MCP 사용법
EOF

echo -e "${GREEN}✅ CLAUDE.md 업데이트 완료${NC}"

# Git 상태 확인
if [ -d ".git" ]; then
    # 변경사항이 있으면 자동 커밋
    if ! git diff --quiet || ! git diff --cached --quiet; then
        echo -e "${YELLOW}변경사항 발견 - 자동 커밋 수행${NC}"
        git add -A
        git commit -m "📝 컨텍스트 업데이트: $(date '+%Y-%m-%d %H:%M')" || true
        echo -e "${GREEN}✅ 자동 커밋 완료${NC}"
    else
        echo -e "${GREEN}✅ 변경사항 없음${NC}"
    fi
fi

# 프로젝트 현황 출력
echo -e "${BLUE}=== 프로젝트 현황 ===${NC}"

# 커밋 수 계산
if [ -d ".git" ]; then
    COMMIT_COUNT=$(git rev-list --count HEAD 2>/dev/null || echo "0")
    echo "총 커밋 수: ${COMMIT_COUNT}"
fi

# 파일 통계
if [ -d "src" ]; then
    CPP_FILES=$(find src -name "*.cpp" -o -name "*.h" 2>/dev/null | wc -l)
    echo "C++ 파일: ${CPP_FILES}개"
fi

if [ -d "python" ] || [ -d "src/python" ]; then
    PY_FILES=$(find . -name "*.py" -not -path "./build/*" 2>/dev/null | wc -l)
    echo "Python 파일: ${PY_FILES}개"
fi

echo -e "${GREEN}=== 컨텍스트 동기화 완료 ===${NC}"