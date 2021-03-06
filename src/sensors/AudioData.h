/**
* Copyright 2017 IBM Corp. All Rights Reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/


#ifndef AUDIO_DATA_H
#define AUDIO_DATA_H

#include "IData.h"
#include "sensors/ISensor.h"
#include "utils/Sound.h"
#include "SelfLib.h"

//! This data type contains raw wave audio data..
class SELF_API AudioData : public IData
{
public:
	RTTI_DECL();

	//! Construction
	AudioData() : m_Freq( 0 ), m_Channels( 0 ), m_BPS( 0 )
	{}
	AudioData(const std::string & a_WaveData, unsigned int a_Freq, unsigned int a_Channels, unsigned int a_BPS ) :
		m_WaveData( a_WaveData ), m_Freq( a_Freq ), m_Channels( a_Channels ), m_BPS( a_BPS )
	{}

	//! ISerializable interface
	virtual void Serialize(Json::Value & json)
	{
		json["m_WaveData"] = StringUtil::EncodeBase64( m_WaveData );
		json["m_Freq"] = m_Freq;
		json["m_Channels"] = m_Channels;
		json["m_BPS"] = m_BPS;
	}

	virtual void Deserialize(const Json::Value & json)
	{
		m_WaveData = StringUtil::DecodeBase64( json["m_WaveData"].asString() );
		m_Freq = json["m_Freq"].asUInt();
		m_Channels = json["m_Channels"].asUInt();
		m_BPS = json["m_BPS"].asUInt();
	}

	//! IData interface
	virtual bool ToBinary( std::string & a_Output )
	{
		a_Output = m_WaveData;
		return true;
	}
	virtual bool FromBinary( const std::string & a_Type, const std::string & a_Input )
	{
		if ( ParseAudioFormat( a_Type, m_Freq, m_BPS, m_Channels ) )
		{
			m_WaveData = a_Input;
			return true;
		}

		return false;
	}

	const std::string & GetWaveData() const
	{
		return m_WaveData;
	}

	unsigned int GetFrequency() const
	{
		return m_Freq;
	}
	unsigned int GetChannels() const
	{
		return m_Channels;
	}
	unsigned int GetBPS() const
	{
		return m_BPS;
	}

	static bool ParseAudioFormat( const std::string & a_Format, 
		unsigned int & a_Freq, unsigned int & a_BPS, unsigned int & a_Channels )
	{
		std::vector<std::string> parts;
		StringUtil::Split( a_Format, ";", parts );

		if ( parts.size() > 0 && StringUtil::Compare( parts[0], "audio/L16", true ) == 0 )
		{
			a_BPS = 16;
			a_Freq = 16000;
			a_Channels = 1;

			for(size_t i=1;i<parts.size();++i)
			{
				std::vector<std::string> kv;
				StringUtil::Split( parts[i], "=", kv );
				if ( kv.size() == 2 )
				{
					if ( StringUtil::Compare( kv[0], "rate", true ) == 0 )
						a_Freq = atoi( kv[1].c_str() );
					else if ( StringUtil::Compare( kv[0], "channels", true ) == 0 )
						a_Channels = atoi( kv[1].c_str() );
				}
			}

			return true;
		}

		return false;
	}

private:
	//! Data
	std::string			m_WaveData;
	unsigned int		m_Freq;
	unsigned int		m_Channels;
	unsigned int		m_BPS;		// bits per sample
};

#endif
