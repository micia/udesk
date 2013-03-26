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
#include <stddef.h>

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
/* hand craft a stdbool.h compatible definition, we use UDboolean for bool */
#ifndef __cplusplus

#ifdef bool
#undef bool
#endif
#ifdef __bool_true_false_are_defined
#undef __bool_true_false_are_defined
#endif

#define bool UDboolean
#define true 1
#define false 0
#define __bool_true_false_are_defined 1

#endif /* __cplusplus */

#endif

#if (defined(__GNUC__) && __GNUC__ >= 4)
/* optimize generated DLL by reducing the exported functions */
#define DANTEAPI extern __attribute__((visibility("hidden")))
#else
#define DANTEAPI extern
#endif

/* compatibility with udesk style declarations */
#define DANTEAPIENTRY

/* VSync environment variable name that defines whether Dante
 * should enable vsync (if possible).
 */
#define DANTE_ENV_VSYNC "DANTE_VSYNC"
/* Accelerated environment variable, defines whether Dante should
 * try to use video acceleration for draw operations
 * (whenever possible).
 */
#define DANTE_ENV_ACCELERATED "DANTE_ACCELERATED"

/* environment variables are sorted by priority,
 * for example vsync has higher priority than acceleration.
 */

/* struct DanteObject_s forward declaration,
 * to use within function pointers' signature.
 */ 
struct DanteObject_s;
 
/* Virtual table, implemented by every dante object, it is used
 * to implement the common set of functions provided by every object.
 */
typedef struct DanteVTable_s {
	/* implements the udeskRegisterHandler() function,
	 * NULL if none is provided by this object.
	 */
	void (*handler)(struct DanteObject_s* self, UDenum param, UDhandlerproc proc);
	/* implements the udeskBegin() function, NULL if none is provided
	 * for this object.
	 */
	void (*begin)(struct DanteObject_s* self, UDenum mode);
	/* implements the udeskEnd() function, NULL if none is provided
	 * for this object.
	 */
	void (*end)(struct DanteObject_s* self);
	/* implements the udeskFlush() function, NULL if flush operations
	 * should be ignored for this object.
	 */
	void (*flush)(struct DanteObject_s* self);
	/* frees any resource specific memory allocated on object
	 * initialization. NULL if no object specific memory is used
	 * by this object.
	 */
	void (*clear)(struct DanteObject_s* self);
} DanteVTable;

/* Handler dispatcher identifier, used to cache an handler resolution result. */
typedef size_t DanteDispatchID;
/* Dante specific optimized event handlers for an event. */
typedef void (*DanteHandlerproc)(struct DanteObject_s* self, DanteDispatchID id, struct DanteObject_s* ev);

/* Dante specific event dispatcher table, every object may handle
 * events in different ways, if any entry is NULL, the object doesn't
 * handle the specified event.
 */
typedef struct DanteEventDispatch_s {
	/* event triggered when a region is entered by the cursor. */
	DanteHandlerproc enter;
	/* event triggered when a region is left by the cursor. */
	DanteHandlerproc leave;
	/* event triggered when an object receives or loses focus. */
	DanteHandlerproc focus;
	/* event triggered when a region needs to be redrawn. */
	DanteHandlerproc draw;
	/* event triggered when an object is destroyed (window close). */
	DanteHandlerproc destroy;
	/* event triggered when a key is pressed or released. */
	DanteHandlerproc key;
	/* event triggered when a mouse or joystick button is pressed or released. */
	DanteHandlerproc button;
	/* mouse motion event. */
	DanteHandlerproc motion;
	/* touchscreen motion/pressure event. */
	DanteHandlerproc touch;
} DanteEventDispatch;

/* Dispatch table events identifier, for cached handler resolution. */
#define DANTE_ENTER_DISPATCH_ID   offsetof(DanteEventDispatch, enter)
#define DANTE_LEAVE_DISPATCH_ID   offsetof(DanteEventDispatch, leave)
#define DANTE_FOCUS_DISPATCH_ID   offsetof(DanteEventDispatch, focus)
#define DANTE_DRAW_DISPATCH_ID    offsetof(DanteEventDispatch, draw)
#define DANTE_DESTROY_DISPATCH_ID offsetof(DanteEventDispatch, destroy)
#define DANTE_KEY_DISPATCH_ID     offsetof(DanteEventDispatch, key)
#define DANTE_BUTTON_DISPATCH_ID  offsetof(DanteEventDispatch, button)
#define DANTE_MOTION_DISPATCH_ID  offsetof(DanteEventDispatch, motion)
#define DANTE_TOUCH_DISPATCH_ID   offsetof(DanteEventDispatch, touch)

/* Extracts an handler from a dispatch table and an handler identifier. */
#define DANTE_DISPATCH_HANDLER(table, id) (*(DanteHandlerproc*)((unsigned char*)(table) + (id)))

/* Empty (freed) object type. */
typedef struct DanteNoneObject_s {
	/* next free object in the same slice, used for the free
	 * object list in the slice.
	 */
	struct DanteObject_s* next;
} DanteNoneObject;

/* Window object type. */
typedef struct DanteWindowObject_s {
	/* actual SDL window handler. */
	SDL_Window* sdl_rc;
	/* SDL window renderer for 'sdl_rc'. */
	SDL_Renderer* render;
	/* window icon, NULL if default icon is used. */
	struct DanteObject_s* icon;
	/* window child, NULL if window has no child. */
	struct DanteObject_s* child;
	/* user defined enter event handler, might be NULL. */
	UDhandlerproc enter;
	/* user defined focus event handler, might be NULL. */
	UDhandlerproc focus;
	/* user defined leave event handler, might be NULL. */
	UDhandlerproc leave;
	/* user defined resize event handler, might be NULL. */
	UDhandlerproc resize;
	/* user defined destroy event handler, might be NULL. */
	UDhandlerproc destroy;
} DanteWindowObject;

/* Event object type. */
typedef struct DanteEventObject_s {
	/* udesk event type. */
	UDenum type;
	/* true if the event should be propagated to object children. */
	UDboolean propagates;
	/* true if a begin has been called on this event. */
	UDboolean building;
	/* true if this event is valid and could be sent. */
	UDboolean valid;
	/* true if this event has been already sent. */
	UDboolean sent;
	/* dante sender object (NULL if system event). */
	struct DanteObject_s* from;
	/* dante receiver object. */
	struct DanteObject_s* to;
	/* original SDL event. */
	SDL_Event sev;
} DanteEventObject;

/* Generic object type, it holds any information necessary to
 * identify and manage a generic object, as well as any object
 * specific data.
 */
typedef struct DanteObject_s {
	/* object type, any of UDESK_HANDLE constants.
	 * If this object is free, this field is UDESK_NONE.
	 */
	UDenum type;
	/* handle to this object, useful for comparing purposes. */
	UDhandle handle;
	/* reference count to this object. */
	UDint refs;
	/* slice this object belongs to. */
	struct DanteSlice_s* slice;
	/* object specific virtual table. */
	const DanteVTable* vt;
	/* object specific event dispatch table. */
	const DanteEventDispatch* dispatch;
	/* object parent, NULL if this is a root object */
	struct DanteObject_s* parent;
	/* object specific data. */
	union {
		/* If the object is free, this field is used to
		 * contain the address to the next free object in
		 * the free list.
		 */
		DanteNoneObject none;
		/* UDESK_HANDLE_WINDOW, window object data. */
		DanteWindowObject win;
		/* UDESK_HANDLE_EVENT event object data. */
		DanteEventObject ev;
		/* TODO implement other objects. */
	} d;
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
	 * [base, base + DANTE_SLICE_CACHESIZE)
	 */
	UDhandle base;
	/* how many objects in this slice are being used, if 0 then
	 * this slice is not being used and could be deallocated.
	 */
	UDint used;
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

/* Defines how many slots should be reserved for fast static
 * objects cache.
 */
#define DANTE_FAST_OBJECT_CACHE_SIZE 128

/* DanteContext defines the context type. According to udesk,
 * this type manages every object allocated with udeskGenObjects(),
 * it also manages the event loop and stores the last error
 * encountered.
 */
typedef struct DanteContext_s {
	/* last error. */
	UDenum error;
	/* true if this context is current, false otherwise. */
	UDboolean current;
	/* true if context should use VSync to render, on context creation
	 * this field is set accordingly to the value of DANTE_ENV_VSYNC
	 * environment variable, by default this is true.
	 */
	UDboolean vsync;
	/* true if drawing operation should be accelerated whenever possible,
	 * on context creation this field is set accordingly to the
	 * DANTE_ENV_ACCELERATED environment variable, by default it is
	 * true.
	 */
	UDboolean accelerated;
	/* currently handled event, NULL if no udesk
	 * event is being handled.
	 */
	DanteObject* ev;
	/* Free static object cache list, if empty even frequently generated
	 * object will fall back to slice memory.
	 */
	DanteObject* cache_free;
	/* slices managed by this context. */
	DanteSlice slice;
	/* This is the fast static cache buffer used for frequently
	 * generated and deleted objects, such as events.
	 * Handles are managed in the following way:
	 * 
	 * [1, DANTE_FAST_OBJECT_CACHE_SIZE] = static objects, have NULL slice field
	 * [DANTE_FAST_OBJECT_CACHE_SIZE, ...] = slice memory allocation.
	 */
	DanteObject cache[DANTE_FAST_OBJECT_CACHE_SIZE];
} DanteContext;

/* Global dante context handle, NULL if no context has been
 * created yet.
 */
DANTEAPI DanteContext* dante_context;

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
 * A newly allocated object has a reference count of one.
 */
DANTEAPI DanteObject* DANTEAPIENTRY danteAllocObject(UDenum type);
/* Returns the object identified by handle, NULL if handle is
 * invalid.
 */
DANTEAPI DanteObject* DANTEAPIENTRY danteGetObject(UDhandle handle);
/* Ensures that the object specified by 'handle' has type 'type',
 * if the context hasn't been yet created, it returns false,
 * it also returns false on invalid handle value.
 * This function is intended as a convenience function to
 * implement the udeskIs'type'('handle') function family.
 */ 
DANTEAPI UDboolean DANTEAPIENTRY danteCheckObjectType(UDhandle handle, UDenum type);
/* Retrieves an object of a specific type from 'handle', returning
 * it on success, it returns NULL on non-existing udesk context,
 * invalid handle or invalid object type, setting an appropriate error
 * whenever necessary.
 * This function is intended to reduce the number of check operations
 * necessary for every object specific function in udesk.
 */
DANTEAPI DanteObject* DANTEAPIENTRY danteRetrieveObject(UDhandle handle, UDenum type);
/* Adds a reference to the specified object, if 'obj' is NULL this
 * function has no effect.
 * The function returns its argument for convenience purposes.
 */
DANTEAPI DanteObject* DANTEAPIENTRY danteRefObject(DanteObject* obj);
/* Decreases reference count for the specified object, if 'obj' is NULL
 * this function has no effect.
 * Once the reference count reaches zero, the object is deallocated
 * and becomes invalid.
 */
DANTEAPI void DANTEAPIENTRY danteUnrefObject(DanteObject* obj);

/* Handles the specified SDL window event.
 * The SDL 'ev' type must be SDL_WINDOWEVENT, if 'ev' is NULL effects are
 * undefined.
 */
DANTEAPI void DANTEAPIENTRY danteHandleWindowEvent(const SDL_Event* ev);
/* Generates a dante event from an existing SDL event of the udesk type 'type'.
 * The SDL event must not be NULL and the type must be correct, such
 * requirements must be met by the caller.
 * SDL event data is copied into the new event.
 * The newly allocated event becomes the current context event, subsequent
 * current event related functions will implicitly reference it, if
 * the event allocation failed, the current event is set to NULL and
 * subsequent event propagation requests are silently ignored.
 */
DANTEAPI void DANTEAPIENTRY danteGenerateFrom(const SDL_Event* sev, UDenum type);
/* Propagates the current context event to a new object, the event
 * is propagated from the 'from' object to the 'to' object, system
 * generated events typically have a NULL 'from' object, also the 'from'
 * field is unlikely to change during event propagation.
 * The dispatch identifier is used to resolve event handler routine and
 * must be retrieved from the original SDL event.
 */
DANTEAPI void DANTEAPIENTRY dantePropagateEvent(DanteDispatchID id, DanteObject* from, DanteObject* to);
/* Finalizes event propagation (if necessary) and resets the current
 * context event to NULL.
 */
DANTEAPI void DANTEAPIENTRY danteFinishEvent(void);
/* Initializes an UDESK_HANDLE_EVENT object and
 * registers its virtual table.
 * It returns true on success, false otherwise,
 * a context error is set appropriately on failure.
 */
DANTEAPI UDboolean DANTEAPIENTRY danteEventInit(DanteObject* obj);

/* Initializes an UDESK_HANDLE_WINDOW object and
 * registers its virtual table.
 * It returns true on success, false otherwise,
 * a context error is set appropriately on failure.
 */
DANTEAPI UDboolean DANTEAPIENTRY danteWindowInit(DanteObject* obj);
/* Retrieves a window object from a SDL window identifier,
 * it returns NULL if the SDL window identifier isn't valid or
 * if it doesn't have a dante object attached to it.
 */
DANTEAPI DanteObject* DANTEAPIENTRY danteGetObjectFromWindowID(Uint32 id);

#ifdef __cplusplus
}
#endif

#endif /* DANTE_H_ */
