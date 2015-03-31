#ifndef __RESOURCE__2lgc_H__
#define __RESOURCE__2lgc_H__

#include <gio/gio.h>

extern GResource *_2lgc_get_resource (void);

extern void _2lgc_register_resource (void);
extern void _2lgc_unregister_resource (void);

#endif
