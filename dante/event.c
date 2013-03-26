/* event.c: Event handling routines.
 *
 * Implements unified event handling routines for dante. 
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

/* Extracts an udesk timestamp from an SDL event, since SDL
 * doesn't provide a timestamp into the common event structure,
 * this is done with a switch.
 */
static UDint danteGetEventTimestamp(const SDL_Event* ev);
/* Event virtual table handlers. */
static void danteEventBegin(DanteObject* self, UDenum type);
static void danteEventEnd(DanteObject* obj);
static void danteEventFlush(DanteObject* obj);
static void danteEventClear(DanteObject* self);

static UDint danteGetEventTimestamp(const SDL_Event* ev)
{
	Uint32 stamp;
	
	switch (ev->type) {
	case SDL_QUIT:
		stamp = ev->quit.timestamp;
		break;
	
	case SDL_WINDOWEVENT:
		stamp = ev->window.timestamp;
		break;
	
	case SDL_SYSWMEVENT:
		stamp = ev->syswm.timestamp;
		break;
	
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		stamp = ev->key.timestamp;
		break;
	
	case SDL_TEXTEDITING:
		stamp = ev->edit.timestamp;
		break;
	
	case SDL_TEXTINPUT:
		stamp = ev->text.timestamp;
		break;
	
	case SDL_MOUSEMOTION:
		stamp = ev->motion.timestamp;
		break;
	
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		stamp = ev->button.timestamp;
		break;
	
	case SDL_MOUSEWHEEL:
		stamp = ev->wheel.timestamp;
		break;
	
	case SDL_JOYAXISMOTION:
		stamp = ev->jaxis.timestamp;
		break;
	
	case SDL_JOYBALLMOTION:
		stamp = ev->jball.timestamp;
		break;
	
	case SDL_JOYHATMOTION:
		stamp = ev->jhat.timestamp;
		break;
	
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
		stamp = ev->jbutton.timestamp;
		break;
	
	case SDL_CONTROLLERAXISMOTION:
		stamp = ev->caxis.timestamp;
		break;
	
	case SDL_CONTROLLERBUTTONDOWN:
	case SDL_CONTROLLERBUTTONUP:
		stamp = ev->cbutton.timestamp;
		break;
	
	case SDL_FINGERDOWN:
	case SDL_FINGERUP:
	case SDL_FINGERMOTION:
		stamp = ev->tfinger.timestamp;
		break;
	
	case SDL_DOLLARGESTURE:
	case SDL_DOLLARRECORD:
		stamp = ev->dgesture.timestamp;
		break;
	
	case SDL_MULTIGESTURE:
		stamp = ev->mgesture.timestamp;
		break;
	
	case SDL_DROPFILE:
		stamp = ev->drop.timestamp;
		break;
	
	case SDL_USEREVENT:
		stamp = ev->user.timestamp;
		break;
	
	case SDL_JOYDEVICEADDED:
	case SDL_JOYDEVICEREMOVED:
	case SDL_CONTROLLERDEVICEADDED:
	case SDL_CONTROLLERDEVICEREMOVED:
		/* these should be handled by Dante... */
	case SDL_CLIPBOARDUPDATE:
		/* this one apparently has no timestamp or event structure. */
	default:
		/* should never happen */
		stamp = 0;
		break;
	}
	
	/* it could overflow... */
	return (UDint)stamp;
}

static void danteEventBegin(DanteObject* self, UDenum type)
{
	/* TODO stub */
}

static void danteEventEnd(DanteObject* obj)
{
	/* TODO stub */	
}

static void danteEventFlush(DanteObject* obj)
{
	/* TODO stub, should send the event here */
}

static void danteEventClear(DanteObject* self)
{
	if (self->handle == dante_context->ev->handle) {
		/* a handler is deleting the current event,
		 * mark the event handling as complete.
		 */
		dante_context->ev = NULL;
	}
}

void DANTEAPIENTRY danteGenerateFrom(const SDL_Event* sev, UDenum type)
{ 
	DanteObject* obj;
	
	obj = danteAllocObject(UDESK_HANDLE_EVENT);
	if (obj) {
		DanteEventObject* ev = &obj->d.ev;
		
		ev->type = type;
		ev->building = false;
		ev->valid = true;
		ev->sent = true;
		ev->sev = *sev;
	}
	
	dante_context->ev = obj;
}

void DANTEAPIENTRY dantePropagateEvent(DanteDispatchID id, DanteObject* from, DanteObject* to)
{
	DanteObject* obj = dante_context->ev;
	
	if (obj) {
		if (to->dispatch) {
			DanteHandlerproc handler = DANTE_DISPATCH_HANDLER(to->dispatch, id);
		
			if (handler) {
				DanteEventObject* ev = &obj->d.ev;
				
				ev->from = from;
				ev->to = to;
				handler(to, id, obj);
			}
		}
	}
}

void DANTEAPIENTRY danteFinishEvent(void)
{
	if (dante_context->ev) {
		danteUnrefObject(dante_context->ev);
		dante_context->ev = NULL;
	}
}

void DANTEAPIENTRY danteHandleWindowEvent(const SDL_Event* ev)
{
	const SDL_WindowEvent* wev;
	DanteObject* to;
	
	wev = &ev->window;
	to = danteGetObjectFromWindowID(wev->windowID);
	if (!to) {
		/* unknown receiver, discard event (should not happen) */
		return;
	}
	
	switch (wev->event) {
	case SDL_WINDOWEVENT_FOCUS_GAINED:
	case SDL_WINDOWEVENT_FOCUS_LOST:
		danteGenerateFrom(ev, UDESK_EVENT_FOCUS);
		dantePropagateEvent(DANTE_FOCUS_DISPATCH_ID, NULL, to);
		break;
	
	case SDL_WINDOWEVENT_ENTER:
		danteGenerateFrom(ev, UDESK_EVENT_ENTER);
		dantePropagateEvent(DANTE_ENTER_DISPATCH_ID, NULL, to);
		break;
		
	case SDL_WINDOWEVENT_LEAVE:
		danteGenerateFrom(ev, UDESK_EVENT_LEAVE);
		dantePropagateEvent(DANTE_LEAVE_DISPATCH_ID, NULL, to);
		break;
	
	case SDL_WINDOWEVENT_CLOSE:
		danteGenerateFrom(ev, UDESK_EVENT_DESTROY);
		dantePropagateEvent(DANTE_DESTROY_DISPATCH_ID, NULL, to);
		break;
	
	case SDL_WINDOWEVENT_SHOWN:
	case SDL_WINDOWEVENT_MAXIMIZED:
	case SDL_WINDOWEVENT_RESIZED:
	case SDL_WINDOWEVENT_RESTORED:
	case SDL_WINDOWEVENT_EXPOSED:
		danteGenerateFrom(ev, UDESK_EVENT_DRAW);
		dantePropagateEvent(DANTE_DRAW_DISPATCH_ID, NULL, to);
		break;
	
	default:
		/* unhandled event type, discard */
		return;
	}
	
	danteFinishEvent();
}

UDboolean DANTEAPIENTRY danteEventInit(DanteObject* obj)
{
	static const DanteVTable ev_table = {
		NULL,
		danteEventBegin,
		danteEventEnd,
		danteEventFlush,
		danteEventClear
	};
	
	obj->vt = &ev_table;
	return true;
}

void UDESKAPIENTRY udeskGetEventiv(UDhandle event, UDenum param, UDint* dst)
{
	DanteObject* obj;
	DanteEventObject* ev;
	
	obj = danteRetrieveObject(event, UDESK_HANDLE_EVENT);
	if (!obj) {
		return;
	}
	
	ev = &obj->d.ev;
	DANTE_ERROR_IF(!ev->valid, UDESK_INVALID_OPERATION);
	
	switch (param) {
	case UDESK_EVENT_TYPE:
		dst[0] = (UDint)ev->type;
		break;
	
	case UDESK_EVENT_TIMESTAMP:
		dst[0] = danteGetEventTimestamp(&ev->sev);
		break;
	
	default:
		dante_context->error = UDESK_INVALID_ENUM;
		return;
	}
}

UDhandle UDESKAPIENTRY udeskGetEventHandle(UDhandle event, UDenum param)
{
	DanteObject* obj;
	DanteEventObject* ev;
	
	obj = danteRetrieveObject(event, UDESK_HANDLE_EVENT);
	if (!obj) {
		return UDESK_HANDLE_NONE;
	}
	
	ev = &obj->d.ev;
	DANTE_ERROR_AND_RETVAL_IF(!ev->valid, UDESK_INVALID_OPERATION, UDESK_HANDLE_NONE);
	
	switch (param) {
	case UDESK_EVENT_SENDER:
		if (ev->from) {
			return ev->from->handle;
		} else {
			return UDESK_HANDLE_NONE;
		}
	
	case UDESK_EVENT_DESTINATION:
		return ev->to->handle;
	
	default:
		dante_context->error = UDESK_INVALID_ENUM;
		return UDESK_HANDLE_NONE;
	}
}

UDboolean UDESKAPIENTRY udeskIsEvent(UDhandle handle)
{
	return danteCheckObjectType(handle, UDESK_HANDLE_EVENT);
}
