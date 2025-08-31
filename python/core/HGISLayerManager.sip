// HGISLayerManager 클래스 바인딩

class HGISLayerManager : QObject
{
%TypeHeaderCode
#include "HGISLayerManager.h"
%End

public:
    // 생성자
    explicit HGISLayerManager(QObject *parent = nullptr);
    virtual ~HGISLayerManager();
    
    // 레이어 추가/제거
    bool addLayer(HGISMapLayer *layer /Transfer/);
    void removeLayer(const QString &layerId);
    void removeLayer(HGISMapLayer *layer);
    void removeAllLayers();
    
    // 레이어 접근
    HGISMapLayer* layer(const QString &id) const;
    HGISMapLayer* layerByName(const QString &name) const;
    HGISMapLayer* layerAt(int index) const;
    QList<HGISMapLayer*> layers() const;
    QList<HGISVectorLayer*> vectorLayers() const;
    
    // 레이어 순서
    void moveLayer(HGISMapLayer *layer, int newIndex);
    void moveLayerToTop(HGISMapLayer *layer);
    void moveLayerToBottom(HGISMapLayer *layer);
    int layerIndex(HGISMapLayer *layer) const;
    
    // 레이어 개수
    int count() const;
    bool isEmpty() const;
    
    // 가시성
    QList<HGISMapLayer*> visibleLayers() const;
    void setAllLayersVisible(bool visible);
    
    // 범위
    QRectF fullExtent() const;
    
    // 프로젝트 CRS
    HGISCoordinateReferenceSystem projectCrs() const;
    void setProjectCrs(const HGISCoordinateReferenceSystem &crs);
    
    // 렌더링 순서
    QList<HGISMapLayer*> layersInRenderOrder() const;
    
signals:
    void layerAdded(HGISMapLayer *layer);
    void layerRemoved(const QString &layerId);
    void layersChanged();
    void layerOrderChanged();
    void projectCrsChanged(const HGISCoordinateReferenceSystem &crs);
    
    // Python 리스트 인터페이스
    int __len__() const;
%MethodCode
    sipRes = sipCpp->count();
%End
    
    HGISMapLayer* __getitem__(int index) const;
%MethodCode
    if (a0 < 0 || a0 >= sipCpp->count()) {
        PyErr_SetString(PyExc_IndexError, "레이어 인덱스가 범위를 벗어났습니다");
        sipIsErr = 1;
    } else {
        sipRes = sipCpp->layerAt(a0);
    }
%End
};