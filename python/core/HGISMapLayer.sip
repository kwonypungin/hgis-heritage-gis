// HGISMapLayer 추상 클래스 바인딩

class HGISMapLayer : QObject
{
%TypeHeaderCode
#include "HGISMapLayer.h"
%End

public:
    enum MapLayerType {
        VectorLayer,
        RasterLayer,
        PluginLayer,
        MeshLayer,
        VectorTileLayer,
        AnnotationLayer,
        PointCloudLayer,
        GroupLayer
    };
    
    // 생성자는 추상 클래스이므로 제외
    virtual ~HGISMapLayer();
    
    // 레이어 ID
    QString id() const;
    void setId(const QString &id);
    
    // 레이어 이름
    QString name() const;
    void setName(const QString &name);
    
    // 레이어 타입
    HGISMapLayer::MapLayerType type() const;
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
    
    // 복제 (순수 가상 함수이므로 Python에서는 구현 불가)
    // virtual HGISMapLayer* clone() const = 0;
    
signals:
    void nameChanged();
    void crsChanged();
    void visibilityChanged(bool visible);
    void opacityChanged(int opacity);
    void extentChanged();
    void dataChanged();
    void repaintRequested();
};