#ifndef HGISLAYERMANAGER_H
#define HGISLAYERMANAGER_H

#include <QObject>
#include <QList>
#include <memory>
#include "HGISMapLayer.h"

class HGISVectorLayer;

class CORE_EXPORT HGISLayerManager : public QObject
{
    Q_OBJECT

public:
    explicit HGISLayerManager(QObject *parent = nullptr);
    ~HGISLayerManager();
    
    // 레이어 추가/제거
    bool addLayer(HGISMapLayer *layer);
    bool insertLayer(HGISMapLayer *layer, int index);
    bool removeLayer(HGISMapLayer *layer);
    bool removeLayer(const QString &layerId);
    void removeAllLayers();
    
    // 레이어 가져오기
    HGISMapLayer* layer(const QString &layerId) const;
    HGISMapLayer* layerByName(const QString &name) const;
    HGISMapLayer* layerAt(int index) const;
    
    // 벡터 레이어 가져오기
    QList<HGISVectorLayer*> vectorLayers() const;
    
    // 모든 레이어
    QList<HGISMapLayer*> layers() const;
    int count() const;
    bool isEmpty() const;
    
    // 레이어 순서
    int layerIndex(HGISMapLayer *layer) const;
    int layerIndex(const QString &layerId) const;
    bool moveLayer(HGISMapLayer *layer, int newIndex);
    bool moveLayerUp(HGISMapLayer *layer);
    bool moveLayerDown(HGISMapLayer *layer);
    void moveLayerToTop(HGISMapLayer *layer);
    void moveLayerToBottom(HGISMapLayer *layer);
    
    // 가시성
    void setAllLayersVisible(bool visible);
    QList<HGISMapLayer*> visibleLayers() const;
    
    // 범위
    QRectF fullExtent() const;
    QRectF visibleExtent() const;
    
    // 좌표계
    HGISCoordinateReferenceSystem projectCrs() const;
    void setProjectCrs(const HGISCoordinateReferenceSystem &crs);
    
    // 렌더링 순서 (아래에서 위로)
    QList<HGISMapLayer*> layersInRenderOrder() const;
    
signals:
    // 레이어 변경 시그널
    void layerAdded(HGISMapLayer *layer);
    void layerRemoved(const QString &layerId);
    void layerOrderChanged();
    void layersChanged();
    
    // 렌더링 요청
    void repaintRequested();
    
private:
    void connectLayerSignals(HGISMapLayer *layer);
    void disconnectLayerSignals(HGISMapLayer *layer);
    
    class Private;
    std::unique_ptr<Private> d;
};

#endif // HGISLAYERMANAGER_H