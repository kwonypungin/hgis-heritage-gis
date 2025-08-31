"""
문화재청 표준 정의

문화재청 발굴조사 표준과 관련 코드 정의를 제공합니다.
"""

from typing import Dict, List, Any
from enum import Enum


class CHAStandards:
    """문화재청 표준 정의 클래스"""
    
    # 문화재청 표준 좌표계
    STANDARD_CRS = {
        'EPSG:5179': 'Korea 2000 / Central Belt',
        'EPSG:5174': 'Korea 1985 / Central Belt', 
        'EPSG:4326': 'WGS84 Geographic',
        'EPSG:5186': 'Korea 2000 / Central Belt 2010'
    }
    
    # 발굴조사 표준 양식 필드
    EXCAVATION_FIELDS = {
        # 기본 정보
        'site_name': {'type': str, 'required': True, 'max_length': 100},
        'site_code': {'type': str, 'required': True, 'pattern': r'^[A-Z]{2,4}-\d{4}-\d{3}$'},
        'feature_id': {'type': str, 'required': True, 'pattern': r'^[A-Z]{1,2}[-]?\d{1,3}$'},
        
        # 분류 정보
        'period': {'type': str, 'required': True, 'enum': [
            '구석기', '신석기', '청동기', '철기', '원삼국', 
            '삼국', '통일신라', '고려', '조선', '근현대', '미상'
        ]},
        'site_type': {'type': str, 'required': True, 'enum': [
            '취락유적', '분묘유적', '성곽유적', '사찰유적', 
            '요업유적', '생산유적', '제사유적', '기타유적'
        ]},
        
        # 공간 정보
        'coordinate_x': {'type': float, 'required': True, 'min': 120000, 'max': 1000000},
        'coordinate_y': {'type': float, 'required': True, 'min': 1000000, 'max': 2000000},
        'elevation': {'type': float, 'required': False, 'min': -100, 'max': 2000},
        'area_m2': {'type': float, 'required': False, 'min': 0.1, 'max': 1000000},
        
        # 발굴 정보
        'excavation_year': {'type': int, 'required': False, 'min': 1900, 'max': 2030},
        'excavation_season': {'type': str, 'required': False, 'max_length': 20},
        'excavator': {'type': str, 'required': False, 'max_length': 100},
        
        # 상태 정보
        'preservation': {'type': str, 'required': False, 'enum': [
            '양호', '보통', '불량', '파괴'
        ]}
    }
    
    # 시대별 상세 분류
    PERIOD_DETAILS = {
        '구석기': {
            'start_year': -500000,
            'end_year': -8000,
            'subdivisions': ['전기', '중기', '후기'],
            'typical_artifacts': ['석기', '골각기']
        },
        '신석기': {
            'start_year': -8000,
            'end_year': -1500,
            'subdivisions': ['전기', '중기', '후기'],
            'typical_artifacts': ['토기', '석기', '골각기']
        },
        '청동기': {
            'start_year': -1500,
            'end_year': -400,
            'subdivisions': ['전기', '중기', '후기'],
            'typical_artifacts': ['청동기', '토기', '석기']
        },
        '철기': {
            'start_year': -400,
            'end_year': 300,
            'subdivisions': ['전기', '중기', '후기'],
            'typical_artifacts': ['철기', '토기']
        },
        '삼국': {
            'start_year': 300,
            'end_year': 668,
            'subdivisions': ['고구려', '백제', '신라', '가야'],
            'typical_artifacts': ['토기', '철기', '금속공예품']
        },
        '통일신라': {
            'start_year': 668,
            'end_year': 935,
            'subdivisions': ['전기', '중기', '후기'],
            'typical_artifacts': ['토기', '기와', '금속공예품']
        },
        '고려': {
            'start_year': 918,
            'end_year': 1392,
            'subdivisions': ['전기', '중기', '후기'],
            'typical_artifacts': ['청자', '백자', '기와']
        },
        '조선': {
            'start_year': 1392,
            'end_year': 1897,
            'subdivisions': ['전기', '중기', '후기'],
            'typical_artifacts': ['백자', '분청사기', '기와']
        }
    }
    
    # 유적 유형별 세부사항
    SITE_TYPE_DETAILS = {
        '취락유적': {
            'description': '사람들이 모여 살았던 생활 터전',
            'typical_features': ['주거지', '저장시설', '작업장', '도로'],
            'required_survey': ['지표조사', '시굴조사', '발굴조사']
        },
        '분묘유적': {
            'description': '죽은 사람을 매장한 무덤',
            'typical_features': ['매장주체부', '부장품', '봉분', '주구'],
            'required_survey': ['지표조사', '발굴조사']
        },
        '성곽유적': {
            'description': '방어를 위해 쌓은 성벽과 관련 시설',
            'typical_features': ['성벽', '문지', '체성', '해자'],
            'required_survey': ['측량조사', '발굴조사']
        },
        '사찰유적': {
            'description': '불교 사찰의 건물터와 관련 유구',
            'typical_features': ['금당지', '탑지', '강당지', '승방'],
            'required_survey': ['지표조사', '발굴조사']
        }
    }
    
    # 표준 보고서 양식
    REPORT_TEMPLATE = {
        'cover': {
            'title': '문화재 발굴조사 보고서',
            'site_name': '',
            'location': '',
            'survey_institution': '',
            'survey_period': '',
            'publication_date': ''
        },
        'summary': {
            'site_overview': '',
            'survey_results': '',
            'major_findings': '',
            'historical_significance': ''
        },
        'methodology': {
            'survey_method': '',
            'grid_system': '',
            'recording_method': '',
            'dating_method': ''
        },
        'results': {
            'stratigraphy': '',
            'features': [],
            'artifacts': [],
            'dating_results': ''
        },
        'conclusion': {
            'interpretation': '',
            'recommendations': '',
            'conservation_plan': ''
        }
    }
    
    # 디지털 자료 표준
    DIGITAL_STANDARDS = {
        'file_formats': {
            'vector': ['shp', 'kml', 'geojson'],
            'raster': ['tif', 'jpg', 'png'],
            'cad': ['dwg', 'dxf'],
            'document': ['pdf', 'hwp', 'docx']
        },
        'naming_convention': {
            'pattern': '{site_code}_{feature_type}_{number}',
            'examples': [
                'ABC-2023-001_PIT_001.shp',
                'DEF-2023-002_HOUSE_001.jpg'
            ]
        },
        'metadata_required': [
            'title', 'description', 'keywords',
            'spatial_coverage', 'temporal_coverage',
            'creator', 'date_created', 'format',
            'coordinate_system', 'accuracy'
        ]
    }
    
    @classmethod
    def get_period_info(cls, period: str) -> Dict[str, Any]:
        """시대 정보 조회"""
        return cls.PERIOD_DETAILS.get(period, {})
    
    @classmethod
    def get_site_type_info(cls, site_type: str) -> Dict[str, Any]:
        """유적유형 정보 조회"""
        return cls.SITE_TYPE_DETAILS.get(site_type, {})
    
    @classmethod
    def get_field_spec(cls, field_name: str) -> Dict[str, Any]:
        """필드 규격 조회"""
        return cls.EXCAVATION_FIELDS.get(field_name, {})
    
    @classmethod
    def validate_crs(cls, crs_code: str) -> bool:
        """좌표계 유효성 검사"""
        return crs_code in cls.STANDARD_CRS
    
    @classmethod
    def get_crs_name(cls, crs_code: str) -> str:
        """좌표계 이름 조회"""
        return cls.STANDARD_CRS.get(crs_code, f'Unknown CRS: {crs_code}')
    
    @classmethod
    def generate_feature_id(cls, site_type: str, sequence: int) -> str:
        """유구번호 생성"""
        type_prefixes = {
            '취락유적': 'H',  # House
            '분묘유적': 'T',  # Tomb
            '성곽유적': 'F',  # Fortress
            '사찰유적': 'B',  # Buddhist temple
            '요업유적': 'K',  # Kiln
            '생산유적': 'P',  # Production
            '제사유적': 'R',  # Ritual
            '기타유적': 'M'   # Miscellaneous
        }
        
        prefix = type_prefixes.get(site_type, 'M')
        return f"{prefix}{sequence:03d}"
    
    @classmethod
    def get_report_template(cls) -> Dict[str, Any]:
        """보고서 템플릿 조회"""
        return cls.REPORT_TEMPLATE.copy()
    
    @classmethod
    def validate_file_format(cls, file_path: str, category: str) -> bool:
        """파일 형식 유효성 검사"""
        if category not in cls.DIGITAL_STANDARDS['file_formats']:
            return False
        
        allowed_formats = cls.DIGITAL_STANDARDS['file_formats'][category]
        file_extension = file_path.lower().split('.')[-1]
        return file_extension in allowed_formats
    
    @classmethod
    def generate_filename(cls, site_code: str, feature_type: str, number: int, extension: str) -> str:
        """표준 파일명 생성"""
        pattern = cls.DIGITAL_STANDARDS['naming_convention']['pattern']
        return f"{pattern.format(site_code=site_code, feature_type=feature_type, number=f'{number:03d}')}.{extension}"


class QualityControl:
    """품질 관리 기준"""
    
    # 측량 정확도 기준
    SURVEY_ACCURACY = {
        'high_precision': 0.01,      # 1cm
        'standard': 0.1,             # 10cm  
        'reconnaissance': 1.0        # 1m
    }
    
    # 필수 문서화 항목
    DOCUMENTATION_REQUIRED = [
        '조사계획서',
        '일일조사일지', 
        '유구카드',
        '유물카드',
        '도면',
        '사진',
        '최종보고서'
    ]
    
    # 사진 촬영 기준
    PHOTO_STANDARDS = {
        'resolution': '최소 300dpi',
        'format': ['jpg', 'tiff', 'raw'],
        'shots_required': {
            '유구 전경': '필수',
            '유구 세부': '필수', 
            '토층 단면': '토층 있는 경우 필수',
            '유물 출토상태': '유물 출토시 필수',
            '작업 전경': '권장'
        }
    }
    
    @classmethod
    def check_documentation_completeness(cls, documents: List[str]) -> Dict[str, bool]:
        """문서화 완성도 검사"""
        results = {}
        for required_doc in cls.DOCUMENTATION_REQUIRED:
            results[required_doc] = required_doc in documents
        return results
    
    @classmethod
    def get_accuracy_level(cls, accuracy: float) -> str:
        """정확도 수준 판정"""
        if accuracy <= cls.SURVEY_ACCURACY['high_precision']:
            return 'high_precision'
        elif accuracy <= cls.SURVEY_ACCURACY['standard']:
            return 'standard'
        else:
            return 'reconnaissance'