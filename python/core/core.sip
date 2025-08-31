// HGIS Core 모듈 SIP 정의
%Module(name=hgis.core, keyword_arguments="Optional")

%Import QtCore/QtCoremod.sip
%Import QtGui/QtGuimod.sip

%Include HGISApplication.sip
%Include HGISCoordinateReferenceSystem.sip
%Include HGISCoordinateTransform.sip
%Include HGISMapLayer.sip
%Include HGISVectorLayer.sip
%Include HGISLayerManager.sip

%ModuleHeaderCode
#include "HGISApplication.h"
%End

%ModuleCode
// 모듈 초기화 코드
static bool hgis_core_initialized = false;

void init_hgis_core()
{
    if (!hgis_core_initialized) {
        // GDAL/OGR 초기화
        hgis_core_initialized = true;
    }
}
%End

%PostInitialisationCode
// 모듈 로드 시 초기화
init_hgis_core();
%End