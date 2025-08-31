#ifndef HGISVECTORLAYER_H
#define HGISVECTORLAYER_H

#include "HGISMapLayer.h"
#include "providers/HGISGdalProvider.h"
#include <QColor>
#include <memory>
#include <vector>

class QPainter;
class QGraphicsItem;

// 지오메트리 타입
enum class HGISGeometryType
{
    Unknown,
    Point,
    LineString,
    Polygon,
    MultiPoint,
    MultiLineString,
    MultiPolygon
};

// 벡터 레이어 렌더러 타입
enum class HGISRendererType
{
    SingleSymbol,      // 단일 심볼
    Categorized,       // 분류 심볼
    Graduated,         // 단계 심볼
    RuleBased         // 규칙 기반
};

// 심볼 설정
struct HGISSymbol
{
    QColor fillColor = QColor(255, 255, 255, 100);
    QColor strokeColor = Qt::black;
    double strokeWidth = 1.0;
    Qt::PenStyle penStyle = Qt::SolidLine;
    Qt::BrushStyle brushStyle = Qt::SolidPattern;
    double pointSize = 3.0;
    
    // 포인트 심볼 타입
    enum PointSymbolType {
        Circle,
        Square,
        Triangle,
        Cross,
        Star
    };
    PointSymbolType pointSymbolType = Circle;
};

class CORE_EXPORT HGISVectorLayer : public HGISMapLayer
{
    Q_OBJECT

public:
    // 생성자
    explicit HGISVectorLayer(const QString &path = QString(),
                            const QString &name = QString(),
                            const QString &providerKey = "gdal");
    ~HGISVectorLayer() override;
    
    // 데이터 소스 열기
    bool loadFromFile(const QString &path);
    
    // 데이터 제공자
    HGISGdalProvider* dataProvider() const;
    
    // 지오메트리 타입
    HGISGeometryType geometryType() const;
    QString geometryTypeAsString() const;
    
    // 피처 수
    long featureCount() const;
    
    // 필드 정보
    QStringList fields() const;
    
    // 범위
    QRectF extent() const override;
    
    // 피처 가져오기
    std::vector<HGISGdalProvider::Feature> features() const;
    std::vector<HGISGdalProvider::Feature> features(const QRectF &extent) const;
    
    // 심볼 설정
    HGISSymbol symbol() const;
    void setSymbol(const HGISSymbol &symbol);
    
    // 렌더러 타입
    HGISRendererType rendererType() const;
    void setRendererType(HGISRendererType type);
    
    // 라벨 설정
    bool labelsEnabled() const;
    void setLabelsEnabled(bool enabled);
    QString labelField() const;
    void setLabelField(const QString &fieldName);
    QFont labelFont() const;
    void setLabelFont(const QFont &font);
    QColor labelColor() const;
    void setLabelColor(const QColor &color);
    
    // 선택된 피처
    QSet<long> selectedFeatureIds() const;
    void selectFeatures(const QSet<long> &ids);
    void selectFeature(long id);
    void deselectFeature(long id);
    void clearSelection();
    bool isFeatureSelected(long id) const;
    
    // 속성 테이블
    QVariant attributeValue(long featureId, const QString &fieldName) const;
    QMap<QString, QVariant> attributes(long featureId) const;
    
    // 렌더링
    void render(QPainter *painter, const QRectF &extent, double scale) override;
    
    // 복제
    HGISMapLayer* clone() const override;
    
    // 유효성
    bool isValid() const override;
    
    // 통계
    double minimumValue(const QString &fieldName) const;
    double maximumValue(const QString &fieldName) const;
    QVariant uniqueValues(const QString &fieldName) const;
    
signals:
    void selectionChanged(const QSet<long> &selectedIds);
    void symbolChanged();
    void labelsChanged();
    
private:
    void renderFeatures(QPainter *painter, const QRectF &extent, double scale);
    void renderLabels(QPainter *painter, const QRectF &extent, double scale);
    void drawPointSymbol(QPainter *painter, const QPointF &point, const HGISSymbol &symbol);
    void drawLineSymbol(QPainter *painter, const QVector<QPointF> &points, const HGISSymbol &symbol);
    void drawPolygonSymbol(QPainter *painter, const QVector<QPointF> &points, const HGISSymbol &symbol);
    
    class Private;
    std::unique_ptr<Private> d;
};

#endif // HGISVECTORLAYER_H