#ifndef HGISCOORDINATEREFERENCESYSTEM_H
#define HGISCOORDINATEREFERENCESYSTEM_H

#include <QString>
#include <QPointF>
#include <memory>

#ifdef HGIS_CORE_EXPORT
  #define CORE_EXPORT Q_DECL_EXPORT
#else
  #define CORE_EXPORT Q_DECL_IMPORT
#endif

class CORE_EXPORT HGISCoordinateReferenceSystem
{
public:
    HGISCoordinateReferenceSystem();
    explicit HGISCoordinateReferenceSystem(int epsgCode);
    explicit HGISCoordinateReferenceSystem(const QString &wkt);
    ~HGISCoordinateReferenceSystem();
    
    // 복사 및 이동 생성자
    HGISCoordinateReferenceSystem(const HGISCoordinateReferenceSystem &other);
    HGISCoordinateReferenceSystem& operator=(const HGISCoordinateReferenceSystem &other);
    HGISCoordinateReferenceSystem(HGISCoordinateReferenceSystem &&other) noexcept;
    HGISCoordinateReferenceSystem& operator=(HGISCoordinateReferenceSystem &&other) noexcept;
    
    // EPSG 코드로 CRS 생성
    bool createFromEpsg(int epsgCode);
    
    // WKT (Well-Known Text)로 CRS 생성
    bool createFromWkt(const QString &wkt);
    
    // PROJ 문자열로 CRS 생성
    bool createFromProj(const QString &proj);
    
    // 한국 좌표계 지원 - Korea 2000 TM (GRS80 타원체)
    static HGISCoordinateReferenceSystem korea2000Central();    // EPSG:5186 - Korea 2000 / Central Belt 중부원점
    static HGISCoordinateReferenceSystem korea2000West();       // EPSG:5185 - Korea 2000 / West Belt 서부원점
    static HGISCoordinateReferenceSystem korea2000East();       // EPSG:5187 - Korea 2000 / East Belt 동부원점
    static HGISCoordinateReferenceSystem korea2000EastSea();    // EPSG:5188 - Korea 2000 / East Sea Belt 동해(울릉)원점
    static HGISCoordinateReferenceSystem koreaUTMK();          // EPSG:5179 - Korea 2000 / Unified CS (UTM-K)
    
    // 한국 좌표계 지원 - Korean 1985 (Bessel 타원체) - 구 좌표계
    static HGISCoordinateReferenceSystem koreaBessel1987Central(); // EPSG:5174 - Korean 1985 중부원점
    static HGISCoordinateReferenceSystem koreaBessel1987West();    // EPSG:5175 - Korean 1985 서부원점
    static HGISCoordinateReferenceSystem koreaBessel1987East();    // EPSG:5176 - Korean 1985 동부원점
    
    // 전 세계 좌표계
    static HGISCoordinateReferenceSystem wgs84();               // EPSG:4326 - WGS 84
    
    // CRS 정보 가져오기
    bool isValid() const;
    int epsgCode() const;
    QString authName() const;
    QString description() const;
    QString toWkt() const;
    QString toProj() const;
    bool isGeographic() const;
    bool isProjected() const;
    
    // 단위 정보
    QString mapUnits() const;
    double metersPerUnit() const;
    
    // 비교 연산자
    bool operator==(const HGISCoordinateReferenceSystem &other) const;
    bool operator!=(const HGISCoordinateReferenceSystem &other) const;
    
private:
    class Private;
    std::unique_ptr<Private> d;
};

#endif // HGISCOORDINATEREFERENCESYSTEM_H