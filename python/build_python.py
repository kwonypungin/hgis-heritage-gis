#!/usr/bin/env python3
"""
HGIS Python 바인딩 빌드 스크립트
"""

import os
import sys
import shutil
import subprocess
from pathlib import Path

# 프로젝트 루트
PROJECT_ROOT = Path(__file__).parent.parent
PYTHON_DIR = PROJECT_ROOT / "python"
BUILD_DIR = PROJECT_ROOT / "build" / "python"
SRC_DIR = PROJECT_ROOT / "src"

# PyQt5 SIP 디렉토리 찾기
def find_pyqt5_sip():
    """PyQt5 SIP 디렉토리 찾기"""
    import PyQt5
    pyqt5_path = Path(PyQt5.__file__).parent
    
    # 가능한 SIP 디렉토리 위치들
    possible_paths = [
        pyqt5_path / "bindings",
        Path("/usr/share/sip/PyQt5"),
        Path("/usr/local/share/sip/PyQt5"),
        Path("/usr/share/pyqt5/sip"),
    ]
    
    for path in possible_paths:
        if path.exists():
            return path
    
    raise RuntimeError("PyQt5 SIP 디렉토리를 찾을 수 없습니다")

def generate_sip_code():
    """SIP 코드 생성"""
    print("SIP 코드 생성 중...")
    
    # 빌드 디렉토리 생성
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    os.chdir(BUILD_DIR)
    
    # SIP 대신 직접 C++ 확장 모듈 생성
    print("C++ Python 확장 모듈 생성...")
    
    # 간단한 Python 확장 모듈 생성
    generate_simple_binding()
    
def generate_simple_binding():
    """간단한 Python 바인딩 생성"""
    
    # C++ 래퍼 코드 생성
    wrapper_code = '''
#include <Python.h>
#include <QString>
#include "HGISApplication.h"
#include "HGISCoordinateReferenceSystem.h"
#include "HGISLayerManager.h"
#include "HGISVectorLayer.h"

// Python 모듈 함수들
static PyObject* hgis_version(PyObject* self, PyObject* args) {
    return PyUnicode_FromString("1.0.0");
}

static PyObject* hgis_create_crs(PyObject* self, PyObject* args) {
    int epsg_code;
    if (!PyArg_ParseTuple(args, "i", &epsg_code)) {
        return NULL;
    }
    
    HGISCoordinateReferenceSystem* crs = new HGISCoordinateReferenceSystem(epsg_code);
    return PyCapsule_New(crs, "HGISCoordinateReferenceSystem", NULL);
}

static PyObject* hgis_crs_epsg(PyObject* self, PyObject* args) {
    PyObject* capsule;
    if (!PyArg_ParseTuple(args, "O", &capsule)) {
        return NULL;
    }
    
    HGISCoordinateReferenceSystem* crs = 
        (HGISCoordinateReferenceSystem*)PyCapsule_GetPointer(capsule, "HGISCoordinateReferenceSystem");
    if (!crs) {
        return NULL;
    }
    
    return PyLong_FromLong(crs->epsgCode());
}

// 메서드 테이블
static PyMethodDef HgisMethods[] = {
    {"version", hgis_version, METH_NOARGS, "HGIS 버전 반환"},
    {"create_crs", hgis_create_crs, METH_VARARGS, "좌표계 생성"},
    {"crs_epsg", hgis_crs_epsg, METH_VARARGS, "좌표계 EPSG 코드 반환"},
    {NULL, NULL, 0, NULL}
};

// 모듈 정의
static struct PyModuleDef hgismodule = {
    PyModuleDef_HEAD_INIT,
    "hgis_core",
    "HGIS Core Python 바인딩",
    -1,
    HgisMethods
};

// 모듈 초기화
PyMODINIT_FUNC PyInit_hgis_core(void) {
    return PyModule_Create(&hgismodule);
}
'''
    
    wrapper_file = BUILD_DIR / "hgis_wrapper.cpp"
    wrapper_file.write_text(wrapper_code)
    
    # setup.py 생성
    setup_code = f'''
from setuptools import setup, Extension
import sys

# Include 디렉토리
include_dirs = [
    "{SRC_DIR}",
    "{SRC_DIR}/core",
    "{SRC_DIR}/gui",
    "{SRC_DIR}/providers",
    "{PROJECT_ROOT}/build/src/core",
    "/usr/include/x86_64-linux-gnu/qt5",
    "/usr/include/x86_64-linux-gnu/qt5/QtCore",
    "/usr/include/x86_64-linux-gnu/qt5/QtGui",
    "/usr/include/x86_64-linux-gnu/qt5/QtWidgets",
]

# 라이브러리 디렉토리
library_dirs = [
    "{PROJECT_ROOT}/build/lib",
    "/usr/lib/x86_64-linux-gnu",
]

# 확장 모듈 정의
hgis_core = Extension(
    "hgis_core",
    sources=["{wrapper_file}"],
    include_dirs=include_dirs,
    library_dirs=library_dirs,
    libraries=["hgis_core", "hgis_providers", "Qt5Core", "Qt5Gui", "Qt5Widgets"],
    extra_compile_args=["-std=c++17", "-fPIC"],
    language="c++"
)

setup(
    name="hgis",
    version="1.0.0",
    description="HGIS Python 바인딩",
    ext_modules=[hgis_core],
    packages=["hgis"],
    package_dir={{"hgis": "{PYTHON_DIR}/hgis"}},
)
'''
    
    setup_file = BUILD_DIR / "setup.py"
    setup_file.write_text(setup_code)
    
    print(f"Setup 파일 생성: {setup_file}")
    
def build_extension():
    """Python 확장 빌드"""
    print("Python 확장 빌드 중...")
    
    os.chdir(BUILD_DIR)
    
    # 빌드 실행
    cmd = [sys.executable, "setup.py", "build_ext", "--inplace"]
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    if result.returncode != 0:
        print("빌드 오류:")
        print(result.stderr)
        return False
    
    print("빌드 성공!")
    return True

def install_module():
    """모듈 설치"""
    print("모듈 설치 중...")
    
    # site-packages에 복사
    import site
    site_packages = Path(site.getusersitepackages())
    site_packages.mkdir(parents=True, exist_ok=True)
    
    hgis_dir = site_packages / "hgis"
    if hgis_dir.exists():
        shutil.rmtree(hgis_dir)
    
    shutil.copytree(PYTHON_DIR / "hgis", hgis_dir)
    
    # 빌드된 확장 모듈 복사
    for so_file in BUILD_DIR.glob("*.so"):
        shutil.copy(so_file, hgis_dir)
    
    print(f"설치 완료: {hgis_dir}")

def test_module():
    """모듈 테스트"""
    print("\n모듈 테스트...")
    
    test_code = """
import sys
sys.path.insert(0, '{build_dir}')

try:
    import hgis
    print(f"✓ hgis 모듈 로드 성공")
    print(f"  버전: {{hgis.version()}}")
    
    import hgis_core
    print(f"✓ hgis_core 모듈 로드 성공")
    print(f"  HGIS 버전: {{hgis_core.version()}}")
    
    # CRS 테스트
    crs = hgis_core.create_crs(5186)
    epsg = hgis_core.crs_epsg(crs)
    print(f"✓ CRS 생성 테스트: EPSG:{{epsg}}")
    
except Exception as e:
    print(f"✗ 오류: {{e}}")
    import traceback
    traceback.print_exc()
""".format(build_dir=BUILD_DIR)
    
    result = subprocess.run([sys.executable, "-c", test_code], 
                          capture_output=True, text=True)
    print(result.stdout)
    if result.stderr:
        print("오류:", result.stderr)

def main():
    print("====================================")
    print("HGIS Python 바인딩 빌드")
    print("====================================\n")
    
    # SIP 코드 생성
    generate_sip_code()
    
    # 확장 빌드
    if build_extension():
        # 테스트
        test_module()
        
        # 설치 옵션
        if input("\n모듈을 설치하시겠습니까? (y/n): ").lower() == 'y':
            install_module()
            print("\n사용법: import hgis")

if __name__ == "__main__":
    main()