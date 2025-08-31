// HGISVectorLayer 클래스 바인딩

class HGISVectorLayer : HGISMapLayer
{
%TypeHeaderCode
#include "HGISVectorLayer.h"
%End

public:
    // 생성자
    HGISVectorLayer(const QString &path = QString(), 
                   const QString &name = QString(), 
                   const QString &providerKey = "gdal");
    virtual ~HGISVectorLayer();
    
    // 파일 로드
    bool loadFromFile(const QString &path);
    
    // 지오메트리 타입
    QString geometryTypeAsString() const;
    
    // 피처 관련
    long featureCount() const;
    QStringList fields() const;
    
    // 범위
    virtual QRectF extent() const;
    
    // 라벨 설정
    bool labelsEnabled() const;
    void setLabelsEnabled(bool enabled);
    QString labelField() const;
    void setLabelField(const QString &fieldName);
    QFont labelFont() const;
    void setLabelFont(const QFont &font);
    QColor labelColor() const;
    void setLabelColor(const QColor &color);
    
    // 선택
    void selectFeature(long id);
    void deselectFeature(long id);
    void clearSelection();
    bool isFeatureSelected(long id) const;
    
    // 복제
    virtual HGISMapLayer* clone() const /Factory/;
    
    // 유효성
    virtual bool isValid() const;
    
signals:
    void symbolChanged();
    void labelsChanged();
    void selectionChanged(const QSet<long> &selectedIds);
    
    // Python 편의 메서드
%MethodCode
    // Python에서 쉽게 사용할 수 있는 추가 메서드
%End
};