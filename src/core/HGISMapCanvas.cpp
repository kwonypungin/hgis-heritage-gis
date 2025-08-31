#include "HGISMapCanvas.h"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QPainter>
#include <QScrollBar>
#include <QDebug>
#include <cmath>

class HGISMapCanvas::Private
{
public:
    QGraphicsScene *scene = nullptr;
    QRectF extent;
    double currentScale = 1.0;
    double rotation = 0.0;
    QColor backgroundColor;
    
    // 상호작용 상태
    bool isPanning = false;
    QPoint lastMousePos;
    QPointF sceneCenter;
    
    // 줌 설정
    double zoomFactor = 1.2;
    double minScale = 0.00001;
    double maxScale = 100000.0;
    
    Private()
    {
        backgroundColor = QColor(255, 255, 255);
        extent = QRectF(-180, -90, 360, 180); // 기본 WGS84 범위
    }
    
    ~Private()
    {
        delete scene;
    }
};

HGISMapCanvas::HGISMapCanvas(QWidget *parent)
    : QGraphicsView(parent)
    , d(std::make_unique<Private>())
{
    // 그래픽스 씬 설정
    d->scene = new QGraphicsScene(this);
    setScene(d->scene);
    
    // 렌더링 최적화 설정
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
    
    // 드래그 모드 설정
    setDragMode(QGraphicsView::NoDrag);
    
    // 스크롤바 숨기기
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    // 배경색 설정
    setBackgroundBrush(QBrush(d->backgroundColor));
    
    // 마우스 추적 활성화
    setMouseTracking(true);
    
    // 초기 씬 설정
    d->scene->setSceneRect(d->extent);
    
    // 테스트용 아이템 추가 (나중에 실제 레이어로 대체)
    QGraphicsRectItem *testRect = new QGraphicsRectItem(d->extent);
    testRect->setPen(QPen(Qt::blue, 1));
    testRect->setBrush(QBrush(QColor(200, 200, 255, 50)));
    d->scene->addItem(testRect);
    
    // 초기 뷰 설정
    fitInView(d->extent, Qt::KeepAspectRatio);
    
    qInfo() << "HGISMapCanvas 초기화 완료";
}

HGISMapCanvas::~HGISMapCanvas()
{
}

QRectF HGISMapCanvas::extent() const
{
    return d->extent;
}

void HGISMapCanvas::setExtent(const QRectF &extent)
{
    d->extent = extent;
    d->scene->setSceneRect(extent);
    fitInView(extent, Qt::KeepAspectRatio);
    updateScale();
    emit extentsChanged();
}

void HGISMapCanvas::zoomIn()
{
    QGraphicsView::scale(d->zoomFactor, d->zoomFactor);
    updateScale();
}

void HGISMapCanvas::zoomOut()
{
    QGraphicsView::scale(1.0 / d->zoomFactor, 1.0 / d->zoomFactor);
    updateScale();
}

void HGISMapCanvas::zoomToFullExtent()
{
    fitInView(d->extent, Qt::KeepAspectRatio);
    updateScale();
}

void HGISMapCanvas::zoomWithCenter(int scaleFactor, const QPointF &center)
{
    // 중심점 기준으로 줌
    centerOn(center);
    
    if (scaleFactor > 0) {
        QGraphicsView::scale(d->zoomFactor, d->zoomFactor);
    } else {
        QGraphicsView::scale(1.0 / d->zoomFactor, 1.0 / d->zoomFactor);
    }
    
    updateScale();
}

void HGISMapCanvas::panToCenter(const QPointF &center)
{
    centerOn(center);
    emit extentsChanged();
}

double HGISMapCanvas::scale() const
{
    return d->currentScale;
}

void HGISMapCanvas::setScale(double scaleValue)
{
    if (scaleValue < d->minScale) scaleValue = d->minScale;
    if (scaleValue > d->maxScale) scaleValue = d->maxScale;
    
    resetTransform();
    QGraphicsView::scale(scaleValue, scaleValue);
    if (d->rotation != 0) {
        rotate(d->rotation);
    }
    
    d->currentScale = scaleValue;
    emit scaleChanged(scaleValue);
}

double HGISMapCanvas::rotation() const
{
    return d->rotation;
}

void HGISMapCanvas::setRotation(double degrees)
{
    d->rotation = degrees;
    updateTransform();
    emit rotationChanged(degrees);
}

void HGISMapCanvas::refresh()
{
    emit renderStarting();
    
    // 씬 업데이트
    d->scene->update();
    viewport()->update();
    
    emit renderComplete();
}

void HGISMapCanvas::setCanvasColor(const QColor &color)
{
    d->backgroundColor = color;
    setBackgroundBrush(QBrush(color));
    refresh();
}

QColor HGISMapCanvas::canvasColor() const
{
    return d->backgroundColor;
}

void HGISMapCanvas::wheelEvent(QWheelEvent *event)
{
    // 마우스 휠로 줌
    QPointF scenePos = mapToScene(event->position().toPoint());
    
    int delta = event->angleDelta().y();
    if (delta > 0) {
        zoomWithCenter(1, scenePos);
    } else if (delta < 0) {
        zoomWithCenter(-1, scenePos);
    }
    
    event->accept();
}

void HGISMapCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        d->isPanning = true;
        d->lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    } else if (event->button() == Qt::MiddleButton) {
        d->isPanning = true;
        d->lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    
    QGraphicsView::mousePressEvent(event);
}

void HGISMapCanvas::mouseMoveEvent(QMouseEvent *event)
{
    // 좌표 표시
    QPointF scenePos = mapToScene(event->pos());
    emit xyCoordinates(scenePos);
    
    // 팬 처리
    if (d->isPanning) {
        QPointF delta = event->pos() - d->lastMousePos;
        d->lastMousePos = event->pos();
        
        // 뷰 이동
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        
        emit extentsChanged();
    }
    
    QGraphicsView::mouseMoveEvent(event);
}

void HGISMapCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton || event->button() == Qt::MiddleButton) {
        d->isPanning = false;
        setCursor(Qt::ArrowCursor);
    }
    
    QGraphicsView::mouseReleaseEvent(event);
}

void HGISMapCanvas::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 더블클릭으로 줌 인
        QPointF scenePos = mapToScene(event->pos());
        zoomWithCenter(1, scenePos);
    }
    
    QGraphicsView::mouseDoubleClickEvent(event);
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
        case Qt::Key_Space:
            zoomToFullExtent();
            break;
        case Qt::Key_R:
            if (event->modifiers() & Qt::ControlModifier) {
                setRotation(0);
            }
            break;
        default:
            QGraphicsView::keyPressEvent(event);
    }
}

void HGISMapCanvas::keyReleaseEvent(QKeyEvent *event)
{
    QGraphicsView::keyReleaseEvent(event);
}

void HGISMapCanvas::paintEvent(QPaintEvent *event)
{
    emit renderStarting();
    QGraphicsView::paintEvent(event);
    emit renderComplete();
}

void HGISMapCanvas::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    updateScale();
}

void HGISMapCanvas::updateScale()
{
    // 현재 변환 행렬에서 축척 계산
    QTransform t = transform();
    d->currentScale = t.m11(); // x축 스케일 팩터
    emit scaleChanged(d->currentScale);
}

void HGISMapCanvas::updateTransform()
{
    resetTransform();
    QGraphicsView::scale(d->currentScale, d->currentScale);
    rotate(d->rotation);
}

QPointF HGISMapCanvas::mapToScene(const QPoint &point) const
{
    return QGraphicsView::mapToScene(point);
}

QPoint HGISMapCanvas::sceneToMap(const QPointF &scenePoint) const
{
    return mapFromScene(scenePoint);
}