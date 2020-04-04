#include <nds.h>
#include <fat.h>
#include <iostream>
#include "sound.h"
#include <maxmod9.h>

#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>
#include <mad.h>
#include <climits>

using namespace std;

struct audio audio;

audio::audio() {
	sys.mod_count = 0;
	sys.samp_count = 0;
	sys.mem_bank = 0;
	sys.fifo_channel = FIFO_MAXMOD;
	mmInit(&sys);
	stream.buffer_length = MAXMODBUFFER;
	stream.format = MM_STREAM_16BIT_STEREO;
	stream.timer = MM_TIMER2;
	stream.sampling_rate = 0;
	stream.manual = 1;
}

void audio::end() {
	if (mp3) {
		mad_synth_finish(&mp3->synth);
		mad_frame_finish(&mp3->frame);
		mad_stream_finish(&mp3->stream);
		delete mp3;
	}
	if (ogg) {
		ov_clear(&ogg->vf);
		delete ogg;
	}
	if (inbuf) {
		fclose(inbuf);
	}
	mmStreamClose();
	stream.sampling_rate = 0;
}

static signed short MadFixedToSshort(mad_fixed_t Fixed) {
	if(Fixed>=MAD_F_ONE)
		return(SHRT_MAX);
	if(Fixed<=-MAD_F_ONE)
		return(-SHRT_MAX);
	Fixed=Fixed>>(MAD_F_FRACBITS-15);
	return((signed short)Fixed);
}

bool loadAudio(string filepath) {
	FILE* inbuf = fopen(filepath.c_str(), "rb");
	if (!inbuf) {
		cout << "\nFailed to open sound file " << filepath;
		return false;
	}
	audio.inbuf = inbuf;
	bool success = false;
	string extension = filepath.substr(filepath.find_last_of(".") + 1);
	if (extension == "mp3") {
		success = loadMp3();
	} else if (extension == "ogg") {
		success = loadOgg();
	} else {
		cout << "\nNot a supported audio file " << filepath;
	}
	return success;
}

bool loadMp3() {
	audio.mp3 = new struct mp3;
	mad_stream_init(&audio.mp3->stream);
	mad_frame_init(&audio.mp3->frame);
	mad_synth_init(&audio.mp3->synth);
	fillMp3();
	while (1) {
		if (mad_header_decode(&audio.mp3->frame.header, &audio.mp3->stream) == 0) {
			audio.stream.sampling_rate = audio.mp3->frame.header.samplerate;
			break;
		} else if (audio.mp3->stream.error != MAD_ERROR_LOSTSYNC) {
			cout << "\nlibmad fatal error: " << mad_stream_errorstr(&audio.mp3->stream);
			audio.end();
			return false;
		};
	}
	audio.stream.callback = mm_mp3_callback;
	return true;
}

void fillMp3() {
	struct mp3* mp3 = audio.mp3;
	int res;
	int rem = 0;
	if (mp3->stream.next_frame != NULL) {
		rem = mp3->stream.bufend - mp3->stream.next_frame;
		memmove(mp3->buffer, mp3->stream.next_frame, rem);
		res = fread(mp3->buffer + rem, 1, MP3BUFFER - rem - MAD_BUFFER_GUARD + 1, audio.inbuf);
	} else {
		res = fread(mp3->buffer, 1, MP3BUFFER - MAD_BUFFER_GUARD + 1, audio.inbuf);
	}
	if (res + rem <= MP3BUFFER - MAD_BUFFER_GUARD) {
		mp3->guard = mp3->buffer + res + rem;
		memset(mp3->guard, 0, MAD_BUFFER_GUARD);
		mad_stream_buffer(&mp3->stream, mp3->buffer, res + rem + MAD_BUFFER_GUARD);
	} else {
		mad_stream_buffer(&mp3->stream, mp3->buffer, MP3BUFFER - MAD_BUFFER_GUARD + 1);
	}
}

mm_word mm_mp3_callback(mm_word length, mm_addr dest, mm_stream_formats format) {
	mp3* mp3 = audio.mp3;
	s16* output = (s16*)dest;
	uint samples = 0;
	while (samples < length) {
		for(int i = mp3->f; i < mp3->synth.pcm.length; i++){
			s16 sample;
			sample = MadFixedToSshort(mp3->synth.pcm.samples[0][i]);
			*(output++) = sample;
			if(MAD_NCHANNELS(&mp3->frame.header)==2){
				sample=MadFixedToSshort(mp3->synth.pcm.samples[1][i]);
			}
			*(output++) = sample;
			samples++;
			mp3->f++;
			if (samples == length) {
				return samples;
			}
		}
		if (mad_frame_decode(&mp3->frame, &mp3->stream)) {
			if (mp3->stream.error == MAD_ERROR_BUFPTR || mp3->stream.error == MAD_ERROR_BUFLEN) {
				if (mp3->stream.this_frame == mp3->guard) {
					audio.end();
					break;
				} else {
					fillMp3();
				}
			} else if (mp3->stream.error != MAD_ERROR_LOSTSYNC && mp3->stream.error != MAD_ERROR_BADSAMPLERATE) {
				cout << "\nlibmad fatal error: " << mad_stream_errorstr(&mp3->stream);
				audio.end();
				break;
			}
		} else {
			mad_synth_frame(&mp3->synth, &mp3->frame);
			//cout << "\nsamplerate: " << mp3->frame.header.samplerate;
			mp3->f = 0;
		}
	}
	return samples;
}

bool loadOgg() {
	audio.ogg = new struct ogg;
	if (ov_open(audio.inbuf, &audio.ogg->vf, NULL, 0) != 0) {
		cout << "\nFailed to open ogg file";
		audio.end();
		return false;
	};
	vorbis_info* info = ov_info(&audio.ogg->vf, -1);
	audio.stream.sampling_rate = info->rate;
	audio.stream.callback = mm_ogg_callback;
	return true;
}

mm_word mm_ogg_callback(mm_word length, mm_addr dest, mm_stream_formats format) {
	char* output = (char*)dest;
	int res = ov_read(&audio.ogg->vf, output, length * 4, NULL);
	if (res) {
		length = res / 4;
	} else {
		audio.end();
		length = 0;
	}
	return length;
}

bool playAudio() {
	if (!(audio.mp3 || audio.ogg)) {
		cout << "\nNo audio loaded";
		return false;
	}
	if (audio.stream.sampling_rate <= 0) {
		cout << "\nInvalid sampling rate";
		return false;
	}
	mmStreamOpen(&audio.stream);
	return true;
}