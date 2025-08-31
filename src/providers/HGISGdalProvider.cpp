#include "HGISGdalProvider.h"
#include <QDebug>
#include <QFileInfo>
#include <gdal.h>
#include <ogr_api.h>
#include <ogr_srs_api.h>
#include <cpl_conv.h>
#include <cpl_string.h>

class HGISGdalProvider::Private
{
public:
    QString uri;
    QString errorMessage;
    GDALDatasetH dataset = nullptr;
    OGRLayerH layer = nullptr;
    bool isValid = false;
    
    // 캐시된 메타데이터
    QString layerName;
    long featureCount = 0;
    QStringList fieldNames;
    QString geomType;
    QRectF extent;
    QString crsWkt;
    int epsgCode = 0;
    
    Private(const QString &dataUri) : uri(dataUri)
    {
        // GDAL 초기화 (한 번만 수행)
        static bool initialized = false;
        if (!initialized) {
            GDALAllRegister();
            OGRRegisterAll();
            CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "YES");
            CPLSetConfigOption("SHAPE_ENCODING", "UTF-8");
            initialized = true;
            qInfo() << "GDAL/OGR 초기화 완료";
        }
    }
    
    ~Private()
    {
        close();
    }
    
    void close()
    {
        if (dataset) {
            GDALClose(dataset);
            dataset = nullptr;
            layer = nullptr;
            isValid = false;
        }
    }
    
    bool extractMetadata()
    {
        if (!layer) return false;
        
        // 레이어 이름
        layerName = QString::fromUtf8(OGR_L_GetName(layer));
        
        // 피처 개수
        featureCount = OGR_L_GetFeatureCount(layer, TRUE);
        
        // 필드 정보
        OGRFeatureDefnH featureDefn = OGR_L_GetLayerDefn(layer);
        int fieldCount = OGR_FD_GetFieldCount(featureDefn);
        for (int i = 0; i < fieldCount; i++) {
            OGRFieldDefnH fieldDefn = OGR_FD_GetFieldDefn(featureDefn, i);
            fieldNames.append(QString::fromUtf8(OGR_Fld_GetNameRef(fieldDefn)));
        }
        
        // 지오메트리 타입
        OGRwkbGeometryType geomTypeEnum = OGR_FD_GetGeomType(featureDefn);
        switch (wkbFlatten(geomTypeEnum)) {
            case wkbPoint:
                geomType = "Point";
                break;
            case wkbLineString:
                geomType = "LineString";
                break;
            case wkbPolygon:
                geomType = "Polygon";
                break;
            case wkbMultiPoint:
                geomType = "MultiPoint";
                break;
            case wkbMultiLineString:
                geomType = "MultiLineString";
                break;
            case wkbMultiPolygon:
                geomType = "MultiPolygon";
                break;
            default:
                geomType = "Unknown";
        }
        
        // 범위
        OGREnvelope envelope;
        if (OGR_L_GetExtent(layer, &envelope, TRUE) == OGRERR_NONE) {
            extent = QRectF(envelope.MinX, envelope.MinY, 
                          envelope.MaxX - envelope.MinX,
                          envelope.MaxY - envelope.MinY);
        }
        
        // 좌표계
        OGRSpatialReferenceH srs = OGR_L_GetSpatialRef(layer);
        if (srs) {
            char *wkt = nullptr;
            OSRExportToWkt(srs, &wkt);
            if (wkt) {
                crsWkt = QString::fromUtf8(wkt);
                CPLFree(wkt);
            }
            
            // EPSG 코드 추출
            const char *auth = OSRGetAuthorityName(srs, nullptr);
            const char *code = OSRGetAuthorityCode(srs, nullptr);
            if (auth && code && strcmp(auth, "EPSG") == 0) {
                epsgCode = QString::fromUtf8(code).toInt();
            }
        }
        
        return true;
    }
};

HGISGdalProvider::HGISGdalProvider(const QString &uri)
    : d(std::make_unique<Private>(uri))
{
}

HGISGdalProvider::~HGISGdalProvider() = default;

bool HGISGdalProvider::open()
{
    if (d->isValid) {
        return true;
    }
    
    // UTF-8 인코딩 보장
    QByteArray utf8Path = d->uri.toUtf8();
    
    // 데이터셋 열기 (읽기 전용)
    d->dataset = GDALOpenEx(utf8Path.constData(), 
                            GDAL_OF_VECTOR | GDAL_OF_READONLY,
                            nullptr, nullptr, nullptr);
    
    if (!d->dataset) {
        d->errorMessage = QString("데이터 소스를 열 수 없습니다: %1").arg(d->uri);
        qWarning() << d->errorMessage;
        return false;
    }
    
    // 첫 번째 레이어 가져오기
    int layerCount = GDALDatasetGetLayerCount(d->dataset);
    if (layerCount == 0) {
        d->errorMessage = "레이어가 없습니다";
        qWarning() << d->errorMessage;
        d->close();
        return false;
    }
    
    d->layer = GDALDatasetGetLayer(d->dataset, 0);
    if (!d->layer) {
        d->errorMessage = "레이어를 가져올 수 없습니다";
        qWarning() << d->errorMessage;
        d->close();
        return false;
    }
    
    // 메타데이터 추출
    if (!d->extractMetadata()) {
        d->errorMessage = "메타데이터를 추출할 수 없습니다";
        qWarning() << d->errorMessage;
        d->close();
        return false;
    }
    
    d->isValid = true;
    qInfo() << "GDAL Provider 열기 성공:" << d->uri;
    qInfo() << "  레이어:" << d->layerName;
    qInfo() << "  피처 수:" << d->featureCount;
    qInfo() << "  지오메트리 타입:" << d->geomType;
    qInfo() << "  EPSG:" << d->epsgCode;
    
    return true;
}

void HGISGdalProvider::close()
{
    d->close();
}

bool HGISGdalProvider::isValid() const
{
    return d->isValid;
}

QString HGISGdalProvider::layerName() const
{
    return d->layerName;
}

long HGISGdalProvider::featureCount() const
{
    return d->featureCount;
}

QStringList HGISGdalProvider::fields() const
{
    return d->fieldNames;
}

QString HGISGdalProvider::geometryType() const
{
    return d->geomType;
}

QRectF HGISGdalProvider::extent() const
{
    return d->extent;
}

QString HGISGdalProvider::crs() const
{
    return d->crsWkt;
}

int HGISGdalProvider::epsgCode() const
{
    return d->epsgCode;
}

std::vector<HGISGdalProvider::Feature> HGISGdalProvider::readFeatures() const
{
    std::vector<Feature> features;
    
    if (!d->isValid || !d->layer) {
        return features;
    }
    
    // 레이어 리셋
    OGR_L_ResetReading(d->layer);
    
    OGRFeatureH feature;
    while ((feature = OGR_L_GetNextFeature(d->layer)) != nullptr) {
        Feature f;
        f.id = OGR_F_GetFID(feature);
        f.geometryType = d->geomType;
        
        // 속성 읽기
        OGRFeatureDefnH featureDefn = OGR_L_GetLayerDefn(d->layer);
        int fieldCount = OGR_FD_GetFieldCount(featureDefn);
        
        for (int i = 0; i < fieldCount; i++) {
            OGRFieldDefnH fieldDefn = OGR_FD_GetFieldDefn(featureDefn, i);
            QString fieldName = QString::fromUtf8(OGR_Fld_GetNameRef(fieldDefn));
            
            if (OGR_F_IsFieldSet(feature, i)) {
                OGRFieldType fieldType = OGR_Fld_GetType(fieldDefn);
                QVariant value;
                
                switch (fieldType) {
                    case OFTInteger:
                        value = OGR_F_GetFieldAsInteger(feature, i);
                        break;
                    case OFTInteger64:
                        value = static_cast<qint64>(OGR_F_GetFieldAsInteger64(feature, i));
                        break;
                    case OFTReal:
                        value = OGR_F_GetFieldAsDouble(feature, i);
                        break;
                    case OFTString:
                        value = QString::fromUtf8(OGR_F_GetFieldAsString(feature, i));
                        break;
                    default:
                        value = QString::fromUtf8(OGR_F_GetFieldAsString(feature, i));
                }
                
                f.attributes[fieldName] = value;
            }
        }
        
        // 지오메트리 읽기
        OGRGeometryH geometry = OGR_F_GetGeometryRef(feature);
        if (geometry) {
            OGRwkbGeometryType geomType = OGR_G_GetGeometryType(geometry);
            
            if (wkbFlatten(geomType) == wkbPoint) {
                double x = OGR_G_GetX(geometry, 0);
                double y = OGR_G_GetY(geometry, 0);
                f.geometry.push_back(QPointF(x, y));
            } else if (wkbFlatten(geomType) == wkbLineString || 
                      wkbFlatten(geomType) == wkbPolygon) {
                OGRGeometryH ring = geometry;
                if (wkbFlatten(geomType) == wkbPolygon) {
                    ring = OGR_G_GetGeometryRef(geometry, 0); // 외부 링만
                }
                
                int pointCount = OGR_G_GetPointCount(ring);
                for (int i = 0; i < pointCount; i++) {
                    double x = OGR_G_GetX(ring, i);
                    double y = OGR_G_GetY(ring, i);
                    f.geometry.push_back(QPointF(x, y));
                }
            }
        }
        
        features.push_back(f);
        OGR_F_Destroy(feature);
    }
    
    return features;
}

std::vector<HGISGdalProvider::Feature> HGISGdalProvider::readFeatures(const QRectF &bounds) const
{
    std::vector<Feature> features;
    
    if (!d->isValid || !d->layer) {
        return features;
    }
    
    // 공간 필터 설정
    OGR_L_SetSpatialFilterRect(d->layer, 
                              bounds.left(), bounds.top(),
                              bounds.right(), bounds.bottom());
    
    features = readFeatures();
    
    // 필터 제거
    OGR_L_SetSpatialFilter(d->layer, nullptr);
    
    return features;
}

QString HGISGdalProvider::errorMessage() const
{
    return d->errorMessage;
}

QStringList HGISGdalProvider::supportedFormats()
{
    return QStringList() << "*.shp" << "*.gpkg" << "*.geojson" << "*.json" 
                        << "*.kml" << "*.gml" << "*.sqlite" << "*.tab";
}

bool HGISGdalProvider::isSupported(const QString &filePath)
{
    QFileInfo fi(filePath);
    QString ext = fi.suffix().toLower();
    
    QStringList supportedExts = {"shp", "gpkg", "geojson", "json", 
                                 "kml", "gml", "sqlite", "tab"};
    
    return supportedExts.contains(ext);
}