/*
 * rda.cpp
 *
 * Created on: Nov 26, 2025
 * Author: wataoxp
 */

#include "rda.h"
#include <cassert>

using namespace RDA5807x;

RDA5807::RDA5807(I2C& i2c,DelayPoicy& pDelay) : wire(i2c), delay(pDelay), RadioReg{0}
{
    ;
}

/* Private Helper */

inline void RDA5807::SetRegister(Register Reg, uint16_t Value)
{
    uint8_t w_Buf[2] = {
        (uint8_t)(Value >> 8),
        (uint8_t)(Value & 0xFF),
    };

    wire.MemWrite(PushAddress, static_cast<uint8_t>(Reg), Wires::MemAddSize8, w_Buf, sizeof(w_Buf));
}

uint16_t RDA5807::GetRegister(Register Reg)
{
    uint8_t r_Buf[2];
    uint16_t Value;

    wire.MemRead(PushAddress, static_cast<uint8_t>(Reg), Wires::MemAddSize8, r_Buf, sizeof(r_Buf));

    Value = (r_Buf[0] << 8) | (r_Buf[1] & 0xFF);

    return Value;
}

void RDA5807::ModifyRegister(Register Reg,uint16_t Mask,uint16_t value)
{
	uint16_t w_value = GetRegister(Reg);
	w_value &= ~Mask;
	w_value |= value;

	SetRegister(Reg, w_value);
}

/* Public Write */

void RDA5807::Init(void)
{
    // 水晶発振安定化待ち
    delay.mDelay(1000);

    // 02H: Basic Configuration
    RadioReg.Config = Config::DhizDisable | Config::DmuteDisable | Config::BassBoost | Config::NewMethod | Config::PowerOn;

    // 03H: Band Selection (WORLD)
    RadioReg.Tuning = Tuning::BandWorld;

    // 04H: De-emphasis and Soft Mute
    RadioReg.GpioSet = DeEmphasis::DeEmphasisJp | DeEmphasis::SoftMuteEnable;

    // 05H: LNA and Volume settings
    RadioReg.VolumeSet = Volume::LnaPortDual | Volume::LnaIcSelMax | Volume::VolumeMax | Volume::ThresholdLow;

    uint8_t init[] = {
        (uint8_t)(RadioReg.Config >> 8), (uint8_t)(RadioReg.Config & 0xFF),
        (uint8_t)(RadioReg.Tuning >> 8), (uint8_t)(RadioReg.Tuning & 0xFF),
        (uint8_t)(RadioReg.GpioSet >> 8), (uint8_t)(RadioReg.GpioSet & 0xFF),
        (uint8_t)(RadioReg.VolumeSet >> 8), (uint8_t)(RadioReg.VolumeSet & 0xFF),
    };

    // 書き込み時の先頭アドレスは02h
    wire.Transmit(SeqAddress, init, sizeof(init));
    delay.mDelay(100);
}

void RDA5807::Tune(uint16_t channel)
{
    // channel = Freq*10
    assert(channel > 100);

    channel -= 760;

    RadioReg.Tuning &= (uint16_t)~Tuning::ChannelMask;
    RadioReg.Tuning |= (channel << Tuning::ChannelPos) | Tuning::TuneStart;

    SetRegister(Register::TUNING, RadioReg.Tuning);
}

void RDA5807::Seek(bool isUpSeek)
{
	uint16_t SeekDirection = (isUpSeek)? (Config::SeekUp):(Config::SeekDown);

	RadioReg.Config |= Config::SeekStart | SeekDirection;
	SetRegister(Register::CONFIG, RadioReg.Config);

	delay.mDelay(100);

	while(!(GetRegister(Register::STATUS) & Status::StcComplete));

	RadioReg.Config &= ~(Config::SeekStart | Config::SeekUp);
}

void RDA5807::RadioPowerSet(bool isenable)
{
    RadioReg.Config &= (uint16_t)~Config::PowerOn;

    if (isenable)
    {
        RadioReg.Config |= Config::PowerOn;
    }

    SetRegister(Register::CONFIG, RadioReg.Config);
}

uint8_t RDA5807::SetVolume(int8_t vol)
{
    int8_t updateVolume = vol;

    if (updateVolume > Volume::VolumeMax)
    {
        updateVolume = Volume::VolumeMax;
    }
    else if (updateVolume < 0)
    {
        updateVolume = 0;
    }

    RadioReg.VolumeSet &= (uint16_t)~Volume::VolumeMask;
    RadioReg.VolumeSet |= (uint16_t)(updateVolume << Volume::VolumePos);
    SetRegister(Register::VOLUME, RadioReg.VolumeSet);

    return (uint8_t)updateVolume;
}

/* Public Read */

uint16_t RDA5807::GetChannel(void)
{
    uint16_t ReadFreq;

    ReadFreq = GetRegister(Register::STATUS);
    ReadFreq &= Status::ReadChanMask;
    ReadFreq += 760;    // 76.0MHz基準のオフセット

    return ReadFreq;
}

uint8_t RDA5807::GetRSSI(void)
{
    uint16_t rssiReg;

    rssiReg = GetRegister(Register::RSSI);

    // RSSIビットフィールドを抽出して右詰めにする
    uint8_t rssi = (uint8_t)((rssiReg & Rssi::RssiMask) >> Rssi::RssiPos);

    return rssi;
}
