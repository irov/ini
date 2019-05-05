#include "tinyini.h"

#include <stdio.h>
#include <string.h>

#ifndef TINYINI_MAX_KEY_SIZE
#define TINYINI_MAX_KEY_SIZE (256)
#endif

#ifndef TINYINI_MAX_VALUE_SIZE
#define TINYINI_MAX_VALUE_SIZE (256)
#endif

#ifndef TINYINI_NULLPTR
#define TINYINI_NULLPTR ((void *)0)
#endif

//////////////////////////////////////////////////////////////////////////
inline static void __tinyini_rtrim( char * s )
{
    size_t len = strlen( s );

    char * e = s + len - 1;

    while( *e == ' ' || *e == '\t' )
    {
        --e;
    }

    *(e + 1) = '\0';
}
//////////////////////////////////////////////////////////////////////////
inline static char * __tinyini_trim( char * s )
{
    while( *s == ' ' || *s == '\t' )
    {
        ++s;
    }

    __tinyini_rtrim( s );

    return s;
}
//////////////////////////////////////////////////////////////////////////
static tinyini_result_t __tinyini_add_property( tinyini_t * _ini, const char * _section, const char * _property, const char * _value )
{
    if( strlen( _value ) + 1 >= TINYINI_MAX_VALUE_SIZE )
    {
        sprintf( _ini->error_message, "elements '%s' key '%s' value '%s' len %zu > max len %u"
            , _section
            , _property
            , _value
            , strlen( _value )
            , TINYINI_MAX_VALUE_SIZE
        );

        return TINYINI_RESULT_FAILURE;
    }

    if( _ini->property_count + 1 == TINYINI_MAX_PROPERTIES )
    {
        sprintf( _ini->error_message, "elements is MAX %d"
            , TINYINI_MAX_PROPERTIES
        );

        return TINYINI_RESULT_FAILURE;
    }

    tinyini_property_t * property = _ini->properties + _ini->property_count;

    property->section = _section;
    property->property = _property;
    property->value = _value;

    ++_ini->property_count;

    return TINYINI_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
static tinyini_result_t __tinyini_load_line( tinyini_t * _ini, char * _line, const char ** _section )
{
    char * trim_line = __tinyini_trim( _line );

    size_t len = strlen( trim_line );

    if( len == 0 )
    {
        return TINYINI_RESULT_SUCCESSFUL;
    }

    if( trim_line[0] == ';' )
    {
        return TINYINI_RESULT_SUCCESSFUL;
    }

    if( trim_line[0] == '#' )
    {
        return TINYINI_RESULT_SUCCESSFUL;
    }

    if( trim_line[0] == '[' && trim_line[len - 1] == ']' )
    {
        char name[TINYINI_MAX_KEY_SIZE];
        if( sscanf( trim_line, "[%[^]]", name ) != 1 )
        {
            sprintf( _ini->error_message, "invalidate parse section: %s"
                , trim_line
            );

            return TINYINI_RESULT_FAILURE;
        }

        char * section_str = strstr( trim_line, name );
        size_t section_len = strlen( name );

        section_str[section_len] = '\0';

        *_section = section_str;

        return TINYINI_RESULT_SUCCESSFUL;
    }

    if( *_section == TINYINI_NULLPTR )
    {
        sprintf( _ini->error_message, "first element is not section"
        );

        return TINYINI_RESULT_FAILURE;
    }

    char key[TINYINI_MAX_KEY_SIZE];
    char value[TINYINI_MAX_VALUE_SIZE];
    if( sscanf( trim_line, "%[^=] = \"%[^\"]\"", key, value ) == 2
        || sscanf( trim_line, "%[^=] = '%[^\']'", key, value ) == 2
        || sscanf( trim_line, "%[^=] = %[^;#]", key, value ) == 2 )
    {
        char * key_str = strstr( trim_line, key );
        size_t key_len = strlen( key );
        key_str[key_len] = '\0';
        __tinyini_rtrim( key_str );

        char * value_str = strstr( trim_line + key_len + 1, value );
        size_t value_len = strlen( value );
        value_str[value_len] = '\0';
        __tinyini_rtrim( value_str );

        if( __tinyini_add_property( _ini, *_section, key_str, value_str ) == TINYINI_RESULT_FAILURE )
        {
            return TINYINI_RESULT_FAILURE;
        }

        return TINYINI_RESULT_SUCCESSFUL;
    }

    if( sscanf( trim_line, "%[^=] =", key ) == 1 )
    {
        char * key_str = strstr( trim_line, key );
        size_t key_len = strlen( key );
        key_str[key_len] = '\0';
        __tinyini_rtrim( key_str );

        if( __tinyini_add_property( _ini, *_section, key_str, key_str + key_len ) == TINYINI_RESULT_FAILURE )
        {
            return TINYINI_RESULT_FAILURE;
        }

        return TINYINI_RESULT_SUCCESSFUL;
    }

    return TINYINI_RESULT_FAILURE;
}
//////////////////////////////////////////////////////////////////////////
tinyini_result_t tinyini_load( tinyini_t * _ini, char * _buffer )
{
    _ini->property_count = 0;
    _ini->error_message[0] = '\0';

    const char * section = TINYINI_NULLPTR;

    char * line = strtok( _buffer, "\r\n" );
    for( ; line != TINYINI_NULLPTR; line = strtok( TINYINI_NULLPTR, "\r\n" ) )
    {
        if( __tinyini_load_line( _ini, line, &section ) == TINYINI_RESULT_FAILURE )
        {
            return TINYINI_RESULT_FAILURE;
        }
    }

    return TINYINI_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
tinyini_result_t tinyini_save( const tinyini_t * _ini, tinyini_save_provider_t _provider, void * _ud )
{
    const char * sections[256];
    uint32_t sections_count = 0;

    const tinyini_property_t * it_property = _ini->properties;
    const tinyini_property_t * it_property_end = _ini->properties + _ini->property_count;
    for( ; it_property != it_property_end; ++it_property )
    {
        uint32_t section_index = 0;
        for( ; section_index != sections_count; ++section_index )
        {
            const char * section = sections[section_index];

            if( strcmp( it_property->section, section ) == 0 )
            {
                sections[sections_count] = it_property->section;
                ++sections_count;

                char line_message[2048];
                sprintf( line_message, "[%s]\n"
                    , section
                );

                (*_provider)(line_message, _ud);

                uint32_t properties_count = tinyini_count_properties( _ini, section );

                uint32_t property_index = 0;
                for( ; property_index != properties_count; ++property_index )
                {
                    const char * property;
                    const char * value;
                    if( tinyini_get_properties( _ini, section, property_index, &property, &value ) == TINYINI_RESULT_SUCCESSFUL )
                    {
                        sprintf( line_message, "%s=%s\n"
                            , property
                            , value
                        );

                        (*_provider)(line_message, _ud);
                    }
                }
            }
        }
    }

    return TINYINI_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
const char * tinyini_get_error_message( const tinyini_t * _ini )
{
    return _ini->error_message;
}
//////////////////////////////////////////////////////////////////////////
tinyini_result_t tinyini_has_section( const tinyini_t * _ini, const char * _section )
{
    const tinyini_property_t * it_property = _ini->properties;
    const tinyini_property_t * it_property_end = _ini->properties + _ini->property_count;
    for( ; it_property != it_property_end; ++it_property )
    {
        if( strcmp( it_property->section, _section ) != 0 )
        {
            continue;
        }

        return TINYINI_RESULT_SUCCESSFUL;
    }

    return TINYINI_RESULT_FAILURE;
}
//////////////////////////////////////////////////////////////////////////
tinyini_result_t tinyini_has_property( const tinyini_t * _ini, const char * _section, const char * _property )
{
    const tinyini_property_t * it_property = _ini->properties;
    const tinyini_property_t * it_property_end = _ini->properties + _ini->property_count;
    for( ; it_property != it_property_end; ++it_property )
    {
        if( strcmp( it_property->section, _section ) != 0 )
        {
            continue;
        }

        if( strcmp( it_property->property, _property ) != 0 )
        {
            continue;
        }

        return TINYINI_RESULT_SUCCESSFUL;
    }

    return TINYINI_RESULT_FAILURE;
}
//////////////////////////////////////////////////////////////////////////
const char * tinyini_get_property_value( const tinyini_t * _ini, const char * _section, const char * _property )
{
    const tinyini_property_t * it_property = _ini->properties;
    const tinyini_property_t * it_property_end = _ini->properties + _ini->property_count;
    for( ; it_property != it_property_end; ++it_property )
    {
        if( strcmp( it_property->section, _section ) != 0 )
        {
            continue;
        }

        if( strcmp( it_property->property, _property ) != 0 )
        {
            continue;
        }

        const char * value = it_property->value;

        return value;
    }

    return TINYINI_NULLPTR;
}
//////////////////////////////////////////////////////////////////////////
tinyini_result_t tinyini_equal_property_value( const tinyini_t * _ini, const char * _section, const char * _property, const char * _value )
{
    const tinyini_property_t * it_property = _ini->properties;
    const tinyini_property_t * it_property_end = _ini->properties + _ini->property_count;
    for( ; it_property != it_property_end; ++it_property )
    {
        if( strcmp( it_property->section, _section ) != 0 )
        {
            continue;
        }

        if( strcmp( it_property->property, _property ) != 0 )
        {
            continue;
        }

        if( strcmp( it_property->value, _value ) != 0 )
        {
            continue;
        }

        return TINYINI_RESULT_SUCCESSFUL;
    }

    return TINYINI_RESULT_FAILURE;
}
//////////////////////////////////////////////////////////////////////////
uint32_t tinyini_count_property_values( const tinyini_t * _ini, const char * _section, const char * _property )
{
    uint32_t count = 0;

    const tinyini_property_t * it_property = _ini->properties;
    const tinyini_property_t * it_property_end = _ini->properties + _ini->property_count;
    for( ; it_property != it_property_end; ++it_property )
    {
        if( strcmp( it_property->section, _section ) != 0 )
        {
            continue;
        }

        if( strcmp( it_property->property, _property ) != 0 )
        {
            continue;
        }

        ++count;
    }

    return count;
}
//////////////////////////////////////////////////////////////////////////
const char * tinyini_get_property_values( const tinyini_t * _ini, const char * _section, const char * _property, uint32_t _index )
{
    uint32_t count = 0;

    const tinyini_property_t * it_property = _ini->properties;
    const tinyini_property_t * it_property_end = _ini->properties + _ini->property_count;
    for( ; it_property != it_property_end; ++it_property )
    {
        if( strcmp( it_property->section, _section ) != 0 )
        {
            continue;
        }

        if( strcmp( it_property->property, _property ) != 0 )
        {
            continue;
        }

        if( count != _index )
        {
            ++count;

            continue;
        }

        const char * value = it_property->value;

        return value;
    }

    return 0;
}
//////////////////////////////////////////////////////////////////////////
uint32_t tinyini_count_properties( const tinyini_t * _ini, const char * _section )
{
    uint32_t count = 0;

    const tinyini_property_t * it_property = _ini->properties;
    const tinyini_property_t * it_property_end = _ini->properties + _ini->property_count;
    for( ; it_property != it_property_end; ++it_property )
    {
        if( strcmp( it_property->section, _section ) != 0 )
        {
            continue;
        }

        ++count;
    }

    return count;
}
//////////////////////////////////////////////////////////////////////////
tinyini_result_t tinyini_get_properties( const tinyini_t * _ini, const char * _section, uint32_t _index, const char ** _property, const char ** _value )
{
    uint32_t count = 0;

    const tinyini_property_t * it_property = _ini->properties;
    const tinyini_property_t * it_property_end = _ini->properties + _ini->property_count;
    for( ; it_property != it_property_end; ++it_property )
    {
        if( strcmp( it_property->section, _section ) != 0 )
        {
            continue;
        }

        if( count != _index )
        {
            ++count;

            continue;
        }

        *_property = it_property->property;
        *_value = it_property->value;

        return TINYINI_RESULT_SUCCESSFUL;
    }

    return TINYINI_RESULT_FAILURE;
}