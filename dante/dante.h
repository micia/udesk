/* dante.h: Dante SDL udesk implementation local header.
 *
 * This file contains every local declaration and function for the
 * Dante library modules any local function or symbol must have
 * the 'dante' prefix for functions or variables, 'Dante' for
 * types and structures.
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
 
#ifndef DANTE_H_
#define DANTE_H_
#include <udesk/udesk.h>
#include <SDL.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/* we want a version string from our build */
#ifndef VERSION
#error Please specify a VERSION string
#endif

/* perform some paranoid checks for STDC and byte width */
#ifndef __STDC__
#error ANSI C89 capable compiler is required
#endif
#if (CHAR_BIT != 8)
#error This code assumes 8 bit bytes
#endif

/* ensure a C99 compatible set of boolean types. */
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
/* use stdbool.h for true and false */
#include <stdbool.h>

#else
/* hand craft a stdbool.h compatible definition, we use FDboolean for bool */
#ifndef __cplusplus

#ifdef bool
#undef bool
#endif
#ifdef __bool_true_false_are_defined
#undef __bool_true_false_are_defined
#endif

typedef UDboolean _Bool
#define bool _Bool
#define true 1
#define false 0
#define __bool_true_false_are_defined 1

#endif /* __cplusplus */

#endif

/* Generic object type, it holds any information necessary to
 * identify and manage a generic object, as well as any object
 * specific data.
 */
typedef struct DanteObject_s {
	/* object type, any of UDESK_HANDLE constants.
	 * If this object is free, this field is UDESK_NONE.
	 */
	UDenum type;
	/* slice this object belongs to. */
	struct DanteSlice_s* slice;
	/* object specific data. */
	union {
		/* If the object is free, this field is used to
		 * contain the address to the next free object in
		 * the free list.
		 */
		DanteNoneObject none;
	}d;
} DanteObject;

/* Memory object cache, it defines a simple memory slice with a
 * DanteObject buffer.
 * Memory is managed in such a way that ensures quick allocations
 * and deallocations reducing malloc() calls.
 * A slice is freed when it is empty, which helps keeping
 * a reasonable memory pressure on the OS.
 */
typedef struct DanteSlice_s {
	/* base handle value, handles in this slice range in the interval:
	 * [base, base + DANTE_SLICE_SIZE)
	 */
	UDhandle base;
	/* how many objects in this slice are being used, if 0 then
	 * this slice is not being used and could be deallocated.
	 */
	int used;
	/* next slice in list. */
	struct DanteSlice_s* next;
	/* previous slice in list. */
	struct DanteSlice_s* prev;
	/* next free slice in list. */
	struct DanteSlice_s* next_free;
	/* previous free slice in list. */
	struct DanteSlice_s* prev_free;
	/* first free object in this slice. */
	DanteObject* first_free;
	/* object buffer, for allocated slices
	 * it's DANTE_SLICE_CACHESIZE elements wide.
	 */
	DanteObject data[1];
} DanteSlice;

/* This macro defines how large an object cache should be. */
#define DANTE_SLICE_CACHESIZE 32
/* This macro defines the DanteSlice data structure size.
 * Since the data field size is dynamic, sizeof() would report an
 * invalid size to malloc().
 */
#define DANTE_SLICE_SIZE (offsetof(DanteSlice, data[0]) + DANTE_SLICE_CACHESIZE * sizeof(DanteObject))

/* DanteContext defines the context type. According to udesk,
 * this type manages every object allocated with udeskGenObjects(),
 * it also manages the event loop and stores the last error
 * encountered.
 */
typedef struct DanteContext_s {
	/* last error. */
	UDenum error;
	/* slices managed by this context. */
	DanteSlice slice;
} DanteContext;

/* Global dante context handle, NULL if no context has been
 * created yet.
 */
extern DanteContext* dante_context;

/* convenience macros */

/* Safe macros that ignore the function call either returning
 * or returning a value if a specific condition is verified.
 * These macros should only be used at the beginning of an exported
 * (UDESKAPIENTRY) function, typically to ensure a context exists.
 */
#define DANTE_IGNORE_IF(cond) do { \
		if (cond) { \
			return; \
		} \
	} while (0)
#define DANTE_IGNORE_AND_RETVAL_IF(cond, val) do { \
		if (cond) { \
			return (val); \
		} \
	} while (0)

/* Safe macros that set the context error to a certain value and
 * return if a certain condition is verified.
 * One of the macros simply return to the caller, while the other
 * version returns a value.
 */
#define DANTE_ERROR_IF(cond, err) do { \
		if (cond) { \
			dante_context->error = (err); \
			return; \
		} \
	} while (0)
#define DANTE_ERROR_AND_RETVAL_IF(cond, err, val) do { \
		if (cond) { \
			dante_context->error = (err); \
			return (val); \
		} \
	} while (0)

/* Allocates a cleared object of the udesk type 'type', returning
 * it on success, it returns NULL on out of memory condition.
 * No check is performed to ensure that the required type is legal,
 * this is left to the caller.
 */
DanteObject* danteAllocObject(UDenum type);
/* Returns the object identified by handle, NULL if handle is
 * invalid.
 */
DanteObject* danteGetObject(UDhandle handle);
/* Ensures that the object specified by 'handle' has type 'type',
 * if the context hasn't been yet created, it returns false,
 * it also returns false on invalid handle value.
 * This function is intended as a convenience function to
 * implement the udeskIs'type'('handle') function family.
 */ 
UDboolean danteCheckObjectType(UDhandle handle, UDenum type);
/* Frees the object 'obj', making it unusable.
 * If 'obj' is NULL effect is undefined.
 */
void danteFreeObject(DanteObject* obj);

#ifdef __cplusplus
}
#endif

#endif /* DANTE_H_ */
