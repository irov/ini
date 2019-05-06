/******************************************************************************
*
* Author, Yuriy Levchenko <irov13@mail.ru>
*
* This is free and unencumbered software released into the public domain.
*
* Anyone is free to copy, modify, publish, use, compile, sell, or
* distribute this software, either in source code form or as a compiled
* binary, for any purpose, commercial or non - commercial, and by any
* means.
*
* In jurisdictions that recognize copyright laws, the author or authors
* of this software dedicate any and all copyright interest in the
* software to the public domain.We make this dedication for the benefit
* of the public at large and to the detriment of our heirs and
* successors.We intend this dedication to be an overt act of
* relinquishment in perpetuity of all present and future rights to this
* software under copyright law.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* For more information, please refer to <http://unlicense.org/>
*
*****************************************************************************/

#ifndef TINYINI_H_
#define TINYINI_H_

#include <stddef.h>
#include <stdint.h>

/**
@addtogroup compositions
@{
*/

#ifndef TINYINI_MAX_PROPERTIES
#define TINYINI_MAX_PROPERTIES 2048
#endif

typedef struct tinyini_property_t
{
    const char * section;
    const char * property;
    const char * value;
} tinyini_property_t;

typedef struct tinyini_t
{
    tinyini_property_t properties[TINYINI_MAX_PROPERTIES];
    uint32_t property_count;

    char error_message[256];
} tinyini_t;

typedef uint32_t tinyini_result_t;

#define TINYINI_RESULT_SUCCESSFUL (0)
#define TINYINI_RESULT_FAILURE (1)

/**
@brief load ini from buffer
@param [out] _ini tinyini struct.
@param [in] _buffer mutable ini buffer.
*/
tinyini_result_t tinyini_load( tinyini_t * _ini, char * _buffer );

/**
@brief get error message if load is failure
@param [in] _ini tinyini struct.
@return error message.
*/
const char * tinyini_get_error_message( const tinyini_t * _ini );

tinyini_result_t tinyini_has_section( const tinyini_t * _ini, const char * _section );
tinyini_result_t tinyini_has_property( const tinyini_t * _ini, const char * _section, const char * _property );
const char * tinyini_get_property_value( const tinyini_t * _ini, const char * _section, const char * _property );
tinyini_result_t tinyini_equal_property_value( const tinyini_t * _ini, const char * _section, const char * _property, const char * _value );
uint32_t tinyini_count_property_values( const tinyini_t * _ini, const char * _section, const char * _property );
const char * tinyini_get_property_values( const tinyini_t * _ini, const char * _section, const char * _property, uint32_t _index );
uint32_t tinyini_count_properties( const tinyini_t * _ini, const char * _section );
tinyini_result_t tinyini_get_properties( const tinyini_t * _ini, const char * _section, uint32_t _index, const char ** _property, const char ** _value );

// subcompositions
/// @}

#endif