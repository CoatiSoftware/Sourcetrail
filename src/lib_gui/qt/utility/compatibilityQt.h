#ifndef COMPATIBILITY_QT_H
#define COMPATIBILITY_QT_H

#include <QWheelEvent>

namespace utility
{
namespace compatibility
{
// Extracts the global position from a QWheelEvent.
// This compatibility wrapper bridges Qt 5.12 and Qt 5.14
QPoint QWheelEvent_globalPos(const QWheelEvent& event);

}	 // namespace compatibility
}	 // namespace utility

#endif	  // COMPATIBILITY_QT_H
