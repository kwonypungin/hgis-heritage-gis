#!/bin/bash
# HGIS 개발 명령어 별칭 설정
# '개발시작' 명령어 하나로 모든 개발 환경 실행

set -e

# 색상 정의
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
RED='\033[0;31m'
BOLD='\033[1m'
NC='\033[0m'

echo ""
echo -e "${CYAN}${BOLD}HGIS 개발 명령어 설정${NC}"
echo ""

# HGIS 프로젝트 디렉토리
HGIS_DIR="$(pwd)"

# 별칭 추가 함수
add_alias() {
    local SHELL_RC=""
    
    # 사용 중인 쉘 확인
    if [ -n "$BASH_VERSION" ] || [ "$SHELL" = "/bin/bash" ]; then
        SHELL_RC="$HOME/.bashrc"
    elif [ -n "$ZSH_VERSION" ] || [ "$SHELL" = "/bin/zsh" ]; then
        SHELL_RC="$HOME/.zshrc"
    else
        SHELL_RC="$HOME/.profile"
    fi
    
    echo -e "${YELLOW}설정 파일: ${SHELL_RC}${NC}"
    
    # 기존 HGIS 별칭 제거
    if grep -q "# HGIS Development Command" "$SHELL_RC" 2>/dev/null; then
        echo -e "${YELLOW}기존 설정 업데이트 중...${NC}"
        # 기존 HGIS 설정 블록 제거
        sed -i '/# HGIS Development Command/,/# HGIS End/d' "$SHELL_RC"
    fi
    
    # 새 별칭 추가
    cat >> "$SHELL_RC" << EOF

# HGIS Development Command
alias 개발시작='cd $HGIS_DIR && ./개발시작.sh'
# HGIS End
EOF
    
    echo -e "${GREEN}✅ $SHELL_RC에 '개발시작' 명령어 추가 완료${NC}"
}

# 별칭 추가
add_alias

echo ""
echo -e "${GREEN}${BOLD}========================================${NC}"
echo -e "${GREEN}${BOLD}✅ '개발시작' 명령어 설정 완료!${NC}"
echo -e "${GREEN}${BOLD}========================================${NC}"
echo ""
echo -e "${CYAN}${BOLD}사용 방법:${NC}"
echo -e "${YELLOW}1. 터미널에서 다음 명령 실행:${NC}"
echo ""
if [ -n "$BASH_VERSION" ] || [ "$SHELL" = "/bin/bash" ]; then
    echo -e "   ${BOLD}source ~/.bashrc${NC}"
elif [ -n "$ZSH_VERSION" ] || [ "$SHELL" = "/bin/zsh" ]; then
    echo -e "   ${BOLD}source ~/.zshrc${NC}"
else
    echo -e "   ${BOLD}source ~/.profile${NC}"
fi
echo ""
echo -e "${YELLOW}2. 이제 어느 디렉토리에서나:${NC}"
echo -e "   ${BOLD}${CYAN}개발시작${NC}"
echo ""
echo -e "${GREEN}   입력하면 HGIS 개발 환경이 자동으로 실행됩니다!${NC}"
echo ""
echo -e "${CYAN}${BOLD}개발시작 명령어가 실행하는 작업:${NC}"
echo "  • 개발 환경 검증"
echo "  • Git 상태 확인"
echo "  • 프로젝트 구조 초기화"
echo "  • 컨텍스트 동기화"
echo "  • 스냅샷 생성"
echo "  • Phase별 체크리스트 표시"
echo ""

# 즉시 적용 옵션
read -p "지금 바로 적용하시겠습니까? (y/N): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    if [ -n "$BASH_VERSION" ] || [ "$SHELL" = "/bin/bash" ]; then
        source ~/.bashrc
        echo -e "${GREEN}✅ 적용 완료! 이제 '개발시작' 명령을 사용할 수 있습니다.${NC}"
    elif [ -n "$ZSH_VERSION" ] || [ "$SHELL" = "/bin/zsh" ]; then
        source ~/.zshrc
        echo -e "${GREEN}✅ 적용 완료! 이제 '개발시작' 명령을 사용할 수 있습니다.${NC}"
    else
        echo -e "${YELLOW}수동으로 source 명령을 실행해주세요.${NC}"
    fi
fi