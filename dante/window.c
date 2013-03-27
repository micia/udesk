/* window.c: udesk window implementation.
 *
 * Implements the window (UDESK_HANDLE_WINDOW) object.
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

/* default window title. */
#define DANTE_WINDOW_TITLE "udesk window"
/* dante window to object property name. */
#define DANTE_WINDOW_OBJECT "dante_object"
/* default window width. */
#define DANTE_WINDOW_WIDTH 320
/* default window height. */
#define DANTE_WINDOW_HEIGHT 240

/* Creates a window renderer, according to the current context.
 * If context requires vsync but it could not be obtained, a
 * renderer with no vertical sync is created.
 * It returns NULL if creation was unsuccessful.
 */
static SDL_Renderer* danteCreateWindowRenderer(SDL_Window* window);
/* Maps the window as specified by the 'mode' parameter,
 * sets the context error accordingly on failure or invalid mode.
 */
static void danteSetWindowMode(SDL_Window* win, UDenum mode);
/* Window event dispatch table handlers. */
static void danteWindowEnterHandler(DanteObject* obj, DanteDispatchID id, DanteObject* ev);
static void danteWindowLeaveHandler(DanteObject* obj, DanteDispatchID id, DanteObject* ev);
static void danteWindowFocusHandler(DanteObject* obj, DanteDispatchID id, DanteObject* ev);
static void danteWindowDrawHandler(DanteObject* obj, DanteDispatchID id, DanteObject* ev);
static void danteWindowMotionHandler(DanteObject* obj, DanteDispatchID id, DanteObject* ev);
static void danteWindowDestroyHandler(DanteObject* obj, DanteDispatchID id, DanteObject* ev);
/* Window virtual table handlers. */
static void danteWindowRegisterHandler(DanteObject* obj, UDenum param, UDhandlerproc proc);
static void danteWindowFlush(DanteObject* obj);
static void danteWindowClear(DanteObject* obj);

static SDL_Renderer* danteCreateWindowRenderer(SDL_Window* window)
{
	SDL_Renderer* ret;
	Uint32 flags;
	
	flags = 0;
	if (dante_context->vsync) {
		flags |= SDL_RENDERER_PRESENTVSYNC;
	}
	if (dante_context->accelerated) {
		flags |= SDL_RENDERER_ACCELERATED;
	}
	
	ret = SDL_CreateRenderer(window, -1, flags);
	if (!ret && (flags & SDL_RENDERER_ACCELERATED) != 0) {
		flags &= ~SDL_RENDERER_ACCELERATED;
		flags |= SDL_RENDERER_SOFTWARE;
		ret = SDL_CreateRenderer(window, -1, flags);
	}
	if (!ret && (flags & SDL_RENDERER_PRESENTVSYNC) != 0) {
		flags &= ~SDL_RENDERER_PRESENTVSYNC;
		ret = SDL_CreateRenderer(window, -1, flags);
	}
	
	return ret;
}

static void danteSetWindowMode(SDL_Window* win, UDint mode)
{
	switch (mode) {
	case UDESK_WINDOW_HIDDEN:
		SDL_HideWindow(win);
		break;
	
	case UDESK_WINDOW_ICONIFIED:
		SDL_ShowWindow(win);
		SDL_MinimizeWindow(win);
		break;
	
	case UDESK_WINDOW_SHOW:
		SDL_ShowWindow(win);
		break;
	
	case UDESK_WINDOW_MAXIMIZED:
		SDL_ShowWindow(win);
		SDL_MaximizeWindow(win);
		break;
	
	default:
		dante_context->error = UDESK_INVALID_VALUE;
		break;
	}
}

static void danteWindowEnterHandler(DanteObject* obj, DanteDispatchID id, DanteObject* ev)
{
	DanteWindowObject* win = &obj->d.win;
	
	if (win->enter) {
		win->enter(ev->handle);
	}
	if (win->child) {
		dantePropagateEvent(id, NULL, win->child);
	}
}

static void danteWindowLeaveHandler(DanteObject* obj, DanteDispatchID id, DanteObject* ev)
{
	DanteWindowObject* win = &obj->d.win;
	
	if (win->leave) {
		win->leave(ev->handle);
	}
	if (win->child) {
		dantePropagateEvent(id, NULL, win->child);
		/* TODO, this should be done only for "focused" child */
	}
}

static void danteWindowFocusHandler(DanteObject* obj, DanteDispatchID id, DanteObject* ev)
{
	/* TODO: stub */
}

static void danteWindowDrawHandler(DanteObject* obj, DanteDispatchID id, DanteObject* ev)
{
	DanteWindowObject* win = &obj->d.win;
	DanteObject* child = win->child;
	
	/* TODO: make the rendering process themeable, a SDL_Renderer
	 * wrapper would be enough.
	 */
	SDL_SetRenderDrawColor(win->render, 128, 128, 128, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(win->render);
	if (child) {
		dantePropagateEvent(id, NULL, child);
	}
	
	danteWindowFlush(obj);
}

static void danteWindowMotionHandler(DanteObject* obj, DanteDispatchID id, DanteObject* ev)
{
	/* TODO: stub, should also perform enter/leave events propagation
	 * it should propagate the event to its child, if any,
	 * to do this, the appropriate region should be selected, according
	 * to the cursor position, notice that the SDL will report enter
	 * events to the Window itself, it has no notion of "our" objects.
	 */
	return;
}

static void danteWindowDestroyHandler(DanteObject* obj, DanteDispatchID id, DanteObject* ev)
{
	DanteWindowObject* win;
	
	(void)id;
	
	win = &obj->d.win;
	if (win->destroy) {
		win->destroy(ev->handle);
	}
}

static void danteWindowRegisterHandler(DanteObject* obj, UDenum param, UDhandlerproc proc)
{
	DanteWindowObject* win = &obj->d.win;
	
	switch (param) {
	case UDESK_EVENT_DESTROY:
		win->destroy = proc;
		break;
	
	case UDESK_EVENT_FOCUS:
		win->focus = proc;
		break;
	
	case UDESK_EVENT_ENTER:
		win->enter = proc;
		break;
	
	case UDESK_EVENT_LEAVE:
		win->leave = proc;
		break;
	
	default:
		dante_context->error = UDESK_INVALID_ENUM;
		break;
	}
}

static void danteWindowFlush(DanteObject* obj)
{
	SDL_RenderPresent(obj->d.win.render);
}

static void danteWindowClear(DanteObject* obj)
{
	DanteWindowObject* win = &obj->d.win;
	
	danteUnrefObject(win->icon);
	danteUnrefObject(win->child);
	SDL_DestroyRenderer(win->render);
	SDL_DestroyWindow(win->swin);
}

UDboolean DANTEAPIENTRY danteWindowInit(DanteObject* obj)
{
	static const DanteVTable win_table = {
		danteWindowRegisterHandler,
		NULL, /* no begin */
		NULL, /* no end */
		danteWindowFlush,
		danteWindowClear
	};
	
	static const DanteEventDispatch dispatch_table = {
		danteWindowEnterHandler,
		danteWindowLeaveHandler,
		danteWindowFocusHandler,
		danteWindowDrawHandler,
		danteWindowDestroyHandler,
		NULL, /* key */
		NULL, /* button */
		danteWindowMotionHandler,
		NULL  /* touch */
	};
	
	DanteWindowObject* win = &obj->d.win;
	SDL_Window* swin = NULL;
	SDL_Renderer* render = NULL;
	
	swin = SDL_CreateWindow(DANTE_WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	                        DANTE_WINDOW_WIDTH, DANTE_WINDOW_HEIGHT, SDL_WINDOW_HIDDEN);
	if (!swin) {
		dante_context->error = UDESK_OPERATION_FAILED;
		goto fail;
	}
	
	render = danteCreateWindowRenderer(swin);
	if (!render) {
		dante_context->error = UDESK_OPERATION_FAILED;
		goto fail;
	}
	
	/* install an userspace identifier to retrieve an object from a
	 * SDL window handle.
	 */
	SDL_SetWindowData(swin, DANTE_WINDOW_OBJECT, obj);
	obj->vt = &win_table;
	obj->dispatch = &dispatch_table;
	win->swin = swin;
	win->render = render;
	return true;

fail:
	if (swin) {
		SDL_DestroyWindow(swin);
	}
	if (render) {
		SDL_DestroyRenderer(render);
	}
	
	return false;
}

DanteObject* DANTEAPIENTRY danteGetObjectFromWindowID(Uint32 id)
{
	SDL_Window* win = SDL_GetWindowFromID(id);
	
	if (!win) {
		return NULL;
	}
	
	return (DanteObject*)SDL_GetWindowData(win, DANTE_WINDOW_OBJECT);
}

void UDESKAPIENTRY udeskWindowChild(UDhandle window, UDhandle child)
{
	/* TODO stub */
}

void UDESKAPIENTRY udeskWindowIcon(UDhandle window, UDhandle icon)
{
	/* TODO stub */
}

void UDESKAPIENTRY udeskSetWindowiv(UDhandle window, UDenum param, const UDint* to)
{
	DanteObject* obj = danteRetrieveObject(window, UDESK_HANDLE_WINDOW);
	
	if (obj) {
		DanteWindowObject* win = &obj->d.win;
		
		switch (param) {
		case UDESK_WINDOW_MODE:
			danteSetWindowMode(win->swin, to[0]);
			break;
		
		default:
			dante_context->error = UDESK_INVALID_ENUM;
			break;
		}
	}
}

void UDESKAPIENTRY udeskSetWindow2i(UDhandle window, UDenum param, UDint x, UDint y)
{
	UDint buff[2];
	
	buff[0] = x;
	buff[1] = y;
	udeskSetWindowiv(window, param, buff);
}

void UDESKAPIENTRY udeskSetWindowi(UDhandle window, UDenum param, UDint x)
{
	udeskSetWindow2i(window, param, x, 0);
}

void UDESKAPIENTRY udeskSetWindowf(UDhandle window, UDenum param, UDfloat f)
{
	/* TODO stub */
}

void UDESKAPIENTRY udeskGetWindowiv(UDhandle window, UDenum param, UDint* dst)
{
	/* TODO stub */
}

void UDESKAPIENTRY udeskSetWindowString(UDhandle window, UDenum param, const char* to)
{
	DanteObject* obj = danteRetrieveObject(window, UDESK_HANDLE_WINDOW);
	
	if (obj) {
		DANTE_ERROR_IF(!to, UDESK_INVALID_VALUE);
	
		switch (param) {
		case UDESK_WINDOW_TITLE:
			SDL_SetWindowTitle(obj->d.win.swin, to);
			return;
		
		default:
			dante_context->error = UDESK_INVALID_ENUM;
			return;
		}
	}
}

const char* UDESKAPIENTRY udeskGetWindowString(UDhandle window, UDenum param)
{
	DanteObject* obj = danteRetrieveObject(window, UDESK_HANDLE_WINDOW);
	
	if (!obj) {
		return NULL;
	}
	
	switch (param) {
	case UDESK_WINDOW_TITLE:
		return SDL_GetWindowTitle(obj->d.win.swin);
	
	default:
		dante_context->error = UDESK_INVALID_ENUM;
		return NULL;
	}
}

UDhandle UDESKAPIENTRY udeskGetWindowHandle(UDhandle window, UDenum param)
{
	DanteObject* obj;
	DanteWindowObject* win;
	
	obj = danteRetrieveObject(window, UDESK_HANDLE_WINDOW);
	if (!obj) {
		return UDESK_HANDLE_NONE;
	}
	
	win = &obj->d.win;
	switch (param) {
	case UDESK_WINDOW_ICON:
		return (win->icon)? win->icon->handle : UDESK_HANDLE_NONE;
	
	case UDESK_WINDOW_CHILD:
		return (win->child)? win->child->handle : UDESK_HANDLE_NONE;
	
	default:
		dante_context->error = UDESK_INVALID_ENUM;
		return UDESK_HANDLE_NONE;
	}
}

UDboolean UDESKAPIENTRY udeskIsWindow(UDhandle handle)
{
	return danteCheckObjectType(handle, UDESK_HANDLE_WINDOW);
}
