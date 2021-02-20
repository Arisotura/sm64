#include <stdio.h>

#include "nds_include.h"
#include <fat.h>

#include "audio/external.h"
#include "game/game_init.h"
#include "nds_renderer.h"

OSMesg D_80339BEC;
OSMesgQueue gSIEventMesgQueue;

s8 gResetTimer;
s8 D_8032C648;
s8 gDebugLevelSelect;
s8 gShowProfiler;
s8 gShowDebugText;

extern void arm7_setup();
extern void create_next_audio_buffer();

void set_vblank_handler(UNUSED s32 index, UNUSED struct VblankHandler *handler, UNUSED OSMesgQueue *queue, UNUSED OSMesg *msg) {
}

void dispatch_audio_sptask(UNUSED struct SPTask *spTask) {
}

void send_display_list(struct SPTask *spTask) {
    draw_frame((Gfx*)spTask->task.t.data_ptr);

    create_next_audio_buffer();
}

void zonp()
{
	while (fifoCheckValue32(FIFO_USER_05))
	{
		u32 val1 = fifoGetValue32(FIFO_USER_04);
		u32 val2 = fifoGetValue32(FIFO_USER_05);
		printf("D: %08X %08X\n", val1, val2);
	}
}

int main(void) {
    static u64 pool[0x165000 / sizeof(u64)];
    main_pool_init(pool, pool + sizeof(pool) / sizeof(pool[0]));
    gEffectsMemoryPool = mem_pool_init(0x4000, MEMORY_POOL_LEFT);

    renderer_init();

#ifdef LIBFAT
    if (!fatInitDefault()) {
        printf("Failed to initialize libfat!\n");
    }
#endif

    audio_init();
    sound_init();

    arm7_setup();
	
	s16* notebuffer = (s16*)malloc(sizeof(s16) * 16 * 32768);
	DC_InvalidateRange(notebuffer, sizeof(s16) * 16 * 32768);
	fifoSendValue32(FIFO_USER_03, notebuffer);

    thread5_game_loop(NULL);
	
	// heh
	free(notebuffer);

    return 0;
}
