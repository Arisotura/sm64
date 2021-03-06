/*---------------------------------------------------------------------------------

	default ARM7 core

		Copyright (C) 2005 - 2010
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/
#include <dswifi7.h>
#include <maxmod7.h>

#include "../../audio/external.h"
#include "../../audio/internal.h"
#include "../nds_include.h"

void osInvalDCache(UNUSED void *a, UNUSED size_t b) {
}

void *dma_sample_data(uintptr_t devAddr, UNUSED u32 size, UNUSED s32 arg2, UNUSED u8 *dmaIndexRef) {
    return devAddr;
}

// gVolRampingLhs136[k] = 2^16 * max(1, (256*k)^(1/17)
f32 gVolRampingLhs136[128] = {
    65536.0f,    90811.555f,  94590.766f,  96873.96f,   98527.26f,   99829.06f,   100905.47f,
    101824.61f,  102627.57f,  103341.086f, 103983.55f,  104568.164f, 105104.75f,  105600.8f,
    106062.14f,  106493.46f,  106898.52f,  107280.414f, 107641.73f,  107984.62f,  108310.93f,
    108622.23f,  108919.875f, 109205.055f, 109478.8f,   109742.0f,   109995.48f,  110239.94f,
    110476.02f,  110704.305f, 110925.3f,   111139.45f,  111347.21f,  111548.945f, 111745.0f,
    111935.7f,   112121.35f,  112302.2f,   112478.51f,  112650.51f,  112818.4f,   112982.38f,
    113142.66f,  113299.37f,  113452.69f,  113602.766f, 113749.734f, 113893.73f,  114034.87f,
    114173.26f,  114309.02f,  114442.26f,  114573.055f, 114701.5f,   114827.69f,  114951.695f,
    115073.6f,   115193.47f,  115311.375f, 115427.39f,  115541.56f,  115653.96f,  115764.63f,
    115873.64f,  115981.04f,  116086.86f,  116191.164f, 116293.99f,  116395.38f,  116495.38f,
    116594.02f,  116691.34f,  116787.39f,  116882.19f,  116975.77f,  117068.17f,  117159.414f,
    117249.54f,  117338.57f,  117426.53f,  117513.45f,  117599.35f,  117684.266f, 117768.2f,
    117851.195f, 117933.266f, 118014.44f,  118094.72f,  118174.14f,  118252.71f,  118330.46f,
    118407.4f,   118483.55f,  118558.914f, 118633.53f,  118707.4f,   118780.54f,  118852.97f,
    118924.695f, 118995.74f,  119066.11f,  119135.82f,  119204.88f,  119273.31f,  119341.125f,
    119408.32f,  119474.92f,  119540.93f,  119606.36f,  119671.22f,  119735.52f,  119799.28f,
    119862.5f,   119925.195f, 119987.36f,  120049.02f,  120110.18f,  120170.84f,  120231.016f,
    120290.71f,  120349.945f, 120408.7f,   120467.016f, 120524.875f, 120582.3f,   120639.28f,
    120695.84f,  120751.984f
};

// gVolRampingRhs136[k] = 1 / max(1, (256*k)^(1/17))
f32 gVolRampingRhs136[128] = {
    1.0f,      0.72167f,  0.692837f, 0.676508f, 0.665156f, 0.656482f, 0.649479f, 0.643616f, 0.638581f,
    0.634172f, 0.630254f, 0.62673f,  0.62353f,  0.620601f, 0.617902f, 0.615399f, 0.613067f, 0.610885f,
    0.608835f, 0.606901f, 0.605073f, 0.603339f, 0.60169f,  0.600119f, 0.598618f, 0.597183f, 0.595806f,
    0.594485f, 0.593215f, 0.591991f, 0.590812f, 0.589674f, 0.588573f, 0.587509f, 0.586478f, 0.585479f,
    0.58451f,  0.583568f, 0.582654f, 0.581764f, 0.580898f, 0.580055f, 0.579233f, 0.578432f, 0.57765f,
    0.576887f, 0.576142f, 0.575414f, 0.574701f, 0.574005f, 0.573323f, 0.572656f, 0.572002f, 0.571361f,
    0.570733f, 0.570118f, 0.569514f, 0.568921f, 0.568339f, 0.567768f, 0.567207f, 0.566656f, 0.566114f,
    0.565582f, 0.565058f, 0.564543f, 0.564036f, 0.563537f, 0.563046f, 0.562563f, 0.562087f, 0.561618f,
    0.561156f, 0.560701f, 0.560253f, 0.559811f, 0.559375f, 0.558945f, 0.558521f, 0.558102f, 0.557689f,
    0.557282f, 0.55688f,  0.556483f, 0.556091f, 0.555704f, 0.555322f, 0.554944f, 0.554571f, 0.554203f,
    0.553839f, 0.553479f, 0.553123f, 0.552772f, 0.552424f, 0.55208f,  0.55174f,  0.551404f, 0.551071f,
    0.550742f, 0.550417f, 0.550095f, 0.549776f, 0.549461f, 0.549148f, 0.548839f, 0.548534f, 0.548231f,
    0.547931f, 0.547634f, 0.54734f,  0.547048f, 0.54676f,  0.546474f, 0.546191f, 0.54591f,  0.545632f,
    0.545357f, 0.545084f, 0.544813f, 0.544545f, 0.54428f,  0.544016f, 0.543755f, 0.543496f, 0.543239f,
    0.542985f, 0.542732f
};

// gVolRampingLhs144[k] = 2^16 * max(1, (256*k)^(1/18))
f32 gVolRampingLhs144[128] = {
    65536.0f,    89180.734f,  92681.9f,    94793.33f,   96320.52f,   97522.02f,  98514.84f,
    99362.14f,   100101.99f,  100759.16f,  101350.664f, 101888.74f,  102382.46f, 102838.75f,
    103263.016f, 103659.58f,  104031.914f, 104382.89f,  104714.88f,  105029.89f, 105329.61f,
    105615.5f,   105888.81f,  106150.63f,  106401.914f, 106643.49f,  106876.12f, 107100.44f,
    107317.05f,  107526.47f,  107729.17f,  107925.6f,   108116.125f, 108301.12f, 108480.88f,
    108655.72f,  108825.91f,  108991.68f,  109153.28f,  109310.914f, 109464.77f, 109615.04f,
    109761.88f,  109905.46f,  110045.92f,  110183.41f,  110318.02f,  110449.91f, 110579.17f,
    110705.914f, 110830.234f, 110952.234f, 111071.99f,  111189.59f,  111305.12f, 111418.64f,
    111530.23f,  111639.95f,  111747.875f, 111854.05f,  111958.54f,  112061.4f,  112162.67f,
    112262.42f,  112360.68f,  112457.51f,  112552.93f,  112647.0f,   112739.76f, 112831.23f,
    112921.46f,  113010.484f, 113098.33f,  113185.02f,  113270.61f,  113355.11f, 113438.555f,
    113520.97f,  113602.375f, 113682.805f, 113762.27f,  113840.81f,  113918.44f, 113995.18f,
    114071.055f, 114146.08f,  114220.266f, 114293.65f,  114366.24f,  114438.06f, 114509.12f,
    114579.44f,  114649.02f,  114717.91f,  114786.086f, 114853.586f, 114920.42f, 114986.6f,
    115052.14f,  115117.055f, 115181.34f,  115245.04f,  115308.13f,  115370.65f, 115432.59f,
    115493.98f,  115554.81f,  115615.11f,  115674.875f, 115734.12f,  115792.85f, 115851.08f,
    115908.82f,  115966.07f,  116022.85f,  116079.16f,  116135.01f,  116190.4f,  116245.35f,
    116299.87f,  116353.945f, 116407.6f,   116460.84f,  116513.67f,  116566.09f, 116618.125f,
    116669.76f,  116721.01f
};

// gVolRampingRhs144[k] = 1 / max(1, (256*k)^(1/18))
f32 gVolRampingRhs144[128] = {
    1.0f,      0.734867f, 0.707107f, 0.691357f, 0.680395f, 0.672012f, 0.66524f,  0.659567f, 0.654692f,
    0.650422f, 0.646626f, 0.643211f, 0.64011f,  0.637269f, 0.634651f, 0.632223f, 0.629961f, 0.627842f,
    0.625852f, 0.623975f, 0.622199f, 0.620515f, 0.618913f, 0.617387f, 0.615929f, 0.614533f, 0.613196f,
    0.611912f, 0.610677f, 0.609487f, 0.60834f,  0.607233f, 0.606163f, 0.605128f, 0.604125f, 0.603153f,
    0.60221f,  0.601294f, 0.600403f, 0.599538f, 0.598695f, 0.597874f, 0.597074f, 0.596294f, 0.595533f,
    0.59479f,  0.594064f, 0.593355f, 0.592661f, 0.591983f, 0.591319f, 0.590669f, 0.590032f, 0.589408f,
    0.588796f, 0.588196f, 0.587608f, 0.58703f,  0.586463f, 0.585906f, 0.58536f,  0.584822f, 0.584294f,
    0.583775f, 0.583265f, 0.582762f, 0.582268f, 0.581782f, 0.581303f, 0.580832f, 0.580368f, 0.579911f,
    0.57946f,  0.579017f, 0.578579f, 0.578148f, 0.577722f, 0.577303f, 0.576889f, 0.576481f, 0.576078f,
    0.575681f, 0.575289f, 0.574902f, 0.574519f, 0.574142f, 0.573769f, 0.5734f,   0.573036f, 0.572677f,
    0.572321f, 0.57197f,  0.571623f, 0.57128f,  0.57094f,  0.570605f, 0.570273f, 0.569945f, 0.56962f,
    0.569299f, 0.568981f, 0.568667f, 0.568355f, 0.568047f, 0.567743f, 0.567441f, 0.567142f, 0.566846f,
    0.566553f, 0.566263f, 0.565976f, 0.565692f, 0.56541f,  0.565131f, 0.564854f, 0.56458f,  0.564309f,
    0.56404f,  0.563773f, 0.563509f, 0.563247f, 0.562987f, 0.56273f,  0.562475f, 0.562222f, 0.561971f,
    0.561722f, 0.561476f
};

// gVolRampingLhs128[k] = 2^16 * max(1, (256*k)^(1/16))
f32 gVolRampingLhs128[128] = {
    65536.0f,    92681.9f,    96785.28f,   99269.31f,   101070.33f,  102489.78f,  103664.336f,
    104667.914f, 105545.09f,  106324.92f,  107027.39f,  107666.84f,  108253.95f,  108796.87f,
    109301.95f,  109774.29f,  110217.98f,  110636.39f,  111032.33f,  111408.164f, 111765.9f,
    112107.234f, 112433.66f,  112746.46f,  113046.766f, 113335.555f, 113613.72f,  113882.02f,
    114141.164f, 114391.77f,  114634.414f, 114869.58f,  115097.74f,  115319.31f,  115534.68f,
    115744.19f,  115948.16f,  116146.875f, 116340.625f, 116529.66f,  116714.195f, 116894.46f,
    117070.64f,  117242.945f, 117411.52f,  117576.55f,  117738.17f,  117896.54f,  118051.77f,
    118204.0f,   118353.35f,  118499.92f,  118643.83f,  118785.16f,  118924.01f,  119060.47f,
    119194.625f, 119326.555f, 119456.336f, 119584.03f,  119709.71f,  119833.445f, 119955.29f,
    120075.31f,  120193.555f, 120310.08f,  120424.94f,  120538.17f,  120649.836f, 120759.97f,
    120868.62f,  120975.82f,  121081.62f,  121186.05f,  121289.14f,  121390.94f,  121491.47f,
    121590.766f, 121688.87f,  121785.79f,  121881.57f,  121976.24f,  122069.82f,  122162.33f,
    122253.805f, 122344.266f, 122433.73f,  122522.23f,  122609.77f,  122696.4f,   122782.11f,
    122866.93f,  122950.89f,  123033.99f,  123116.26f,  123197.72f,  123278.37f,  123358.24f,
    123437.34f,  123515.69f,  123593.3f,   123670.19f,  123746.36f,  123821.84f,  123896.63f,
    123970.76f,  124044.23f,  124117.04f,  124189.23f,  124260.78f,  124331.73f,  124402.07f,
    124471.83f,  124540.99f,  124609.59f,  124677.63f,  124745.12f,  124812.055f, 124878.47f,
    124944.34f,  125009.71f,  125074.57f,  125138.92f,  125202.79f,  125266.164f, 125329.06f,
    125391.5f,   125453.47f
};

// gVolRampingRhs128[k] = 1 / max(1, (256*k)^(1/16))
f32 gVolRampingRhs128[128] = {
    1.0f,      0.707107f, 0.677128f, 0.660184f, 0.64842f,  0.639439f, 0.632194f, 0.626133f, 0.620929f,
    0.616375f, 0.612329f, 0.608693f, 0.605391f, 0.60237f,  0.599587f, 0.597007f, 0.594604f, 0.592355f,
    0.590243f, 0.588251f, 0.586369f, 0.584583f, 0.582886f, 0.581269f, 0.579725f, 0.578247f, 0.576832f,
    0.575473f, 0.574166f, 0.572908f, 0.571696f, 0.570525f, 0.569394f, 0.5683f,   0.567241f, 0.566214f,
    0.565218f, 0.564251f, 0.563311f, 0.562398f, 0.561508f, 0.560642f, 0.559799f, 0.558976f, 0.558173f,
    0.55739f,  0.556625f, 0.555877f, 0.555146f, 0.554431f, 0.553732f, 0.553047f, 0.552376f, 0.551719f,
    0.551075f, 0.550443f, 0.549823f, 0.549216f, 0.548619f, 0.548033f, 0.547458f, 0.546892f, 0.546337f,
    0.545791f, 0.545254f, 0.544726f, 0.544206f, 0.543695f, 0.543192f, 0.542696f, 0.542209f, 0.541728f,
    0.541255f, 0.540788f, 0.540329f, 0.539876f, 0.539429f, 0.538988f, 0.538554f, 0.538125f, 0.537702f,
    0.537285f, 0.536873f, 0.536467f, 0.536065f, 0.535669f, 0.535277f, 0.534891f, 0.534509f, 0.534131f,
    0.533759f, 0.53339f,  0.533026f, 0.532666f, 0.53231f,  0.531958f, 0.53161f,  0.531266f, 0.530925f,
    0.530588f, 0.530255f, 0.529926f, 0.529599f, 0.529277f, 0.528957f, 0.528641f, 0.528328f, 0.528018f,
    0.527711f, 0.527407f, 0.527106f, 0.526808f, 0.526513f, 0.52622f,  0.525931f, 0.525644f, 0.525359f,
    0.525077f, 0.524798f, 0.524522f, 0.524247f, 0.523975f, 0.523706f, 0.523439f, 0.523174f, 0.522911f,
    0.522651f, 0.522393f
};

s8 gReverbDownsampleRate = 1;
s16 gVolume = 0x7FFF;
s32 gMaxSimultaneousNotes = 16;// TEMP!! 16;
s32 gAudioErrorFlags = 0;
s8 gAudioUpdatesPerFrame = 1;//4;

struct Note *gNotes;
u8 *gBankLoadStatus9;

u8 gSoundMode;
u8 gDefaultPanVolume;
u8 gStereoPanVolume;
u8 gHeadsetPanVolume;
u8 gHeadsetPanQuantization;

s16 buffer_left[528];
s16 buffer_right[528];
int frame_count;

s16* notebuffer;

extern u32 numdecode;

//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------
	//Wifi_Update();
    frame_count++;
}


//---------------------------------------------------------------------------------
void VcountHandler() {
//---------------------------------------------------------------------------------
	inputGetAndSend();
}

volatile bool exitflag = false;

//---------------------------------------------------------------------------------
void powerButtonCB() {
//---------------------------------------------------------------------------------
	exitflag = true;
}

void debug(u32 val1, u32 val2)
{
	fifoSendValue32(FIFO_USER_04, val1);
	fifoSendValue32(FIFO_USER_05, val2);
}

void audio_generator()
{
	u64 cmdBuf[4096];
	s32 writtenCmds;
	s16 audio_buffer[528 * 2];
	synthesis_execute(cmdBuf, &writtenCmds, audio_buffer, 528);
}

//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------
	// clear sound registers
	dmaFillWords(0, (void*)0x04000400, 0x100);

	REG_SOUNDCNT |= SOUND_ENABLE;
	writePowerManagement(PM_CONTROL_REG, ( readPowerManagement(PM_CONTROL_REG) & ~PM_SOUND_MUTE ) | PM_SOUND_AMP );
	powerOn(POWER_SOUND);

	readUserSettings();
	ledBlink(0);

	irqInit();
	// Start the RTC tracking IRQ
	initClockIRQ();
	fifoInit();
	touchInit();

	//mmInstall(FIFO_MAXMOD);

	SetYtrigger(80);

	installWifiFIFO();
	installSoundFIFO();

	installSystemFIFO();

	irqSet(IRQ_VCOUNT, VcountHandler);
	irqSet(IRQ_VBLANK, VblankHandler);

	irqEnable( IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);

	setPowerButtonCB(powerButtonCB);

    while (!fifoCheckValue32(FIFO_USER_03));
    gBankLoadStatus9 = fifoGetValue32(FIFO_USER_03);

    while (!fifoCheckValue32(FIFO_USER_02));
    gNotes = fifoGetValue32(FIFO_USER_02);
	
	while (!fifoCheckValue32(FIFO_USER_03));
	notebuffer = fifoGetValue32(FIFO_USER_03);
	
	frame_count = 0;

    enableSound();

    /*SCHANNEL_SOURCE(0) = buffer_left;
    SCHANNEL_REPEAT_POINT(0) = 0;
    SCHANNEL_LENGTH(0) = 528 / 2;
    SCHANNEL_TIMER(0) = SOUND_FREQ(32000);
    SCHANNEL_CR(0) = SOUND_VOL(127) | SOUND_PAN(0) | (1 << 29) | SOUND_ONE_SHOT;

    SCHANNEL_SOURCE(1) = buffer_right;
    SCHANNEL_REPEAT_POINT(1) = 0;
    SCHANNEL_LENGTH(1) = 528 / 2;
    SCHANNEL_TIMER(1) = SOUND_FREQ(32000);
    SCHANNEL_CR(1) = SOUND_VOL(127) | SOUND_PAN(127) | (1 << 29) | SOUND_ONE_SHOT;*/
	
	// 240Hz ~= 0x2217C cycles
	TIMER_CR(0) = 0;
	TIMER_CR(1) = 0;
	irqSet(IRQ_TIMER(1), audio_generator);
	irqEnable(IRQ_TIMER(1));
	TIMER_DATA(0) = 0x10000-0x4;
	TIMER_CR(0) = TIMER_ENABLE;
	TIMER_DATA(1) = 0x10000-0x885F;
	TIMER_CR(1) = TIMER_ENABLE | TIMER_IRQ_REQ | TIMER_CASCADE;

	// Keep the ARM7 mostly idle
	while (!exitflag) {
		if ( 0 == (REG_KEYINPUT & (KEY_SELECT | KEY_START | KEY_L | KEY_R))) {
			exitflag = true;
		}
		
		numdecode = 0;

		//for (int derp = 0; derp < 4; derp++)
		/*{
        u64 cmdBuf[4096];
        s32 writtenCmds;
        s16 audio_buffer[528 * 2];
        synthesis_execute(cmdBuf, &writtenCmds, audio_buffer, 528);
		}*/

        /*for (int i = 0; i < 528; i++) {
            buffer_left[i]  = audio_buffer[i * 2 + 0];
            buffer_right[i] = audio_buffer[i * 2 + 1];
        }*/

        //SCHANNEL_CR(0) |= SCHANNEL_ENABLE;
        //SCHANNEL_CR(1) |= SCHANNEL_ENABLE;

        if (frame_count == 0) swiWaitForVBlank();
		//debug(numdecode, frame_count);
        frame_count = 0;
	}
	return 0;
}

