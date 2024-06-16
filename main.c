#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <portaudio.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 256

typedef struct {
    float volume;
    float lastOutput;
} paTestData;

static int brownNoiseCallback(const void *inputBuffer, void *outputBuffer,
                              unsigned long framesPerBuffer,
                              const PaStreamCallbackTimeInfo *timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void *userData) {
    paTestData *data = (paTestData *)userData;
    float *out = (float *)outputBuffer;
    (void)inputBuffer; /* Prevent unused variable warning. */

    for (unsigned long i = 0; i < framesPerBuffer; i++) {
        float white = ((float)rand() / (float)RAND_MAX) * 2 - 1;
        data->lastOutput = (data->lastOutput + (0.02 * white)) / 1.02;
        data->lastOutput *= 0.98;
        *out++ = data->lastOutput * data->volume;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <volume>\n", argv[0]);
        return 1;
    }

    float volume = atof(argv[1]);
    if (volume < 0.0 || volume > 1.0) {
        fprintf(stderr, "Volume must be between 0.0 and 1.0\n");
        return 1;
    }

    PaStream *stream;
    PaError err;
    paTestData data = {volume, 0};

    err = Pa_Initialize();
    if (err != paNoError) goto error;

    err = Pa_OpenDefaultStream(&stream,
                               0,          /* no input channels */
                               1,          /* mono output */
                               paFloat32,  /* 32 bit floating point output */
                               SAMPLE_RATE,
                               FRAMES_PER_BUFFER,
                               brownNoiseCallback,
                               &data);
    if (err != paNoError) goto error;

    err = Pa_StartStream(stream);
    if (err != paNoError) goto error;

    printf("Press Ctrl+C to stop the program...\n");
    while (1) {
        Pa_Sleep(1000); /* Sleep for 1 second */
    }

    err = Pa_StopStream(stream);
    if (err != paNoError) goto error;

    err = Pa_CloseStream(stream);
    if (err != paNoError) goto error;

    Pa_Terminate();
    return 0;

error:
    Pa_Terminate();
    fprintf(stderr, "An error occurred while using the PortAudio stream\n");
    fprintf(stderr, "Error number: %d\n", err);
    fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
    return 1;
}
