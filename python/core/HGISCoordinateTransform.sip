// HGISCoordinateTransform 클래스 바인딩

class HGISCoordinateTransform
{
%TypeHeaderCode
#include "HGISCoordinateTransform.h"
%End

public:
    // 생성자
    HGISCoordinateTransform();
    HGISCoordinateTransform(const HGISCoordinateReferenceSystem &source, 
                           const HGISCoordinateReferenceSystem &dest);
    HGISCoordinateTransform(const HGISCoordinateTransform &other);
    ~HGISCoordinateTransform();
    
    // CRS 설정
    void setSourceCrs(const HGISCoordinateReferenceSystem &crs);
    void setDestinationCrs(const HGISCoordinateReferenceSystem &crs);
    HGISCoordinateReferenceSystem sourceCrs() const;
    HGISCoordinateReferenceSystem destinationCrs() const;
    
    // 유효성
    bool isValid() const;
    bool isIdentity() const;
    
    // 좌표 변환
    QPointF transform(const QPointF &point) const;
    QPolygonF transform(const QPolygonF &polygon) const;
    QRectF transformBoundingBox(const QRectF &rect) const;
    
    // 역변환
    QPointF transformReverse(const QPointF &point) const;
    QPolygonF transformReverse(const QPolygonF &polygon) const;
    
    // Python 편의 메서드
    SIP_PYTUPLE transformPoint(double x, double y) const;
%MethodCode
    QPointF result = sipCpp->transform(QPointF(a0, a1));
    sipRes = PyTuple_New(2);
    PyTuple_SET_ITEM(sipRes, 0, PyFloat_FromDouble(result.x()));
    PyTuple_SET_ITEM(sipRes, 1, PyFloat_FromDouble(result.y()));
%End
};