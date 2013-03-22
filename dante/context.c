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

/* Walks the slice list finding the first empty slot, a new
 * slice is allocated into it, inizialized and returned.
 * If allocation fails an appropriate error is set into the context.
 */
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
	
	prev = &dante_context->slice;
	next = dante_context->slice.next;
	base = 1;
	while (next->base != UDESK_HANDLE_NONE) {
		if (next->base != base) {
			break;
		}
		
		prev = next;
		next = next->next;
		base += DANTE_SLICE_SIZE;
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
	ret->first_free = NULL;
	for (i = 0; i < DANTE_SLICE_CACHESIZE; i++) {
		DanteObject* obj = &ret->data[i];
		
		obj->type = UDESK_NONE;
		obj->slice = ret;
		obj->d.none.next = ret->first_free;
		ret->first_free = obj;
	}
	
	return ret;
}

DanteObject* danteAllocObject(UDenum type)
{
	DanteSlice* slice;
	DanteObject* obj;
	
	slice = dante_context->slice.next_free;
	if (!slice) {
		/* no free slices left, allocate a new one */
		slice = danteAllocSlice();
		if (!slice) {
			return NULL;
		}
	}
	
	obj = slice->first_free;
	slice->first_free = obj->d.none.next;
	slice->used++;
	if (slice->used == DANTE_SLICE_SIZE) {
		/* slice is full, remove from free list */
		dante_context->slice.next_free = slice->next_free;
		dante_context->slice.prev_free = slice->prev_free;
	}
	
	obj->type = type;
	return obj;
}

DanteObject* danteGetObject(UDhandle handle)
{
	DanteSlice* slice;
	
	for (slice = dante_context->slice.next; slice->base != UDESK_HANDLE_NONE; slice = slice->next) {
		if (handle < slice->base + DANTE_SLICE_SIZE) {
			break;
		}
	}
	
	if (slice->base == UDESK_HANDLE_NONE || handle < slice->base) {
		return NULL;
	}
	
	return &slice->data[handle - slice->base];
}

void danteFreeObject(DanteObject* obj)
{
	DanteSlice* slice = obj->slice;
	
	obj->type = UDESK_NONE;
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
}

UDenum UDESKAPIENTRY udeskCreateContext(int* argc, char** argv[])
{
	DanteContext* ctx;
	
	DANTE_IGNORE_AND_RETVAL_IF(!argc || !argv || *argc <= 0 || !*argv[0], UDESK_INVALID_VALUE);
	DANTE_IGNORE_AND_RETVAL_IF(dante_context, UDESK_INVALID_OPERATION);
	
	ctx = (DanteContext*)malloc(sizeof(*ctx));
	if (!ctx) {
		return UDESK_OUT_OF_MEMORY;
	}
	
	memset(ctx, 0, sizeof(*ctx));
	ctx->error = UDESK_NO_ERROR;
	ctx->slice.base = UDESK_HANDLE_NONE;
	ctx->slice.used = 0;
	ctx->slice.next = &ctx->slice;
	ctx->slice.prev = &ctx->slice;
	ctx->slice.next_free = &ctx->slice;
	ctx->slice.prev_free = &ctx->slice;
	
	dante_context = ctx;
	return UDESK_NO_ERROR;
}

void UDESKAPIENTRY udeskGenObjects(UDenum type, UDint num, UDhandle* dst)
{
	DANTE_IGNORE_IF(!dante_context);
	DANTE_ERROR_IF(num < 0 || dst == NULL, UDESK_INVALID_VALUE);
	
	switch (type) {
	case UDESK_HANDLE_WINDOW:
	case UDESK_HANDLE_CONTAINER:
	case UDESK_HANDLE_PIXMAP:
	case UDESK_HANDLE_LAYER:
	case UDESK_HANDLE_BAR:
	case UDESK_HANDLE_MENU:
	case UDESK_HANDLE_EVENT:
	case UDESK_HANDLE_TIMER:
		/* TODO: STUB! Implement this. */
		break;
	
	default:
		dante_context->error = UDESK_INVALID_ENUM;
		return;
	}
}

void UDESKAPIENTRY udeskDeleteObjects(UDint num, const UDhandle* buff)
{
	UDint i;
	
	DANTE_IGNORE_IF(!dante_context);
	DANTE_ERROR_IF(num < 0 || buff == NULL, UDESK_INVALID_VALUE);
	
	for (i = 0; i < num; i++) {
		DanteObject* obj = danteGetObject(buff[i]);
		if (obj) {
			danteFreeObject(obj);
		}
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

UDenum UDESKAPIENTRY udeskDestroyContext(void)
{	
	DANTE_IGNORE_AND_RETVAL_IF(!dante_context, UDESK_INVALID_OPERATION);
	
	while (dante_context->slice.next->base != UDESK_HANDLE_NONE) {
		UDint i;
		DanteSlice* slice;
		
		slice = dante_context->slice.next;
		for (i = 0; i < DANTE_SLICE_CACHESIZE; i++) {
			DanteObject* obj = &slice->data[i];
			
			if (obj->type != UDESK_NONE) {
				danteFreeObject(obj);
			}
		}
		
		/* slice is freed when empty */
	}
	
	free(dante_context);
	dante_context = NULL;
	return UDESK_NO_ERROR;
}
