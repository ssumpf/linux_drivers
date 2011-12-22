/*
 * \brief   DDE Linux 2.6 based audio driver
 * \author  Christian Helmuth
 * \date    2009-12-19
 */

#include <asoundlib.h>

#include <dde_linux26/general.h>
#include <dde_linux26/audio.h>
#include <dde_kit/printf.h>

/**************************
 ** Initialization calls **
 **************************/

extern int (*dde_kit_initcall_1_dde_linux26_page_cache_init)(void);
extern int (*dde_kit_initcall_1_helper_init)(void);
extern int (*dde_kit_initcall_2_pcibus_class_init)(void);
extern int (*dde_kit_initcall_2_pci_driver_init)(void);
extern int (*dde_kit_initcall_4__call_init_workqueues)(void);
extern int (*dde_kit_initcall_4_dde_linux26_init_pci)(void);
extern int (*dde_kit_initcall_4_param_sysfs_init)(void);
extern int (*dde_kit_initcall_4_ac97_bus_init)(void);
extern int (*dde_kit_initcall_6_init_soundcore)(void);
extern int (*dde_kit_initcall_6_alsa_ac97_init)(void);
extern int (*dde_kit_initcall_6_alsa_ak4531_init)(void);
extern int (*dde_kit_initcall_6_alsa_card_ens137x_init)(void);
extern int (*dde_kit_initcall_6_alsa_card_intel8x0_init)(void);
extern int (*dde_kit_initcall_6_alsa_card_azx_init)(void);
extern int (*dde_kit_initcall_6_alsa_pcm_init)(void);
extern int (*dde_kit_initcall_6_alsa_rawmidi_init)(void);
extern int (*dde_kit_initcall_6_alsa_sound_init)(void);
extern int (*dde_kit_initcall_6_alsa_sound_last_init)(void);
extern int (*dde_kit_initcall_6_alsa_timer_init)(void);
extern int (*dde_kit_initcall_6_latency_init)(void);
extern int (*dde_kit_initcall_6_pci_init)(void);
extern int (*dde_kit_initcall_6_snd_mem_init)(void);


void do_initcalls(void)
{
	dde_kit_initcall_1_dde_linux26_page_cache_init();
	dde_kit_initcall_1_helper_init();
	dde_kit_initcall_2_pcibus_class_init();
	dde_kit_initcall_2_pci_driver_init();
	dde_kit_initcall_4__call_init_workqueues();
	dde_kit_initcall_4_dde_linux26_init_pci();
	dde_kit_initcall_4_ac97_bus_init();
	dde_kit_initcall_6_init_soundcore();
	dde_kit_initcall_6_alsa_ac97_init();
	dde_kit_initcall_6_alsa_ak4531_init();
	dde_kit_initcall_6_alsa_card_ens137x_init();
	dde_kit_initcall_6_alsa_card_intel8x0_init();
	dde_kit_initcall_6_alsa_card_azx_init();
	dde_kit_initcall_6_alsa_pcm_init();
	dde_kit_initcall_6_alsa_rawmidi_init();
	dde_kit_initcall_6_alsa_sound_init();
	dde_kit_initcall_6_alsa_sound_last_init();
	dde_kit_initcall_6_alsa_timer_init();
	dde_kit_initcall_6_pci_init();
	dde_kit_initcall_6_snd_mem_init();
}


/**********************
 ** Driver interface **
 **********************/

enum { PERIOD_SIZE = 1024, PERIODS = 8 };

static snd_pcm_t *pcm_handle;

int dde_linux26_audio_init(void)
{
	dde_linux26_init();
	do_initcalls();

	int count = dde_linux26_audio_init_devices();
	dde_kit_printf("found %d PCM devices\n", count);

	snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;

	struct sndrv_pcm_hw_params _hwparams;
	snd_pcm_hw_params_t *hwparams = &_hwparams;
	memset(hwparams, 0, sizeof(*hwparams));

	const char *pcm_name = "hw:0,0";

	snd_output_t *output;
	snd_output_stdio_attach(&output, stdout, 0);

	int err;

	if ((err = snd_pcm_open(&pcm_handle, pcm_name, stream, 0)) < 0) {
		dde_kit_printf("Error %d opening PCM device %s\n", err, pcm_name);
		return -1;
	}

	if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0) {
		dde_kit_printf("Can not configure this PCM device.\n");
		return -2;
	}

//	dde_kit_printf("initial hw_params\n");
//	snd_pcm_hw_params_dump(hwparams, output);

	int              rate      = 44100;
	int              channels  = 2;
	snd_pcm_access_t access    = SND_PCM_ACCESS_RW_INTERLEAVED;
	snd_pcm_format_t format    = SND_PCM_FORMAT_S16_LE;

	if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, access) < 0) {
		dde_kit_printf("Error setting access.\n");
		return -3;
	}

	if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, format) < 0) {
		dde_kit_printf("Error setting format.\n");
		return -4;
	}

	if (snd_pcm_hw_params_set_rate(pcm_handle, hwparams, rate, 0) < 0) {
		dde_kit_printf("Error setting rate.\n");
		return -5;
	}

	if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, channels) < 0) {
		dde_kit_printf("Error setting channels.\n");
		return -6;
	}

	if (snd_pcm_hw_params_set_periods(pcm_handle, hwparams, PERIODS, 0) < 0) {
		dde_kit_printf("Error setting periods.\n");
		return -7;
	}

	if (snd_pcm_hw_params_set_period_size(pcm_handle, hwparams, PERIOD_SIZE, 0) < 0) {
		dde_kit_printf("Error setting period size.\n");
		return -8;
	}

	if (snd_pcm_hw_params_set_buffer_size(pcm_handle, hwparams, PERIOD_SIZE * PERIODS) < 0) {
		dde_kit_printf("Error setting buffersize.\n");
		return -9;
	}

	if (snd_pcm_hw_params(pcm_handle, hwparams) < 0) {
		dde_kit_printf("Error setting HW params.\n");
		return -10;
	}

//	dde_kit_printf("final hw_params\n");
//	snd_pcm_hw_params_dump(hwparams, output);

	snd_pcm_prepare(pcm_handle);

	/*
	 * Mixer
	 */
	snd_mixer_t *handle = 0;

	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, "hw:0");
	snd_mixer_load(handle);

	int foo;
	long min, max = 0;
	struct _snd_mixer_selem_id _sid;
	snd_mixer_selem_id_t *sid = &_sid;
	memset(sid, 0, sizeof(*sid));
	snd_mixer_elem_t *elem;

	snd_mixer_selem_id_set_index(sid, 0);

	snd_mixer_selem_id_set_name(sid, "Master");
	elem = snd_mixer_find_selem(handle, sid);
	if (elem) {
		foo = snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
		foo = snd_mixer_selem_set_playback_volume_all(elem, max / 2);
		foo = snd_mixer_selem_set_playback_switch_all(elem, 1);
	}

	snd_mixer_selem_id_set_name(sid, "PCM");
	elem = snd_mixer_find_selem(handle, sid);
	if (elem) {
		foo = snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
		foo = snd_mixer_selem_set_playback_volume_all(elem, max - 2);
		foo = snd_mixer_selem_set_playback_switch_all(elem, 1);
	}

	snd_mixer_selem_id_set_name(sid, "Front");
	elem = snd_mixer_find_selem(handle, sid);
	if (elem) {
		foo = snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
		foo = snd_mixer_selem_set_playback_volume_all(elem, max - 2);
		foo = snd_mixer_selem_set_playback_switch_all(elem, 1);
	}

	snd_mixer_close(handle);

	return 0;
}


void dde_linux26_audio_adopt_myself(void)
{
	dde_linux26_process_add_worker("backend");
}


void dde_linux26_audio_stop(void)
{
	snd_pcm_drain(pcm_handle);
}


void dde_linux26_audio_start(void)
{
	snd_pcm_prepare(pcm_handle);
}


static void flush_hw(void)
{
	/* zero out sound-hardware buffer, this is necessary because the VirtualBox
	 * AC97 seems to play more than a period after buffer underrun
	 */
	char *hwbuf =  dde_linux26_get_hwbuf_by_fd(pcm_handle->fd);
	if (hwbuf)
		memset(hwbuf, 0, PERIOD_SIZE * PERIODS * 4);
}


int dde_linux26_audio_play(void *data, int frame_cnt)
{
	int played;

	if (snd_pcm_state(pcm_handle) == SND_PCM_STATE_XRUN) {
			flush_hw();
			dde_linux26_audio_start();
	}

	played = snd_pcm_writei(pcm_handle, data, frame_cnt);
	if (played != frame_cnt) {

		if (played == -EPIPE) {
			return -2;
		}

		dde_kit_printf("played (%d) != frame_cnt (%d)\n", played, frame_cnt);
		return -1;
	}

	return 0;
}
