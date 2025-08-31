#include "HGISMapCanvas.h"
#include "HGISLayerManager.h"
#include "HGISMapLayer.h"
#include "HGISVectorLayer.h"
#include "HGISCoordinateTransform.h"
#include <QGraphicsScene>
#include <QPainter>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>
#include <QRubberBand>
#include <cmath>

class HGISMapCanvas::Private
{
public:
    HGISLayerManager *layerManager = nullptr;
    HGISCoordinateReferenceSystem crs;
    QGraphicsScene *scene = nullptr;
    HGISMapTool *mapTool = nullptr;
    
    QRectF mapExtent;
    double mapScale = 1.0;
    QPointF mapCenter;
    
    bool isDragging = false;
    QPoint lastMousePos;
    
    QTransform mapToCanvas;
    QTransform canvasToMap;
    
    Private()
    {
        scene = new QGraphicsScene();
        crs = HGISCoordinateReferenceSystem::wgs84();
    }
    
    ~Private()
    {
        delete scene;
    }
    
    void updateTransforms(const QSize &canvasSize)
    {
        if (mapExtent.isEmpty() || canvasSize.isEmpty()) {
            return;
        }
        
        double xScale = canvasSize.width() / mapExtent.width();
        double yScale = canvasSize.height() / mapExtent.height();
        mapScale = std::min(xScale, yScale);
        
        mapToCanvas = QTransform();
        mapToCanvas.translate(canvasSize.width() / 2.0, canvasSize.height() / 2.0);
        mapToCanvas.scale(mapScale, -mapScale);  // Y축 반전
        mapToCanvas.translate(-mapCenter.x(), -mapCenter.y());
        
        canvasToMap = mapToCanvas.inverted();
    }
};

HGISMapCanvas::HGISMapCanvas(QWidget *parent)
    : QGraphicsView(parent)
    , d(std::make_unique<Private>())
{
    setScene(d->scene);
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::NoDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    qDebug() << "HGISMapCanvas 생성됨";
}

HGISMapCanvas::~HGISMapCanvas() = default;

HGISLayerManager* HGISMapCanvas::layerManager() const
{
    return d->layerManager;
}

void HGISMapCanvas::setLayerManager(HGISLayerManager *manager)
{
    if (d->layerManager) {
        disconnect(d->layerManager, nullptr, this, nullptr);
    }
    
    d->layerManager = manager;
    
    if (d->layerManager) {
        connect(d->layerManager, &HGISLayerManager::layersChanged,
                this, &HGISMapCanvas::refresh);
        connect(d->layerManager, &HGISLayerManager::layerAdded,
                this, &HGISMapCanvas::refresh);
        connect(d->layerManager, &HGISLayerManager::layerRemoved,
                this, &HGISMapCanvas::refresh);
        
        d->crs = d->layerManager->projectCrs();
        zoomToFullExtent();
    }
}

HGISCoordinateReferenceSystem HGISMapCanvas::crs() const
{
    return d->crs;
}

void HGISMapCanvas::setCrs(const HGISCoordinateReferenceSystem &crs)
{
    if (d->crs != crs) {
        d->crs = crs;
        emit crsChanged();
        refresh();
    }
}

QRectF HGISMapCanvas::extent() const
{
    return d->mapExtent;
}

void HGISMapCanvas::setExtent(const QRectF &extent)
{
    if (extent.isEmpty()) {
        return;
    }
    
    d->mapExtent = extent;
    d->mapCenter = extent.center();
    updateTransform();
    
    emit extentChanged(extent);
    refresh();
}

void HGISMapCanvas::zoomToFullExtent()
{
    if (!d->layerManager) {
        return;
    }
    
    QRectF fullExtent = d->layerManager->fullExtent();
    if (!fullExtent.isEmpty()) {
        // 10% 여백 추가
        double margin = 0.1;
        double width = fullExtent.width();
        double height = fullExtent.height();
        fullExtent.adjust(-width * margin, -height * margin,
                         width * margin, height * margin);
        setExtent(fullExtent);
    }
}

void HGISMapCanvas::zoomIn()
{
    zoomToScale(d->mapScale * 1.5);
}

void HGISMapCanvas::zoomOut()
{
    zoomToScale(d->mapScale / 1.5);
}

void HGISMapCanvas::zoomToScale(double scale)
{
    if (scale <= 0) {
        return;
    }
    
    d->mapScale = scale;
    
    // 현재 중심점 기준으로 새 범위 계산
    double width = size().width() / scale;
    double height = size().height() / scale;
    
    QRectF newExtent(d->mapCenter.x() - width / 2,
                     d->mapCenter.y() - height / 2,
                     width, height);
    
    d->mapExtent = newExtent;
    updateTransform();
    
    emit scaleChanged(scale);
    emit extentChanged(newExtent);
    refresh();
}

double HGISMapCanvas::scale() const
{
    return d->mapScale;
}

void HGISMapCanvas::panToCenter(const QPointF &center)
{
    d->mapCenter = center;
    
    double width = d->mapExtent.width();
    double height = d->mapExtent.height();
    
    d->mapExtent = QRectF(center.x() - width / 2,
                          center.y() - height / 2,
                          width, height);
    
    updateTransform();
    emit extentChanged(d->mapExtent);
    refresh();
}

QPointF HGISMapCanvas::center() const
{
    return d->mapCenter;
}

QPointF HGISMapCanvas::toMapCoordinates(const QPoint &point) const
{
    return d->canvasToMap.map(QPointF(point));
}

QPoint HGISMapCanvas::toCanvasCoordinates(const QPointF &point) const
{
    return d->mapToCanvas.map(point).toPoint();
}

void HGISMapCanvas::refresh()
{
    viewport()->update();
}

void HGISMapCanvas::refreshMap()
{
    refresh();
}

void HGISMapCanvas::setMapTool(HGISMapTool *tool)
{
    d->mapTool = tool;
}

HGISMapTool* HGISMapCanvas::mapTool() const
{
    return d->mapTool;
}

void HGISMapCanvas::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    updateTransform();
    refresh();
}

void HGISMapCanvas::paintEvent(QPaintEvent *event)
{
    if (!d->layerManager) {
        QGraphicsView::paintEvent(event);
        return;
    }
    
    emit renderStarting();
    
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 배경색
    painter.fillRect(rect(), QColor(240, 240, 240));
    
    // 변환 행렬 설정
    painter.setTransform(d->mapToCanvas);
    
    // 레이어 렌더링
    renderLayers();
    
    // 레이어들을 역순으로 그리기 (아래에서 위로)
    QList<HGISMapLayer*> layers = d->layerManager->layersInRenderOrder();
    for (int i = layers.size() - 1; i >= 0; --i) {
        HGISMapLayer *layer = layers[i];
        if (layer && layer->isVisible()) {
            layer->render(&painter, d->mapExtent, d->mapScale);
        }
    }
    
    emit renderComplete();
}

void HGISMapCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        d->isDragging = true;
        d->lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    
    QGraphicsView::mousePressEvent(event);
}

void HGISMapCanvas::mouseMoveEvent(QMouseEvent *event)
{
    // 현재 좌표 표시
    QPointF mapPos = toMapCoordinates(event->pos());
    emit xyCoordinates(mapPos);
    
    // 드래그로 이동
    if (d->isDragging && (event->buttons() & Qt::LeftButton)) {
        QPoint delta = event->pos() - d->lastMousePos;
        d->lastMousePos = event->pos();
        
        // 맵 좌표계에서 이동량 계산
        QPointF mapDelta = toMapCoordinates(QPoint(0, 0)) - 
                          toMapCoordinates(delta);
        
        panToCenter(d->mapCenter + mapDelta);
    }
    
    QGraphicsView::mouseMoveEvent(event);
}

void HGISMapCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        d->isDragging = false;
        setCursor(Qt::ArrowCursor);
    }
    
    QGraphicsView::mouseReleaseEvent(event);
}

void HGISMapCanvas::wheelEvent(QWheelEvent *event)
{
    // 마우스 위치를 중심으로 줌
    QPointF mapPos = toMapCoordinates(event->pos());
    
    double delta = event->angleDelta().y();
    double scaleFactor = delta > 0 ? 1.2 : 0.8;
    
    // 새 스케일 계산
    double newScale = d->mapScale * scaleFactor;
    
    // 마우스 위치가 같은 지점을 유지하도록 중심 조정
    QPointF oldOffset = mapPos - d->mapCenter;
    QPointF newOffset = oldOffset * (d->mapScale / newScale);
    QPointF newCenter = mapPos - newOffset;
    
    d->mapCenter = newCenter;
    zoomToScale(newScale);
    
    event->accept();
}

void HGISMapCanvas::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Plus:
        case Qt::Key_Equal:
            zoomIn();
            break;
        case Qt::Key_Minus:
            zoomOut();
            break;
        case Qt::Key_F:
            zoomToFullExtent();
            break;
        default:
            QGraphicsView::keyPressEvent(event);
    }
}

void HGISMapCanvas::updateTransform()
{
    d->updateTransforms(size());
}

void HGISMapCanvas::renderLayers()
{
    // 레이어 렌더링은 paintEvent에서 직접 처리
}