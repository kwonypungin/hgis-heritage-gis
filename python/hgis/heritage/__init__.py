"""
HGIS Heritage Module

문화재 데이터 처리를 위한 모듈입니다.
문화재청 표준 데이터 모델과 검증 기능을 제공합니다.
"""

from .data_model import HeritageDataModel, HeritageFeature
from .validator import HeritageValidator
from .standards import CHAStandards

__all__ = [
    'HeritageDataModel',
    'HeritageFeature', 
    'HeritageValidator',
    'CHAStandards'
]

__version__ = '1.0.0'