#!/bin/bash
# HGIS 프로젝트 스냅샷 복원 스크립트

set -e

# 색상 정의
BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${BLUE}=== HGIS 컨텍스트 스냅샷 복원 ===${NC}"

# 스냅샷 이름 확인
if [ -z "$1" ]; then
    echo -e "${RED}❌ 사용법: $0 <snapshot_name>${NC}"
    echo ""
    echo -e "${YELLOW}사용 가능한 스냅샷:${NC}"
    if [ -f "snapshots/index.txt" ]; then
        cat snapshots/index.txt
    else
        echo "  스냅샷 없음"
    fi
    exit 1
fi

SNAPSHOT_NAME="$1"
SNAPSHOT_DIR="snapshots/${SNAPSHOT_NAME}"

# 스냅샷 존재 확인
if [ ! -d "${SNAPSHOT_DIR}" ]; then
    echo -e "${RED}❌ 스냅샷을 찾을 수 없습니다: ${SNAPSHOT_NAME}${NC}"
    echo ""
    echo -e "${YELLOW}사용 가능한 스냅샷:${NC}"
    if [ -f "snapshots/index.txt" ]; then
        cat snapshots/index.txt
    fi
    exit 1
fi

echo -e "${YELLOW}복원할 스냅샷: ${SNAPSHOT_NAME}${NC}"

# 메타데이터 표시
if [ -f "${SNAPSHOT_DIR}/metadata.txt" ]; then
    echo ""
    echo -e "${BLUE}📋 스냅샷 정보:${NC}"
    cat "${SNAPSHOT_DIR}/metadata.txt"
    echo ""
fi

# 복원 확인
read -p "정말로 이 스냅샷으로 복원하시겠습니까? 현재 상태가 덮어씌워집니다. (y/N): " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo -e "${YELLOW}복원 취소됨${NC}"
    exit 0
fi

# 현재 상태 백업
echo -e "${BLUE}현재 상태를 백업 중...${NC}"
./scripts/create-snapshot.sh "before_restore" 2>/dev/null || true

# 설정 파일 복원
echo -e "${BLUE}⚙️ 설정 파일 복원 중...${NC}"
if [ -d "${SNAPSHOT_DIR}/config_files" ]; then
    for file in "${SNAPSHOT_DIR}/config_files"/*; do
        if [ -f "$file" ]; then
            filename=$(basename "$file")
            cp "$file" "./${filename}"
            echo "  ✅ ${filename} 복원"
        fi
    done
fi

# Git 상태 정보 표시
if [ -f "${SNAPSHOT_DIR}/git_status.txt" ]; then
    echo ""
    echo -e "${BLUE}📊 스냅샷 시점의 Git 상태:${NC}"
    head -n 10 "${SNAPSHOT_DIR}/git_status.txt"
fi

# 프로젝트 구조 확인
if [ -f "${SNAPSHOT_DIR}/project_structure.txt" ]; then
    echo ""
    echo -e "${BLUE}📁 스냅샷 시점의 프로젝트 구조:${NC}"
    head -n 20 "${SNAPSHOT_DIR}/project_structure.txt"
fi

echo ""
echo -e "${GREEN}✅ 스냅샷 복원 완료!${NC}"
echo ""
echo -e "${YELLOW}📋 다음 단계:${NC}"
echo "1. 복원된 파일 확인: git status"
echo "2. 필요시 추가 복원: 수동으로 ${SNAPSHOT_DIR} 내용 확인"
echo "3. 개발 계속: ./개발시작.sh"