#   pragma once

#   include <stddef.h>
#	include <stdint.h>

#	ifndef TINYINI_MAX_SETTINGS
#	define TINYINI_MAX_SETTINGS 512U
#	endif

//////////////////////////////////////////////////////////////////////////
namespace tinyini
{
	class tinyini
	{
	public:
		tinyini();

	public:
		bool load( char * _buffer );

	public:
		bool hasSection( const char * _section ) const;

	public:
		const char * getSettingValue( const char * _section, const char * _key ) const;

	public:
		uint32_t countSettingValues( const char * _section, const char * _key ) const;
		const char * getSettingValues( const char * _section, const char * _key, uint32_t _index ) const;

	public:
		bool getSettings( const char * _section, uint32_t _index, const char ** _key, const char ** _value ) const;
		uint32_t countSettings( const char * _section ) const;

	public:
		const char * getError() const;

	protected:
		bool addSetting_( const char * _section, const char * _key, const char * _value );
		bool loadLine_( char * _buf, const char ** _currentSection );

	protected:
		struct Setting
		{
			const char * section;
			const char * key;
			const char * value;
		};

		Setting m_settings[TINYINI_MAX_SETTINGS];
		uint32_t m_settingsCount;

		char m_error[256];
	};
}