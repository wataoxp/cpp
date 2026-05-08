/*
 * rda.h
 *
 *  Created on: Nov 26, 2025
 *      Author: wataoxp
 */

#ifndef INC_RDA_H_
#define INC_RDA_H_

#include <stdint.h>
#include "i2c.h"
#include "delay.h"

namespace RDA5807x{
	constexpr uint8_t PushAddress = (0x11 << 1);
	constexpr uint8_t SeqAddress = (0x10 << 1);		// シーケンシャルモードでは内部でレジスタアドレスが進んでいく

	enum class Register :uint8_t{
		CHIPID = 0x00,
		CONFIG = 0x02,	// Config,Clock,Seek
		TUNING = 0x03,	// Tune,Band,Space,FrequencySpace
		GPIO   = 0x04,	// De-Emphasis.GPIOinterrupt(RDA5807FP)
		VOLUME = 0x05,	// 5msInterrupt,Volume
		MODE   = 0x07,	// FREQ_MODE
		STATUS = 0x0A,	// RDS,Tune,SeekFlag,ReadChannel
		RSSI   = 0x0B,	// ReadRSSI
	};

	struct Config{
		Config() = delete;

		enum Pos : uint8_t{
			DhizPos     = 15U,
			DmutePos    = 14U,
			MonoPos     = 13U,
			BassPos     = 12U,
			SeekUpPos   = 9U,
			SeekPos     = 8U,
			SkModePos   = 7U,
			ClockPos    = 4U,
			NewPos      = 2U,
			ResetPos    = 1U,
			PowerPos    = 0U,
		};

		enum Value : uint16_t{
			DhizDisable  = 1 << DhizPos,
			DmuteDisable = 1 << DmutePos,
			MonoForce    = 1 << MonoPos,
			BassBoost    = 1 << BassPos,
			SeekUp       = 1 << SeekUpPos,
			SeekDown     = 0 << SeekUpPos,
			SeekStart    = 1 << SeekPos,
			SkModeWrap   = 0 << SkModePos,
			ClockMask    = 0x7 << ClockPos,
			Clock32k     = 0x0 << ClockPos,
			Clock12m     = 0x1 << ClockPos,
			Clock24m     = 0x5 << ClockPos,
			Clock38m     = 0x7 << ClockPos,
			NewMethod    = 1 << NewPos,
			SoftReset    = 1 << ResetPos,
			PowerOn      = 1 << PowerPos,
		};
	};

	struct Tuning{
		Tuning() = delete;

		enum Pos : uint8_t{
			ChannelPos  = 6U,
			TunePos     = 4U,
			BandPos     = 2U,
			SpacePos    = 0U,
		};

		enum Value : uint16_t{
			ChannelMask = 0x3FF << ChannelPos,
			TuneStart   = 0x1 << TunePos,
			BandMask    = 0x3 << BandPos,
			BandEuropa  = 0x0 << BandPos,
			BandJapan   = 0x1 << BandPos,
			BandWorld   = 0x2 << BandPos,
			BandEastEur = 0x3 << BandPos,
			SpaceMask   = 0x3 << SpacePos,
		};
	};

	struct DeEmphasis{
		DeEmphasis() = delete;

		enum Pos : uint8_t{
			StcIntPos      = 14U,
			DeEmphasisPos  = 11U,
			SoftMutePos    = 9U,
			Gpio2IntPos    = 2U,
		};

		enum Value : uint16_t{
			StcIntEnable   = 1 << StcIntPos,
			DeEmphasisJp   = 1 << DeEmphasisPos,
			SoftMuteEnable = 1 << SoftMutePos,
			Gpio2IntMask   = 0xC,
			Gpio2IntOn     = 0x0004,
			Gpio2IntOff    = 0x0000,
		};
	};

	struct Volume{
		Volume() = delete;

		enum Pos : uint8_t{
			IntPos         = 15U,
			ThresholdPos   = 8U,
			LnaPortPos     = 6U,
			LnaIcSelPos    = 4U,
			VolumePos      = 0U,
		};

		enum Value : uint16_t{
			IntDisable     = 1 << IntPos,
			IntEnable      = 0 << IntPos,

			DefaultTh	   = 0x1 << ThresholdPos,
			ThresholdLow   = 0x3 << ThresholdPos,
			ThresholdMid   = 0x7 << ThresholdPos,
			ThresholdMax   = 0xF << ThresholdPos,

			LnaPortMask    = 0x3 << LnaPortPos,
			LnaPortLnan    = 0x1 << LnaPortPos,
			LnaPortLnap    = 0x2 << LnaPortPos,
			LnaPortDual    = 0x3 << LnaPortPos,
			LnaIcSelMask   = 0x3 << LnaIcSelPos,
			LnaIcSelLow    = 0x0 << LnaIcSelPos,
			LnaIcSelMax    = 0x3 << LnaIcSelPos,

			VolumeMask     = 0xF << VolumePos,
			VolumeMax      = 0xF << VolumePos,
		};
	};

	struct Status{
		Status() = delete;

		enum Pos : uint8_t{
			StcPos       = 14U,
			StereoPos    = 10U,
			ReadChanPos  = 0U,
		};

		enum Value : uint16_t{
			StcComplete  = 1 << StcPos,
			StereoReady  = 1 << StereoPos,
			ReadChanMask = 0x03FF,
		};
	};

	struct Rssi{
		Rssi() = delete;

		enum Pos : uint8_t{
			RssiPos      = 9U,
			FmTruePos    = 8U,
			FmReadyPos   = 7U,
		};

		enum Value : uint16_t{
			RssiMask     = 0x7F << RssiPos,
			FmTrue       = 1 << FmTruePos,
			FmReady      = 1 << FmReadyPos,
		};
	};

	    typedef struct{
	        uint16_t Config;
	        uint16_t Tuning;
	        uint16_t Gpio;
	        uint16_t Volume;
	        uint16_t Mode;
	    } RegParameter;

		// CHAN(10進) = (周波数f - 76) / 0.1
	    namespace Frequency{
	        enum List : uint16_t{
	        	FM_YOKOHAMA  = 847,
				TOKYO_FM     = 800,
				BAYFM78		 = 780,
				interfm      = 765,
				J_WAVE       = 813,
				NHK_TOKYO    = 825,
				NHK_YOKOHAMA = 819,
				NACK5    	 = 795,
				BUNKA     	 = 916,
				NIPPON    	 = 930,
				SHONAN 		 = 831,
	        };
	    }

	    /* Register Save */
		typedef struct{
			uint16_t Config;
			uint16_t Tuning;
			uint16_t GpioSet;
			uint16_t VolumeSet;
			uint16_t I2SReg;
			uint16_t ModeSet;
		}ConfigStruct;
	}

class RDA5807{
private:
	I2C& wire;
	DelayPoicy& delay;
	RDA5807x::ConfigStruct RadioReg;
	void SetRegister(RDA5807x::Register Reg,uint16_t Value);
	uint16_t GetRegister(RDA5807x::Register Reg);
	void ModifyRegister(RDA5807x::Register Reg,uint16_t Mask,uint16_t value);
public:
	RDA5807(I2C& i2c,DelayPoicy& pDelay);

	// Write
	void Init(void);
	void Tune(uint16_t channel);
	void Seek(bool isUpSeek);
	void RadioPowerSet(bool enable);

	// Read
	uint8_t SetVolume(int8_t vol);
	uint16_t GetChannel(void);
	uint8_t GetRSSI(void);
};







#endif /* INC_RDA_H_ */
