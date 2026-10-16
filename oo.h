/*
 *  The Regina Rexx Interpreter - object-oriented extension
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  Objects are represented in Rexx variables by "handle" strings: a
 *  16 byte string that cannot be typed in a program and that identifies
 *  an entry in the per-thread object table. Everything that copies
 *  strings (assignment, stems, arguments, RETURN, the queue) therefore
 *  copies object references for free. Wherever a string value is really
 *  needed (SAY, concatenation, built-in function arguments, commands,
 *  PARSE, arithmetic) the object's STRING method is called.
 *
 *  None of this is active until a program uses OO syntax, so classic
 *  programs run exactly as before (TSD->oo_active stays 0).
 */
#ifndef REGINA_OO_H_INCLUDED
#define REGINA_OO_H_INCLUDED

#define OO_HANDLE_LEN 16

/* quick test without a table lookup; s must not be NULL */
#define OO_LOOKS_LIKE_HANDLE(s) ( (s)->len == OO_HANDLE_LEN \
                                  && (s)->value[0] == '\0' \
                                  && (s)->value[1] == '\001' \
                                  && (s)->value[2] == 'O' )

/*
 * Returns a newly allocated string for an object handle (by sending it
 * the STRING message) or NULL if s is no object handle. Cheap for plain
 * strings.
 */
streng *oo_stringify( tsd_t *TSD, const streng *s );

/*
 * Replaces *s by its string value if it is an object handle. *owned tells
 * whether *s must be freed by the caller; it is updated.
 */
void oo_stringify_inplace( tsd_t *TSD, streng **s, int *owned );

/* charnr of temporary operand nodes; they are not traced again */
#define OO_TEMP_NODE (-4711)

/* operators with an object as left operand, see oo.c */
streng *oo_operator( tsd_t *TSD, nodeptr thisptr, int *handled, streng **leftval );

/* "an Array" for an object handle without calling methods, else NULL */
streng *oo_describe( tsd_t *TSD, const streng *s );

/* simple variable value without tracing or NOVALUE (variable.c) */
const streng *oo_peek_simple( tsd_t *TSD, const streng *name );

/* expression nodes */
streng *oo_eval_message( tsd_t *TSD, nodeptr thisptr, int need_result );
streng *oo_eval_index( tsd_t *TSD, nodeptr thisptr );
streng *oo_eval_envsymbol( tsd_t *TSD, nodeptr thisptr );
streng *oo_eval_over_init( tsd_t *TSD, nodeptr thisptr );
streng *oo_eval_over_item( tsd_t *TSD, nodeptr thisptr );
void oo_over_assign( tsd_t *TSD, nodeptr thisptr );

/* instructions */
void oo_exec_init( tsd_t *TSD, nodeptr thisptr );
void oo_exec_message( tsd_t *TSD, nodeptr thisptr );
void oo_exec_assign( tsd_t *TSD, nodeptr thisptr );
void oo_exec_expose( tsd_t *TSD, nodeptr thisptr );
void oo_exec_usearg( tsd_t *TSD, nodeptr thisptr );

/*
 * ::ROUTINE support. Returns NOFUNC if name is no routine, otherwise the
 * routine's result (may be NULL). args is consumed.
 */
streng *oo_call_routine( tsd_t *TSD, const streng *name, paramboxptr args );
int oo_has_routine( tsd_t *TSD, const streng *name );

/* takes over the final state of a kept parse tree (see killsystem()) */
void oo_take_tree( tsd_t *TSD, const internal_parser_type *tree );

/* frees all OO state of a thread when its program has ended */
void oo_end_program( tsd_t *TSD );

/* label lookup inside methods and routines; see getlabel() */
nodeptr oo_getlabel( const tsd_t *TSD, const streng *name );

/* true if the X_CON_SYMBOL is an environment symbol like .NIL */
#define OO_IS_ENVSYMBOL(n) ( (n)->name && (n)->name->len > 1 \
                             && (n)->name->value[0] == '.' \
                             && rx_isalpha( (n)->name->value[1] ) )

#endif
