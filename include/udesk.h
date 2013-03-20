/* udesk.h: The Unix Desktop Environment interface.
 *
 * portable desktop environment interface core functions and objects.
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

#ifndef UDESK_H_
#define UDESK_H_

#include "udeskplatform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==========
 * Global values
 */

/* placed at the end of the available range: [0x7e00, 0x7fff] */
enum {

  /* Constant that indicates a `none' value, guaranteed to be
   * different than any other enumerated value.
   */
  UDESK_NONE = 0x7e00,
#define UDESK_NONE       UDESK_NONE

  /* Best choice for speed. */
  UDESK_FASTEST = 0x7e01,
#define UDESK_FASTEST    UDESK_FASTEST

  /* Best choice for quality. */
  UDESK_NICEST = 0x7e02,
#define UDESK_NICEST     UDESK_NICEST

  /* Horizontal orientation identifier. */
  UDESK_HORIZONTAL = 0x7e03,
#define UDESK_HORIZONTAL UDESK_HORIZONTAL

  /* Vertical orientation identifier. */
  UDESK_VERTICAL = 0x7e04,
#define UDESK_VERTICAL   UDESK_VERTICAL

  /* Value that represents the RGBA image format, 8 bits per channel. */
  UDESK_RGBA = 0x7e05,
#define UDESK_RGBA       UDESK_RGBA

  /* ==========
   * Handle types,
   * placed at offset end to keep meaningful sorting
   */

  /* Window object identifier. */
  UDESK_HANDLE_WINDOW = 0x7f00,
#define UDESK_HANDLE_WINDOW    UDESK_HANDLE_WINDOW

  /* Container object identifier. */
  UDESK_HANDLE_CONTAINER = 0x7f01,
#define UDESK_HANDLE_CONTAINER UDESK_HANDLE_CONTAINER

  /* Button object identifier. */
  UDESK_HANDLE_BUTTON = 0x7f02,
#define UDESK_HANDLE_BUTTON    UDESK_HANDLE_BUTTON

  /* Generic pixel storage identifier. */
  UDESK_HANDLE_PIXMAP = 0x7f03,
#define UDESK_HANDLE_PIXMAP    UDESK_HANDLE_PIXMAP

  /* Image layer identifier. */
  UDESK_HANDLE_LAYER = 0x7f04,
#define UDESK_HANDLE_LAYER     UDESK_HANDLE_LAYER

  /* Timer object identifier. */
  UDESK_HANDLE_TIMER = 0x7f05,
#define UDESK_HANDLE_TIMER     UDESK_HANDLE_TIMER

  /* Label display object identifier. */
  UDESK_HANDLE_LABEL = 0x7f06,
#define UDESK_HANDLE_LABEL     UDESK_HANDLE_LABEL

  /* Menu object identifier. */
  UDESK_HANDLE_MENU = 0x7f07,
#define UDESK_HANDLE_MENU      UDESK_HANDLE_MENU

  /* Generic bar grouping object identifier. */
  UDESK_HANDLE_BAR = 0x7f08,
#define UDESK_HANDLE_BAR       UDESK_HANDLE_BAR

  /* Event object identifier. */
  UDESK_HANDLE_EVENT = 0x7f09
#define UDESK_HANDLE_EVENT     UDESK_HANDLE_EVENT

};

/* ==========
 * Context-less queries
 */

/* right after errors: [0x0100, 0x01ff] */
enum {

  /* String, must be human readable and should
   * uniquely identify a vendor. Applications
   * must be able to rely on this string to
   * determine in a safe way the specific
   * implementation that is being used.
   */
  UDESK_VENDOR = 0x0100,
#define UDESK_VENDOR         UDESK_VENDOR

  /* String, must be human readable, identifies
   * in a deeper way the vendor and version of
   * this library, might change in different versions,
   * it is intended to be a meaningful description of the
   * library implementation, for display or printing purposes.
   * This string should not be used to retrieve informations
   * about the specific implementation since it is explicitly
   * allowed to change, UDESK_VENDOR_STRING, UDESK_VERSION_MAJOR and
   * UDESK_VERSION_MINOR should be used instead.
   */
  UDESK_VERSION = 0x0101,
#define UDESK_VERSION        UDESK_VERSION

  /* Positive int value, identifies the major number of
   * the specification version implemented by the library.
   */
  UDESK_VERSION_MAJOR = 0x0102,
#define UDESK_VERSION_MAJOR  UDESK_VERSION_MAJOR

  /* Positive integer value, identifies the minor number
   * of the specification version implemented by the library.
   */
  UDESK_VERSION_MINOR = 0x0103,
#define UDESK_VERSION_MINOR  UDESK_VERSION_MINOR

  /* Positive integer value, implementation number of supported
   * extensions.
   */
  UDESK_NUM_EXTENSIONS = 0x0104
#define UDESK_NUM_EXTENSIONS UDESK_NUM_EXTENSIONS

};

/* Query the string value `param', returns the queried value
 * or NULL if `param' is not a legal string value.
 */
UDESKAPI const char* UDESKAPIENTRY udeskQueryString(UDenum param);

/* Query the string value `param', returns the queried value
 * in the memory area referenced by `dst'.
 * Returns UDESK_NO_ERROR if successful, otherwise an error
 * value describing the error:
 *
 * UDESK_INVALID_ENUM  if `param' is not a legal enum value or if it
 *                     does not represent an integer value.
 * UDESK_INVALID_VALUE if `dst' is NULL.
 */
UDESKAPI UDenum UDESKAPIENTRY udeskQueryiv(UDenum param, UDint* dst);

/* Requests the `extnum'-th extension supported by the
 * library, `extnum' should be a value ranging in
 * [0, UDESK_EXTENSION_NUM).
 * Returns the extension string if successful, NULL if
 * the extension number is out of range.
 */
UDESKAPI const char* UDESKAPIENTRY udeskQueryExtension(UDint extnum);

/* ==========
 * Context creation and management
 */

/* Creates a context with the specified arguments, each implementaton
 * specific argument might be interpreted as intended by the implementation.
 * Any implementation must support a standard context which can
 * be created with:
 * udeskCreateContext(&argc, &argv)
 *
 * with `argc' and `argv' being the arguments as passed to the
 * application main() function.
 * Any implementation is free to modify the `argc' and `argv'
 * parameter to remove (thus hiding from the application) parameters
 * intended for the implementation.
 *
 * Any implementation specific context creation can be exposed via the
 * extensions and query facilities, that allows the application to determine
 * and support different context creation procedures wherever needed.
 * This function returns UDESK_NO_ERROR on success, the generated error
 * value on failure.
 * This function shall fail if:
 *
 * UDESK_INVALID_VALUE:       `argc' is NULL, or it points to a value which is
 *                            either 0 or negative, or `argv' is NULL or invalid.
 * UDESK_INVALID_OPERATION:   context already exists.
 * UDESK_OUT_OF_MEMORY:       insufficient memory to perform the operation.
 * UDESK_OPERATION_FAILED:    context creation failed unexpectedly.
 */
UDESKAPI UDenum UDESKAPIENTRY udeskCreateContext(int* argc, char** argv[]);

/* Sets the created context as active, entering in the main loop,
 * the function shall return when the event loop will terminate.
 * This function must be called when a context exists, otherwise it
 * has no effect.
 * The function shall fail if:
 *
 * UDESK_INVALID_OPERATION: the created context is already active.
 * UDESK_OPERATION_FAILED:  operation failed for some implementation defined reason.
 * UDESK_OUT_OF_MEMORY:     insufficient memory to perform the operation.
 */
UDESKAPI void UDESKAPIENTRY udeskMakeContextCurrent(void);

/* Suspends the current context, making it inactive,
 * the event loop is terminated, returning the control over the function
 * that called udeskMakeContextCurrent().
 * The function shall fail if:
 *
 * UDESK_INVALID_OPERATION: no context is currently active.
 */
UDESKAPI void UDESKAPIENTRY udeskMakeContextNone(void);

/* Queries the existing context for a procedure address,
 * this is the main mechanism used to query for extension functions,
 * allowing the implementation to extend the standard features in a
 * uniform way.
 * The returned procedure should be casted to the appropriate type.
 * This function returns the procedure if successful, NULL otherwise.
 * Notice that even if a procedure is returned it is not guaranteed
 * that it is safe to use or call, ensure that an extension is present,
 * or the library version is the expected one before making use of it.
 */
UDESKAPI void (UDESKAPIENTRYP udeskGetProcAddress(const char* name))(void) UDESKAPIENTRY;

/* Destroys the existing context, closing any window
 * belonging to the context and freeing any resource.
 * Returns UDESK_NO_ERROR on success.
 * This function shall fail if:
 *
 * UDESK_INVALID_OPERATION: no context exists.
 */
UDESKAPI UDenum UDESKAPIENTRY udeskDestroyContext(void);

/* ==========
 * Error checking functions and values.
 */

/* first range, UDESK_NO_ERROR must be 0, [0x0000, 0x00ff] */
enum {

  /* No error occurred. */
  UDESK_NO_ERROR = 0x0000,
#define UDESK_NO_ERROR            UDESK_NO_ERROR

  /* Memory allocation for the specified operation failed. */
  UDESK_OUT_OF_MEMORY = 0x0001,
#define UDESK_OUT_OF_MEMORY       UDESK_OUT_OF_MEMORY

  /* Invalid or unrecognized enumeration value
   * was provided to the object or function.
   */
  UDESK_INVALID_ENUM = 0x0002,
#define UDESK_INVALID_ENUM        UDESK_INVALID_ENUM

  /* A value was out of range or invalid
   * during a function call.
   */
  UDESK_INVALID_VALUE = 0x0003,
#define UDESK_INVALID_VALUE       UDESK_INVALID_VALUE

  /* Requested operation was invalid
   * (for example the specific object can't perform
   * the requested action).
   */
  UDESK_INVALID_OPERATION = 0x0004,
#define UDESK_INVALID_OPERATION   UDESK_INVALID_OPERATION

  /* Requested operation failed, reasons
   * for such failure are insufficient resources
   * (notice that UDESK_OUT_OF_MEMORY refers to
   * memory resources only, while this is more
   * generic and might refer to devices), unexpected
   * circumstances or implementation defined error statuses.
   */
  UDESK_OPERATION_FAILED = 0x0005,
#define UDESK_OPERATION_FAILED    UDESK_OPERATION_FAILED

  /* Such feature is marked as optional by the specifications,
   * and it is not supported by this implementation.
   */
  UDESK_FEATURE_UNSUPPORTED = 0x0006
#define UDESK_FEATURE_UNSUPPORTED UDESK_FEATURE_UNSUPPORTED

};

/* Returns the most recently detected error in the existing context,
 * the error indicator is set back to UDESK_NO_ERROR after
 * the function call, this means that udeskGetError()
 * won't report any other error unless a new one is generated.
 * If no context exists, the function returns UDESK_NO_ERROR.
 */
UDESKAPI UDenum UDESKAPIENTRY udeskGetError(void);

/* ==========
 * Context-wide queries
 */

/* right after context-less queries: [0x0200, 0x02ff] */
enum {

  /* 2 positive int values greater than 0 (default value
   * is implementation defined), represent the width and
   * height, respectively, of the screen in which the
   * windows are drawn.
   */
  UDESK_SCREEN_SIZE = 0x0200,
#define UDESK_SCREEN_SIZE         UDESK_SCREEN_SIZE

  /* String (value is implementation defined) that
   * sets or queries the clipboard contents.
   * An empty string is returned if data is
   * unavailable.
   * If clipboard is not supported by the implementation,
   * UDESK_FEATURE_UNSUPPORTED error is generated.
   */
  UDESK_CLIPBOARD_TEXT = 0x0201,
#define UDESK_CLIPBOARD_TEXT      UDESK_CLIPBOARD_TEXT

  /* Boolean value (value is implementation defined)
   * that contains the screensaver status, an implementation
   * that doesn't support or doesn't provide a screensaver
   * might raise a UDESK_FEATURE_UNSUPPORTED error.
   * Value is 0 if screensaver isn't active, non-zero
   * if screensaver is active.
   */
  UDESK_SCREENSAVER_ENABLE = 0x0202
#define UDESK_SCREENSAVER_ENABLE  UDESK_SCREENSAVER_ENABLE

};

/* Queries the existing context for an integer or floating value or vector of
 * values, or a string, the query is either returned by the function or stored
 * in the destination vector `dst', according to the query type.
 * If the function fails, the destination vector is left unchanged.
 * If the value is supposed to be returned, NULL is returned on failure.
 * This function shall fail if:
 *
 * UDESK_INVALID_ENUM:        `param' is an invalid enum, or it does not
 *                            represent a value of the queried type.
 * UDESK_INVALID_VALUE:       `dst' is NULL.
 * UDESK_FEATURE_UNSUPPORTED: albeit the operation being legal, the feature (declared
 *                            as optional by the library specification) is unsupported
 *                            by this implementation.
 */
UDESKAPI void UDESKAPIENTRY udeskGetiv(UDenum param, UDint* dst);
UDESKAPI void UDESKAPIENTRY udeskGetfv(UDenum param, UDfloat* dst);
UDESKAPI const char* UDESKAPIENTRY udeskGetString(UDenum param);

/* Sets an integer or floating value or vector of values, or a string in the
 * existing context, the new value is set on success, otherwise it is left
 * unchanged and an error is generated.
 * This function shall fail if:
 *
 * UDESK_INVALID_ENUM:        `param' is an invalid enum, or it does not
 *                            represent a writable value.
 * UDESK_INVALID_VALUE:       `to' is NULL, the value is invalid for the specified
 *                            field, or a type mismatch exists.
 * UDESK_FEATURE_UNSUPPORTED: albeit the operation being legal, the feature (declared
 *                            as optional by the library specification) is unsupported
 *                            by this implementation.
 */
UDESKAPI void UDESKAPIENTRY udeskSetiv(UDenum param, const UDint* to);
UDESKAPI void UDESKAPIENTRY udeskSetfv(UDenum param, const UDfloat* to);
UDESKAPI void UDESKAPIENTRY udeskSetString(UDenum param, const char* to);

/* ==========
 * Objects creation and management functions
 */

/* Creates `num' objects of type `type', storing their handles into `dst'.
 * Upon successful completion `dst' shall store the specified number of valid
 * objects handles.
 * If allocation fails no object shall be stored into `dst' and its contents are
 * unspecified.
 * This function shall fail if:
 *
 * UDESK_INVALID_VALUE:       `num' is < 0 or `dst' is NULL.
 * UDESK_INVALID_ENUM:        `type' is an unknown, invalid, or unsupported type
 * UDESK_OUT_OF_MEMORY:       insufficient memory to perform the request
 * UDESK_OPERATION_FAILED:    operation failed unexpectedly.
 * UDESK_FEATURE_UNSUPPORTED: albeit the operation being legal, the feature (declared
 *                            as optional by the library specification) is unsupported
 *                            by this implementation.
 */
UDESKAPI void UDESKAPIENTRY udeskGenObjects(UDenum type, UDint num, UDhandle* dst);

/* Registers the handler `func' for the event type `param' on the object `handle', overriding
 * any existing handler for the same event and object.
 * If `func' is NULL, the function is interpreted as ignoring the specified event.
 * This function shall fail if:
 *
 * UDESK_INVALID_VALUE:       `handle' is an invalid identifier.
 * UDESK_INVALID_ENUM:        `param' is an unknown, invalid, or unsupported event
 *                            type for the specified object.
 * UDESK_OUT_OF_MEMORY:       insufficient memory to perform such operation.
 * UDESK_OPERATION_FAILED:    operation failed unexpectedly.
 * UDESK_FEATURE_UNSUPPORTED: albeit the operation being legal, the feature (declared
 *                            as optional by the library specification) is unsupported
 *                            by this implementation.
 */
UDESKAPI void UDESKAPIENTRY udeskRegisterHandler(UDhandle handle, UDenum param, UDhandlerproc func);

/* Starts building the object identified by `handle', each object
 * might support different building modes, each of them can be
 * accessed with a different value for the `mode' argument.
 * Any object specific query or get operation when being built is
 * illegal and it shall trigger a UDESK_INVALID_OPERATION error.
 * Any udeskBegin() operation must be terminated by udeskEnd(), the
 * changes are not guaranteed to be visually applied unless an explicit
 * udeskFlush() on the same object is applied, see udeskEnd() for additional details.
 * No more than one build mode can be active for any object, this means
 * that no recursive udeskBegin() is allowed for the same object.
 * This function shall fail if:
 *
 * UDESK_INVALID_VALUE:       `handle' is an invalid identifier.
 * UDESK_INVALID_OPERATION:   the specified object does not support any building
 *                            operation.
 * UDESK_INVALID_ENUM:        `mode' is an unknown, invalid, or unsupported building mode
 *                            for the specified object identifier.
 * UDESK_OUT_OF_MEMORY:       not enough memory for the specified operation.
 * UDESK_OPERATION_FAILED:    operation failed unexpectedly.
 * UDESK_FEATURE_UNSUPPORTED: albeit the operation being legal, the feature (declared
 *                            as optional by the library specification) is unsupported
 *                            by this implementation.
 */
UDESKAPI void udeskBegin(UDhandle handle, UDenum mode);

/* Terminates a started building operation for the object identified by
 * `handle', after a successful call to this function another operation
 * can be issued by calling again udeskBegin() on the `handle' object.
 * Once this function is called any other query function can be issued on
 * the object and it shall reflect the updated values after the build operation,
 * it is not guaranteed that such values are reflected on the Graphical
 * User Interface, though, such behavior is guaranteed by an explicit
 * call to the udeskFlush() function, that shall update the interface
 * to represent the current state of the object. See udeskFlush() for
 * additional details.
 * This function shall fail if:
 *
 * UDESK_INVALID_VALUE:     `handle' is an invalid object identifier.
 * UDESK_INVALID_OPERATION: no building operation is active for the specified object.
 * UDESK_OUT_OF_MEMORY:     not enough memory to perform the required operation.
 * UDESK_OPERATION_FAILED:  operation failed unexpectedly.
 */
UDESKAPI void udeskEnd(UDhandle handle);

/* Flushes every pending graphical update for the specified identifier on screen,
 * if called with UDESK_HANDLE_NONE argument, it flushes changes for any
 * object belonging to the current context.
 * Any implementation is free to buffer graphical update operations to reduce
 * rendering overhead and submit changes to the interface whenever it is
 * considered optimal performance-wise, a portable application must not assume such
 * changes are reflected immediately to the user interface, as well as it
 * must not assume they are buffered until an explicit udeskFlush() is
 * called. But it is perfectly legal to assume that objects' internal state
 * reflects the latest issued changes immediately.
 * The application can force the latest object state to be applied on screen with
 * this function, that is guaranteed to terminate once the screen has been updated.
 * Every object support the flush operation, an object that doesn't require
 * a flush operation, either because it has not any graphical resource attached,
 * or because it is already updated, or for any implementation defined reason,
 * shall silently ignore the flush request.
 * A portable application is encouraged to take a conservative approach and
 * always perform a flush operation whenever an object needs immediate
 * update on screen, otherwise the actual behavior is implementation
 * defined.
 * This function shall fail if:
 *
 * UDESK_INVALID_VALUE:     `handle' is not UDESK_HANDLE_NONE and it
 *                          represents an invalid identifier.
 */
UDESKAPI void UDESKAPIENTRY udeskFlush(UDhandle handle);

/* Deletes `num' objects referenced by `buff', this function
 * silently ignores UDESK_HANDLE_NONE or invalid handles in the
 * buffer, passing an invalid handle to this function shall not
 * trigger any error.
 * The actual deallocation of an object is delayed until no
 * other object refers the deleted instance. In other words the
 * implementation guarantees that a deleted object remains valid and
 * reachable using its handle, until no other object is referring or
 * rendering it, no implicit deletion of a child object takes place when
 * the parent object is deleted, every object must be deleted explicitly
 * or is implicitly deleted when every context it belongs to is destroyed.
 * This function shall fail if:
 *
 * UDESK_INVALID_VALUE: `num' is < 0 or `buff' is NULL.
 */
UDESKAPI void UDESKAPIENTRY udeskDeleteObjects(UDint num, UDhandle* buff);

/* ==========
 * Window objects
 *
 * Received events:
 *
 * UDESK_EVENT_DESTROY: Window was closed by the user, default handling
 *                      is ignoring the event, any application should
 *                      handle this event since the user expects a
 *                      response from the application.
 * UDESK_EVENT_ENTER:   The cursor entered into the window area.
 * UDESK_EVENT_LEAVE:   The cursor left the window area.
 */

/* right after context-wide queries: [0x0300, 0x37f] */
enum {

 /* 2 positive int values (default is implementation
  * defined), represent the window's upper left angle
  * position on screen, in pixels, starting from the
  * screen's upper left angle (considered as 0,0).
  * 0,0
  * +--------------------
  * |    (x,y)
  * |     +--------
  * |     |
  * |     |
  * |
  */
  UDESK_WINDOW_POSITION = 0x0300,
#define UDESK_WINDOW_POSITION  UDESK_WINDOW_POSITION

  /* 2 positive int values (default is implementation
   * defined), represent window width and height,
   * respectively, in pixels.
   */
  UDESK_WINDOW_SIZE = 0x0301,
#define UDESK_WINDOW_SIZE      UDESK_WINDOW_SIZE

  /* Window title string (default is implementation
   * defined), used by the implementation to identify
   * the window to the user.
   */
  UDESK_WINDOW_TITLE = 0x0302,
#define UDESK_WINDOW_TITLE     UDESK_WINDOW_TITLE

  /* Resizable window property, boolean value (true
   * by default), defines whether a window can change
   * its geometry, if set to false the implementation
   * should prevent the user to resize the window.
   */
  UDESK_WINDOW_RESIZE = 0x0303,
#define UDESK_WINDOW_RESIZE    UDESK_WINDOW_RESIZE

  /* Hidden window mode, the window is not shown to
   * the user at all, this is the default mode, to allow
   * window setup without ugly artifacts.
   */
  UDESK_WINDOW_HIDDEN = 0x0304,
#define UDESK_WINDOW_HIDDEN    UDESK_WINDOW_HIDDEN

  UDESK_WINDOW_MODE = 0x0305,
#define UDESK_WINDOW_MODE      UDESK_WINDOW_MODE

  /* Iconified window mode, window is placed into a panel, dock
   * or any other implementation defined way to map the window
   * away from the screen.
   */
  UDESK_WINDOW_ICONIFIED = 0x0306,
#define UDESK_WINDOW_ICONIFIED UDESK_WINDOW_ICONIFIED

  /* Show window mode, window is mapped on screen and shown
   * to the user, this is the normal viewing mode.
   */
  UDESK_WINDOW_SHOW = 0x0307,
#define UDESK_WINDOW_SHOW      UDESK_WINDOW_SHOW

  /* Maximized window mode, the implementation allows the
   * window to take as much screen space as possible.
   */
  UDESK_WINDOW_MAXIMIZED = 0x0308,
#define UDESK_WINDOW_MAXIMIZED UDESK_WINDOW_MAXIMIZED

  /* Decoration property, boolean value
   * (default is implementation defined),
   * 0 if the window has (or requires) no decoration,
   * non-zero otherwise.
   * A maximized decoration-less window should be interpreted as
   * a fullscreen window by the implementation, but an implementation
   * is free to not provide a fullscreen window policy.
   */
  UDESK_WINDOW_DECORATE = 0x0309,
#define UDESK_WINDOW_DECORATE  UDESK_WINDOW_DECORATE

  UDESK_WINDOW_OPACITY = 0x030a,
#define UDESK_WINDOW_OPACITY   UDESK_WINDOW_OPACITY

  UDESK_WINDOW_ICON = 0x030b,
#define UDESK_WINDOW_ICON      UDESK_WINDOW_ICON

  UDESK_WINDOW_CHILD = 0x030c
#define UDESK_WINDOW_CHILD     UDESK_WINDOW_CHILD

};

UDESKAPI void UDESKAPIENTRY udeskWindowChild(UDhandle window, UDhandle handle);
UDESKAPI void UDESKAPIENTRY udeskWindowIcon(UDhandle window, UDhandle icon);

UDESKAPI void UDESKAPIENTRY udeskSetWindowi(UDhandle window, UDenum param, UDint x);
UDESKAPI void UDESKAPIENTRY udeskSetWindow2i(UDhandle window, UDenum param, UDint x, UDint y);
UDESKAPI void UDESKAPIENTRY udeskSetWindowiv(UDhandle window, UDenum param, const UDint* to);
UDESKAPI void UDESKAPIENTRY udeskSetWindowf(UDhandle window, UDenum param, UDfloat x);
UDESKAPI void UDESKAPIENTRY udeskSetWindowString(UDhandle window, UDenum param, const char* to);

UDESKAPI void UDESKAPIENTRY udeskGetWindowiv(UDhandle window, UDenum param, UDint* dst);
UDESKAPI void UDESKAPIENTRY udeskGetWindowfv(UDhandle window, UDenum param, UDfloat* dst);
UDESKAPI const char* UDESKAPIENTRY udeskGetWindowString(UDhandle window, UDenum param);
UDESKAPI UDhandle UDESKAPIENTRY udeskGetWindowHandle(UDhandle window, UDenum param);

UDESKAPI UDboolean UDESKAPIENTRY udeskIsWindow(UDhandle handle);

/* ==========
 * Container functions
 */

UDESKAPI void UDESKAPIENTRY udeskContainerChild(UDhandle container, UDint x, UDint y, UDint cells_width, UDint cells_height, UDboolean horiz_expand, UDboolean vert_expand, UDhandle handle);
UDESKAPI void UDESKAPIENTRY udeskDetachContainerChild(UDhandle container, UDint x, UDint y);
UDESKAPI UDhandle UDESKAPIENTRY udeskGetContainerHandle(UDhandle container, UDint x, UDint y);
UDESKAPI UDboolean UDESKAPIENTRY udeskIsContainer(UDhandle handle);

/* ==========
 * Button functions
 *
 * Received events:
 *
 * UDESK_EVENT_CLICK: The button has been clicked.
 * UDESK_EVENT_ENTER: Cursor entered into the button area.
 * UDESK_EVENT_LEAVE: Cursor left the button area.
 */

/* right after window: [0x0380, 0x03ff] */
enum {

  /* non-NULL String value, the button label text,
   * default value is empty string.
   */
  UDESK_BUTTON_TEXT = 0x0380,
#define UDESK_BUTTON_LABEL UDESK_BUTTON_LABEL

  /* Handle to button icon image, default value is
   * no image, which is UDESK_HANDLE_NONE.
   */
  UDESK_BUTTON_ICON = 0x0381
#define UDESK_BUTTON_ICON  UDESK_BUTTON_ICON

};

UDESKAPI void UDESKAPIENTRY udeskButtonIcon(UDhandle button, UDhandle icon);
UDESKAPI void UDESKAPIENTRY udeskSetButtonString(UDhandle button, UDenum param, const char* to);

UDESKAPI const char* UDESKAPIENTRY udeskGetButtonString(UDhandle button, UDenum param);
UDESKAPI UDhandle UDESKAPIENTRY udeskGetButtonHandle(UDhandle button, UDenum param);

UDESKAPI UDboolean UDESKAPIENTRY udeskIsButton(UDhandle handle);

/* ==========
 * Pixmap storage functions and enumerations
 */

/* right after button: [0x0400, 0x047f] */
enum {

  UDESK_PIXMAP_TARGET = 0x0400,
#define UDESK_PIXMAP_TARGET               UDESK_PIXMAP_TARGET

  UDESK_PIXMAP_IMAGE = 0x0401,
#define UDESK_PIXMAP_IMAGE                UDESK_PIXMAP_IMAGE

  UDESK_PIXMAP_ICON = 0x0402,
#define UDESK_PIXMAP_ICON                 UDESK_PIXMAP_ICON

  UDESK_PIXMAP_USAGE_STATIC = 0x0403,
#define UDESK_PIXMAP_USAGE_STATIC         UDESK_PIXMAP_USAGE_STATIC

  UDESK_PIXMAP_USAGE_MENU = 0x0404,
#define UDESK_PIXMAP_USAGE_MENU           UDESK_PIXMAP_USAGE_MENU

  UDESK_PIXMAP_USAGE_TOOLBAR_SMALL = 0x0405,
#define UDESK_PIXMAP_USAGE_TOOLBAR_SMALL  UDESK_PIXMAP_USAGE_TOOLBAR_SMALL

  UDESK_PIXMAP_USAGE_TOOLBAR_MEDIUM = 0x0406,
#define UDESK_PIXMAP_USAGE_TOOLBAR_MEDIUM UDESK_PIXMAP_USAGE_TOOLBAR_MEDIUM

  UDESK_PIXMAP_USAGE_TOOLBAR_LARGE = 0x0407,
#define DEESk_PIXMAP_USAGE_TOOLBAR_LARGE  UDESK_PIXMAP_USAGE_TOOLBAR_LARGE

  UDESK_PIXMAP_USAGE_ICON_SMALL = 0x0408,
#define UDESK_PIXMAP_USAGE_ICON_SMALL     UDESK_PIXMAP_USAGE_ICON_SMALL

  UDESK_PIXMAP_USAGE_ICON_MEDIUM = 0x0409,
#define UDESK_PIXMAP_USAGE_ICON_MEDIUM    UDESK_PIXMAP_USAGE_ICON_MEDIUM

  UDESK_PIXMAP_USAGE_ICON_LARGE = 0x040a
#define UDESK_PIXMAP_USAGE_ICON_LARGE     UDESK_PIXMAP_USAGE_ICON_LARGE

};

UDESKAPI void UDESKAPIENTRY udeskPixmapFile(UDhandle pixmap, UDenum target, const char* name, UDenum usage);
UDESKAPI void UDESKAPIENTRY udeskPixmapData(UDhandle pixmap, UDint width, UDint height, UDenum format, const void* data);
UDESKAPI void UDESKAPIENTRY udeskGetPixmapiv(UDhandle pixmap, UDenum param, UDint* dst);
UDESKAPI UDboolean UDESKAPIENTRY udeskIsPixmap(UDhandle handle);

/* ==========
 * Layer objects
 *
 * A layer is a resource built on top of a graphical buffer, it
 * can be transformed independently from its underlying buffer,
 * it is often used to edit a pixmap resource by scaling or
 * transforming it.
 * This abstraction allows pixel buffers sharing, as well as offering
 * an unique and uniform way to manage multiple underlying objects,
 * while also keeping pixel storage and loading separated from the
 * transformation logic.
 *
 * Supported udeskBegin()/udeskEnd() operations:
 *
 * none.
 *
 * Received events:
 *
 * none.
 */

/* right after pixmap: [0x0480, 0x04ff] */
enum {

  /* Hint for image filtering and transformation operations.
   * Parameter is read and write.
   * This parameter is merely a hint for the implementation, which is
   * free to choose the most suitable image transformation methods,
   * an implementation could even provide identical performance and
   * quality for every hint.
   * Possible values for this parameter are:
   *
   * UDESK_FASTEST: Use fastest algorithms (default).
   * UDESK_NICEST:  Use algorithms that delivers the best quality.
   */
  UDESK_LAYER_FILTER_HINT = 0x0480,
#define UDESK_LAYER_FILTER_HINT UDESK_LAYER_FILTER_HINT

};

UDESKAPI void UDESKAPIENTRY udeskLayerPixmap(UDhandle layer, UDhandle pixmap);

UDESKAPI void UDESKAPIENTRY udeskLayerScale(UDhandle layer, UDint width, UDint height, UDboolean keep_aspect);

UDESKAPI void UDESKAPIENTRY udeskSetLayeri(UDhandle layer, UDenum param, UDint to);
UDESKAPI void UDESKAPIENTRY udeskSetLayeriv(UDhandle layer, UDenum param, const UDint* to);

UDESKAPI void UDESKAPIENTRY udeskGetLayeriv(UDhandle layer, UDenum param, UDint* dst);

UDESKAPI UDboolean UDESKAPIENTRY udeskIsLayer(UDhandle handle);

/* ==========
 * Timer objects
 *
 * A timer is an object that will periodically produce a timeout event
 * that an application can use to perform operations at regular time
 * intervals. A timer is not required to be extremely precise, a portable
 * application should not rely on timer objects to perform time critical
 * tasks, but it can legally assume that any implementation provides a
 * reasonable approximation of the required time interval.
 * A portable application should also assume that timers are a relatively
 * expensive resource and use them only when required, possibly sharing
 * a timer object instance between multiple tasks, an implementation could
 * provide only a limited amount of timer objects to the application.
 * A timer object shall be stopped by default and has an implementation
 * defined default interval. An application is expected to set a timer
 * to the desired interval and start it when appropriate, a started
 * timer shall produce timeout events at regular intervals, that are
 * a reasonable approximation of the actual interval required by the
 * application. When a timer is stopped it can be resumed safely later
 * or cleared to start anew.
 *
 * Supported udeskBegin()/udeskEnd() operations:
 *
 * none.
 *
 * Received events:
 *
 * UDESK_EVENT_TIMEOUT: a timer interval has elapsed.
 */

/* right after layers: [0x0500, 0x057f] */
enum {

  /* Positive timer interval, each interval a timeout event is delivered. */
  UDESK_TIMER_INTERVAL = 0x0500,
#define UDESK_TIMER_INTERVAL UDESK_TIMER_INTERVAL

  /* Symbolic enumeration, clear the timer and start. */
  UDESK_TIMER_CLEAR = 0x0501,
#define UDESK_TIMER_CLEAR    UDESK_TIMER_CLEAR
 
  /* Symbolic enumeration, resume the timer. */
  UDESK_TIMER_RESUME = 0x0502,
#define UDESK_TIMER_RESUME   UDESK_TIMER_RESUME

  /* Symbolic enumeration, represents the timer status. */
  UDESK_TIMER_STATUS = 0x0503,
#define UDESK_TIMER_STATUS   UDESK_TIMER_STATUS

  /* Symbolic enumeration, represents the timer running status. */
  UDESK_TIMER_RUNNING = 0x0504,
#define UDESK_TIMER_RUNNING  UDESK_TIMER_RUNNING

  /* Symbolic enumeration, represents the timer stopped status. */
  UDESK_TIMER_STOPPED = 0x0505
#define UDESK_TIMER_STOPPED  UDESK_TIMER_STOPPED

};

UDESKAPI void UDESKAPIENTRY udeskSetTimeriv(UDhandle timer, UDenum param, const UDint* to);
UDESKAPI void UDESKAPIENTRY udeskSetTimeri(UDhandle timer, UDenum param, UDint to);

UDESKAPI void UDESKAPIENTRY udeskGetTimeriv(UDhandle timer, UDenum param, UDint* dst);

UDESKAPI void UDESKAPIENTRY udeskStartTimer(UDhandle timer, UDenum mode);
UDESKAPI void UDESKAPIENTRY udeskStopTimer(UDhandle timer);

UDESKAPI UDboolean UDESKAPIENTRY udeskIsTimer(UDhandle handle);


/* **** **** EXPERIMENTAL **** **** 

enum {
  UDESK_LABEL_TARGET,
#define UDESK_LABEL_TARGET    UDESK_LABEL_TARGET

  UDESK_LABEL_MENU,

  UDESK_LABEL_TOOLBAR,

  UDESK_LABEL_MESSAGE,

  UDESK_LABEL_ACTIVE,

  UDESK_LABEL_TEXT,

  UDESK_LABEL_ICON,
#define UDESK_LABEL_ICON      UDESK_LABEL_ICON

  UDESK_LABEL_WRAP,
#define UDESK_LABEL_WRAP      UDESK_LABEL_WRAP

  UDESK_LABEL_WRAP_NONE,
#define UDESK_LABEL_WRAP_NONE UDESK_LABEL_WRAP_NONE

  UDESK_LABEL_WRAP_WORD
};

UDESKAPI void UDESKAPIENTRY udeskLabelParameterHandle(UDhandle label, UDenum param, UDhandle to);
UDESKAPI void UDESKAPIENTRY udeskLabelParameterString(UDhandle label, UDenum param, const char* to);
UDESKAPI void UDESKAPIENTRY udeskLabelParameteriv(UDhandle label, UDenum param, const UDint* to);
UDESKAPI void UDESKAPIENTRY udeskLabelParameteri(UDhandle label, UDenum param, UDint to);

UDESKAPI const char* UDESKAPIENTRY udeskGetLabelString(UDhandle label, UDenum param);
UDESKAPI void UDESKAPIENTRY udeskGetLabeliv(UDhandle label, UDenum param, UDint* dst);
UDESKAPI UDhandle UDESKAPIENTRY udeskGetLabelHandle(UDhandle label, UDenum param);

UDESKAPI UDboolean UDESKAPIENTRY udeskIsLabel(UDhandle handle);

enum {

  UDESK_MENU_TYPE,
#define UDESK_MENU_TYPE        UDESK_MENU_TYPE

  UDESK_MENU_POPUP,
#define UDESK_MENU_POPUP       UDESK_MENU_POPUP

  UDESK_MENU_EMBEDDED,
#define UDESK_MENU_EMBEDDED    UDESK_MENU_EMBEDDED

  UDESK_MENU_NAME,
#define UDESK_MENU_NAME        UDESK_MENU_TITLE

  UDESK_MENU_NUM_ENTRIES,
#define UDESK_MENU_NUM_ENTRIES UDESK_MENU_NUM_ENTRIES

  UDESK_MENU_ENTRY
#define UDESK_MENU_ENTRY       UDESK_MENU_ENTRY

};

UDESKAPI const char* UDESKAPIENTRY udeskGetMenuString(UDhandle menu, UDenum param);
UDESKAPI void UDESKAPIENTRY udeskGetMenuiv(UDhandle menu, UDenum param, UDint* dst);
UDESKAPI UDhandle UDESKAPIENTRY udeskGetMenuHandle(UDhandle menu, UDenum param, UDint idx);

UDESKAPI void UDESKAPIENTRY udeskMenuFieldHandle(UDhandle menu, UDenum param, UDhandle to);

UDESKAPI UDboolean UDESKAPIENTRY udeskIsMenu(UDhandle handle);

enum {

  UDESK_BAR_TYPE,
#define UDESK_BAR_TYPE        UDESK_BAR_TYPE

  UDESK_BAR_MENU,
#define UDESK_BAR_MENU        UDESK_BAR_MENU

  UDESK_BAR_TOOLS,
#define UDESK_BAR_TOOLS       UDESK_BAR_TOOLS

  UDESK_BAR_ORIENTATION,
#define UDESK_BAR_ORIENTATION UDESK_BAR_ORIENTATION

  UDESK_BAR_NUM_ENTRIES,
#define UDESK_BAR_NUM_ENTRIES UDESK_BAR_NUM_ENTRIES

  UDESK_BAR_ENTRY
#define UDESK_BAR_ENTRY       UDESK_BAR_ENTRY

};

UDESKAPI const char* UDESKAPIENTRY udeskGetBarString(UDhandle bar, UDenum param);
UDESKAPI void UDESKAPIENTRY udeskGetBariv(UDhandle bar, UDenum param, UDint* dst);
UDESKAPI UDhandle UDESKAPIENTRY udeskGetBarHandle(UDhandle bar, UDenum param, UDint idx);

UDESKAPI void UDESKAPIENTRY udeskBarFieldHandle(UDhandle bar, UDenum param, UDhandle to);
UDESKAPI void UDESKAPIENTRY udeskBarFieldSeparator(UDhandle bar, UDboolean allow_draw, UDboolean allow_expand);

UDESKAPI UDboolean UDESKAPIENTRY udeskIsBar(UDhandle handle);

 ******************************** */

/* ==========
 * Event management and fields
 */

/* 256 reserved values before the last range: [0x7c00, 0x7dff] */
enum {

  UDESK_EVENT_SENDER = 0x7c00,
#define UDESK_EVENT_SENDER       UDESK_EVENT_SENDER

  UDESK_EVENT_DESTINATION = 0x7c01,
#define UDESK_EVENT_DESTINATION  UDESK_EVENT_DESTINATION

  UDESK_EVENT_TIMESTAMP = 0x7c02,
#define UDESK_EVENT_TIMESTAMP    UDESK_EVENT_TIMESTAMP

  UDESK_EVENT_POSITION = 0x7c03,
#define UDESK_EVENT_POSITION     UDESK_EVENT_POSITION

  UDESK_EVENT_TYPE = 0x7c04,
#define UDESK_EVENT_TYPE         UDESK_EVENT_TYPE

  UDESK_EVENT_DESTROY = 0x7c05,
#define UDESK_EVENT_DESTROY      UDESK_EVENT_DESTROY

  UDESK_EVENT_PRESS = 0x7c06,
#define UDESK_EVENT_PRESS        UDESK_EVENT_PRESS

  UDESK_EVENT_RELEASE = 0x7c07,
#define UDESK_EVENT_RELEASE      UDESK_EVENT_RELEASE

  UDESK_EVENT_MOTION = 0x7c08,
#define UDESK_EVENT_MOTION       UDESK_EVENT_MOTION

  UDESK_EVENT_CLICK = 0x7c09,
#define UDESK_EVENT_CLICK        UDESK_EVENT_CLICK

  UDESK_EVENT_DOUBLE_CLICK = 0x7c0a,
#define UDESK_EVENT_DOUBLE_CLICK UDESK_EVENT_DOUBLE_CLICK

  UDESK_EVENT_TOUCH = 0x7c0b,
#define UDESK_EVENT_TOUCH        UDESK_EVENT_TOUCH

  UDESK_EVENT_KEYBOARD = 0x7c0c,
#define UDESK_EVENT_KEYBOARD     UDESK_EVENT_KEYBOARD

  UDESK_EVENT_ENTER = 0x7c0d,
#define UDESK_EVENT_ENTER        UDESK_EVENT_ENTER

  UDESK_EVENT_LEAVE = 0x7c0e,
#define UDESK_EVENT_LEAVE        UDESK_EVENT_LEAVE

  UDESK_EVENT_TIMEOUT = 0x7c0f
#define UDESK_EVENT_TIMEOUT      UDESK_EVENT_TIMEOUT

};

UDESKAPI void UDESKAPIENTRY udeskGetEventiv(UDhandle event, UDenum param, UDint* dst);
UDESKAPI void UDESKAPIENTRY udeskGetEventfv(UDhandle event, UDenum param, UDfloat* dst);
UDESKAPI UDhandle UDESKAPIENTRY udeskGetEventHandle(UDhandle event, UDenum param);

UDESKAPI void UDESKAPIENTRY udeskEventFieldi(UDhandle event, UDenum param, UDint x);
UDESKAPI void UDESKAPIENTRY udeskEventField2i(UDhandle event, UDenum param, UDint x, UDint y);
UDESKAPI void UDESKAPIENTRY udeskEventFieldiv(UDhandle event, UDenum param, const UDint* to);
UDESKAPI void UDESKAPIENTRY udeskEventFieldf(UDhandle event, UDenum param, UDfloat x);
UDESKAPI void UDESKAPIENTRY udeskEventField2f(UDhandle event, UDenum param, UDfloat x, UDfloat y);
UDESKAPI void UDESKAPIENTRY udeskEventFieldfv(UDhandle event, UDenum param, const UDfloat* to);
UDESKAPI void UDESKAPIENTRY udeskEventFieldHandle(UDhandle event, UDenum param, UDhandle to);

UDESKAPI UDboolean UDESKAPIENTRY udeskIsEvent(UDhandle handle);

#ifdef __cplusplus
}
#endif

#endif /* UDESK_H_ */
