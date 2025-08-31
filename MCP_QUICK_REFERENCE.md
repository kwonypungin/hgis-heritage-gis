# GitHub MCP 도구 빠른 참조

> 💡 GitHub 작업시 반드시 이 MCP 도구들을 사용하세요!

## 🔍 검색 & 조회

### 리포지토리 검색
```javascript
mcp__smithery-ai-github__search_repositories
// 예: query: "language:python qgis"
```

### 코드 검색  
```javascript
mcp__smithery-ai-github__search_code
// 예: q: "QgsApplication in:file language:cpp"
```

### 리포 정보 조회
```javascript
mcp__smithery-ai-github__get_repository
// 예: owner: "qgis", repo: "QGIS"
```

### 파일 내용 조회
```javascript
mcp__smithery-ai-github__get_file_contents
// 예: owner: "qgis", repo: "QGIS", path: "src/core/qgsapplication.cpp"
```

## 📝 이슈 관리

### 이슈 목록
```javascript
mcp__smithery-ai-github__list_issues
// 예: owner: "owner", repo: "repo", state: "open"
```

### 이슈 생성
```javascript
mcp__smithery-ai-github__create_issue
// 필수: owner, repo, title
// 선택: body, labels, assignees
```

### 이슈 댓글
```javascript
mcp__smithery-ai-github__add_issue_comment
// 필수: owner, repo, issue_number, body
```

## 🔀 PR 관리

### PR 목록
```javascript
mcp__smithery-ai-github__list_pull_requests
// 예: owner, repo, state: "open"
```

### PR 생성
```javascript
mcp__smithery-ai-github__create_pull_request
// 필수: owner, repo, title, head, base
// 선택: body, draft
```

### PR 상세 조회
```javascript
mcp__smithery-ai-github__get_pull_request
// 필수: owner, repo, pullNumber
```

## 📁 파일 작업

### 단일 파일 생성/수정
```javascript
mcp__smithery-ai-github__create_or_update_file
// 필수: owner, repo, path, content, message, branch
// 업데이트시: sha (40자 전체 SHA 필요)
```

### 여러 파일 한번에 푸시
```javascript
mcp__smithery-ai-github__push_files
// 필수: owner, repo, branch, files[], message
```

## ⚡ HGIS 개발에 자주 사용하는 패턴

### 1. QGIS 소스 분석 (독립 프로그램 참조용)
```javascript
// main.cpp 분석
mcp__smithery-ai-github__get_file_contents({
    owner: "qgis",
    repo: "QGIS",
    path: "src/app/main.cpp",
    mode: "full"
})

// QgsApplication 구조 분석
mcp__smithery-ai-github__search_code({
    q: "QgsApplication repo:qgis/QGIS path:src/core"
})

// 맵 캔버스 구현 참조
mcp__smithery-ai-github__get_file_contents({
    owner: "qgis",
    repo: "QGIS",
    path: "src/gui/qgsmapcanvas.cpp",
    mode: "overview"
})

// Python 바인딩 방식 분석
mcp__smithery-ai-github__search_code({
    q: "repo:qgis/QGIS path:src/python extension:cpp"
})
```

### 2. HGIS 리포지토리 관리
```javascript
// 이슈 생성
mcp__smithery-ai-github__create_issue({
    owner: "hgis-team",
    repo: "hgis",
    title: "Phase 1: 기초 구축",
    body: "독립 실행형 프로그램 기초 구현"
})
// 2단계: 댓글 추가  
mcp__smithery-ai-github__add_issue_comment({...})
```

### 3. PR 생성 전 파일 업데이트
```javascript
// 1단계: 파일 수정
mcp__smithery-ai-github__create_or_update_file({...})
// 2단계: PR 생성
mcp__smithery-ai-github__create_pull_request({...})
```

## 🚨 주의사항

1. **일반 git 명령어 금지** - GitHub 작업시 MCP만 사용
2. **owner/repo 필수** - 모든 도구에 필수 파라미터
3. **SHA 주의** - 파일 업데이트시 정확한 40자 SHA 필요
4. **브랜치명 정확히** - head/base 브랜치명 오타 주의

---
*🔔 GitHub 키워드 감지시 자동으로 이 참조를 확인하세요!*