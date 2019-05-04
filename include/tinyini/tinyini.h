#ifndef TINYINI_H_
#define TINYINI_H_

#include <stddef.h>
#include <stdint.h>

#ifndef TINYINI_MAX_PROPERTIES
#define TINYINI_MAX_PROPERTIES 2048
#endif

//////////////////////////////////////////////////////////////////////////

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

tinyini_result_t tinyini_load( tinyini_t * _ini, char * _buffer );

typedef void( *tinyini_save_provider_t )(const char * _line, void * _ud);
tinyini_result_t tinyini_save( const tinyini_t * _ini, tinyini_save_provider_t _provider, void * _ud );

const char * tinyini_get_error_message( const tinyini_t * _ini );

tinyini_result_t tinyini_has_section( const tinyini_t * _ini, const char * _section );
tinyini_result_t tinyini_has_property( const tinyini_t * _ini, const char * _section, const char * _property );
const char * tinyini_get_property_value( const tinyini_t * _ini, const char * _section, const char * _property );
uint32_t tinyini_count_property_values( const tinyini_t * _ini, const char * _section, const char * _property );
const char * tinyini_get_property_values( const tinyini_t * _ini, const char * _section, const char * _property, uint32_t _index );
uint32_t tinyini_count_properties( const tinyini_t * _ini, const char * _section );
tinyini_result_t tinyini_get_properties( const tinyini_t * _ini, const char * _section, uint32_t _index, const char ** _property, const char ** _value );

#endif