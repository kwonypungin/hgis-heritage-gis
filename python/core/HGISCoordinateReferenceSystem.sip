// HGISCoordinateReferenceSystem 클래스 바인딩

class HGISCoordinateReferenceSystem
{
%TypeHeaderCode
#include "HGISCoordinateReferenceSystem.h"
%End

public:
    // 생성자
    HGISCoordinateReferenceSystem();
    HGISCoordinateReferenceSystem(int epsgCode);
    HGISCoordinateReferenceSystem(const QString &wkt);
    HGISCoordinateReferenceSystem(const HGISCoordinateReferenceSystem &other);
    ~HGISCoordinateReferenceSystem();
    
    // EPSG 코드
    int epsgCode() const;
    void setEpsgCode(int code);
    
    // WKT
    QString toWkt() const;
    bool fromWkt(const QString &wkt);
    
    // Proj 문자열
    QString toProj() const;
    bool fromProj(const QString &proj);
    
    // 유효성 검사
    bool isValid() const;
    bool isGeographic() const;
    
    // 설명
    QString description() const;
    QString authId() const;
    
    // 비교 연산자
    bool operator==(const HGISCoordinateReferenceSystem &other) const;
    bool operator!=(const HGISCoordinateReferenceSystem &other) const;
    
    // 자주 사용하는 CRS
    static HGISCoordinateReferenceSystem wgs84();
    static HGISCoordinateReferenceSystem webMercator();
    static HGISCoordinateReferenceSystem korea2000Central();
    static HGISCoordinateReferenceSystem korea2000West();
    static HGISCoordinateReferenceSystem korea2000East();
    static HGISCoordinateReferenceSystem korea2000EastSea();
    static HGISCoordinateReferenceSystem koreaUTMK();
    
    // 문자열 표현
    QString __str__() const;
%MethodCode
    sipRes = new QString(QString("CRS(EPSG:%1)").arg(sipCpp->epsgCode()));
%End
};