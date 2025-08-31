"""
HGIS Python API
고급 공간정보 시스템 Python 바인딩
"""

__version__ = '1.0.0'
__author__ = 'HGIS Development Team'

# 버전 정보
VERSION = '1.0.0'
VERSION_INT = 10000
RELEASE_NAME = 'Seoul'

def version():
    """HGIS 버전 반환"""
    return __version__

def versionInt():
    """HGIS 버전 정수값 반환"""
    return VERSION_INT

def releaseName():
    """HGIS 릴리즈 이름 반환"""
    return RELEASE_NAME