/* udeskplatform.h: The Unix Desktop Environment platform types.
 *
 * Platform specific types and macros.
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

#ifndef UDESKPLATFORM_H_
#define UDESKPLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif

/* udesk API calling convention or
 * platform or compiler specific linking mode.
 */
#define UDESKAPI extern

/* udesk API entry platform or compiler specific calling convention. */
#define UDESKAPIENTRY

/* convenience macro for API entry function pointer. */
#define UDESKAPIENTRYP UDESKAPIENTRY *

/* 32 bit signed integer type. */
typedef int UDint;

/* Boolean value, it can be casted safely to a C99/C++ bool, or
 * to a UDint value, without any data loss.
 * Wherever the specification accepts a boolean value, any 0 value
 * is considered as false, while any non-zero value is considered as true,
 * according to the C language. Wherever the specification
 * returns a boolean value, 0 is returned to indicate false, 1 is
 * returned to indicate true.
 * No convenience macros are defined for true and false values, this is
 * intended to encourage the usage of C99 stdbool.h, this type has to
 * be considered as backward compatibility with C89 only.
 */
typedef signed char UDboolean;

/* Implementation specific floating point type,
 * might be either single or double precision
 * depending on the platform.
 */
typedef double UDfloat;

/* A graphic handle is an identifier to refer
 * to any graphical resource allocated by the
 * library, it is not guaranteed to be an
 * integer type.
 */
typedef UDint UDhandle;

/* Reserved identifier for the null handle, every handle
 * can be compared with it, any handle allocated
 * by the library will be different than this one.
 */
#define UDESK_HANDLE_NONE ((UDhandle)0)

/* Integer value that can hold any enumeration
 * value available in this library, can be legally
 * casted to UDint without any data loss.
 */
typedef unsigned short UDenum;

/* note, possible enum mappings:
 * [0x0000, 0x7fff] = reserved for standard symbols
 * [0x8000, 0xffff] = reserved for extensions
 *
 * for unsigned 16 bits UDenum
 */

/* Event handler function type, a pointer to a standard
 * C linked function that returns a void value and takes one
 * UDhandle argument.
 * Valid event handler signature examples are:
 *
 * void exampleHandler(UDhandle argument);
 * static void staticExampleHandler(UDhandle argument);
 *
 * The UDhandle argument is the identifier of the event
 * that must be handled by the function and it shall be
 * a valid UDESK_HANDLE_EVENT type object identifier during the
 * entire event handling procedure.
 * If the event requires no further propagation after handler
 * execution, a delete should be called on the event object
 * before returning, otherwise the event follows its specific
 * propagation rules.
 * If the event propagation terminates without any handler explicitly
 * deleting it, a default handler shall be executed,
 * which deletes the event object invalidating the handle.
 */
typedef void (*UDhandlerproc)(UDhandle event);

/* Mouse buttons, modifiers and keycodes values. */
enum {

  /* Special none value. */
  UDESK_KEYCODE_NONE = 0,
#define UDESK_KEYCODE_NONE         UDESK_KEYCODE_NONE

  /* Mouse / joystick buttons,
   * guaranteed to be successive values:
   *
   * UDESK_CLICK_BUTTON<N> - UDESK_CLICK_BUTTON0 = <N>
   *
   * UDESK_CLICK_BUTTON0 should be considered as a
   * left mouse click
   *
   * UDESK_CLICK_BUTTON1 should be considered as a
   * middle button mouse click
   *
   * UDESK_CLICK_BUTTON2 should be considered as a
   * right mouse click
   */
  UDESK_CLICK_BUTTON0,
#define UDESK_CLICK_BUTTON0        UDESK_CLICK_BUTTON0

  UDESK_CLICK_BUTTON1,
#define UDESK_CLICK_BUTTON1        UDESK_CLICK_BUTTON1

  UDESK_CLICK_BUTTON2,
#define UDESK_CLICK_BUTTON2        UDESK_CLICK_BUTTON2

  UDESK_CLICK_BUTTON3,
#define UDESK_CLICK_BUTTON3        UDESK_CLICK_BUTTON3

  UDESK_CLICK_BUTTON4,
#define UDESK_CLICK_BUTTON4        UDESK_CLICK_BUTTON4

  UDESK_CLICK_BUTTON5,
#define UDESK_CLICK_BUTTON5        UDESK_CLICK_BUTTON5

  UDESK_CLICK_BUTTON6,
#define UDESK_CLICK_BUTTON6        UDESK_CLICK_BUTTON6

  UDESK_CLICK_BUTTON7,
#define UDESK_CLICK_BUTTON7        UDESK_CLICK_BUTTON7

  UDESK_CLICK_BUTTON8,
#define UDESK_CLICK_BUTTON8        UDESK_CLICK_BUTTON8

  UDESK_CLICK_BUTTON9,
#define UDESK_CLICK_BUTTON9        UDESK_CLICK_BUTTON9

  UDESK_CLICK_BUTTON10,
#define UDESK_CLICK_BUTTON10       UDESK_CLICK_BUTTON10

  UDESK_CLICK_BUTTON11,
#define UDESK_CLICK_BUTTON11       UDESK_CLICK_BUTTON11

  UDESK_CLICK_BUTTON12,
#define UDESK_CLICK_BUTTON12       UDESK_CLICK_BUTTON12

  UDESK_CLICK_BUTTON13,
#define UDESK_CLICK_BUTTON13       UDESK_CLICK_BUTTON13

  UDESK_CLICK_BUTTON14,
#define UDESK_CLICK_BUTTON14       UDESK_CLICK_BUTTON14

  UDESK_CLICK_BUTTON15,
#define UDESK_CLICK_BUTTON15       UDESK_CLICK_BUTTON15

  UDESK_CLICK_BUTTON16,
#define UDESK_CLICK_BUTTON16       UDESK_CLICK_BUTTON16

  UDESK_CLICK_BUTTON17,
#define UDESK_CLICK_BUTTON17       UDESK_CLICK_BUTTON17

  UDESK_CLICK_BUTTON18,
#define UDESK_CLICK_BUTTON18       UDESK_CLICK_BUTTON18

  UDESK_CLICK_BUTTON19,
#define UDESK_CLICK_BUTTON19       UDESK_CLICK_BUTTON19

  UDESK_CLICK_BUTTON20,
#define UDESK_CLICK_BUTTON20       UDESK_CLICK_BUTTON20

  UDESK_CLICK_BUTTON21,
#define UDESK_CLICK_BUTTON21       UDESK_CLICK_BUTTON21

  UDESK_CLICK_BUTTON22,
#define UDESK_CLICK_BUTTON22       UDESK_CLICK_BUTTON22

  UDESK_CLICK_BUTTON23,
#define UDESK_CLICK_BUTTON23       UDESK_CLICK_BUTTON23

  UDESK_CLICK_BUTTON24,
#define UDESK_CLICK_BUTTON24       UDESK_CLICK_BUTTON24

  UDESK_CLICK_BUTTON25,
#define UDESK_CLICK_BUTTON25       UDESK_CLICK_BUTTON25

  UDESK_CLICK_BUTTON26,
#define UDESK_CLICK_BUTTON26       UDESK_CLICK_BUTTON26

  UDESK_CLICK_BUTTON27,
#define UDESK_CLICK_BUTTON27       UDESK_CLICK_BUTTON27

  UDESK_CLICK_BUTTON28,
#define UDESK_CLICK_BUTTON28       UDESK_CLICK_BUTTON28

  UDESK_CLICK_BUTTON29,
#define UDESK_CLICK_BUTTON29       UDESK_CLICK_BUTTON29

  UDESK_CLICK_BUTTON30,
#define UDESK_CLICK_BUTTON30       UDESK_CLICK_BUTTON30

  UDESK_CLICK_BUTTON31,
#define UDESK_CLICK_BUTTON31       UDESK_CLICK_BUTTON31

  /* Keyboard FN buttons,
   * guaranteed to be successive values:
   *
   * UDESK_KEYCODE_F<1 + N> - UDESK_KEYCODE_F1 = <N>
   */
  UDESK_KEYCODE_F1,
#define UDESK_KEYCODE_F1           UDESK_KEYCODE_F1

  UDESK_KEYCODE_F2,
#define UDESK_KEYCODE_F2           UDESK_KEYCODE_F2

  UDESK_KEYCODE_F3,
#define UDESK_KEYCODE_F3           UDESK_KEYCODE_F3

  UDESK_KEYCODE_F4,
#define UDESK_KEYCODE_F4           UDESK_KEYCODE_F4

  UDESK_KEYCODE_F5,
#define UDESK_KEYCODE_F5           UDESK_KEYCODE_F5

  UDESK_KEYCODE_F6,
#define UDESK_KEYCODE_F6           UDESK_KEYCODE_F6

  UDESK_KEYCODE_F7,
#define UDESK_KEYCODE_F7           UDESK_KEYCODE_F7

  UDESK_KEYCODE_F8,
#define UDESK_KEYCODE_F8           UDESK_KEYCODE_F8

  UDESK_KEYCODE_F9,
#define UDESK_KEYCODE_F9           UDESK_KEYCODE_F9

  UDESK_KEYCODE_F10,
#define UDESK_KEYCODE_F10          UDESK_KEYCODE_F10

  UDESK_KEYCODE_F11,
#define UDESK_KEYCODE_F11          UDESK_KEYCODE_F11

  UDESK_KEYCODE_F12,
#define UDESK_KEYCODE_F12          UDESK_KEYCODE_F12

  UDESK_KEYCODE_F13,
#define UDESK_KEYCODE_F13          UDESK_KEYCODE_F13

  UDESK_KEYCODE_F14,
#define UDESK_KEYCODE_F14          UDESK_KEYCODE_F14

  UDESK_KEYCODE_F15,
#define UDESK_KEYCODE_F15          UDESK_KEYCODE_F15

  UDESK_KEYCODE_F16
#define UDESK_KEYCODE_F16          UDESK_KEYCODE_F16

};

#ifdef __cplusplus
}
#endif

#endif /* UDESKPLATFORM_H_ */
