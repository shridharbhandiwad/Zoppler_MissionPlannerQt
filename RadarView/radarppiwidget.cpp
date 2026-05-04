#include "radarppiwidget.h"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QtMath>
#include <QToolTip>
#include <cmath>

// ─── colour palette ────────────────────────────────────────────────────
static const QColor kBg          ("#0a0f14");
static const QColor kScopeRim    ("#003320");
static const QColor kRingColor   ("#004422");
static const QColor kRingLabel   ("#336644");
static const QColor kGridLine    ("#002a18");
static const QColor kSweepColor  ("#00ff8840");
static const QColor kCenterMark  ("#00ff88");
static const QColor kDetection   ("#00ff88");
static const QColor kDetHover    ("#ffdd44");
static const QColor kDetTrail    ("#00aa5560");
static const QColor kTextColor   ("#88ccaa");
static const QColor kLabelColor  ("#00ff88");
static const int    kDetRadius   = 5;
static const int    kHoverRadius = 7;
static const int    kMinPad      = 30;   // px padding around scope

// ─── ctor ───────────────────────────────────────────────────────────────
RadarPPIWidget::RadarPPIWidget(const RadarView::Radar &radar, QWidget *parent)
    : QWidget(parent)
    , m_radar(radar)
    , m_hoveredTrackId(-1)
{
    setMouseTracking(true);
    setMinimumSize(300, 300);
    setAttribute(Qt::WA_OpaquePaintEvent);
    rebuildScreenPositions();
}

void RadarPPIWidget::updateRadar(const RadarView::Radar &radar)
{
    m_radar = radar;
    rebuildScreenPositions();
    update();
}

QSize RadarPPIWidget::sizeHint() const
{
    return QSize(500, 500);
}

// ─── geometry helpers ───────────────────────────────────────────────────
QPointF RadarPPIWidget::scopeCenter() const
{
    return QPointF(width() / 2.0, height() / 2.0);
}

double RadarPPIWidget::scopeRadius() const
{
    return qMin(width(), height()) / 2.0 - kMinPad;
}

QRectF RadarPPIWidget::scopeRect() const
{
    QPointF c = scopeCenter();
    double  r = scopeRadius();
    return QRectF(c.x() - r, c.y() - r, 2 * r, 2 * r);
}

// Azimuth 0° = North (up), increases clockwise — standard radar convention
QPointF RadarPPIWidget::polarToScreen(double range, double azimuthDeg) const
{
    double ratio   = (m_radar.maxRange > 0.0) ? (range / m_radar.maxRange) : 0.0;
    ratio = qBound(0.0, ratio, 1.0);
    double px      = ratio * scopeRadius();
    double radians = qDegreesToRadians(azimuthDeg - 90.0); // rotate so 0° = up
    QPointF c      = scopeCenter();
    return QPointF(c.x() + px * qCos(radians),
                   c.y() + px * qSin(radians));
}

// ─── rebuild screen positions ───────────────────────────────────────────
void RadarPPIWidget::rebuildScreenPositions()
{
    m_screenDetections.clear();
    for (const auto &d : m_radar.detections) {
        DetectionScreen ds;
        ds.pos       = polarToScreen(d.range, d.azimuth);
        ds.trackId   = d.trackId;
        ds.range     = d.range;
        ds.azimuth   = d.azimuth;
        ds.elevation = d.elevation;
        m_screenDetections.append(ds);
    }
}

void RadarPPIWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    rebuildScreenPositions();
}

// ─── painting ───────────────────────────────────────────────────────────
void RadarPPIWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // ── 1. Background ──────────────────────────────────────────────────
    p.fillRect(rect(), kBg);

    double  R = scopeRadius();
    QPointF C = scopeCenter();

    // ── 2. Scope circle fill (very dark green tint) ────────────────────
    p.setBrush(QBrush(QColor("#060e09")));
    p.setPen(Qt::NoPen);
    p.drawEllipse(scopeRect());

    // ── 3. Azimuth grid lines (every 30°) ──────────────────────────────
    {
        QPen gp(kGridLine, 1, Qt::DotLine);
        p.setPen(gp);
        for (int az = 0; az < 360; az += 30) {
            QPointF end = polarToScreen(m_radar.maxRange, az);
            p.drawLine(C, end);
        }
    }

    // ── 4. Coverage sector arc ─────────────────────────────────────────
    {
        const auto &cov = m_radar.coverage;
        double minAz  = cov.minAzimuthDeg;
        double maxAz  = cov.maxAzimuthDeg;
        double sweepDeg = maxAz - minAz;
        if (sweepDeg <= 0.0) sweepDeg += 360.0;
        bool fullCircle = (std::abs(sweepDeg - 360.0) < 0.5);

        if (!fullCircle) {
            // Draw sector fill for coverage zone
            QColor sectorFill(0, 255, 136, 12);
            p.setBrush(QBrush(sectorFill));
            p.setPen(Qt::NoPen);
            QPainterPath sectorPath;
            sectorPath.moveTo(C);
            // Qt arc convention: 0° = 3-o-clock, CCW positive
            // Radar convention: 0° = North (12-o-clock), CW positive
            double qtStart = -(minAz - 90.0);
            double qtSpan  = -sweepDeg;
            sectorPath.arcTo(QRectF(C.x() - R, C.y() - R, 2 * R, 2 * R), qtStart, qtSpan);
            sectorPath.closeSubpath();
            p.drawPath(sectorPath);

            // Draw sector boundary lines
            p.setPen(QPen(QColor("#00ff8840"), 1, Qt::DashLine));
            p.drawLine(C, polarToScreen(m_radar.maxRange, minAz));
            p.drawLine(C, polarToScreen(m_radar.maxRange, maxAz));
        }
    }

    // ── 5. Range rings ─────────────────────────────────────────────────
    {
        const auto &cov = m_radar.coverage;
        double spacing = (cov.rangeRingSpacingKm > 0.0) ? cov.rangeRingSpacingKm : 10.0;
        double maxRange = m_radar.maxRange;

        p.setBrush(Qt::NoBrush);
        QFont labelFont("Monospace", 7);
        p.setFont(labelFont);

        for (double r = spacing; r <= maxRange + 0.01; r += spacing) {
            double frac   = r / maxRange;
            frac = qBound(0.0, frac, 1.0);
            double ringR  = frac * R;
            QRectF ringRect(C.x() - ringR, C.y() - ringR, 2 * ringR, 2 * ringR);

            bool isOuter = (r >= maxRange - 0.01);
            p.setPen(QPen(kRingColor, isOuter ? 2 : 1));
            p.drawEllipse(ringRect);

            // Label
            QString lbl = QString("%1 km").arg(static_cast<int>(r));
            p.setPen(kRingLabel);
            p.drawText(QPointF(C.x() + 4, C.y() - ringR + 10), lbl);
        }
    }

    // ── 6. Azimuth labels ──────────────────────────────────────────────
    {
        QFont azFont("Monospace", 8);
        p.setFont(azFont);
        p.setPen(kTextColor);

        struct AzLabel { int az; const char *txt; };
        static const AzLabel labels[] = {
            {  0, "N" }, { 45, "NE"}, { 90, "E" },
            {135, "SE"}, {180, "S" }, {225, "SW"},
            {270, "W" }, {315, "NW"}
        };
        for (const auto &l : labels) {
            QPointF tp = polarToScreen(m_radar.maxRange * 1.08, l.az);
            QFontMetrics fm(azFont);
            QRect br = fm.boundingRect(l.txt);
            tp -= QPointF(br.width() / 2.0, -br.height() / 4.0);
            p.drawText(tp, l.txt);
        }
    }

    // ── 7. Scope rim ───────────────────────────────────────────────────
    {
        p.setPen(QPen(kScopeRim, 2));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(scopeRect());
    }

    // ── 8. Cross-hair at center ────────────────────────────────────────
    {
        double crossLen = 10.0;
        p.setPen(QPen(kCenterMark, 2));
        p.drawLine(QPointF(C.x() - crossLen, C.y()),
                   QPointF(C.x() + crossLen, C.y()));
        p.drawLine(QPointF(C.x(), C.y() - crossLen),
                   QPointF(C.x(), C.y() + crossLen));
        p.setBrush(kCenterMark);
        p.setPen(Qt::NoPen);
        p.drawEllipse(C, 4.0, 4.0);
    }

    // ── 9. Detections ──────────────────────────────────────────────────
    for (const auto &ds : m_screenDetections) {
        bool hovered = (ds.trackId == m_hoveredTrackId);
        QColor fillColor  = hovered ? kDetHover  : kDetection;
        double radius     = hovered ? kHoverRadius : kDetRadius;

        // Outer glow ring
        p.setPen(QPen(fillColor.darker(120), 1));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(ds.pos, radius + 3, radius + 3);

        // Detection marker (filled diamond via rotated square)
        p.save();
        p.translate(ds.pos);
        p.rotate(45);
        p.setBrush(fillColor);
        p.setPen(Qt::NoPen);
        double s = radius * 0.85;
        p.drawRect(QRectF(-s, -s, 2*s, 2*s));
        p.restore();

        // Track ID label
        QFont idFont("Monospace", 7);
        p.setFont(idFont);
        p.setPen(hovered ? kDetHover : kLabelColor);
        p.drawText(ds.pos + QPointF(radius + 4, -2),
                   QString("T%1").arg(ds.trackId));
    }

    // ── 10. Radar title + stats ─────────────────────────────────────────
    {
        QFont titleFont("Monospace", 9, QFont::Bold);
        p.setFont(titleFont);
        p.setPen(kLabelColor);

        QString title = QString("[R%1] %2").arg(m_radar.radarId).arg(m_radar.radarName);
        p.drawText(QPointF(8, 18), title);

        const auto &cov = m_radar.coverage;
        QString stats = QString("Range: %1 km   Az: %2°–%3°   El: %4°–%5°   Det: %6")
                            .arg(cov.maxRangeKm, 0, 'f', 0)
                            .arg(cov.minAzimuthDeg, 0, 'f', 0)
                            .arg(cov.maxAzimuthDeg, 0, 'f', 0)
                            .arg(cov.minElevationDeg, 0, 'f', 0)
                            .arg(cov.maxElevationDeg, 0, 'f', 0)
                            .arg(m_radar.detections.size());
        QFont statsFont("Monospace", 7);
        p.setFont(statsFont);
        p.setPen(kTextColor);
        p.drawText(QPointF(8, 32), stats);

        // Status pill
        QString statusTxt = m_radar.active ? "ACTIVE" : "INACTIVE";
        QColor  statusCol = m_radar.active ? QColor("#00ff88") : QColor("#ff4444");
        p.setPen(Qt::NoPen);
        p.setBrush(statusCol.darker(180));
        QRect pill(width() - 80, 6, 72, 18);
        p.drawRoundedRect(pill, 4, 4);
        p.setPen(statusCol);
        QFont statusFont("Monospace", 7, QFont::Bold);
        p.setFont(statusFont);
        p.drawText(pill, Qt::AlignCenter, statusTxt);
    }

    // ── 11. Hover tooltip box ──────────────────────────────────────────
    if (m_hoveredTrackId >= 0) {
        for (const auto &ds : m_screenDetections) {
            if (ds.trackId != m_hoveredTrackId) continue;

            QString tip = QString(
                "Track ID   : %1\n"
                "Range      : %2 km\n"
                "Azimuth    : %3°\n"
                "Elevation  : %4°"
            ).arg(ds.trackId)
             .arg(ds.range,     0, 'f', 1)
             .arg(ds.azimuth,   0, 'f', 1)
             .arg(ds.elevation, 0, 'f', 1);

            QFont tipFont("Monospace", 8);
            QFontMetrics fm(tipFont);
            QStringList lines = tip.split('\n');
            int tw = 0;
            for (const auto &l : lines) tw = qMax(tw, fm.horizontalAdvance(l));
            int th = fm.height() * lines.size();

            int bx = m_hoverPos.x() + 14;
            int by = m_hoverPos.y() - th - 14;
            if (bx + tw + 16 > width())  bx = m_hoverPos.x() - tw - 30;
            if (by < 4)                  by = m_hoverPos.y() + 14;

            QRect tipRect(bx - 6, by - 4, tw + 16, th + 12);
            p.setPen(Qt::NoPen);
            p.setBrush(QColor("#0e1e16ee"));
            p.drawRoundedRect(tipRect, 5, 5);
            p.setPen(QColor("#00ff8888"));
            p.drawRoundedRect(tipRect, 5, 5);

            p.setFont(tipFont);
            p.setPen(QColor("#ccffdd"));
            int ly = by + fm.ascent() + 2;
            for (const auto &l : lines) {
                p.drawText(QPointF(bx, ly), l);
                ly += fm.height();
            }
            break;
        }
    }
}

// ─── mouse events ───────────────────────────────────────────────────────
void RadarPPIWidget::mouseMoveEvent(QMouseEvent *event)
{
    m_hoverPos    = event->pos();
    m_hoveredTrackId = -1;

    for (const auto &ds : m_screenDetections) {
        double dx = ds.pos.x() - m_hoverPos.x();
        double dy = ds.pos.y() - m_hoverPos.y();
        if (dx*dx + dy*dy <= (kHoverRadius + 4) * (kHoverRadius + 4)) {
            m_hoveredTrackId = ds.trackId;
            setCursor(Qt::CrossCursor);
            update();
            return;
        }
    }
    setCursor(Qt::ArrowCursor);
    update();
}

void RadarPPIWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    m_hoveredTrackId = -1;
    update();
}
