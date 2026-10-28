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

/* stems by reference and Stem objects (variable.c) */
variableptr oo_stem_box( tsd_t *TSD, var_hashtable *vars, const streng *stemname );
void oo_link_stem( tsd_t *TSD, const streng *localname, variableptr target );
const streng *oo_stem_default( tsd_t *TSD, variableptr stem );
const streng *oo_stem_get( tsd_t *TSD, variableptr stem, const streng *tail );
int oo_stem_each( tsd_t *TSD, variableptr st,
                  void (*cb)( tsd_t *, void *, const streng *, const streng * ),
                  void *data );
void oo_stem_put( tsd_t *TSD, var_hashtable *vars, const streng *stemname,
                  const streng *tail, streng *value );
void oo_stem_assign( tsd_t *TSD, var_hashtable *vars, const streng *name, variableptr from );

/* the real stem box of a Stem object handle, else NULL (oo.c) */
variableptr oo_stem_object_box( tsd_t *TSD, const streng *s );
/* "RETURN x." in OO code: a Stem object with a copy of the stem */
streng *oo_eval_stemobj( tsd_t *TSD, nodeptr thisptr );

/*
 * The OO mode of the process: 1 forces the extension on everywhere, -1
 * switches it off completely (Regina 3.9.7 behaviour), 0 decides per
 * program. Set by the --oo / --no-oo switches, else by the environment
 * variable REGINA_OO (1/on/yes/oo or 0/off/no/classic).
 */
int oo_mode( void );
void oo_set_mode( int mode );

/* expression nodes */
streng *oo_eval_message( tsd_t *TSD, nodeptr thisptr, int need_result );
streng *oo_eval_index( tsd_t *TSD, nodeptr thisptr );
streng *oo_eval_envsymbol( tsd_t *TSD, nodeptr thisptr );
streng *oo_dot_call( tsd_t *TSD, nodeptr thisptr, int need_result, int *handled );
int oo_dot_instruction( tsd_t *TSD, nodeptr expr );
streng *oo_is_as( tsd_t *TSD, nodeptr thisptr );
streng *oo_typeof( tsd_t *TSD, nodeptr thisptr );
void oo_dialect_start( tsd_t *TSD, int dialect );
streng *oo_dialect_compound( tsd_t *TSD, nodeptr thisptr );
int oo_dialect_assign( tsd_t *TSD, nodeptr target, streng *value );
streng *oo_typecheck( tsd_t *TSD, nodeptr thisptr );
streng *oo_dialect_symbol( tsd_t *TSD, const streng *name );
streng *oo_eval_over_init( tsd_t *TSD, nodeptr thisptr );
streng *oo_eval_over_item( tsd_t *TSD, nodeptr thisptr );
void oo_over_assign( tsd_t *TSD, nodeptr thisptr );
void oo_do_counter( tsd_t *TSD, nodeptr var, long n );

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
int oo_label_main( const tsd_t *TSD );
streng *oo_condition_object( tsd_t *TSD, const sigtype *sig );
void oo_level_ends( tsd_t *TSD, proclevel level );
long oo_gc( tsd_t *TSD, int forced );
/* workspace functions []NL ... (see oo.c) */
streng *oo_q_clear( tsd_t *TSD, cparamboxptr parms );
streng *oo_q_cr( tsd_t *TSD, cparamboxptr parms );
streng *oo_q_ex( tsd_t *TSD, cparamboxptr parms );
streng *oo_q_fx( tsd_t *TSD, cparamboxptr parms );
streng *oo_q_gc( tsd_t *TSD, cparamboxptr parms );
streng *oo_q_in( tsd_t *TSD, cparamboxptr parms );
streng *oo_q_load( tsd_t *TSD, cparamboxptr parms );
streng *oo_q_nl( tsd_t *TSD, cparamboxptr parms );
streng *oo_q_out( tsd_t *TSD, cparamboxptr parms );
streng *oo_q_save( tsd_t *TSD, cparamboxptr parms );
streng *oo_q_wsid( tsd_t *TSD, cparamboxptr parms );
streng *oo_q_wsitem( tsd_t *TSD, cparamboxptr parms );
streng *oo_q_wsobj( tsd_t *TSD, cparamboxptr parms );
streng *oo_q_wsvar( tsd_t *TSD, cparamboxptr parms );
int oo_ws_echo( tsd_t *TSD, const streng *envir, const streng *cmd );
void oo_gc_set_mode( int mode );
streng *oo_eval_block( tsd_t *TSD, nodeptr thisptr );
streng *oo_run_block( tsd_t *TSD, nodeptr body, paramboxptr args, void *ctx,
                      var_hashtable *share, void (*setup)( tsd_t *, void * ), void *data );

/* true if the X_CON_SYMBOL is an environment symbol like .NIL */
#define OO_IS_ENVSYMBOL(n) ( (n)->name && (n)->name->len > 1 \
                             && (n)->name->value[0] == '.' \
                             && rx_isalpha( (n)->name->value[1] ) )

#endif
