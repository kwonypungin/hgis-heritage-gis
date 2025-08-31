#ifndef HGISCOORDINATETRANSFORM_H
#define HGISCOORDINATETRANSFORM_H

#include "HGISCoordinateReferenceSystem.h"
#include <QPointF>
#include <QRectF>
#include <QPolygonF>
#include <memory>

class CORE_EXPORT HGISCoordinateTransform
{
public:
    HGISCoordinateTransform();
    HGISCoordinateTransform(const HGISCoordinateReferenceSystem &source,
                           const HGISCoordinateReferenceSystem &destination);
    ~HGISCoordinateTransform();
    
    // 복사 및 이동 생성자
    HGISCoordinateTransform(const HGISCoordinateTransform &other);
    HGISCoordinateTransform& operator=(const HGISCoordinateTransform &other);
    HGISCoordinateTransform(HGISCoordinateTransform &&other) noexcept;
    HGISCoordinateTransform& operator=(HGISCoordinateTransform &&other) noexcept;
    
    // 좌표계 설정
    void setSourceCrs(const HGISCoordinateReferenceSystem &crs);
    void setDestinationCrs(const HGISCoordinateReferenceSystem &crs);
    
    // 좌표계 가져오기
    HGISCoordinateReferenceSystem sourceCrs() const;
    HGISCoordinateReferenceSystem destinationCrs() const;
    
    // 변환 가능 여부
    bool isValid() const;
    bool isShortCircuitable() const; // 동일한 CRS인 경우
    
    // 좌표 변환 - 단일 포인트
    QPointF transform(const QPointF &point) const;
    QPointF transform(double x, double y) const;
    
    // 좌표 변환 - 여러 포인트
    QPolygonF transform(const QPolygonF &polygon) const;
    QRectF transformBoundingBox(const QRectF &rect) const;
    
    // 역변환
    QPointF transformReverse(const QPointF &point) const;
    QPointF transformReverse(double x, double y) const;
    
    // 한국 좌표계 변환 헬퍼 메서드
    static QPointF wgs84ToKorea2000Central(const QPointF &wgs84Point);
    static QPointF korea2000CentralToWgs84(const QPointF &koreaPoint);
    static QPointF wgs84ToKoreaBessel1987Central(const QPointF &wgs84Point);
    static QPointF koreaBessel1987CentralToWgs84(const QPointF &koreaPoint);
    
    // 오류 메시지
    QString lastError() const;
    
private:
    class Private;
    std::unique_ptr<Private> d;
    
    void initialize();
};

#endif // HGISCOORDINATETRANSFORM_H