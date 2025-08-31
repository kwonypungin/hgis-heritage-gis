#ifndef HGISMAPLAYER_H
#define HGISMAPLAYER_H

#include <QString>
#include <QObject>
#include <QRectF>
#include <memory>
#include "HGISCoordinateReferenceSystem.h"

class QPainter;

#ifdef HGIS_CORE_EXPORT
  #define CORE_EXPORT Q_DECL_EXPORT
#else
  #define CORE_EXPORT Q_DECL_IMPORT
#endif

// 레이어 타입 열거형
enum class HGISMapLayerType
{
    VectorLayer,
    RasterLayer,
    PluginLayer,
    MeshLayer,
    VectorTileLayer,
    AnnotationLayer,
    PointCloudLayer,
    GroupLayer
};

class CORE_EXPORT HGISMapLayer : public QObject
{
    Q_OBJECT

public:
    // 생성자
    HGISMapLayer(HGISMapLayerType type = HGISMapLayerType::VectorLayer,
                 const QString &name = QString(),
                 const QString &source = QString());
    virtual ~HGISMapLayer();
    
    // 레이어 ID
    QString id() const;
    void setId(const QString &id);
    
    // 레이어 이름
    QString name() const;
    void setName(const QString &name);
    
    // 레이어 타입
    HGISMapLayerType type() const;
    QString typeAsString() const;
    
    // 데이터 소스
    QString source() const;
    void setSource(const QString &source);
    
    // 좌표계
    HGISCoordinateReferenceSystem crs() const;
    void setCrs(const HGISCoordinateReferenceSystem &crs);
    
    // 가시성
    bool isVisible() const;
    void setVisible(bool visible);
    
    // 유효성
    virtual bool isValid() const;
    
    // 범위
    virtual QRectF extent() const = 0;
    
    // 투명도 (0-100)
    int opacity() const;
    void setOpacity(int opacity);
    
    // 최소/최대 축척
    double minimumScale() const;
    void setMinimumScale(double scale);
    double maximumScale() const;
    void setMaximumScale(double scale);
    bool isInScaleRange(double scale) const;
    
    // 메타데이터
    QString abstract() const;
    void setAbstract(const QString &abstract);
    QString attribution() const;
    void setAttribution(const QString &attrib);
    
    // 복제
    virtual HGISMapLayer* clone() const = 0;
    
    // 렌더링
    virtual void render(QPainter *painter, const QRectF &extent, double scale) = 0;
    
signals:
    // 레이어 변경 시그널
    void nameChanged();
    void crsChanged();
    void visibilityChanged(bool visible);
    void opacityChanged(int opacity);
    void extentChanged();
    void dataChanged();
    void repaintRequested();
    
protected:
    // 고유 ID 생성
    static QString generateId(const QString &name);
    
private:
    class Private;
    std::unique_ptr<Private> d;
};

#endif // HGISMAPLAYER_H