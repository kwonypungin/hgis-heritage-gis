#!/bin/bash
# HGIS 프로젝트 스냅샷 생성 스크립트
# 중요 시점의 프로젝트 상태를 저장

set -e

# 색상 정의
BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${BLUE}=== HGIS 컨텍스트 스냅샷 생성 ===${NC}"

# 스냅샷 이름 설정
if [ -z "$1" ]; then
    SNAPSHOT_NAME="snapshot_$(date +%Y%m%d_%H%M%S)"
else
    SNAPSHOT_NAME="$1_$(date +%Y%m%d_%H%M%S)"
fi

echo -e "${YELLOW}스냅샷명: ${SNAPSHOT_NAME}${NC}"

# 스냅샷 디렉토리 생성
SNAPSHOT_DIR="snapshots/${SNAPSHOT_NAME}"
mkdir -p "${SNAPSHOT_DIR}"

# Git 상태 저장
echo -e "${BLUE}📊 Git 상태 저장 중...${NC}"
if [ -d ".git" ]; then
    git status > "${SNAPSHOT_DIR}/git_status.txt"
    git log --oneline -10 > "${SNAPSHOT_DIR}/git_log.txt"
fi

# 프로젝트 구조 저장
echo -e "${BLUE}📁 프로젝트 구조 저장 중...${NC}"
tree -I 'build|__pycache__|*.pyc|.git' > "${SNAPSHOT_DIR}/project_structure.txt" 2>/dev/null || \
    find . -type d -not -path "./build/*" -not -path "./.git/*" | sort > "${SNAPSHOT_DIR}/project_structure.txt"

# 중요 설정 파일 백업
echo -e "${BLUE}⚙️ 설정 파일 백업 중...${NC}"
mkdir -p "${SNAPSHOT_DIR}/config_files"

# 백업할 파일 목록
FILES_TO_BACKUP=(
    "HGIS_Plan.md"
    "WORKFLOW.md"
    "CLAUDE.md"
    "CMakeLists.txt"
    "README.md"
    ".gitignore"
    "requirements.txt"
    "setup.py"
)

for file in "${FILES_TO_BACKUP[@]}"; do
    if [ -f "$file" ]; then
        cp "$file" "${SNAPSHOT_DIR}/config_files/"
        echo "✅ $file 백업 완료"
    fi
done

# 소스 파일 목록 저장
if [ -d "src" ]; then
    find src -name "*.cpp" -o -name "*.h" -o -name "*.py" > "${SNAPSHOT_DIR}/source_files.txt" 2>/dev/null
fi

# 빌드 상태 확인
echo -e "${BLUE}🔨 빌드 상태 확인 중...${NC}"
if [ -d "build" ]; then
    echo "빌드 디렉토리 존재" > "${SNAPSHOT_DIR}/build_status.txt"
    if [ -f "build/Makefile" ]; then
        echo "Makefile 생성됨" >> "${SNAPSHOT_DIR}/build_status.txt"
    fi
    if [ -f "build/hgis" ]; then
        echo "실행 파일 생성됨" >> "${SNAPSHOT_DIR}/build_status.txt"
    fi
else
    echo "빌드 디렉토리 없음" > "${SNAPSHOT_DIR}/build_status.txt"
fi

# 메타데이터 생성
echo -e "${BLUE}📝 메타데이터 생성 중...${NC}"
cat > "${SNAPSHOT_DIR}/metadata.txt" << EOF
스냅샷 생성 시간: $(date '+%Y-%m-%d %H:%M:%S')
스냅샷 이름: ${SNAPSHOT_NAME}
현재 Phase: Phase 1 - 기초 구축
사용자: $(whoami)
호스트: $(hostname)
작업 디렉토리: $(pwd)
EOF

# 스냅샷 인덱스 업데이트
echo "${SNAPSHOT_NAME} - $(date '+%Y-%m-%d %H:%M:%S')" >> snapshots/index.txt

echo -e "${GREEN}✅ 스냅샷 생성 완료: ${SNAPSHOT_DIR}${NC}"

# 스냅샷 내용 요약
echo -e "${YELLOW}📋 스냅샷 내용:${NC}"
ls -la "${SNAPSHOT_DIR}"

echo -e "${BLUE}🔍 스냅샷 조회 방법:${NC}"
echo "  전체 스냅샷 목록: cat snapshots/index.txt"
echo "  스냅샷 복원: ./scripts/restore-snapshot.sh ${SNAPSHOT_NAME}"