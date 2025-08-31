#include "HGISCoordinateTransform.h"
#include <QDebug>
#include <proj.h>
#include <cmath>

class HGISCoordinateTransform::Private
{
public:
    HGISCoordinateReferenceSystem sourceCrs;
    HGISCoordinateReferenceSystem destCrs;
    PJ *pj = nullptr;
    PJ_CONTEXT *ctx = nullptr;
    QString lastError;
    bool valid = false;
    
    Private() = default;
    
    ~Private()
    {
        clear();
    }
    
    void clear()
    {
        if (pj) {
            proj_destroy(pj);
            pj = nullptr;
        }
        if (ctx) {
            proj_context_destroy(ctx);
            ctx = nullptr;
        }
        valid = false;
        lastError.clear();
    }
    
    bool initialize()
    {
        clear();
        
        if (!sourceCrs.isValid() || !destCrs.isValid()) {
            lastError = "소스 또는 대상 좌표계가 유효하지 않습니다";
            return false;
        }
        
        // 동일한 CRS인 경우
        if (sourceCrs == destCrs) {
            valid = true;
            return true;
        }
        
        ctx = proj_context_create();
        
        // 변환 객체 생성
        QString srcAuth = QString("EPSG:%1").arg(sourceCrs.epsgCode());
        QString dstAuth = QString("EPSG:%1").arg(destCrs.epsgCode());
        
        if (sourceCrs.epsgCode() > 0 && destCrs.epsgCode() > 0) {
            // EPSG 코드를 사용한 변환
            pj = proj_create_crs_to_crs(ctx, 
                                        srcAuth.toUtf8().constData(),
                                        dstAuth.toUtf8().constData(),
                                        nullptr);
        } else {
            // WKT를 사용한 변환
            pj = proj_create_crs_to_crs_from_pj(ctx,
                                                proj_create_from_wkt(ctx, sourceCrs.toWkt().toUtf8().constData(), nullptr, nullptr, nullptr),
                                                proj_create_from_wkt(ctx, destCrs.toWkt().toUtf8().constData(), nullptr, nullptr, nullptr),
                                                nullptr, nullptr);
        }
        
        if (!pj) {
            int errNo = proj_context_errno(ctx);
            const char *errStr = proj_context_errno_string(ctx, errNo);
            lastError = QString("좌표 변환 객체 생성 실패: %1").arg(errStr ? errStr : "알 수 없는 오류");
            clear();
            return false;
        }
        
        // 변환 정규화 (경도가 항상 -180~180 범위 내에 있도록)
        PJ *normalized = proj_normalize_for_visualization(ctx, pj);
        if (normalized) {
            proj_destroy(pj);
            pj = normalized;
        }
        
        valid = true;
        return true;
    }
    
    QPointF transformPoint(double x, double y, bool reverse = false) const
    {
        if (!valid) {
            return QPointF(x, y);
        }
        
        // 동일한 CRS인 경우 변환 없이 반환
        if (sourceCrs == destCrs) {
            return QPointF(x, y);
        }
        
        if (!pj) {
            return QPointF(x, y);
        }
        
        PJ_COORD coord;
        coord.xy.x = x;
        coord.xy.y = y;
        
        PJ_COORD result;
        if (reverse) {
            result = proj_trans(pj, PJ_INV, coord);
        } else {
            result = proj_trans(pj, PJ_FWD, coord);
        }
        
        // 변환 실패 체크
        if (result.xy.x == HUGE_VAL || result.xy.y == HUGE_VAL) {
            qWarning() << "좌표 변환 실패:" << x << y;
            return QPointF(x, y);
        }
        
        return QPointF(result.xy.x, result.xy.y);
    }
};

HGISCoordinateTransform::HGISCoordinateTransform()
    : d(std::make_unique<Private>())
{
}

HGISCoordinateTransform::HGISCoordinateTransform(const HGISCoordinateReferenceSystem &source,
                                                 const HGISCoordinateReferenceSystem &destination)
    : d(std::make_unique<Private>())
{
    d->sourceCrs = source;
    d->destCrs = destination;
    initialize();
}

HGISCoordinateTransform::~HGISCoordinateTransform() = default;

HGISCoordinateTransform::HGISCoordinateTransform(const HGISCoordinateTransform &other)
    : d(std::make_unique<Private>())
{
    d->sourceCrs = other.d->sourceCrs;
    d->destCrs = other.d->destCrs;
    initialize();
}

HGISCoordinateTransform& HGISCoordinateTransform::operator=(const HGISCoordinateTransform &other)
{
    if (this != &other) {
        d->sourceCrs = other.d->sourceCrs;
        d->destCrs = other.d->destCrs;
        initialize();
    }
    return *this;
}

HGISCoordinateTransform::HGISCoordinateTransform(HGISCoordinateTransform &&other) noexcept = default;
HGISCoordinateTransform& HGISCoordinateTransform::operator=(HGISCoordinateTransform &&other) noexcept = default;

void HGISCoordinateTransform::setSourceCrs(const HGISCoordinateReferenceSystem &crs)
{
    d->sourceCrs = crs;
    initialize();
}

void HGISCoordinateTransform::setDestinationCrs(const HGISCoordinateReferenceSystem &crs)
{
    d->destCrs = crs;
    initialize();
}

HGISCoordinateReferenceSystem HGISCoordinateTransform::sourceCrs() const
{
    return d->sourceCrs;
}

HGISCoordinateReferenceSystem HGISCoordinateTransform::destinationCrs() const
{
    return d->destCrs;
}

bool HGISCoordinateTransform::isValid() const
{
    return d->valid;
}

bool HGISCoordinateTransform::isShortCircuitable() const
{
    return d->sourceCrs == d->destCrs;
}

QPointF HGISCoordinateTransform::transform(const QPointF &point) const
{
    return d->transformPoint(point.x(), point.y());
}

QPointF HGISCoordinateTransform::transform(double x, double y) const
{
    return d->transformPoint(x, y);
}

QPolygonF HGISCoordinateTransform::transform(const QPolygonF &polygon) const
{
    QPolygonF result;
    result.reserve(polygon.size());
    
    for (const QPointF &point : polygon) {
        result.append(transform(point));
    }
    
    return result;
}

QRectF HGISCoordinateTransform::transformBoundingBox(const QRectF &rect) const
{
    if (!d->valid || rect.isNull()) {
        return rect;
    }
    
    // 사각형의 4개 모서리와 중간점들을 변환
    QVector<QPointF> points;
    points.reserve(9);
    
    // 모서리
    points.append(transform(rect.topLeft()));
    points.append(transform(rect.topRight()));
    points.append(transform(rect.bottomLeft()));
    points.append(transform(rect.bottomRight()));
    
    // 중간점 (곡선 투영을 위해)
    points.append(transform(QPointF(rect.center().x(), rect.top())));
    points.append(transform(QPointF(rect.center().x(), rect.bottom())));
    points.append(transform(QPointF(rect.left(), rect.center().y())));
    points.append(transform(QPointF(rect.right(), rect.center().y())));
    points.append(transform(rect.center()));
    
    // 변환된 점들의 최소/최대값 계산
    double minX = points[0].x();
    double maxX = points[0].x();
    double minY = points[0].y();
    double maxY = points[0].y();
    
    for (const QPointF &p : points) {
        minX = qMin(minX, p.x());
        maxX = qMax(maxX, p.x());
        minY = qMin(minY, p.y());
        maxY = qMax(maxY, p.y());
    }
    
    return QRectF(QPointF(minX, minY), QPointF(maxX, maxY));
}

QPointF HGISCoordinateTransform::transformReverse(const QPointF &point) const
{
    return d->transformPoint(point.x(), point.y(), true);
}

QPointF HGISCoordinateTransform::transformReverse(double x, double y) const
{
    return d->transformPoint(x, y, true);
}

void HGISCoordinateTransform::initialize()
{
    d->initialize();
    
    if (d->valid) {
        qDebug() << "좌표 변환 초기화 성공:"
                 << "EPSG:" << d->sourceCrs.epsgCode() 
                 << "->" 
                 << "EPSG:" << d->destCrs.epsgCode();
    }
}

// 정적 헬퍼 메서드들
QPointF HGISCoordinateTransform::wgs84ToKorea2000Central(const QPointF &wgs84Point)
{
    HGISCoordinateReferenceSystem wgs84(4326);
    HGISCoordinateReferenceSystem korea2000 = HGISCoordinateReferenceSystem::korea2000Central();
    HGISCoordinateTransform transform(wgs84, korea2000);
    return transform.transform(wgs84Point);
}

QPointF HGISCoordinateTransform::korea2000CentralToWgs84(const QPointF &koreaPoint)
{
    HGISCoordinateReferenceSystem wgs84(4326);
    HGISCoordinateReferenceSystem korea2000 = HGISCoordinateReferenceSystem::korea2000Central();
    HGISCoordinateTransform transform(korea2000, wgs84);
    return transform.transform(koreaPoint);
}

QPointF HGISCoordinateTransform::wgs84ToKoreaBessel1987Central(const QPointF &wgs84Point)
{
    HGISCoordinateReferenceSystem wgs84(4326);
    HGISCoordinateReferenceSystem koreaBessel = HGISCoordinateReferenceSystem::koreaBessel1987Central();
    HGISCoordinateTransform transform(wgs84, koreaBessel);
    return transform.transform(wgs84Point);
}

QPointF HGISCoordinateTransform::koreaBessel1987CentralToWgs84(const QPointF &koreaPoint)
{
    HGISCoordinateReferenceSystem wgs84(4326);
    HGISCoordinateReferenceSystem koreaBessel = HGISCoordinateReferenceSystem::koreaBessel1987Central();
    HGISCoordinateTransform transform(koreaBessel, wgs84);
    return transform.transform(koreaPoint);
}

QString HGISCoordinateTransform::lastError() const
{
    return d->lastError;
}