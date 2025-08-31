#include "HGISCoordinateReferenceSystem.h"
#include <QDebug>
#include <proj.h>
#include <ogr_srs_api.h>

class HGISCoordinateReferenceSystem::Private
{
public:
    PJ *pj = nullptr;
    int epsg = 0;
    QString wkt;
    QString proj;
    QString description;
    bool valid = false;
    
    Private() = default;
    
    ~Private()
    {
        if (pj) {
            proj_destroy(pj);
            pj = nullptr;
        }
    }
    
    void clear()
    {
        if (pj) {
            proj_destroy(pj);
            pj = nullptr;
        }
        epsg = 0;
        wkt.clear();
        proj.clear();
        description.clear();
        valid = false;
    }
    
    bool createFromEpsg(int epsgCode)
    {
        clear();
        
        QString authString = QString("EPSG:%1").arg(epsgCode);
        PJ_CONTEXT *ctx = proj_context_create();
        pj = proj_create(ctx, authString.toUtf8().constData());
        
        if (!pj) {
            proj_context_destroy(ctx);
            return false;
        }
        
        epsg = epsgCode;
        
        // WKT 추출
        const char *wktStr = proj_as_wkt(ctx, pj, PJ_WKT2_2019, nullptr);
        if (wktStr) {
            wkt = QString::fromUtf8(wktStr);
        }
        
        // PROJ 문자열 추출
        const char *projStr = proj_as_proj_string(ctx, pj, PJ_PROJ_5, nullptr);
        if (projStr) {
            proj = QString::fromUtf8(projStr);
        }
        
        // 설명 추출
        PJ *crs = proj_get_source_crs(ctx, pj);
        if (!crs) {
            crs = pj;
        }
        
        const char *name = proj_get_name(crs);
        if (name) {
            description = QString::fromUtf8(name);
        }
        
        if (crs != pj) {
            proj_destroy(crs);
        }
        
        proj_context_destroy(ctx);
        valid = true;
        return true;
    }
    
    bool createFromWkt(const QString &wktString)
    {
        clear();
        
        PJ_CONTEXT *ctx = proj_context_create();
        pj = proj_create_from_wkt(ctx, wktString.toUtf8().constData(), nullptr, nullptr, nullptr);
        
        if (!pj) {
            proj_context_destroy(ctx);
            return false;
        }
        
        wkt = wktString;
        
        // EPSG 코드 추출 시도
        const char *authName = proj_get_id_auth_name(pj, 0);
        const char *authCode = proj_get_id_code(pj, 0);
        if (authName && authCode && QString::fromUtf8(authName) == "EPSG") {
            epsg = QString::fromUtf8(authCode).toInt();
        }
        
        // PROJ 문자열 추출
        const char *projStr = proj_as_proj_string(ctx, pj, PJ_PROJ_5, nullptr);
        if (projStr) {
            proj = QString::fromUtf8(projStr);
        }
        
        // 설명 추출
        const char *name = proj_get_name(pj);
        if (name) {
            description = QString::fromUtf8(name);
        }
        
        proj_context_destroy(ctx);
        valid = true;
        return true;
    }
    
    bool createFromProj(const QString &projString)
    {
        clear();
        
        PJ_CONTEXT *ctx = proj_context_create();
        pj = proj_create(ctx, projString.toUtf8().constData());
        
        if (!pj) {
            proj_context_destroy(ctx);
            return false;
        }
        
        proj = projString;
        
        // WKT 추출
        const char *wktStr = proj_as_wkt(ctx, pj, PJ_WKT2_2019, nullptr);
        if (wktStr) {
            wkt = QString::fromUtf8(wktStr);
        }
        
        // EPSG 코드 추출 시도
        const char *authName = proj_get_id_auth_name(pj, 0);
        const char *authCode = proj_get_id_code(pj, 0);
        if (authName && authCode && QString::fromUtf8(authName) == "EPSG") {
            epsg = QString::fromUtf8(authCode).toInt();
        }
        
        // 설명 추출
        const char *name = proj_get_name(pj);
        if (name) {
            description = QString::fromUtf8(name);
        }
        
        proj_context_destroy(ctx);
        valid = true;
        return true;
    }
};

HGISCoordinateReferenceSystem::HGISCoordinateReferenceSystem()
    : d(std::make_unique<Private>())
{
}

HGISCoordinateReferenceSystem::HGISCoordinateReferenceSystem(int epsgCode)
    : d(std::make_unique<Private>())
{
    createFromEpsg(epsgCode);
}

HGISCoordinateReferenceSystem::HGISCoordinateReferenceSystem(const QString &wkt)
    : d(std::make_unique<Private>())
{
    createFromWkt(wkt);
}

HGISCoordinateReferenceSystem::~HGISCoordinateReferenceSystem() = default;

HGISCoordinateReferenceSystem::HGISCoordinateReferenceSystem(const HGISCoordinateReferenceSystem &other)
    : d(std::make_unique<Private>())
{
    if (other.d->valid) {
        if (other.d->epsg > 0) {
            d->createFromEpsg(other.d->epsg);
        } else if (!other.d->wkt.isEmpty()) {
            d->createFromWkt(other.d->wkt);
        } else if (!other.d->proj.isEmpty()) {
            d->createFromProj(other.d->proj);
        }
    }
}

HGISCoordinateReferenceSystem& HGISCoordinateReferenceSystem::operator=(const HGISCoordinateReferenceSystem &other)
{
    if (this != &other) {
        d->clear();
        if (other.d->valid) {
            if (other.d->epsg > 0) {
                d->createFromEpsg(other.d->epsg);
            } else if (!other.d->wkt.isEmpty()) {
                d->createFromWkt(other.d->wkt);
            } else if (!other.d->proj.isEmpty()) {
                d->createFromProj(other.d->proj);
            }
        }
    }
    return *this;
}

HGISCoordinateReferenceSystem::HGISCoordinateReferenceSystem(HGISCoordinateReferenceSystem &&other) noexcept = default;
HGISCoordinateReferenceSystem& HGISCoordinateReferenceSystem::operator=(HGISCoordinateReferenceSystem &&other) noexcept = default;

bool HGISCoordinateReferenceSystem::createFromEpsg(int epsgCode)
{
    bool result = d->createFromEpsg(epsgCode);
    if (result) {
        qInfo() << "CRS 생성 성공 - EPSG:" << epsgCode << "(" << d->description << ")";
    } else {
        qWarning() << "CRS 생성 실패 - EPSG:" << epsgCode;
    }
    return result;
}

bool HGISCoordinateReferenceSystem::createFromWkt(const QString &wkt)
{
    return d->createFromWkt(wkt);
}

bool HGISCoordinateReferenceSystem::createFromProj(const QString &proj)
{
    return d->createFromProj(proj);
}

// 한국 좌표계 정적 메서드들
HGISCoordinateReferenceSystem HGISCoordinateReferenceSystem::korea2000Central()
{
    // Korea 2000 / Central Belt - 중부원점 (EPSG:5186)
    // 사용지역: 경기도, 충청남도, 전라북도, 전라남도
    HGISCoordinateReferenceSystem crs(5186);
    qInfo() << "Korea 2000 / Central Belt 중부원점 (EPSG:5186) CRS 생성";
    return crs;
}

HGISCoordinateReferenceSystem HGISCoordinateReferenceSystem::korea2000West()
{
    // Korea 2000 / West Belt - 서부원점 (EPSG:5185)
    // 사용지역: 서해안 및 서부 지역
    HGISCoordinateReferenceSystem crs(5185);
    qInfo() << "Korea 2000 / West Belt 서부원점 (EPSG:5185) CRS 생성";
    return crs;
}

HGISCoordinateReferenceSystem HGISCoordinateReferenceSystem::korea2000East()
{
    // Korea 2000 / East Belt - 동부원점 (EPSG:5187)
    // 사용지역: 강원도, 경상북도, 경상남도, 부산
    HGISCoordinateReferenceSystem crs(5187);
    qInfo() << "Korea 2000 / East Belt 동부원점 (EPSG:5187) CRS 생성";
    return crs;
}

HGISCoordinateReferenceSystem HGISCoordinateReferenceSystem::korea2000EastSea()
{
    // Korea 2000 / East Sea Belt - 동해(울릉)원점 (EPSG:5188)
    // 사용지역: 동해 및 울릉도 지역
    HGISCoordinateReferenceSystem crs(5188);
    qInfo() << "Korea 2000 / East Sea Belt 동해(울릉)원점 (EPSG:5188) CRS 생성";
    return crs;
}

HGISCoordinateReferenceSystem HGISCoordinateReferenceSystem::koreaBessel1987Central()
{
    HGISCoordinateReferenceSystem crs(5174);
    qInfo() << "Korean 1985 중부원점 (EPSG:5174) CRS 생성";
    return crs;
}

HGISCoordinateReferenceSystem HGISCoordinateReferenceSystem::koreaBessel1987West()
{
    HGISCoordinateReferenceSystem crs(5175);
    qInfo() << "Korean 1985 서부원점 (EPSG:5175) CRS 생성";
    return crs;
}

HGISCoordinateReferenceSystem HGISCoordinateReferenceSystem::koreaBessel1987East()
{
    HGISCoordinateReferenceSystem crs(5176);
    qInfo() << "Korean 1985 동부원점 (EPSG:5176) CRS 생성";
    return crs;
}

HGISCoordinateReferenceSystem HGISCoordinateReferenceSystem::koreaUTMK()
{
    // Korea 2000 / Unified CS (UTM-K) - 통일원점 (EPSG:5179)
    // 네이버 지도 등에서 사용
    HGISCoordinateReferenceSystem crs(5179);
    qInfo() << "Korea 2000 / Unified CS (UTM-K) 통일원점 (EPSG:5179) CRS 생성";
    return crs;
}

HGISCoordinateReferenceSystem HGISCoordinateReferenceSystem::wgs84()
{
    HGISCoordinateReferenceSystem crs(4326);
    return crs;
}

bool HGISCoordinateReferenceSystem::isValid() const
{
    return d->valid;
}

int HGISCoordinateReferenceSystem::epsgCode() const
{
    return d->epsg;
}

QString HGISCoordinateReferenceSystem::authName() const
{
    if (!d->pj) return QString();
    
    const char *auth = proj_get_id_auth_name(d->pj, 0);
    return auth ? QString::fromUtf8(auth) : QString();
}

QString HGISCoordinateReferenceSystem::description() const
{
    return d->description;
}

QString HGISCoordinateReferenceSystem::toWkt() const
{
    return d->wkt;
}

QString HGISCoordinateReferenceSystem::toProj() const
{
    return d->proj;
}

bool HGISCoordinateReferenceSystem::isGeographic() const
{
    if (!d->pj) return false;
    
    PJ_CONTEXT *ctx = proj_context_create();
    PJ_TYPE type = proj_get_type(d->pj);
    proj_context_destroy(ctx);
    
    return type == PJ_TYPE_GEOGRAPHIC_2D_CRS || type == PJ_TYPE_GEOGRAPHIC_3D_CRS;
}

bool HGISCoordinateReferenceSystem::isProjected() const
{
    if (!d->pj) return false;
    
    PJ_CONTEXT *ctx = proj_context_create();
    PJ_TYPE type = proj_get_type(d->pj);
    proj_context_destroy(ctx);
    
    return type == PJ_TYPE_PROJECTED_CRS;
}

QString HGISCoordinateReferenceSystem::mapUnits() const
{
    if (!d->pj) return QString();
    
    if (isGeographic()) {
        return "degrees";
    } else if (isProjected()) {
        return "meters";
    }
    
    return QString();
}

double HGISCoordinateReferenceSystem::metersPerUnit() const
{
    if (!d->pj) return 1.0;
    
    if (isGeographic()) {
        return 111319.490793; // 적도에서의 1도당 대략적인 미터
    }
    
    return 1.0; // 투영 좌표계는 기본적으로 미터 단위
}

bool HGISCoordinateReferenceSystem::operator==(const HGISCoordinateReferenceSystem &other) const
{
    if (!d->valid || !other.d->valid) {
        return d->valid == other.d->valid;
    }
    
    // EPSG 코드가 같으면 같은 CRS
    if (d->epsg > 0 && other.d->epsg > 0) {
        return d->epsg == other.d->epsg;
    }
    
    // WKT 비교
    return d->wkt == other.d->wkt;
}

bool HGISCoordinateReferenceSystem::operator!=(const HGISCoordinateReferenceSystem &other) const
{
    return !(*this == other);
}