#include "HGISVectorLayer.h"
#include "HGISCoordinateTransform.h"
#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QFileInfo>
#include <cmath>

class HGISVectorLayer::Private
{
public:
    std::unique_ptr<HGISGdalProvider> provider;
    HGISGeometryType geometryType = HGISGeometryType::Unknown;
    HGISSymbol symbol;
    HGISRendererType rendererType = HGISRendererType::SingleSymbol;
    
    // 라벨 설정
    bool labelsEnabled = false;
    QString labelField;
    QFont labelFont{"맑은 고딕", 9};
    QColor labelColor = Qt::black;
    
    // 선택된 피처
    QSet<long> selectedFeatureIds;
    
    // 캐시된 피처
    mutable std::vector<HGISGdalProvider::Feature> cachedFeatures;
    mutable bool featuresCached = false;
    
    Private()
    {
        // 기본 심볼 설정
        symbol.fillColor = QColor(100, 150, 200, 100);
        symbol.strokeColor = Qt::darkBlue;
        symbol.strokeWidth = 1.5;
    }
    
    void updateGeometryType()
    {
        if (!provider || !provider->isValid()) {
            geometryType = HGISGeometryType::Unknown;
            return;
        }
        
        QString typeStr = provider->geometryType();
        if (typeStr == "Point") {
            geometryType = HGISGeometryType::Point;
        } else if (typeStr == "LineString") {
            geometryType = HGISGeometryType::LineString;
        } else if (typeStr == "Polygon") {
            geometryType = HGISGeometryType::Polygon;
        } else if (typeStr == "MultiPoint") {
            geometryType = HGISGeometryType::MultiPoint;
        } else if (typeStr == "MultiLineString") {
            geometryType = HGISGeometryType::MultiLineString;
        } else if (typeStr == "MultiPolygon") {
            geometryType = HGISGeometryType::MultiPolygon;
        } else {
            geometryType = HGISGeometryType::Unknown;
        }
    }
};

HGISVectorLayer::HGISVectorLayer(const QString &path, const QString &name, const QString &providerKey)
    : HGISMapLayer(HGISMapLayerType::VectorLayer, name.isEmpty() ? QFileInfo(path).baseName() : name, path)
    , d(std::make_unique<Private>())
{
    if (!path.isEmpty()) {
        loadFromFile(path);
    }
}

HGISVectorLayer::~HGISVectorLayer() = default;

bool HGISVectorLayer::loadFromFile(const QString &path)
{
    d->provider = std::make_unique<HGISGdalProvider>(path);
    
    if (!d->provider->open()) {
        qWarning() << "벡터 레이어 열기 실패:" << path;
        return false;
    }
    
    setSource(path);
    if (name().isEmpty()) {
        setName(d->provider->layerName());
    }
    
    // CRS 설정
    HGISCoordinateReferenceSystem layerCrs(d->provider->epsgCode());
    setCrs(layerCrs);
    
    // 지오메트리 타입 업데이트
    d->updateGeometryType();
    
    // 캐시 초기화
    d->featuresCached = false;
    
    qInfo() << "벡터 레이어 로드 성공:" << name()
            << "피처 수:" << featureCount()
            << "타입:" << geometryTypeAsString();
    
    emit dataChanged();
    emit extentChanged();
    
    return true;
}

HGISGdalProvider* HGISVectorLayer::dataProvider() const
{
    return d->provider.get();
}

HGISGeometryType HGISVectorLayer::geometryType() const
{
    return d->geometryType;
}

QString HGISVectorLayer::geometryTypeAsString() const
{
    switch (d->geometryType) {
        case HGISGeometryType::Point:
            return "포인트";
        case HGISGeometryType::LineString:
            return "라인";
        case HGISGeometryType::Polygon:
            return "폴리곤";
        case HGISGeometryType::MultiPoint:
            return "멀티포인트";
        case HGISGeometryType::MultiLineString:
            return "멀티라인";
        case HGISGeometryType::MultiPolygon:
            return "멀티폴리곤";
        default:
            return "알 수 없음";
    }
}

long HGISVectorLayer::featureCount() const
{
    if (!d->provider) {
        return 0;
    }
    return d->provider->featureCount();
}

QStringList HGISVectorLayer::fields() const
{
    if (!d->provider) {
        return QStringList();
    }
    return d->provider->fields();
}

QRectF HGISVectorLayer::extent() const
{
    if (!d->provider) {
        return QRectF();
    }
    return d->provider->extent();
}

std::vector<HGISGdalProvider::Feature> HGISVectorLayer::features() const
{
    if (!d->provider) {
        return std::vector<HGISGdalProvider::Feature>();
    }
    
    if (!d->featuresCached) {
        d->cachedFeatures = d->provider->readFeatures();
        d->featuresCached = true;
    }
    
    return d->cachedFeatures;
}

std::vector<HGISGdalProvider::Feature> HGISVectorLayer::features(const QRectF &extent) const
{
    if (!d->provider) {
        return std::vector<HGISGdalProvider::Feature>();
    }
    return d->provider->readFeatures(extent);
}

HGISSymbol HGISVectorLayer::symbol() const
{
    return d->symbol;
}

void HGISVectorLayer::setSymbol(const HGISSymbol &symbol)
{
    d->symbol = symbol;
    emit symbolChanged();
    emit repaintRequested();
}

HGISRendererType HGISVectorLayer::rendererType() const
{
    return d->rendererType;
}

void HGISVectorLayer::setRendererType(HGISRendererType type)
{
    d->rendererType = type;
    emit repaintRequested();
}

bool HGISVectorLayer::labelsEnabled() const
{
    return d->labelsEnabled;
}

void HGISVectorLayer::setLabelsEnabled(bool enabled)
{
    if (d->labelsEnabled != enabled) {
        d->labelsEnabled = enabled;
        emit labelsChanged();
        emit repaintRequested();
    }
}

QString HGISVectorLayer::labelField() const
{
    return d->labelField;
}

void HGISVectorLayer::setLabelField(const QString &fieldName)
{
    if (d->labelField != fieldName) {
        d->labelField = fieldName;
        emit labelsChanged();
        emit repaintRequested();
    }
}

QFont HGISVectorLayer::labelFont() const
{
    return d->labelFont;
}

void HGISVectorLayer::setLabelFont(const QFont &font)
{
    d->labelFont = font;
    emit labelsChanged();
    emit repaintRequested();
}

QColor HGISVectorLayer::labelColor() const
{
    return d->labelColor;
}

void HGISVectorLayer::setLabelColor(const QColor &color)
{
    d->labelColor = color;
    emit labelsChanged();
    emit repaintRequested();
}

QSet<long> HGISVectorLayer::selectedFeatureIds() const
{
    return d->selectedFeatureIds;
}

void HGISVectorLayer::selectFeatures(const QSet<long> &ids)
{
    d->selectedFeatureIds = ids;
    emit selectionChanged(d->selectedFeatureIds);
    emit repaintRequested();
}

void HGISVectorLayer::selectFeature(long id)
{
    d->selectedFeatureIds.insert(id);
    emit selectionChanged(d->selectedFeatureIds);
    emit repaintRequested();
}

void HGISVectorLayer::deselectFeature(long id)
{
    d->selectedFeatureIds.remove(id);
    emit selectionChanged(d->selectedFeatureIds);
    emit repaintRequested();
}

void HGISVectorLayer::clearSelection()
{
    d->selectedFeatureIds.clear();
    emit selectionChanged(d->selectedFeatureIds);
    emit repaintRequested();
}

bool HGISVectorLayer::isFeatureSelected(long id) const
{
    return d->selectedFeatureIds.contains(id);
}

void HGISVectorLayer::render(QPainter *painter, const QRectF &extent, double scale)
{
    if (!isVisible() || !isValid()) {
        return;
    }
    
    if (!isInScaleRange(scale)) {
        return;
    }
    
    painter->save();
    
    // 투명도 설정
    painter->setOpacity(opacity() / 100.0);
    
    // 피처 렌더링
    renderFeatures(painter, extent, scale);
    
    // 라벨 렌더링
    if (d->labelsEnabled) {
        renderLabels(painter, extent, scale);
    }
    
    painter->restore();
}

void HGISVectorLayer::renderFeatures(QPainter *painter, const QRectF &extent, double scale)
{
    auto featuresToRender = features(extent);
    
    for (const auto &feature : featuresToRender) {
        HGISSymbol symbolToUse = d->symbol;
        
        // 선택된 피처는 다른 색상으로
        if (isFeatureSelected(feature.id)) {
            symbolToUse.fillColor = QColor(255, 255, 0, 150);
            symbolToUse.strokeColor = Qt::yellow;
            symbolToUse.strokeWidth = 2.0;
        }
        
        // 지오메트리 타입에 따라 렌더링
        switch (d->geometryType) {
            case HGISGeometryType::Point:
            case HGISGeometryType::MultiPoint:
                for (const auto &pt : feature.geometry) {
                    drawPointSymbol(painter, pt, symbolToUse);
                }
                break;
                
            case HGISGeometryType::LineString:
            case HGISGeometryType::MultiLineString: {
                QVector<QPointF> linePoints(feature.geometry.begin(), feature.geometry.end());
                drawLineSymbol(painter, linePoints, symbolToUse);
                break;
            }
                
            case HGISGeometryType::Polygon:
            case HGISGeometryType::MultiPolygon: {
                QVector<QPointF> polyPoints(feature.geometry.begin(), feature.geometry.end());
                drawPolygonSymbol(painter, polyPoints, symbolToUse);
                break;
            }
                
            default:
                break;
        }
    }
}

void HGISVectorLayer::renderLabels(QPainter *painter, const QRectF &extent, double scale)
{
    if (d->labelField.isEmpty()) {
        return;
    }
    
    painter->setFont(d->labelFont);
    painter->setPen(d->labelColor);
    
    auto featuresToLabel = features(extent);
    
    for (const auto &feature : featuresToLabel) {
        if (feature.geometry.empty()) {
            continue;
        }
        
        // 라벨 텍스트 가져오기
        QVariant labelValue = feature.attributes.value(d->labelField);
        QString labelText = labelValue.toString();
        
        if (labelText.isEmpty()) {
            continue;
        }
        
        // 라벨 위치 결정 (지오메트리 중심)
        QPointF labelPos;
        if (d->geometryType == HGISGeometryType::Point || 
            d->geometryType == HGISGeometryType::MultiPoint) {
            labelPos = feature.geometry.front();
        } else {
            // 폴리곤이나 라인의 경우 중심점 계산
            double sumX = 0, sumY = 0;
            for (const auto &pt : feature.geometry) {
                sumX += pt.x();
                sumY += pt.y();
            }
            labelPos = QPointF(sumX / feature.geometry.size(), 
                              sumY / feature.geometry.size());
        }
        
        // 라벨 그리기
        painter->drawText(labelPos, labelText);
    }
}

void HGISVectorLayer::drawPointSymbol(QPainter *painter, const QPointF &point, const HGISSymbol &symbol)
{
    painter->setPen(QPen(symbol.strokeColor, symbol.strokeWidth, symbol.penStyle));
    painter->setBrush(QBrush(symbol.fillColor, symbol.brushStyle));
    
    double size = symbol.pointSize;
    
    switch (symbol.pointSymbolType) {
        case HGISSymbol::Circle:
            painter->drawEllipse(point, size, size);
            break;
            
        case HGISSymbol::Square:
            painter->drawRect(QRectF(point.x() - size, point.y() - size, 
                                    size * 2, size * 2));
            break;
            
        case HGISSymbol::Triangle: {
            QPolygonF triangle;
            triangle << QPointF(point.x(), point.y() - size)
                    << QPointF(point.x() - size, point.y() + size)
                    << QPointF(point.x() + size, point.y() + size);
            painter->drawPolygon(triangle);
            break;
        }
            
        case HGISSymbol::Cross:
            painter->drawLine(QPointF(point.x() - size, point.y()), 
                            QPointF(point.x() + size, point.y()));
            painter->drawLine(QPointF(point.x(), point.y() - size), 
                            QPointF(point.x(), point.y() + size));
            break;
            
        case HGISSymbol::Star: {
            QPolygonF star;
            for (int i = 0; i < 10; i++) {
                double angle = M_PI * i / 5.0;
                double r = (i % 2 == 0) ? size : size / 2.0;
                star << QPointF(point.x() + r * cos(angle), 
                              point.y() + r * sin(angle));
            }
            painter->drawPolygon(star);
            break;
        }
    }
}

void HGISVectorLayer::drawLineSymbol(QPainter *painter, const QVector<QPointF> &points, const HGISSymbol &symbol)
{
    if (points.size() < 2) {
        return;
    }
    
    painter->setPen(QPen(symbol.strokeColor, symbol.strokeWidth, symbol.penStyle));
    
    QPainterPath path;
    path.moveTo(points[0]);
    for (int i = 1; i < points.size(); ++i) {
        path.lineTo(points[i]);
    }
    
    painter->drawPath(path);
}

void HGISVectorLayer::drawPolygonSymbol(QPainter *painter, const QVector<QPointF> &points, const HGISSymbol &symbol)
{
    if (points.size() < 3) {
        return;
    }
    
    painter->setPen(QPen(symbol.strokeColor, symbol.strokeWidth, symbol.penStyle));
    painter->setBrush(QBrush(symbol.fillColor, symbol.brushStyle));
    
    QPolygonF polygon(points);
    painter->drawPolygon(polygon);
}

HGISMapLayer* HGISVectorLayer::clone() const
{
    HGISVectorLayer *layer = new HGISVectorLayer();
    layer->setName(name());
    layer->setSource(source());
    layer->setCrs(crs());
    layer->setVisible(isVisible());
    layer->setOpacity(opacity());
    layer->setSymbol(d->symbol);
    layer->setRendererType(d->rendererType);
    layer->setLabelsEnabled(d->labelsEnabled);
    layer->setLabelField(d->labelField);
    layer->setLabelFont(d->labelFont);
    layer->setLabelColor(d->labelColor);
    
    if (!source().isEmpty()) {
        layer->loadFromFile(source());
    }
    
    return layer;
}

bool HGISVectorLayer::isValid() const
{
    return HGISMapLayer::isValid() && d->provider && d->provider->isValid();
}