#include "HGISLayerManager.h"
#include "HGISVectorLayer.h"
#include <QDebug>
#include <algorithm>

class HGISLayerManager::Private
{
public:
    QList<HGISMapLayer*> layers;
    HGISCoordinateReferenceSystem projectCrs;
    
    Private()
    {
        // 기본 프로젝트 CRS는 WGS84
        projectCrs = HGISCoordinateReferenceSystem::wgs84();
    }
    
    ~Private()
    {
        // 레이어 정리
        qDeleteAll(layers);
        layers.clear();
    }
};

HGISLayerManager::HGISLayerManager(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<Private>())
{
    qDebug() << "HGISLayerManager 초기화";
}

HGISLayerManager::~HGISLayerManager()
{
    removeAllLayers();
}

bool HGISLayerManager::addLayer(HGISMapLayer *layer)
{
    if (!layer) {
        return false;
    }
    
    // 중복 체크
    if (d->layers.contains(layer)) {
        qWarning() << "레이어가 이미 존재합니다:" << layer->name();
        return false;
    }
    
    // 레이어 추가 (상단에)
    d->layers.prepend(layer);
    
    // 시그널 연결
    connectLayerSignals(layer);
    
    qInfo() << "레이어 추가:" << layer->name() << "타입:" << layer->typeAsString();
    
    emit layerAdded(layer);
    emit layersChanged();
    emit repaintRequested();
    
    return true;
}

bool HGISLayerManager::insertLayer(HGISMapLayer *layer, int index)
{
    if (!layer) {
        return false;
    }
    
    if (d->layers.contains(layer)) {
        qWarning() << "레이어가 이미 존재합니다:" << layer->name();
        return false;
    }
    
    index = qBound(0, index, d->layers.size());
    d->layers.insert(index, layer);
    
    connectLayerSignals(layer);
    
    emit layerAdded(layer);
    emit layersChanged();
    emit repaintRequested();
    
    return true;
}

bool HGISLayerManager::removeLayer(HGISMapLayer *layer)
{
    if (!layer) {
        return false;
    }
    
    int index = d->layers.indexOf(layer);
    if (index < 0) {
        return false;
    }
    
    QString layerId = layer->id();
    
    disconnectLayerSignals(layer);
    d->layers.removeAt(index);
    
    emit layerRemoved(layerId);
    emit layersChanged();
    emit repaintRequested();
    
    delete layer;
    
    return true;
}

bool HGISLayerManager::removeLayer(const QString &layerId)
{
    HGISMapLayer *layer = this->layer(layerId);
    if (!layer) {
        return false;
    }
    
    return removeLayer(layer);
}

void HGISLayerManager::removeAllLayers()
{
    for (HGISMapLayer *layer : d->layers) {
        disconnectLayerSignals(layer);
    }
    
    qDeleteAll(d->layers);
    d->layers.clear();
    
    emit layersChanged();
    emit repaintRequested();
}

HGISMapLayer* HGISLayerManager::layer(const QString &layerId) const
{
    for (HGISMapLayer *layer : d->layers) {
        if (layer->id() == layerId) {
            return layer;
        }
    }
    return nullptr;
}

HGISMapLayer* HGISLayerManager::layerByName(const QString &name) const
{
    for (HGISMapLayer *layer : d->layers) {
        if (layer->name() == name) {
            return layer;
        }
    }
    return nullptr;
}

HGISMapLayer* HGISLayerManager::layerAt(int index) const
{
    if (index < 0 || index >= d->layers.size()) {
        return nullptr;
    }
    return d->layers.at(index);
}

QList<HGISVectorLayer*> HGISLayerManager::vectorLayers() const
{
    QList<HGISVectorLayer*> result;
    for (HGISMapLayer *layer : d->layers) {
        if (layer->type() == HGISMapLayerType::VectorLayer) {
            HGISVectorLayer *vectorLayer = qobject_cast<HGISVectorLayer*>(layer);
            if (vectorLayer) {
                result.append(vectorLayer);
            }
        }
    }
    return result;
}

QList<HGISMapLayer*> HGISLayerManager::layers() const
{
    return d->layers;
}

int HGISLayerManager::count() const
{
    return d->layers.size();
}

bool HGISLayerManager::isEmpty() const
{
    return d->layers.isEmpty();
}

int HGISLayerManager::layerIndex(HGISMapLayer *layer) const
{
    return d->layers.indexOf(layer);
}

int HGISLayerManager::layerIndex(const QString &layerId) const
{
    HGISMapLayer *layer = this->layer(layerId);
    if (!layer) {
        return -1;
    }
    return layerIndex(layer);
}

bool HGISLayerManager::moveLayer(HGISMapLayer *layer, int newIndex)
{
    if (!layer) {
        return false;
    }
    
    int currentIndex = d->layers.indexOf(layer);
    if (currentIndex < 0) {
        return false;
    }
    
    if (currentIndex == newIndex) {
        return true;
    }
    
    newIndex = qBound(0, newIndex, d->layers.size() - 1);
    
    d->layers.move(currentIndex, newIndex);
    
    emit layerOrderChanged();
    emit repaintRequested();
    
    return true;
}

bool HGISLayerManager::moveLayerUp(HGISMapLayer *layer)
{
    int index = layerIndex(layer);
    if (index <= 0) {
        return false;
    }
    return moveLayer(layer, index - 1);
}

bool HGISLayerManager::moveLayerDown(HGISMapLayer *layer)
{
    int index = layerIndex(layer);
    if (index < 0 || index >= d->layers.size() - 1) {
        return false;
    }
    return moveLayer(layer, index + 1);
}

void HGISLayerManager::moveLayerToTop(HGISMapLayer *layer)
{
    moveLayer(layer, 0);
}

void HGISLayerManager::moveLayerToBottom(HGISMapLayer *layer)
{
    moveLayer(layer, d->layers.size() - 1);
}

void HGISLayerManager::setAllLayersVisible(bool visible)
{
    for (HGISMapLayer *layer : d->layers) {
        layer->setVisible(visible);
    }
}

QList<HGISMapLayer*> HGISLayerManager::visibleLayers() const
{
    QList<HGISMapLayer*> result;
    for (HGISMapLayer *layer : d->layers) {
        if (layer->isVisible()) {
            result.append(layer);
        }
    }
    return result;
}

QRectF HGISLayerManager::fullExtent() const
{
    QRectF extent;
    
    for (HGISMapLayer *layer : d->layers) {
        if (layer->isValid()) {
            QRectF layerExtent = layer->extent();
            if (!layerExtent.isNull()) {
                if (extent.isNull()) {
                    extent = layerExtent;
                } else {
                    extent = extent.united(layerExtent);
                }
            }
        }
    }
    
    return extent;
}

QRectF HGISLayerManager::visibleExtent() const
{
    QRectF extent;
    
    for (HGISMapLayer *layer : d->layers) {
        if (layer->isVisible() && layer->isValid()) {
            QRectF layerExtent = layer->extent();
            if (!layerExtent.isNull()) {
                if (extent.isNull()) {
                    extent = layerExtent;
                } else {
                    extent = extent.united(layerExtent);
                }
            }
        }
    }
    
    return extent;
}

HGISCoordinateReferenceSystem HGISLayerManager::projectCrs() const
{
    return d->projectCrs;
}

void HGISLayerManager::setProjectCrs(const HGISCoordinateReferenceSystem &crs)
{
    d->projectCrs = crs;
    emit repaintRequested();
}

QList<HGISMapLayer*> HGISLayerManager::layersInRenderOrder() const
{
    // 렌더링 순서는 역순 (아래에서 위로)
    QList<HGISMapLayer*> result = d->layers;
    std::reverse(result.begin(), result.end());
    return result;
}

void HGISLayerManager::connectLayerSignals(HGISMapLayer *layer)
{
    if (!layer) {
        return;
    }
    
    connect(layer, &HGISMapLayer::repaintRequested,
            this, &HGISLayerManager::repaintRequested);
    connect(layer, &HGISMapLayer::extentChanged,
            this, &HGISLayerManager::repaintRequested);
    connect(layer, &HGISMapLayer::dataChanged,
            this, &HGISLayerManager::layersChanged);
}

void HGISLayerManager::disconnectLayerSignals(HGISMapLayer *layer)
{
    if (!layer) {
        return;
    }
    
    disconnect(layer, nullptr, this, nullptr);
}