"""
SIP 빌드 설정 파일
"""

import os
import sys
from pathlib import Path

# 프로젝트 경로
PROJECT_ROOT = Path(__file__).parent.parent
SRC_DIR = PROJECT_ROOT / "src"
BUILD_DIR = PROJECT_ROOT / "build"
PYTHON_DIR = PROJECT_ROOT / "python"

# Qt5 설정
QT5_DIR = "/usr/include/x86_64-linux-gnu/qt5"

# 컴파일러 플래그
CXX_FLAGS = [
    "-std=c++17",
    "-fPIC",
    "-Wall",
    "-O2"
]

# Include 경로
INCLUDE_DIRS = [
    str(SRC_DIR),
    str(SRC_DIR / "core"),
    str(SRC_DIR / "gui"),
    str(SRC_DIR / "providers"),
    str(BUILD_DIR / "src" / "core"),  # MOC 생성 파일용
    QT5_DIR,
    f"{QT5_DIR}/QtCore",
    f"{QT5_DIR}/QtGui",
    f"{QT5_DIR}/QtWidgets",
    "/usr/include/gdal",
]

# 라이브러리 경로
LIBRARY_DIRS = [
    str(BUILD_DIR / "lib"),
    "/usr/lib/x86_64-linux-gnu",
]

# 링크할 라이브러리
LIBRARIES = [
    "hgis_core",
    "hgis_gui",
    "hgis_providers",
    "Qt5Core",
    "Qt5Gui",
    "Qt5Widgets",
    "gdal",
    "proj",
]

# SIP 모듈 설정
SIP_MODULE = "hgis"
SIP_MODULE_VERSION = "1.0.0"

def get_sip_flags():
    """SIP 플래그 반환"""
    flags = []
    if sys.platform == "win32":
        flags.append("-t WS_WIN")
    elif sys.platform == "darwin":
        flags.append("-t WS_MACX")
    else:
        flags.append("-t WS_X11")
    
    flags.extend(["-t", "Qt_5_15_0"])
    return flags

def get_compile_command(source_file, output_file):
    """컴파일 명령 생성"""
    includes = " ".join([f"-I{d}" for d in INCLUDE_DIRS])
    flags = " ".join(CXX_FLAGS)
    return f"g++ {flags} {includes} -c {source_file} -o {output_file}"

def get_link_command(object_files, output_file):
    """링크 명령 생성"""
    lib_dirs = " ".join([f"-L{d}" for d in LIBRARY_DIRS])
    libs = " ".join([f"-l{lib}" for lib in LIBRARIES])
    objects = " ".join(object_files)
    return f"g++ -shared {objects} {lib_dirs} {libs} -o {output_file}"