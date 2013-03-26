/* context.c: Context management and udesk functions implementation.
 *
 * This file contains the context management functions and object
 * allocations routines.
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
#include <stdlib.h>
#include <string.h>

DanteContext* dante_context = NULL;

/* Retrieves a value for the specified environment variable,
 * if the environment variable has an invalid value or can't be found,
 * the default value is returned.
 */
static UDboolean danteGetEnvVariable(const char* name, UDboolean defval);
/* Walks the slice list finding the first empty slot, a new
 * slice is allocated into it, inizialized and returned.
 * No error is set on allocation failure (which can only happen
 * on UDESK_OUT_OF_MEMORY condition), this is in order to allow
 * silent memory allocation failures.
 */
static DanteSlice* danteAllocSlice(void);

static UDboolean danteGetEnvVariable(const char* name, UDboolean defval)
{
	char* var = getenv(name);
	
	if (var && var[0] != '\0') {
		long val = strtol(var, &var, 0);
		
		if (var[0] == '\0') {
			/* value was valid */
			return (val != 0l);
		}
	}
	
	return defval;
}

static DanteSlice* danteAllocSlice(void)
{
	DanteSlice* prev;
	DanteSlice* next;
	DanteSlice* ret;
	UDhandle base;
	UDint i;
	
	ret = (DanteSlice*)malloc(DANTE_SLICE_SIZE);
	if (!ret) {
		return NULL;
	}
	
	/* find a free handle set in the slice list */
	prev = &dante_context->slice;
	next = dante_context->slice.next;
	base = DANTE_FAST_OBJECT_CACHE_SIZE + 1;
	while (next->base != UDESK_HANDLE_NONE) {
		if (next->base != base) {
			break;
		}
		
		prev = next;
		next = next->next;
		base += DANTE_SLICE_CACHESIZE;
	}
	
	/* insert into the slice list */
	ret->next = next;
	ret->prev = prev;
	next->prev = ret;
	prev->next = ret;
	/* insert into the free list */
	ret->next_free = dante_context->slice.next_free;
	ret->prev_free = &dante_context->slice;
	dante_context->slice.next_free->prev_free = ret;
	dante_context->slice.next_free = ret;
	
	/* initialize the slice */
	ret->base = base;
	ret->used = 0;
	ret->first_free = NULL;
	for (i = 0; i < DANTE_SLICE_CACHESIZE; i++) {
		DanteObject* obj = &ret->data[i];
		
		obj->type = UDESK_NONE;
		obj->handle = base + i;
		obj->slice = ret;
		obj->d.none.next = ret->first_free;
		ret->first_free = obj;
	}
	
	return ret;
}

DanteObject* DANTEAPIENTRY danteAllocObject(UDenum type)
{
	DanteObject* obj = NULL;
	
	if (type == UDESK_HANDLE_EVENT) {
		/* try to allocate into fast cache */
		obj = dante_context->cache_free;
		if (obj) {
			dante_context->cache_free = obj->d.none.next;
		}
	}
	
	if (!obj) {
		/* allocate into slice managed memory */
		DanteSlice* slice = dante_context->slice.next_free;
		
		if (slice->base == UDESK_HANDLE_NONE) {
			/* no free slices left, allocate a new one */
			slice = danteAllocSlice();
			if (!slice) {
				return NULL;
			}
		}
		
		obj = slice->first_free;
		slice->first_free = obj->d.none.next;
		slice->used++;
		if (slice->used == DANTE_SLICE_CACHESIZE) {
			/* slice is full, remove from free list */
			slice->next_free->prev_free = slice->prev_free;
			slice->prev_free->next_free = slice->next_free;
		}
	}
	
	/* initialize the object common fields */
	obj->type = type;
	obj->refs = 1;
	obj->vt = NULL;
	obj->dispatch = NULL;
	obj->parent = NULL;
	memset(&obj->d, 0, sizeof(obj->d));
	return obj;
}

DanteObject* DANTEAPIENTRY danteGetObject(UDhandle handle)
{	
	DanteObject* obj;

	if (handle <= UDESK_HANDLE_NONE) {
		return NULL;
	}
	
	if (handle > DANTE_FAST_OBJECT_CACHE_SIZE) {
		/* search into the slice managed memory */
		DanteSlice* slice;
	
		for (slice = dante_context->slice.next; slice->base != UDESK_HANDLE_NONE; slice = slice->next) {
			if (handle < slice->base + DANTE_SLICE_CACHESIZE) {
				break;
			}
		}
		
		if (slice->base == UDESK_HANDLE_NONE || handle < slice->base) {
			return NULL;
		}
		
		obj = &slice->data[handle - slice->base];
		
	} else {
		/* object belongs to the fast cache */
		obj = &dante_context->cache[handle - 1];
	}
	
	return (obj->type != UDESK_NONE)? obj : NULL;
}

UDboolean DANTEAPIENTRY danteCheckObjectType(UDhandle handle, UDenum type)
{
	DanteObject* obj;
	
	DANTE_IGNORE_AND_RETVAL_IF(!dante_context, false);
	
	obj = danteGetObject(handle);
	if (!obj) {
		return false;
	}
	
	return (obj->type == type);
}

DanteObject* DANTEAPIENTRY danteRetrieveObject(UDhandle handle, UDenum type)
{
	DanteObject* obj;
	
	DANTE_IGNORE_AND_RETVAL_IF(!dante_context, NULL);
	
	obj = danteGetObject(handle);
	DANTE_ERROR_AND_RETVAL_IF(!obj, UDESK_INVALID_VALUE, NULL);
	DANTE_ERROR_AND_RETVAL_IF(obj->type != type, UDESK_INVALID_VALUE, NULL);
	return obj;
}

DanteObject* DANTEAPIENTRY danteRefObject(DanteObject* obj)
{
	if (obj) {
		obj->refs++;
	}
	
	return obj;
}

void DANTEAPIENTRY danteUnrefObject(DanteObject* obj)
{
	if (obj) {
		obj->refs--;
		if (obj->refs == 0) {
			DanteSlice* slice = obj->slice;
	
			/* partially initialized objects may lack a virtual table */
			if (obj->vt && obj->vt->clear) {
				obj->vt->clear(obj);
			}
			
			obj->type = UDESK_NONE;
			if (slice) {
				/* object belongs to slice managed memory */
				obj->d.none.next = slice->first_free;
				slice->first_free = obj;
				slice->used--;
				if (slice->used == 0) {
					/* free the slice, give back memory to the OS */
					slice->next_free->prev_free = slice->prev_free;
					slice->prev_free->next_free = slice->next_free;
					slice->next->prev = slice->prev;
					slice->prev->next = slice->next;
					free(slice);
				}
				
			} else {
				/* object belongs to static fast cache */
				obj->d.none.next = dante_context->cache_free;
				dante_context->cache_free = obj;
			}
		}
	}
}

UDenum UDESKAPIENTRY udeskCreateContext(int* argc, char** argv[])
{
	DanteContext* ctx;
	UDint i;
	
	DANTE_IGNORE_AND_RETVAL_IF(!argc || !argv || *argc <= 0 || !*argv[0], UDESK_INVALID_VALUE);
	DANTE_IGNORE_AND_RETVAL_IF(dante_context, UDESK_INVALID_OPERATION);
	
	/* initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) != 0) {
		return UDESK_OPERATION_FAILED;
	}
	
	/* initialize context */
	ctx = (DanteContext*)malloc(sizeof(*ctx));
	if (!ctx) {
		return UDESK_OUT_OF_MEMORY;
	}
	
	memset(ctx, 0, sizeof(*ctx));
	ctx->error = UDESK_NO_ERROR;
	ctx->vsync = danteGetEnvVariable(DANTE_ENV_VSYNC, true);
	ctx->accelerated = danteGetEnvVariable(DANTE_ENV_ACCELERATED, true);
	ctx->slice.base = UDESK_HANDLE_NONE;
	ctx->slice.used = 0;
	ctx->slice.next = &ctx->slice;
	ctx->slice.prev = &ctx->slice;
	ctx->slice.next_free = &ctx->slice;
	ctx->slice.prev_free = &ctx->slice;
	
	/* initialize the fast cache */
	for (i = 0; i < DANTE_FAST_OBJECT_CACHE_SIZE; i++) {
		ctx->cache[i].type = UDESK_NONE;
		ctx->cache[i].handle = 1 + i;
		ctx->cache[i].slice = NULL;
		ctx->cache[i].d.none.next = ctx->cache_free;
		ctx->cache_free = &ctx->cache[i];
	}
	
	dante_context = ctx;
	return UDESK_NO_ERROR;
}

void UDESKAPIENTRY udeskGenObjects(UDenum type, UDint num, UDhandle* dst)
{
	UDint i;
	
	DANTE_IGNORE_IF(!dante_context);
	DANTE_ERROR_IF(num < 0 || dst == NULL, UDESK_INVALID_VALUE);
	
	/* this isn't really the most optimized loop around...
	 * but at least it's simple
	 */
	i = 0;
	while (i < num) {
		DanteObject* obj = danteAllocObject(type);
		UDboolean success = false;
		
		if (obj) {
			dst[i++] = obj->handle;
			
			switch (type) {
			case UDESK_HANDLE_CONTAINER:
			case UDESK_HANDLE_PIXMAP:
			case UDESK_HANDLE_LAYER:
			case UDESK_HANDLE_BAR:
			case UDESK_HANDLE_MENU:
			case UDESK_HANDLE_TIMER:
				/* TODO: STUB! Implement this. */
				break;
			
			case UDESK_HANDLE_WINDOW:
				success = danteWindowInit(obj);
				break;
			
			case UDESK_HANDLE_EVENT:
				success = danteEventInit(obj);
				break;
	
			default:
				/* can only fail on first iteration */
				dante_context->error = UDESK_INVALID_ENUM;
				break;
			}
		
		} else {
			/* object allocation failed */
			dante_context->error = UDESK_OUT_OF_MEMORY;
		}
		
		if (!success) {
			/* free every object allocated this far */
			udeskDeleteObjects(i, dst);
			return;
		}
	}
}

void UDESKAPIENTRY udeskDeleteObjects(UDint num, const UDhandle* buff)
{
	UDint i;
	
	DANTE_IGNORE_IF(!dante_context);
	DANTE_ERROR_IF(num < 0 || buff == NULL, UDESK_INVALID_VALUE);
	
	for (i = 0; i < num; i++) {
		danteUnrefObject(danteGetObject(buff[i]));
	}
}

UDenum UDESKAPIENTRY udeskGetError(void)
{
	UDenum err = UDESK_NO_ERROR;
	
	if (dante_context) {
		err = dante_context->error;
		dante_context->error = UDESK_NO_ERROR;
	}
	
	return err;
}

void UDESKAPIENTRY udeskRegisterHandler(UDhandle handle, UDenum param, UDhandlerproc proc)
{
	DanteObject* obj;
	
	DANTE_IGNORE_IF(!dante_context);
	
	obj = danteGetObject(handle);
	DANTE_ERROR_IF(!obj, UDESK_INVALID_VALUE);
	DANTE_ERROR_IF(!obj->vt->handler, UDESK_INVALID_OPERATION);
	
	obj->vt->handler(obj, param, proc);
}

void UDESKAPIENTRY udeskBegin(UDhandle handle, UDenum mode)
{
	DanteObject* obj;
	
	DANTE_IGNORE_IF(!dante_context);
	
	obj = danteGetObject(handle);
	DANTE_ERROR_IF(!obj, UDESK_INVALID_VALUE);
	DANTE_ERROR_IF(!obj->vt->begin, UDESK_INVALID_OPERATION);
	
	obj->vt->begin(obj, mode);
}

void UDESKAPIENTRY udeskEnd(UDhandle handle)
{
	DanteObject* obj;
	
	DANTE_IGNORE_IF(!dante_context);
	
	obj = danteGetObject(handle);
	DANTE_ERROR_IF(!obj, UDESK_INVALID_VALUE);
	DANTE_ERROR_IF(!obj->vt->end, UDESK_INVALID_OPERATION);
	
	obj->vt->end(obj);
}

void UDESKAPIENTRY udeskFlush(UDhandle handle)
{
	DanteObject* obj;
	
	DANTE_IGNORE_IF(!dante_context);
	
	obj = danteGetObject(handle);
	DANTE_ERROR_IF(!obj, UDESK_INVALID_VALUE);
	
	if (obj->vt->flush) {
		obj->vt->flush(obj);
	}
}

void UDESKAPIENTRY udeskMakeContextCurrent(void)
{
	SDL_Event ev;
	
	DANTE_IGNORE_IF(!dante_context);
	DANTE_ERROR_IF(dante_context->current, UDESK_INVALID_OPERATION);
	
	dante_context->current = true;
	do {
		if (SDL_WaitEvent(&ev)) {
			switch (ev.type) {
			/* window event */
			case SDL_WINDOWEVENT:
				danteHandleWindowEvent(&ev);
				break;
			
			default:
				break;
			}
		}
		
	} while (dante_context->current);
}

void UDESKAPIENTRY udeskMakeContextNone(void)
{
	DANTE_IGNORE_IF(!dante_context);
	DANTE_ERROR_IF(!dante_context->current, UDESK_INVALID_OPERATION);
	
	dante_context->current = false;
}

UDenum UDESKAPIENTRY udeskDestroyContext(void)
{	
	DanteSlice *slice;
	UDint i;
	
	DANTE_IGNORE_AND_RETVAL_IF(!dante_context, UDESK_INVALID_OPERATION);
	
	slice = dante_context->slice.next;
	while (slice->base != UDESK_HANDLE_NONE) {
		DanteSlice* next = slice->next;
		
		for (i = 0; i < DANTE_SLICE_CACHESIZE; i++) {
			DanteObject* obj = &slice->data[i];
			
			if (obj->type != UDESK_NONE) {
				danteUnrefObject(obj);
			}
		}
		
		/* slice is freed when empty */
		slice = next;
	}
	
	for (i = 0; i < DANTE_FAST_OBJECT_CACHE_SIZE; i++) {
		DanteObject* obj = &dante_context->cache[i];
		
		if (obj->type != UDESK_NONE) {
			danteUnrefObject(obj);
		}
	}
	
	free(dante_context);
	SDL_Quit();
	
	dante_context = NULL;
	return UDESK_NO_ERROR;
}
