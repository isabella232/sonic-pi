#include <string>

#include <lame/lame.h>

#include "audio_tools.h"


int AudioTools::convert_wav_to_mp3(std::string src_path, std::string dst_path) {
    /**
     * Convert a wav file to mp3 using the Lame library
     */

    FILE * src = fopen(src_path.c_str(), "rb");

    if (!src)
        return -1;

    FILE * dst = fopen(dst_path.c_str(), "wb");

    const int PCM_SIZE = 8192;
    const int MP3_SIZE = 8192;

    short int pcm_buffer[PCM_SIZE * 2];
    unsigned char mp3_buffer[MP3_SIZE];

    // Lame config
    lame_t lame = lame_init();

    lame_set_in_samplerate(lame, 44100);
    lame_set_VBR(lame, vbr_default);
    lame_set_num_channels(lame, 2);

    lame_init_params(lame);

    int read, write;

    // Convert
    do {
        read = fread(pcm_buffer, 2 * sizeof(short int),
                     PCM_SIZE, src);

        if (read == 0)
            write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
        else
            write = lame_encode_buffer_interleaved(lame, pcm_buffer, read,
                                                   mp3_buffer, MP3_SIZE);

        fwrite(mp3_buffer, write, 1, dst);
    } while (read != 0);

    // Clean up
    lame_close(lame);

    fclose(src);
    fclose(dst);

    return 0;
}
