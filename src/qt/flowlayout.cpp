#include <QtGui>
#include <QStyle>
#include <QWidget>

#include "flowlayout.h"
//! [1]
FlowLayout::FlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing)
    : QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing)
{
    setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::FlowLayout(int margin, int hSpacing, int vSpacing)
    : m_hSpace(hSpacing), m_vSpace(vSpacing)
{
    setContentsMargins(margin, margin, margin, margin);
}
//! [1]

//! [2]
FlowLayout::~FlowLayout()
{
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
}
//! [2]

//! [3]
void FlowLayout::addItem(QLayoutItem *item)
{
    itemList.append(item);
}
//! [3]

//! [4]
int FlowLayout::horizontalSpacing() const
{
    if (m_hSpace >= 0) {
        return m_hSpace;
    } else {
        return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
    }
}

int FlowLayout::verticalSpacing() const
{
    if (m_vSpace >= 0) {
        return m_vSpace;
    } else {
        return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
    }
}
//! [4]

//! [5]
int FlowLayout::count() const
{
    return itemList.size();
}

QLayoutItem *FlowLayout::itemAt(int index) const
{
    return itemList.value(index);
}

QLayoutItem *FlowLayout::takeAt(int index)
{
    if (index >= 0 && index < itemList.size())
        return itemList.takeAt(index);
    else
        return 0;
}
//! [5]

//! [6]
Qt::Orientations FlowLayout::expandingDirections() const
{
    return Qt::Horizontal;
}
//! [6]

//! [7]
bool FlowLayout::hasHeightForWidth() const
{
    return true;
}

int FlowLayout::heightForWidth(int width) const
{
    int height = doLayout(QRect(0, 0, width, 0), true);
    return height;
}
//! [7]

//! [8]
void FlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

QSize FlowLayout::sizeHint() const
{
    return minimumSize();
}

QSize FlowLayout::minimumSize() const
{
    QSize size;
    // v2.0.0.9 Qt6: `foreach (item, itemList)` used a PRE-DECLARED variable,
    // which a range-for cannot express.  Folding the declaration into the loop
    // is behaviour-preserving here -- verified that `item` is not referenced
    // after the loop in this function.
    for (QLayoutItem *item : itemList)
        size = size.expandedTo(item->minimumSize());

    // v2.0.0.9 Qt6: QLayout::margin() was deprecated in Qt 5.13 and REMOVED in
    // Qt6.  contentsMargins() is the replacement and is available on both.
    // margin() returned a SINGLE value, so `2*margin()` assumed all four sides
    // were equal -- true here, since the constructors call
    // setContentsMargins(margin, margin, margin, margin) (lines 10 and 16).
    // Summing the opposing pairs is equivalent and correct even if the margins
    // ever differ.
    const QMargins mrg = contentsMargins();
    size += QSize(mrg.left() + mrg.right(), mrg.top() + mrg.bottom());
    return size;
}
//! [8]

//! [9]
int FlowLayout::doLayout(const QRect &rect, bool testOnly) const
{
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;
//! [9]

//! [10]
    // v2.0.0.9 Qt6: pre-declared loop variable folded into the range-for.
    // Verified `item` is not referenced after the loop in this function.
    for (QLayoutItem *item : itemList) {
        QWidget *wid = item->widget();
        int spaceX = horizontalSpacing();
        if (spaceX == -1)
            spaceX = wid->style()->layoutSpacing(
                QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
        int spaceY = verticalSpacing();
        if (spaceY == -1)
            spaceY = wid->style()->layoutSpacing(
                QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);
//! [10]
//! [11]
        int nextX = x + item->sizeHint().width() + spaceX;
        if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
            x = effectiveRect.x();
            y = y + lineHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            lineHeight = 0;
        }

        if (!testOnly)
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));

        x = nextX;
        lineHeight = qMax(lineHeight, item->sizeHint().height());
    }
    return y + lineHeight - rect.y() + bottom;
}
//! [11]
//! [12]
int FlowLayout::smartSpacing(QStyle::PixelMetric pm) const
{
    QObject *parent = this->parent();
    if (!parent) {
        return -1;
    } else if (parent->isWidgetType()) {
        QWidget *pw = static_cast<QWidget *>(parent);
        return pw->style()->pixelMetric(pm, 0, pw);
    } else {
        return static_cast<QLayout *>(parent)->spacing();
    }
}
//! [12]
