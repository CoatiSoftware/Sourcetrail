#include "compatibilityQt.h"

#include <QtGlobal>

namespace utility
{
namespace compatibility
{
QPoint QWheelEvent_globalPos(const QWheelEvent& event)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
	return event.globalPosition().toPoint();
#else
	return event.globalPos();
#endif
}
}	 // namespace compatibility
}	 // namespace utility
