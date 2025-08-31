"""
문화재 데이터 검증 시스템

문화재청 표준에 따른 데이터 검증 기능을 제공합니다.
"""

from typing import List, Dict, Any, Optional, Union
import re
from datetime import datetime
from .data_model import HeritageFeature, PeriodType, SiteType, PreservationState


class ValidationError:
    """검증 오류 정보"""
    
    def __init__(self, field: str, message: str, severity: str = "error"):
        self.field = field
        self.message = message  
        self.severity = severity  # error, warning, info
        self.timestamp = datetime.now()
    
    def __str__(self):
        return f"[{self.severity.upper()}] {self.field}: {self.message}"
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'field': self.field,
            'message': self.message,
            'severity': self.severity,
            'timestamp': self.timestamp.isoformat()
        }


class HeritageValidator:
    """문화재 데이터 검증기"""
    
    # 유적코드 패턴 (예: ABC-2023-001)
    SITE_CODE_PATTERN = re.compile(r'^[A-Z]{2,4}-\d{4}-\d{3}$')
    
    # 유구번호 패턴 (예: H1, P-01, T001)
    FEATURE_ID_PATTERN = re.compile(r'^[A-Z]{1,2}[-]?\d{1,3}$')
    
    # 좌표 범위 (한국 영역)
    COORD_X_MIN, COORD_X_MAX = 120000, 1000000  # UTM-K 기준
    COORD_Y_MIN, COORD_Y_MAX = 1000000, 2000000
    
    # 표고 범위 (미터)
    ELEVATION_MIN, ELEVATION_MAX = -100, 2000
    
    # 면적 범위 (제곱미터)
    AREA_MIN, AREA_MAX = 0.1, 1000000
    
    def __init__(self):
        self.validation_rules = {
            'site_name': self._validate_site_name,
            'site_code': self._validate_site_code,
            'feature_id': self._validate_feature_id,
            'period': self._validate_period,
            'site_type': self._validate_site_type,
            'coordinate_x': self._validate_coordinate_x,
            'coordinate_y': self._validate_coordinate_y,
            'elevation': self._validate_elevation,
            'area_m2': self._validate_area,
            'excavation_year': self._validate_excavation_year,
            'preservation': self._validate_preservation,
        }
    
    def validate_feature(self, feature: HeritageFeature) -> List[ValidationError]:
        """단일 피처 검증"""
        errors = []
        
        # 필수 필드 검증
        for field_name, validator in self.validation_rules.items():
            try:
                field_value = getattr(feature, field_name, None)
                field_errors = validator(field_value, feature)
                errors.extend(field_errors)
            except Exception as e:
                errors.append(ValidationError(
                    field_name, 
                    f"검증 중 오류 발생: {str(e)}",
                    "error"
                ))
        
        # 종합적 검증
        errors.extend(self._validate_comprehensive(feature))
        
        return errors
    
    def validate_features(self, features: List[HeritageFeature]) -> Dict[str, List[ValidationError]]:
        """다중 피처 검증"""
        results = {}
        
        for feature in features:
            feature_errors = self.validate_feature(feature)
            if feature_errors:
                results[feature.feature_id] = feature_errors
        
        # 중복 검사
        duplicate_errors = self._check_duplicates(features)
        if duplicate_errors:
            results['_duplicates'] = duplicate_errors
        
        return results
    
    def _validate_site_name(self, value: str, feature: HeritageFeature) -> List[ValidationError]:
        """유적명 검증"""
        errors = []
        
        if not value or not value.strip():
            errors.append(ValidationError('site_name', '유적명은 필수입니다'))
            return errors
        
        if len(value.strip()) < 2:
            errors.append(ValidationError('site_name', '유적명은 최소 2자 이상이어야 합니다'))
        
        if len(value) > 100:
            errors.append(ValidationError('site_name', '유적명은 100자를 초과할 수 없습니다'))
        
        # 특수문자 검사
        if re.search(r'[<>"\'\\/]', value):
            errors.append(ValidationError('site_name', '유적명에 허용되지 않는 특수문자가 포함되어 있습니다', 'warning'))
        
        return errors
    
    def _validate_site_code(self, value: str, feature: HeritageFeature) -> List[ValidationError]:
        """유적코드 검증"""
        errors = []
        
        if not value or not value.strip():
            errors.append(ValidationError('site_code', '유적코드는 필수입니다'))
            return errors
        
        if not self.SITE_CODE_PATTERN.match(value):
            errors.append(ValidationError(
                'site_code', 
                '유적코드 형식이 올바르지 않습니다 (예: ABC-2023-001)'
            ))
        
        # 연도 검증
        if '-' in value:
            parts = value.split('-')
            if len(parts) >= 2:
                try:
                    year = int(parts[1])
                    current_year = datetime.now().year
                    if year < 1900 or year > current_year + 1:
                        errors.append(ValidationError(
                            'site_code',
                            f'유적코드의 연도가 유효하지 않습니다: {year}',
                            'warning'
                        ))
                except ValueError:
                    pass  # 패턴 매칭에서 이미 검사됨
        
        return errors
    
    def _validate_feature_id(self, value: str, feature: HeritageFeature) -> List[ValidationError]:
        """유구번호 검증"""
        errors = []
        
        if not value or not value.strip():
            errors.append(ValidationError('feature_id', '유구번호는 필수입니다'))
            return errors
        
        if not self.FEATURE_ID_PATTERN.match(value):
            errors.append(ValidationError(
                'feature_id',
                '유구번호 형식이 올바르지 않습니다 (예: H1, P-01, T001)'
            ))
        
        return errors
    
    def _validate_period(self, value: Union[PeriodType, str], feature: HeritageFeature) -> List[ValidationError]:
        """시대 검증"""
        errors = []
        
        if not value:
            errors.append(ValidationError('period', '시대는 필수입니다'))
            return errors
        
        if isinstance(value, str):
            valid_periods = [p.value for p in PeriodType]
            if value not in valid_periods:
                errors.append(ValidationError(
                    'period',
                    f'유효하지 않은 시대입니다: {value}. 허용값: {", ".join(valid_periods)}'
                ))
        
        return errors
    
    def _validate_site_type(self, value: Union[SiteType, str], feature: HeritageFeature) -> List[ValidationError]:
        """유적유형 검증"""
        errors = []
        
        if not value:
            errors.append(ValidationError('site_type', '유적유형은 필수입니다'))
            return errors
        
        if isinstance(value, str):
            valid_types = [t.value for t in SiteType]
            if value not in valid_types:
                errors.append(ValidationError(
                    'site_type',
                    f'유효하지 않은 유적유형입니다: {value}. 허용값: {", ".join(valid_types)}'
                ))
        
        return errors
    
    def _validate_coordinate_x(self, value: float, feature: HeritageFeature) -> List[ValidationError]:
        """X좌표 검증"""
        errors = []
        
        if value is None:
            errors.append(ValidationError('coordinate_x', 'X좌표는 필수입니다'))
            return errors
        
        if not isinstance(value, (int, float)):
            errors.append(ValidationError('coordinate_x', 'X좌표는 숫자여야 합니다'))
            return errors
        
        if not (self.COORD_X_MIN <= value <= self.COORD_X_MAX):
            errors.append(ValidationError(
                'coordinate_x',
                f'X좌표가 유효 범위를 벗어났습니다 ({self.COORD_X_MIN} ~ {self.COORD_X_MAX})'
            ))
        
        return errors
    
    def _validate_coordinate_y(self, value: float, feature: HeritageFeature) -> List[ValidationError]:
        """Y좌표 검증"""
        errors = []
        
        if value is None:
            errors.append(ValidationError('coordinate_y', 'Y좌표는 필수입니다'))
            return errors
        
        if not isinstance(value, (int, float)):
            errors.append(ValidationError('coordinate_y', 'Y좌표는 숫자여야 합니다'))
            return errors
        
        if not (self.COORD_Y_MIN <= value <= self.COORD_Y_MAX):
            errors.append(ValidationError(
                'coordinate_y',
                f'Y좌표가 유효 범위를 벗어났습니다 ({self.COORD_Y_MIN} ~ {self.COORD_Y_MAX})'
            ))
        
        return errors
    
    def _validate_elevation(self, value: Optional[float], feature: HeritageFeature) -> List[ValidationError]:
        """표고 검증"""
        errors = []
        
        if value is None:
            return errors  # 표고는 선택사항
        
        if not isinstance(value, (int, float)):
            errors.append(ValidationError('elevation', '표고는 숫자여야 합니다'))
            return errors
        
        if not (self.ELEVATION_MIN <= value <= self.ELEVATION_MAX):
            errors.append(ValidationError(
                'elevation',
                f'표고가 유효 범위를 벗어났습니다 ({self.ELEVATION_MIN}m ~ {self.ELEVATION_MAX}m)',
                'warning'
            ))
        
        return errors
    
    def _validate_area(self, value: Optional[float], feature: HeritageFeature) -> List[ValidationError]:
        """면적 검증"""
        errors = []
        
        if value is None:
            return errors  # 면적은 선택사항
        
        if not isinstance(value, (int, float)):
            errors.append(ValidationError('area_m2', '면적은 숫자여야 합니다'))
            return errors
        
        if value <= 0:
            errors.append(ValidationError('area_m2', '면적은 0보다 커야 합니다'))
        
        if not (self.AREA_MIN <= value <= self.AREA_MAX):
            errors.append(ValidationError(
                'area_m2',
                f'면적이 유효 범위를 벗어났습니다 ({self.AREA_MIN}㎡ ~ {self.AREA_MAX}㎡)',
                'warning'
            ))
        
        return errors
    
    def _validate_excavation_year(self, value: Optional[int], feature: HeritageFeature) -> List[ValidationError]:
        """발굴연도 검증"""
        errors = []
        
        if value is None:
            return errors  # 발굴연도는 선택사항
        
        if not isinstance(value, int):
            errors.append(ValidationError('excavation_year', '발굴연도는 정수여야 합니다'))
            return errors
        
        current_year = datetime.now().year
        if not (1900 <= value <= current_year + 1):
            errors.append(ValidationError(
                'excavation_year',
                f'발굴연도가 유효하지 않습니다 (1900 ~ {current_year + 1})'
            ))
        
        return errors
    
    def _validate_preservation(self, value: Union[PreservationState, str], feature: HeritageFeature) -> List[ValidationError]:
        """보존상태 검증"""
        errors = []
        
        if value is None:
            return errors  # 보존상태는 선택사항 (기본값 있음)
        
        if isinstance(value, str):
            valid_states = [s.value for s in PreservationState]
            if value not in valid_states:
                errors.append(ValidationError(
                    'preservation',
                    f'유효하지 않은 보존상태입니다: {value}. 허용값: {", ".join(valid_states)}'
                ))
        
        return errors
    
    def _validate_comprehensive(self, feature: HeritageFeature) -> List[ValidationError]:
        """종합적 검증 (필드 간 상관관계 등)"""
        errors = []
        
        # 발굴연도와 유적코드 연도 일치성 검사
        if feature.excavation_year and feature.site_code and '-' in feature.site_code:
            try:
                parts = feature.site_code.split('-')
                if len(parts) >= 2:
                    code_year = int(parts[1])
                    if abs(feature.excavation_year - code_year) > 1:
                        errors.append(ValidationError(
                            'excavation_year',
                            f'발굴연도({feature.excavation_year})와 유적코드 연도({code_year})가 일치하지 않습니다',
                            'warning'
                        ))
            except (ValueError, IndexError):
                pass
        
        # 시대와 유적유형 상관관계 검사
        if hasattr(feature, 'period') and hasattr(feature, 'site_type'):
            period_str = feature.period.value if hasattr(feature.period, 'value') else str(feature.period)
            site_type_str = feature.site_type.value if hasattr(feature.site_type, 'value') else str(feature.site_type)
            
            # 예: 구석기시대에 사찰유적은 불가능
            if period_str == "구석기" and site_type_str == "사찰유적":
                errors.append(ValidationError(
                    'site_type',
                    '구석기시대에 사찰유적은 존재할 수 없습니다',
                    'warning'
                ))
        
        return errors
    
    def _check_duplicates(self, features: List[HeritageFeature]) -> List[ValidationError]:
        """중복 검사"""
        errors = []
        
        # 유구번호 중복 검사
        feature_ids = {}
        for feature in features:
            if feature.feature_id in feature_ids:
                errors.append(ValidationError(
                    'feature_id',
                    f'유구번호 중복: {feature.feature_id}'
                ))
            else:
                feature_ids[feature.feature_id] = feature
        
        # 좌표 중복 검사 (동일 좌표에 다른 유구)
        coordinates = {}
        for feature in features:
            coord_key = (feature.coordinate_x, feature.coordinate_y)
            if coord_key in coordinates:
                errors.append(ValidationError(
                    'coordinates',
                    f'동일 좌표에 여러 유구 존재: {coord_key}',
                    'warning'
                ))
            else:
                coordinates[coord_key] = feature
        
        return errors
    
    def get_validation_report(self, features: List[HeritageFeature]) -> Dict[str, Any]:
        """검증 보고서 생성"""
        validation_results = self.validate_features(features)
        
        total_errors = 0
        total_warnings = 0
        
        for feature_errors in validation_results.values():
            for error in feature_errors:
                if error.severity == 'error':
                    total_errors += 1
                elif error.severity == 'warning':
                    total_warnings += 1
        
        report = {
            'summary': {
                'total_features': len(features),
                'features_with_errors': len(validation_results),
                'total_errors': total_errors,
                'total_warnings': total_warnings,
                'validation_passed': total_errors == 0
            },
            'details': {
                feature_id: [error.to_dict() for error in errors]
                for feature_id, errors in validation_results.items()
            },
            'timestamp': datetime.now().isoformat()
        }
        
        return report