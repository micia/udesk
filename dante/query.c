/* query.c: Context-less queries.
 *
 * This file implements context-less queries for eztensions or
 * implementation features and identification.
 *
 * Copyright (C) 2012-2013 Lorenzo Cogotti
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required. 
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution. 
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

#include "dante.h"

/* major version of the standard implemented by Dante. */
#define DANTE_UDESK_VERSION_MAJOR 0
/* minor version of the standard implemented by Dante. */
#define DANTE_UDESK_VERSION_MINOR 1

const char* UDESKAPIENTRY udeskQueryString(UDenum param)
{
	switch (param) {
	case UDESK_VENDOR:
		return "dante";
	
	case UDESK_VERSION:
		return "dante " VERSION " SDL reference implementation";
	
	default:
		return NULL;
	}
}

UDenum UDESKAPIENTRY udeskQueryiv(UDenum param, UDint* dst)
{
	switch (param) {
	case UDESK_VERSION_MAJOR:
		dst[0] = DANTE_UDESK_VERSION_MAJOR;
		return UDESK_NO_ERROR;
	
	case UDESK_VERSION_MINOR:
		dst[0] = DANTE_UDESK_VERSION_MINOR;
		return UDESK_NO_ERROR;
	
	case UDESK_NUM_EXTENSIONS:
		dst[0] = 0;
		return UDESK_NO_ERROR;
		
	default:
		return UDESK_INVALID_ENUM;
	}
}

const char* UDESKAPIENTRY udeskQueryExtension(UDint extnum)
{
	(void)extnum;
	
	return NULL;
}
