#ifndef __privilege_H__
#define __privilege_H__

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>

#include <privacy_privilege_manager.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "privilege"

#if !defined(PACKAGE)
#define PACKAGE "my.package.privilege"
#endif

#endif /* __privilege_H__ */