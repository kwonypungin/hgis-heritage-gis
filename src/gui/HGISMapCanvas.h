#ifndef HGISMAPCANVAS_H
#define HGISMAPCANVAS_H

#include <QGraphicsView>
#include <QPointF>
#include <memory>
#include "HGISCoordinateReferenceSystem.h"

class HGISLayerManager;
class HGISMapLayer;
class HGISMapTool;
class QRubberBand;

class HGISMapCanvas : public QGraphicsView
{
    Q_OBJECT

public:
    explicit HGISMapCanvas(QWidget *parent = nullptr);
    ~HGISMapCanvas();
    
    // 레이어 관리
    HGISLayerManager* layerManager() const;
    void setLayerManager(HGISLayerManager *manager);
    
    // 좌표계
    HGISCoordinateReferenceSystem crs() const;
    void setCrs(const HGISCoordinateReferenceSystem &crs);
    
    // 범위 설정
    QRectF extent() const;
    void setExtent(const QRectF &extent);
    void zoomToFullExtent();
    
    // 줌 컨트롤
    void zoomIn();
    void zoomOut();
    void zoomToScale(double scale);
    double scale() const;
    
    // 이동
    void panToCenter(const QPointF &center);
    QPointF center() const;
    
    // 좌표 변환
    QPointF toMapCoordinates(const QPoint &point) const;
    QPoint toCanvasCoordinates(const QPointF &point) const;
    
    // 새로고침
    void refresh();
    void refreshMap();
    
    // 맵 도구
    void setMapTool(HGISMapTool *tool);
    HGISMapTool* mapTool() const;
    
signals:
    void extentChanged(const QRectF &extent);
    void scaleChanged(double scale);
    void crsChanged();
    void xyCoordinates(const QPointF &point);
    void renderStarting();
    void renderComplete();
    
protected:
    // 이벤트 핸들러
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    
private:
    void updateTransform();
    void renderLayers();
    
private:
    class Private;
    std::unique_ptr<Private> d;
};

#endif // HGISMAPCANVAS_H