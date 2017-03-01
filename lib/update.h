#ifndef APTC_UPDATE_H_
#define APTC_UPDATE_H_

#include <apt-pkg/cmndline.h>

// XXX This is implemented in apt/apt-private/private-update.cc, but since the
//     headers of libapt-private are not packaged, we need to "vendor" them.
bool DoUpdate(CommandLine &);

#endif // APTC_UPDATE_H_
