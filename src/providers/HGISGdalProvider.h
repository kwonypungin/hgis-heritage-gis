#ifndef HGISGDALPROVIDER_H
#define HGISGDALPROVIDER_H

#include <QString>
#include <QStringList>
#include <QRectF>
#include <QVariant>
#include <memory>
#include <vector>

// Forward declarations for GDAL
typedef void *GDALDatasetH;
typedef void *OGRLayerH;
typedef void *OGRFeatureH;
typedef void *OGRGeometryH;

/**
 * GDAL/OGR 데이터 제공자
 * Shapefile, GeoPackage, GeoJSON 등 다양한 벡터 포맷 지원
 */
class HGISGdalProvider
{
public:
    /**
     * 생성자
     * @param uri 데이터 소스 경로
     */
    explicit HGISGdalProvider(const QString &uri);
    
    /**
     * 소멸자
     */
    ~HGISGdalProvider();
    
    /**
     * 데이터 소스 열기
     * @return 성공 여부
     */
    bool open();
    
    /**
     * 데이터 소스 닫기
     */
    void close();
    
    /**
     * 유효성 검사
     * @return 데이터 소스가 유효한지 여부
     */
    bool isValid() const;
    
    /**
     * 레이어 이름 반환
     * @return 레이어 이름
     */
    QString layerName() const;
    
    /**
     * 피처 개수 반환
     * @return 피처 개수
     */
    long featureCount() const;
    
    /**
     * 필드 이름 목록 반환
     * @return 필드 이름 목록
     */
    QStringList fields() const;
    
    /**
     * 지오메트리 타입 반환
     * @return 지오메트리 타입 문자열 (Point, LineString, Polygon 등)
     */
    QString geometryType() const;
    
    /**
     * 범위 반환
     * @return 데이터의 공간 범위
     */
    QRectF extent() const;
    
    /**
     * 좌표계 반환 (WKT 형식)
     * @return 좌표계 WKT 문자열
     */
    QString crs() const;
    
    /**
     * EPSG 코드 반환
     * @return EPSG 코드 (예: 4326, 5179)
     */
    int epsgCode() const;
    
    /**
     * 피처 데이터 구조
     */
    struct Feature {
        long id;                           // 피처 ID
        QVariantMap attributes;            // 속성 데이터
        std::vector<QPointF> geometry;     // 지오메트리 좌표
        QString geometryType;              // 지오메트리 타입
    };
    
    /**
     * 모든 피처 읽기
     * @return 피처 목록
     */
    std::vector<Feature> readFeatures() const;
    
    /**
     * 특정 범위의 피처 읽기
     * @param bounds 공간 범위
     * @return 피처 목록
     */
    std::vector<Feature> readFeatures(const QRectF &bounds) const;
    
    /**
     * 에러 메시지 반환
     * @return 마지막 에러 메시지
     */
    QString errorMessage() const;
    
    /**
     * 지원 포맷 목록 반환 (정적 메서드)
     * @return 지원되는 파일 확장자 목록
     */
    static QStringList supportedFormats();
    
    /**
     * 파일이 지원되는 포맷인지 확인 (정적 메서드)
     * @param filePath 파일 경로
     * @return 지원 여부
     */
    static bool isSupported(const QString &filePath);

private:
    class Private;
    std::unique_ptr<Private> d;
    
    // 복사 방지
    HGISGdalProvider(const HGISGdalProvider &) = delete;
    HGISGdalProvider &operator=(const HGISGdalProvider &) = delete;
};

#endif // HGISGDALPROVIDER_H