"""
문화재청 표준 데이터 모델

문화재청의 유적 발굴조사 표준 데이터 모델을 구현합니다.
"""

from typing import Dict, Any, Optional, List
from dataclasses import dataclass, field
from enum import Enum
import datetime


class PeriodType(Enum):
    """시대 구분"""
    PALEOLITHIC = "구석기"
    NEOLITHIC = "신석기"  
    BRONZE_AGE = "청동기"
    IRON_AGE = "철기"
    PROTO_THREE_KINGDOMS = "원삼국"
    THREE_KINGDOMS = "삼국"
    UNIFIED_SILLA = "통일신라"
    GORYEO = "고려"
    JOSEON = "조선"
    MODERN = "근현대"
    UNKNOWN = "미상"


class SiteType(Enum):
    """유적 유형"""
    SETTLEMENT = "취락유적"
    TOMB = "분묘유적"
    FORTRESS = "성곽유적"
    TEMPLE = "사찰유적"
    KILN = "요업유적"
    PRODUCTION = "생산유적"
    RITUAL = "제사유적"
    OTHERS = "기타유적"


class PreservationState(Enum):
    """보존 상태"""
    EXCELLENT = "양호"
    GOOD = "보통"
    POOR = "불량"
    DESTROYED = "파괴"


@dataclass
class HeritageFeature:
    """문화재 피처 클래스"""
    
    # 기본 정보
    site_name: str  # 유적명
    site_code: str  # 유적코드
    feature_id: str  # 유구번호
    
    # 시공간 정보
    period: PeriodType  # 시대
    site_type: SiteType  # 유적 유형
    
    # 공간 정보
    coordinate_x: float  # X좌표 (미터)
    coordinate_y: float  # Y좌표 (미터)
    elevation: Optional[float] = None  # 표고 (미터)
    area_m2: Optional[float] = None  # 면적 (제곱미터)
    
    # 발굴 정보
    excavation_year: Optional[int] = None  # 발굴년도
    excavation_season: Optional[str] = None  # 발굴차수
    excavator: Optional[str] = None  # 발굴기관
    
    # 보존 상태
    preservation: PreservationState = PreservationState.GOOD
    
    # 추가 속성
    attributes: Dict[str, Any] = field(default_factory=dict)
    
    # 기하정보 (GeoJSON 형태)
    geometry: Optional[Dict[str, Any]] = None
    
    def __post_init__(self):
        """데이터 유효성 검사"""
        if not self.site_name.strip():
            raise ValueError("유적명은 필수입니다")
        if not self.site_code.strip():
            raise ValueError("유적코드는 필수입니다")
        if not self.feature_id.strip():
            raise ValueError("유구번호는 필수입니다")
            
    def to_dict(self) -> Dict[str, Any]:
        """딕셔너리로 변환"""
        return {
            'site_name': self.site_name,
            'site_code': self.site_code,
            'feature_id': self.feature_id,
            'period': self.period.value if isinstance(self.period, PeriodType) else self.period,
            'site_type': self.site_type.value if isinstance(self.site_type, SiteType) else self.site_type,
            'coordinate_x': self.coordinate_x,
            'coordinate_y': self.coordinate_y,
            'elevation': self.elevation,
            'area_m2': self.area_m2,
            'excavation_year': self.excavation_year,
            'excavation_season': self.excavation_season,
            'excavator': self.excavator,
            'preservation': self.preservation.value if isinstance(self.preservation, PreservationState) else self.preservation,
            'attributes': self.attributes,
            'geometry': self.geometry
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'HeritageFeature':
        """딕셔너리에서 생성"""
        # Enum 변환
        if 'period' in data and isinstance(data['period'], str):
            for period in PeriodType:
                if period.value == data['period']:
                    data['period'] = period
                    break
        
        if 'site_type' in data and isinstance(data['site_type'], str):
            for site_type in SiteType:
                if site_type.value == data['site_type']:
                    data['site_type'] = site_type
                    break
                    
        if 'preservation' in data and isinstance(data['preservation'], str):
            for preservation in PreservationState:
                if preservation.value == data['preservation']:
                    data['preservation'] = preservation
                    break
        
        return cls(**data)


class HeritageDataModel:
    """문화재 데이터 모델 관리 클래스"""
    
    # 문화재청 표준 필드 정의
    REQUIRED_FIELDS = {
        'site_name': str,      # 유적명
        'site_code': str,      # 유적코드  
        'feature_id': str,     # 유구번호
        'period': str,         # 시대
        'site_type': str,      # 유적유형
        'coordinate_x': float, # X좌표
        'coordinate_y': float, # Y좌표
    }
    
    OPTIONAL_FIELDS = {
        'elevation': float,         # 표고
        'area_m2': float,          # 면적
        'excavation_year': int,    # 발굴년도
        'excavation_season': str,  # 발굴차수
        'excavator': str,          # 발굴기관
        'preservation': str,       # 보존상태
        'description': str,        # 설명
        'material': str,          # 재질
        'dating_method': str,     # 연대측정법
        'cultural_layer': str,    # 문화층
        'associated_artifacts': str  # 관련유물
    }
    
    def __init__(self):
        self.features: List[HeritageFeature] = []
        self.metadata = {
            'created_date': datetime.datetime.now(),
            'version': '1.0.0',
            'coordinate_system': 'EPSG:5179',  # Korea 2000 / Central Belt
            'creator': 'HGIS Heritage Module'
        }
    
    def add_feature(self, feature: HeritageFeature) -> None:
        """피처 추가"""
        if not isinstance(feature, HeritageFeature):
            raise TypeError("HeritageFeature 인스턴스여야 합니다")
        self.features.append(feature)
    
    def remove_feature(self, feature_id: str) -> bool:
        """피처 제거"""
        for i, feature in enumerate(self.features):
            if feature.feature_id == feature_id:
                del self.features[i]
                return True
        return False
    
    def find_feature(self, feature_id: str) -> Optional[HeritageFeature]:
        """피처 검색"""
        for feature in self.features:
            if feature.feature_id == feature_id:
                return feature
        return None
    
    def filter_by_period(self, period: PeriodType) -> List[HeritageFeature]:
        """시대별 필터링"""
        return [f for f in self.features if f.period == period]
    
    def filter_by_site_type(self, site_type: SiteType) -> List[HeritageFeature]:
        """유적유형별 필터링"""
        return [f for f in self.features if f.site_type == site_type]
    
    def get_statistics(self) -> Dict[str, Any]:
        """통계 정보 반환"""
        if not self.features:
            return {}
            
        periods = {}
        site_types = {}
        
        for feature in self.features:
            # 시대별 통계
            period_name = feature.period.value if isinstance(feature.period, PeriodType) else str(feature.period)
            periods[period_name] = periods.get(period_name, 0) + 1
            
            # 유적유형별 통계
            site_type_name = feature.site_type.value if isinstance(feature.site_type, SiteType) else str(feature.site_type)
            site_types[site_type_name] = site_types.get(site_type_name, 0) + 1
        
        return {
            'total_features': len(self.features),
            'periods': periods,
            'site_types': site_types,
            'metadata': self.metadata
        }
    
    def validate_all(self) -> List[Dict[str, str]]:
        """모든 피처 유효성 검사"""
        from .validator import HeritageValidator
        validator = HeritageValidator()
        errors = []
        
        for feature in self.features:
            feature_errors = validator.validate_feature(feature)
            if feature_errors:
                errors.extend(feature_errors)
        
        return errors
    
    def export_to_dict(self) -> Dict[str, Any]:
        """전체 데이터를 딕셔너리로 내보내기"""
        return {
            'metadata': self.metadata,
            'features': [f.to_dict() for f in self.features],
            'statistics': self.get_statistics()
        }