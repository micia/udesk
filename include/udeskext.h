/* udeskext.h: The Unix Desktop Environment extensions file.
 *
 * Implementation specific extensions and utilities,
 * extended functionality and types offered by specific
 * implementations.
 *
 * Copyright (C) 2012-2013 Lorenzo Cogotti
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UDESKEXT_H_
#define UDESKEXT_H_

#include "udeskplatform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* UDESK_EXT_PROTOTYPES enables extension functions prototypes,
 * otherwise prototypes are unavailable and extension functions have
 * to be retrieved using udeskGetProcAddress(), which is the safest,
 * most portable and recommended way.
 */

/* ==========
 * Direct EGL support: UDESK_EGL_CONTEXT_EXT
 */
#ifndef UDESK_EGL_CONTEXT_EXT
#define UDESK_EGL_CONTEXT_EXT
/* TODO stub */

#endif /* UDESK_EGL_CONTEXT_EXT */

/* ==========
 * Extended image file formats support: UDESK_PIXMAP_EXTENDED_FILE_FORMATS_EXT
 */
#ifndef UDESK_PIXMAP_EXTENDED_FILE_FORMATS_EXT
#define UDESK_PIXMAP_EXTENDED_FILE_FORMATS_EXT
/* TODO stub */

#endif /* UDESK_PIXMAP_EXTENDED_FILE_FORMATS_EXT */

/* ==========
 * Animation object support: UDESK_ANIMATION_OBJECT_EXT
 */
#ifndef UDESK_ANIMATION_OBJECT_EXT
#define UDESK_ANIMATION_OBJECT_EXT

enum {
  UDESK_HANDLE_ANIMATION_EXT = 0x8001
#define UDESK_HANDLE_ANIMATION_EXT UDESK_HANDLE_ANIMATION_EXT

};

#ifdef UDESK_EXT_PROTOTYPES
UDESKAPI void UDESKAPIENTRY udeskAnimationFileEXT(UDhandle animation, const char* name);
UDESKAPI void UDESKAPIENTRY udeskAnimationDataEXT(UDhandle animation, UDint frame, UDint width, UDint height, UDenum format, const void* buff, UDint delay);
UDESKAPI void UDESKAPIENTRY udeskAnimationResetEXT(UDhandle animation);
UDESKAPI void UDESKAPIENTRY udeskGetAnimationivEXT(UDhandle animation, UDint param, UDint frame, UDint* dst);
UDESKAPI UDboolean UDESKAPIENTRY udeskIsAnimationEXT(UDhandle handle);
UDESKAPI void UDESKAPIENTRY udeskLayerAnimationEXT(UDhandle layer, UDhandle animation);
#endif
typedef void (UDESKAPIENTRYP PFNUDESKANIMATIONFILEEXTPROC)(UDhandle animation, const char* name);
typedef void (UDESKAPIENTRYP PFNUDESKANIMATIONDATAEXTPROC)(UDhandle animation, UDint frame, UDint width, UDint height, UDenum format, const void* buff, UDint delay);
typedef void (UDESKAPIENTRYP PFNUDESKANIMATIONRESETEXTPROC)(UDhandle animation);
typedef void (UDESKAPIENTRYP PFNUDESKGETANIMATIONIVEXTPROC)(UDhandle animation, UDenum param, UDint* dst);
typedef UDboolean (UDESKAPIENTRYP PFNUDESKISANIMATIONEXTPROC)(UDhandle handle);
typedef void (UDESKAPIENTRYP PFNUDESKLAYERANIMATIONEXTPROC)(UDhandle layer, UDhandle animation);
#endif /* UDESK_ANIMATION_OBJECT_EXT */

#ifdef __cplusplus
}
#endif

#endif /* UDESKEXT_H_ */
