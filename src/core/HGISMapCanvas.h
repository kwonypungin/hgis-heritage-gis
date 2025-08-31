#ifndef HGISMAPCANVAS_H
#define HGISMAPCANVAS_H

#include <QGraphicsView>
#include <QPointF>
#include <QRectF>
#include <memory>

#ifdef HGIS_CORE_EXPORT
  #define CORE_EXPORT Q_DECL_EXPORT
#else
  #define CORE_EXPORT Q_DECL_IMPORT
#endif

class QWheelEvent;
class QKeyEvent;
class QPaintEvent;
class QResizeEvent;
class QMouseEvent;
class QGraphicsScene;

/**
 * HGIS 지도 캔버스 - QGIS QgsMapCanvas 참조 구현
 * 지도 렌더링과 상호작용을 담당하는 핵심 위젯
 */
class CORE_EXPORT HGISMapCanvas : public QGraphicsView
{
    Q_OBJECT

public:
    explicit HGISMapCanvas(QWidget *parent = nullptr);
    virtual ~HGISMapCanvas();

    //! 현재 지도 범위 반환
    QRectF extent() const;
    
    //! 지도 범위 설정
    void setExtent(const QRectF &extent);
    
    //! 줌 인
    void zoomIn();
    
    //! 줌 아웃
    void zoomOut();
    
    //! 전체 범위로 줌
    void zoomToFullExtent();
    
    //! 지정된 배율로 줌
    void zoomWithCenter(int scaleFactor, const QPointF &center);
    
    //! 팬 모드 설정
    void panToCenter(const QPointF &center);
    
    //! 현재 축척 반환
    double scale() const;
    
    //! 축척 설정
    void setScale(double scale);
    
    //! 회전 각도 반환
    double rotation() const;
    
    //! 회전 각도 설정
    void setRotation(double degrees);
    
    //! 새로고침
    void refresh();
    
    //! 배경색 설정
    void setCanvasColor(const QColor &color);
    
    //! 배경색 반환
    QColor canvasColor() const;

signals:
    //! 지도 범위 변경 시그널
    void extentsChanged();
    
    //! 축척 변경 시그널
    void scaleChanged(double scale);
    
    //! 회전 변경 시그널
    void rotationChanged(double rotation);
    
    //! 렌더링 시작 시그널
    void renderStarting();
    
    //! 렌더링 완료 시그널
    void renderComplete();
    
    //! 마우스 좌표 변경 시그널
    void xyCoordinates(const QPointF &point);

protected:
    //! 마우스 휠 이벤트
    void wheelEvent(QWheelEvent *event) override;
    
    //! 마우스 누르기 이벤트
    void mousePressEvent(QMouseEvent *event) override;
    
    //! 마우스 이동 이벤트
    void mouseMoveEvent(QMouseEvent *event) override;
    
    //! 마우스 놓기 이벤트
    void mouseReleaseEvent(QMouseEvent *event) override;
    
    //! 마우스 더블클릭 이벤트
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    
    //! 키 누르기 이벤트
    void keyPressEvent(QKeyEvent *event) override;
    
    //! 키 놓기 이벤트
    void keyReleaseEvent(QKeyEvent *event) override;
    
    //! 페인트 이벤트
    void paintEvent(QPaintEvent *event) override;
    
    //! 리사이즈 이벤트
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateScale();
    void updateTransform();
    QPointF mapToScene(const QPoint &point) const;
    QPoint sceneToMap(const QPointF &scenePoint) const;
    
    class Private;
    std::unique_ptr<Private> d;
};

#endif // HGISMAPCANVAS_H