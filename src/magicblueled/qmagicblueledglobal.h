#ifndef QTQMAGICBLUELEDGLOBAL_H
#define QTQMAGICBLUELEDGLOBAL_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

#ifndef QT_STATIC
#  if defined(QT_BUILD_MAGICBLUELED_LIB)
#    define Q_MAGICBLUELED_EXPORT Q_DECL_EXPORT
#  else
#    define Q_MAGICBLUELED_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_MAGICBLUELED_EXPORT
#endif

namespace QMagicBlueLed
{


}

QT_END_NAMESPACE

#endif //QTQMAGICBLUELEDGLOBAL_H