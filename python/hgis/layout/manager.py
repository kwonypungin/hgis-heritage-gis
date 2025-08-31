"""
HGIS Layout Manager

QGIS QgsLayoutManager 클래스를 참조하여 구현한 레이아웃 관리 시스템입니다.
문화재 도면 생성과 출력을 위한 레이아웃을 관리합니다.
"""

from typing import Dict, List, Optional, Any, Union
import uuid
from datetime import datetime
from dataclasses import dataclass, field
import json
import os

from .template import LayoutTemplate, A4Template, A3Template
from .composer import MapComposer, LegendComposer, TextComposer
from .exporter import PDFExporter, PNGExporter


@dataclass
class LayoutSettings:
    """레이아웃 기본 설정"""
    
    # 단위 설정 (QGIS Qgis::LayoutUnit 참조)
    units: str = "mm"  # mm, cm, inch, px
    
    # 해상도 설정
    dpi: int = 300
    
    # 페이지 설정
    page_width: float = 297.0  # A4 width in mm
    page_height: float = 210.0  # A4 height in mm
    
    # 여백 설정
    margin_left: float = 10.0
    margin_right: float = 10.0  
    margin_top: float = 10.0
    margin_bottom: float = 10.0
    
    # 그리드 설정 (QGIS QgsLayoutGridSettings 참조)
    grid_enabled: bool = True
    grid_spacing: float = 10.0
    grid_offset_x: float = 0.0
    grid_offset_y: float = 0.0
    
    # 스내핑 설정 (QGIS QgsLayoutSnapper 참조)
    snap_to_grid: bool = True
    snap_to_guides: bool = True
    snap_tolerance: float = 5.0
    
    def to_dict(self) -> Dict[str, Any]:
        """딕셔너리로 변환"""
        return {
            'units': self.units,
            'dpi': self.dpi,
            'page_width': self.page_width,
            'page_height': self.page_height,
            'margin_left': self.margin_left,
            'margin_right': self.margin_right,
            'margin_top': self.margin_top,
            'margin_bottom': self.margin_bottom,
            'grid_enabled': self.grid_enabled,
            'grid_spacing': self.grid_spacing,
            'grid_offset_x': self.grid_offset_x,
            'grid_offset_y': self.grid_offset_y,
            'snap_to_grid': self.snap_to_grid,
            'snap_to_guides': self.snap_to_guides,
            'snap_tolerance': self.snap_tolerance
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'LayoutSettings':
        """딕셔너리에서 생성"""
        return cls(**data)


@dataclass  
class LayoutItem:
    """레이아웃 아이템 (QGIS QgsLayoutItem 참조)"""
    
    uuid: str = field(default_factory=lambda: str(uuid.uuid4()))
    item_type: str = ""  # map, label, legend, scalebar, etc
    position_x: float = 0.0
    position_y: float = 0.0
    width: float = 100.0
    height: float = 50.0
    rotation: float = 0.0
    visible: bool = True
    locked: bool = False
    z_value: int = 1
    
    # 아이템별 속성
    properties: Dict[str, Any] = field(default_factory=dict)
    
    def __post_init__(self):
        if not self.uuid:
            self.uuid = str(uuid.uuid4())
    
    def to_dict(self) -> Dict[str, Any]:
        """딕셔너리로 변환"""
        return {
            'uuid': self.uuid,
            'item_type': self.item_type,
            'position_x': self.position_x,
            'position_y': self.position_y,
            'width': self.width,
            'height': self.height,
            'rotation': self.rotation,
            'visible': self.visible,
            'locked': self.locked,
            'z_value': self.z_value,
            'properties': self.properties
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'LayoutItem':
        """딕셔너리에서 생성"""
        return cls(**data)


class HGISLayout:
    """HGIS 레이아웃 클래스 (QGIS QgsLayout 참조)"""
    
    def __init__(self, name: str = "", description: str = ""):
        self.uuid = str(uuid.uuid4())
        self.name = name or f"Layout_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
        self.description = description
        
        # 설정
        self.settings = LayoutSettings()
        
        # 아이템들 (QGIS의 QGraphicsScene 아이템들과 유사)
        self.items: Dict[str, LayoutItem] = {}
        
        # 페이지 정보
        self.pages: List[Dict[str, Any]] = []
        self._add_default_page()
        
        # 메타데이터
        self.created_date = datetime.now()
        self.modified_date = datetime.now()
        self.author = "HGIS"
        
        # 사용자 정의 속성 (QGIS QgsObjectCustomProperties 참조)
        self.custom_properties: Dict[str, Any] = {}
        
        # 변수 (QGIS Expression Context Variables)
        self.variables: Dict[str, Any] = {}
        
    def _add_default_page(self):
        """기본 페이지 추가"""
        page = {
            'uuid': str(uuid.uuid4()),
            'width': self.settings.page_width,
            'height': self.settings.page_height,
            'orientation': 'landscape' if self.settings.page_width > self.settings.page_height else 'portrait'
        }
        self.pages.append(page)
    
    def add_item(self, item: LayoutItem) -> None:
        """아이템 추가 (QGIS addLayoutItem 참조)"""
        if not item.uuid:
            item.uuid = str(uuid.uuid4())
        
        self.items[item.uuid] = item
        self.modified_date = datetime.now()
    
    def remove_item(self, item_uuid: str) -> bool:
        """아이템 제거 (QGIS removeLayoutItem 참조)"""
        if item_uuid in self.items:
            del self.items[item_uuid]
            self.modified_date = datetime.now()
            return True
        return False
    
    def get_item_by_uuid(self, item_uuid: str) -> Optional[LayoutItem]:
        """UUID로 아이템 조회 (QGIS itemByUuid 참조)"""
        return self.items.get(item_uuid)
    
    def get_items_by_type(self, item_type: str) -> List[LayoutItem]:
        """타입별 아이템 조회 (QGIS layoutItems 템플릿 참조)"""
        return [item for item in self.items.values() if item.item_type == item_type]
    
    def get_selected_items(self) -> List[LayoutItem]:
        """선택된 아이템 조회 (QGIS selectedLayoutItems 참조)"""
        return [item for item in self.items.values() if item.properties.get('selected', False)]
    
    def clear_selection(self) -> None:
        """선택 해제 (QGIS deselectAll 참조)"""
        for item in self.items.values():
            item.properties['selected'] = False
    
    def select_item(self, item_uuid: str) -> bool:
        """아이템 선택 (QGIS setSelectedItem 참조)"""
        self.clear_selection()
        if item_uuid in self.items:
            self.items[item_uuid].properties['selected'] = True
            return True
        return False
    
    def raise_item(self, item_uuid: str) -> bool:
        """아이템 앞으로 가져오기 (QGIS raiseItem 참조)"""
        if item_uuid in self.items:
            current_item = self.items[item_uuid]
            max_z = max((item.z_value for item in self.items.values() if item.uuid != item_uuid), default=0)
            current_item.z_value = max_z + 1
            return True
        return False
    
    def lower_item(self, item_uuid: str) -> bool:
        """아이템 뒤로 보내기 (QGIS lowerItem 참조)"""
        if item_uuid in self.items:
            current_item = self.items[item_uuid]
            min_z = min((item.z_value for item in self.items.values() if item.uuid != item_uuid), default=1)
            current_item.z_value = min_z - 1 if min_z > 1 else 1
            return True
        return False
    
    def update_z_values(self) -> None:
        """Z-값 업데이트 (QGIS updateZValues 참조)"""
        sorted_items = sorted(self.items.values(), key=lambda x: x.z_value)
        for i, item in enumerate(sorted_items):
            item.z_value = i + 1
    
    def set_custom_property(self, key: str, value: Any) -> None:
        """사용자 정의 속성 설정 (QGIS setCustomProperty 참조)"""
        self.custom_properties[key] = value
        self.modified_date = datetime.now()
    
    def get_custom_property(self, key: str, default_value: Any = None) -> Any:
        """사용자 정의 속성 조회 (QGIS customProperty 참조)"""
        return self.custom_properties.get(key, default_value)
    
    def remove_custom_property(self, key: str) -> None:
        """사용자 정의 속성 제거 (QGIS removeCustomProperty 참조)"""
        self.custom_properties.pop(key, None)
        self.modified_date = datetime.now()
    
    def get_layout_bounds(self) -> Dict[str, float]:
        """레이아웃 범위 계산 (QGIS layoutBounds 참조)"""
        if not self.items:
            return {'x': 0, 'y': 0, 'width': 0, 'height': 0}
        
        min_x = min(item.position_x for item in self.items.values())
        min_y = min(item.position_y for item in self.items.values())
        max_x = max(item.position_x + item.width for item in self.items.values())
        max_y = max(item.position_y + item.height for item in self.items.values())
        
        return {
            'x': min_x,
            'y': min_y,
            'width': max_x - min_x,
            'height': max_y - min_y
        }
    
    def refresh(self) -> None:
        """레이아웃 새로고침 (QGIS refresh 참조)"""
        # 모든 아이템의 데이터 의존적 속성 업데이트
        for item in self.items.values():
            if 'needs_refresh' in item.properties:
                item.properties['needs_refresh'] = True
        
        self.modified_date = datetime.now()
    
    def to_dict(self) -> Dict[str, Any]:
        """딕셔너리로 변환"""
        return {
            'uuid': self.uuid,
            'name': self.name,
            'description': self.description,
            'settings': self.settings.to_dict(),
            'items': {uuid: item.to_dict() for uuid, item in self.items.items()},
            'pages': self.pages,
            'created_date': self.created_date.isoformat(),
            'modified_date': self.modified_date.isoformat(),
            'author': self.author,
            'custom_properties': self.custom_properties,
            'variables': self.variables
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'HGISLayout':
        """딕셔너리에서 생성"""
        layout = cls()
        
        layout.uuid = data.get('uuid', layout.uuid)
        layout.name = data.get('name', layout.name)
        layout.description = data.get('description', layout.description)
        
        if 'settings' in data:
            layout.settings = LayoutSettings.from_dict(data['settings'])
        
        if 'items' in data:
            layout.items = {
                uuid: LayoutItem.from_dict(item_data) 
                for uuid, item_data in data['items'].items()
            }
        
        layout.pages = data.get('pages', layout.pages)
        
        if 'created_date' in data:
            layout.created_date = datetime.fromisoformat(data['created_date'])
        if 'modified_date' in data:
            layout.modified_date = datetime.fromisoformat(data['modified_date'])
            
        layout.author = data.get('author', layout.author)
        layout.custom_properties = data.get('custom_properties', {})
        layout.variables = data.get('variables', {})
        
        return layout


class LayoutManager:
    """HGIS 레이아웃 관리자 (QGIS QgsLayoutManager 참조)"""
    
    def __init__(self, project_path: Optional[str] = None):
        self.project_path = project_path
        self.layouts: Dict[str, HGISLayout] = {}
        
        # 템플릿 관리
        self.templates: Dict[str, LayoutTemplate] = {}
        self._load_default_templates()
        
        # 익스포터
        self.pdf_exporter = PDFExporter()
        self.png_exporter = PNGExporter()
    
    def _load_default_templates(self):
        """기본 템플릿 로드"""
        # A4 템플릿
        a4_template = A4Template()
        self.templates[a4_template.name] = a4_template
        
        # A3 템플릿
        a3_template = A3Template()
        self.templates[a3_template.name] = a3_template
    
    def create_layout(self, name: str, template_name: Optional[str] = None) -> HGISLayout:
        """새 레이아웃 생성"""
        layout = HGISLayout(name=name)
        
        # 템플릿 적용
        if template_name and template_name in self.templates:
            template = self.templates[template_name]
            template.apply_to_layout(layout)
        
        self.layouts[layout.uuid] = layout
        return layout
    
    def add_layout(self, layout: HGISLayout) -> None:
        """레이아웃 추가 (QGIS addLayout 참조)"""
        if not layout.uuid:
            layout.uuid = str(uuid.uuid4())
        self.layouts[layout.uuid] = layout
    
    def remove_layout(self, layout_uuid: str) -> bool:
        """레이아웃 제거 (QGIS removeLayout 참조)"""
        if layout_uuid in self.layouts:
            del self.layouts[layout_uuid]
            return True
        return False
    
    def get_layout_by_uuid(self, layout_uuid: str) -> Optional[HGISLayout]:
        """UUID로 레이아웃 조회"""
        return self.layouts.get(layout_uuid)
    
    def get_layout_by_name(self, name: str) -> Optional[HGISLayout]:
        """이름으로 레이아웃 조회 (QGIS layoutByName 참조)"""
        for layout in self.layouts.values():
            if layout.name == name:
                return layout
        return None
    
    def get_layouts(self) -> List[HGISLayout]:
        """모든 레이아웃 조회 (QGIS layouts 참조)"""
        return list(self.layouts.values())
    
    def duplicate_layout(self, layout_uuid: str, new_name: str) -> Optional[HGISLayout]:
        """레이아웃 복제"""
        original = self.get_layout_by_uuid(layout_uuid)
        if not original:
            return None
        
        # 딥 카피를 위해 JSON 직렬화/역직렬화 사용
        layout_data = original.to_dict()
        duplicated = HGISLayout.from_dict(layout_data)
        
        # 새로운 UUID와 이름 설정
        duplicated.uuid = str(uuid.uuid4())
        duplicated.name = new_name
        duplicated.created_date = datetime.now()
        duplicated.modified_date = datetime.now()
        
        self.layouts[duplicated.uuid] = duplicated
        return duplicated
    
    def save_layout_as_template(self, layout_uuid: str, template_name: str) -> bool:
        """레이아웃을 템플릿으로 저장 (QGIS saveAsTemplate 참조)"""
        layout = self.get_layout_by_uuid(layout_uuid)
        if not layout:
            return False
        
        template = LayoutTemplate(
            name=template_name,
            description=f"Template created from {layout.name}",
            page_width=layout.settings.page_width,
            page_height=layout.settings.page_height
        )
        
        # 레이아웃 아이템들을 템플릿 아이템으로 변환
        for item in layout.items.values():
            template.add_item(
                item_type=item.item_type,
                position=(item.position_x, item.position_y),
                size=(item.width, item.height),
                properties=item.properties.copy()
            )
        
        self.templates[template_name] = template
        return True
    
    def export_layout_to_pdf(self, layout_uuid: str, output_path: str, **kwargs) -> bool:
        """레이아웃을 PDF로 내보내기"""
        layout = self.get_layout_by_uuid(layout_uuid)
        if not layout:
            return False
        
        return self.pdf_exporter.export(layout, output_path, **kwargs)
    
    def export_layout_to_png(self, layout_uuid: str, output_path: str, **kwargs) -> bool:
        """레이아웃을 PNG로 내보내기"""
        layout = self.get_layout_by_uuid(layout_uuid)
        if not layout:
            return False
        
        return self.png_exporter.export(layout, output_path, **kwargs)
    
    def save_to_file(self, file_path: str) -> bool:
        """레이아웃들을 파일로 저장"""
        try:
            data = {
                'project_path': self.project_path,
                'layouts': {uuid: layout.to_dict() for uuid, layout in self.layouts.items()},
                'templates': {name: template.to_dict() for name, template in self.templates.items()},
                'saved_date': datetime.now().isoformat()
            }
            
            with open(file_path, 'w', encoding='utf-8') as f:
                json.dump(data, f, indent=2, ensure_ascii=False)
            
            return True
        except Exception as e:
            print(f"레이아웃 저장 실패: {e}")
            return False
    
    def load_from_file(self, file_path: str) -> bool:
        """파일에서 레이아웃들을 로드"""
        try:
            if not os.path.exists(file_path):
                return False
            
            with open(file_path, 'r', encoding='utf-8') as f:
                data = json.load(f)
            
            self.project_path = data.get('project_path', self.project_path)
            
            # 레이아웃 로드
            if 'layouts' in data:
                self.layouts = {
                    uuid: HGISLayout.from_dict(layout_data)
                    for uuid, layout_data in data['layouts'].items()
                }
            
            # 템플릿 로드
            if 'templates' in data:
                for name, template_data in data['templates'].items():
                    template = LayoutTemplate.from_dict(template_data)
                    self.templates[name] = template
            
            return True
        except Exception as e:
            print(f"레이아웃 로드 실패: {e}")
            return False
    
    def clear(self) -> None:
        """모든 레이아웃 제거 (QGIS clear 참조)"""
        self.layouts.clear()
    
    def get_layout_names(self) -> List[str]:
        """모든 레이아웃 이름 목록"""
        return [layout.name for layout in self.layouts.values()]
    
    def rename_layout(self, layout_uuid: str, new_name: str) -> bool:
        """레이아웃 이름 변경"""
        layout = self.get_layout_by_uuid(layout_uuid)
        if not layout:
            return False
        
        layout.name = new_name
        layout.modified_date = datetime.now()
        return True