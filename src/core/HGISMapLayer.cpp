#include "HGISMapLayer.h"
#include <QUuid>
#include <QDebug>
#include <QRegExp>
#include <QPainter>

class HGISMapLayer::Private
{
public:
    QString id;
    QString name;
    QString source;
    HGISMapLayerType type;
    HGISCoordinateReferenceSystem crs;
    bool visible = true;
    int opacity = 100;
    double minimumScale = 0;
    double maximumScale = 0;
    QString abstract;
    QString attribution;
    
    Private(HGISMapLayerType layerType, const QString &layerName, const QString &layerSource)
        : name(layerName)
        , source(layerSource)
        , type(layerType)
    {
        // 고유 ID 생성
        id = HGISMapLayer::generateId(layerName);
    }
};

HGISMapLayer::HGISMapLayer(HGISMapLayerType type, const QString &name, const QString &source)
    : QObject()
    , d(std::make_unique<Private>(type, name, source))
{
    qDebug() << "HGISMapLayer 생성:" << name << "타입:" << static_cast<int>(type);
}

HGISMapLayer::~HGISMapLayer() = default;

QString HGISMapLayer::id() const
{
    return d->id;
}

void HGISMapLayer::setId(const QString &id)
{
    d->id = id;
}

QString HGISMapLayer::name() const
{
    return d->name;
}

void HGISMapLayer::setName(const QString &name)
{
    if (d->name != name) {
        d->name = name;
        emit nameChanged();
    }
}

HGISMapLayerType HGISMapLayer::type() const
{
    return d->type;
}

QString HGISMapLayer::typeAsString() const
{
    switch (d->type) {
        case HGISMapLayerType::VectorLayer:
            return "벡터";
        case HGISMapLayerType::RasterLayer:
            return "래스터";
        case HGISMapLayerType::PluginLayer:
            return "플러그인";
        case HGISMapLayerType::MeshLayer:
            return "메시";
        case HGISMapLayerType::VectorTileLayer:
            return "벡터타일";
        case HGISMapLayerType::AnnotationLayer:
            return "주석";
        case HGISMapLayerType::PointCloudLayer:
            return "포인트클라우드";
        case HGISMapLayerType::GroupLayer:
            return "그룹";
        default:
            return "알 수 없음";
    }
}

QString HGISMapLayer::source() const
{
    return d->source;
}

void HGISMapLayer::setSource(const QString &source)
{
    d->source = source;
}

HGISCoordinateReferenceSystem HGISMapLayer::crs() const
{
    return d->crs;
}

void HGISMapLayer::setCrs(const HGISCoordinateReferenceSystem &crs)
{
    if (d->crs != crs) {
        d->crs = crs;
        emit crsChanged();
    }
}

bool HGISMapLayer::isVisible() const
{
    return d->visible;
}

void HGISMapLayer::setVisible(bool visible)
{
    if (d->visible != visible) {
        d->visible = visible;
        emit visibilityChanged(visible);
        emit repaintRequested();
    }
}

bool HGISMapLayer::isValid() const
{
    return !d->id.isEmpty() && !d->name.isEmpty();
}

int HGISMapLayer::opacity() const
{
    return d->opacity;
}

void HGISMapLayer::setOpacity(int opacity)
{
    opacity = qBound(0, opacity, 100);
    if (d->opacity != opacity) {
        d->opacity = opacity;
        emit opacityChanged(opacity);
        emit repaintRequested();
    }
}

double HGISMapLayer::minimumScale() const
{
    return d->minimumScale;
}

void HGISMapLayer::setMinimumScale(double scale)
{
    d->minimumScale = scale;
}

double HGISMapLayer::maximumScale() const
{
    return d->maximumScale;
}

void HGISMapLayer::setMaximumScale(double scale)
{
    d->maximumScale = scale;
}

bool HGISMapLayer::isInScaleRange(double scale) const
{
    if (d->minimumScale > 0 && scale < d->minimumScale) {
        return false;
    }
    if (d->maximumScale > 0 && scale > d->maximumScale) {
        return false;
    }
    return true;
}

QString HGISMapLayer::abstract() const
{
    return d->abstract;
}

void HGISMapLayer::setAbstract(const QString &abstract)
{
    d->abstract = abstract;
}

QString HGISMapLayer::attribution() const
{
    return d->attribution;
}

void HGISMapLayer::setAttribution(const QString &attrib)
{
    d->attribution = attrib;
}

QString HGISMapLayer::generateId(const QString &name)
{
    QString baseId = name.isEmpty() ? "layer" : name;
    baseId = baseId.replace(QRegExp("[^a-zA-Z0-9_]"), "_");
    return QString("%1_%2").arg(baseId).arg(QUuid::createUuid().toString().mid(1, 8));
}