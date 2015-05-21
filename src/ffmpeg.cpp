#include "ffmpeg.h"

void FFmpeg::processFrame(ofxFftBasic* fft, const AVCodecContext* codecContext, const AVFrame* frame)
{ 
	(this->*convertData)(frame);
					
	for(int i = 0; i <frame->nb_samples; i++)
	{
		for(int j = 0; j < channels; j++) {
			samples[j][current_index] = converted_data[j][i];
		}
		
		++count;

		if(++current_index == windowsize) {
			// samples array is full
			// we can execute FFT
			doFfts(fft);
			current_index = 0;
		}
	}
}

void FFmpeg::doFfts(ofxFftBasic* fft)
{
	for(int i = 0; i < channels; ++i) {
		doFft(fft, i);
	}
}

void FFmpeg::doFft(ofxFftBasic* fft, int i)
{
	fft->setSignal(samples[i]);
	fft->executeFft();
	int size = fft->getBinSize() * sizeof(float);
	float* amps = (float*)malloc(size);
	memcpy(amps, fft->getAmplitude(), size);
	amplitudes[i].push_back(amps);

	// Find the highest amplitude for each 
	// of the three frequency ranges (treble, vocal, bass).
	// This will be used for normalisation.

	int j = fft->getBinSize();
	while(--j >= treble) {
		if(amps[j] > maxAmplitude[2]) {
			maxAmplitude[2] = amps[j];
		}
	}
	
	while(--j >= bass) {
		if(amps[j] > maxAmplitude[1]) {
			maxAmplitude[1] = amps[j];
		}
	}
	
	while(--j >= 0) {
		if(amps[j] > maxAmplitude[0]) {
			maxAmplitude[0] = amps[j];
		}
	}
}

FFmpeg::FFmpeg() {
	amplitudes = 0;
}
void FFmpeg::setup()
{
    // Initialize FFmpeg
    av_register_all();
}

int FFmpeg::error(string message)
{
	cerr << message << endl;
	return 1;
}

int FFmpeg::run(string filename)
{
	/* 
	 * Follows the standard way of reading an audio file
	 * derived from http://www.gamedev.net/topic/624876-how-to-read-an-audio-file-with-ffmpeg-in-c/
	 */
	////////////////////////////////////////////////////////////////
	// Allocate frame
    AVFrame* frame = av_frame_alloc();
    if (!frame)
    {
        return error("Error av_frame_alloc");
    }

	// Open file
    AVFormatContext* formatContext = NULL;
    if (avformat_open_input(&formatContext, filename.c_str(), NULL, NULL) != 0)
    {
        av_free(frame);
        return error("Error avformat_open_input");
    }

	// Find stream info
    if (avformat_find_stream_info(formatContext, NULL) < 0)
    {
        av_free(frame);
        avformat_close_input(&formatContext);
        return error("Error avformat_find_stream_info");
    }

	// Find stream
    AVCodec* codec = nullptr;
    int streamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);
    if (streamIndex < 0)
    {
        av_free(frame);
        avformat_close_input(&formatContext);
        return error("Error av_find_best_stream");
    }
    AVStream* audioStream = formatContext->streams[streamIndex];

	// Find codec
    AVCodecContext* codecContext = audioStream->codec;
    codecContext->codec = codec;
	if (avcodec_open2(codecContext, codecContext->codec, NULL) != 0)
    {
        av_free(frame);
        avformat_close_input(&formatContext);
        return error("Error avcodec_open2");
    }
	////////////////////////////////////////////////////////////////
	/* 
	 * Code below is not derived from any particular code
	 */
	////////////////////////////////////////////////////////////////
	
	// Set up the conversion function
	// FFmpeg's resampling utilities could probably be used instead

	switch(codecContext->sample_fmt) {
	case AV_SAMPLE_FMT_S16:
		//16-bit (packed), e.g. flac
		convertData = &FFmpeg::convertS16; break;
	case AV_SAMPLE_FMT_S16P:
		//16-bit (planar), e.g. mp3
		convertData = &FFmpeg::convertS16P; break;
	case AV_SAMPLE_FMT_S32:
		//32-bit (packed), e.g. flac
		convertData = &FFmpeg::convertS32; break;
	case AV_SAMPLE_FMT_S32P:
		//32-bit (planar)
		convertData = &FFmpeg::convertS32P; break;
	default:
		//other (to be implemented in case the app should support less popular file extensions)
        av_free(frame);
        avformat_close_input(&formatContext);
		error("Sample format not supported: " + string(av_get_sample_fmt_name(codecContext->sample_fmt)));
	}

	// initialise
	count = 0;
	current_index = 0;
	maxAmplitude = new float[3];

	maxAmplitude[0] = 0;
	maxAmplitude[1] = 0;
	maxAmplitude[2] = 0;

	windowsize = codecContext->sample_rate / 10;
	channels = codecContext->channels;
	bass = windowsize / 75;
	treble = windowsize / 20;

	samples = new float*[channels];
	converted_data = new float*[channels];
	for(int i = 0; i < channels; i++) {
		samples[i] = new float[windowsize];
		converted_data[i] = new float[windowsize];
	}
	amplitudes = new vector<float*>[channels];
	
	// Set up FFT
	ofxFftBasic fft;
	fft.setup(windowsize, OF_FFT_WINDOW_HAMMING);

	////////////////////////////////////////////////////////////////
	/* 
	 * Code below (including comments) is derived from http://www.gamedev.net/topic/624876-how-to-read-an-audio-file-with-ffmpeg-in-c/
	 */
	////////////////////////////////////////////////////////////////

    AVPacket readingPacket;
    av_init_packet(&readingPacket);

    // Read the packets in a loop
    while (av_read_frame(formatContext, &readingPacket) == 0)
    {
        if (readingPacket.stream_index == audioStream->index)
        {
            AVPacket decodingPacket = readingPacket;

            // Audio packets can have multiple audio frames in a single packet
            while (decodingPacket.size > 0)
            {
                // Try to decode the packet into a frame
                // Some frames rely on multiple packets, so we have to make sure the frame is finished before
                // we can use it
                int gotFrame = 0;
                int result = avcodec_decode_audio4(codecContext, frame, &gotFrame, &decodingPacket);

                if (result >= 0 && gotFrame)
                {
                    decodingPacket.size -= result;
                    decodingPacket.data += result;

                    // Process the fully decoded audio frame
					processFrame(&fft, codecContext, frame);
				}
                else
                {
                    decodingPacket.size = 0;
                    decodingPacket.data = nullptr;
                }
            }
        }
        // You *must* call av_free_packet() after each call to av_read_frame() or else you'll leak memory
        av_free_packet(&readingPacket);
    }
	
    // Some codecs will cause frames to be buffered up in the decoding process. If the CODEC_CAP_DELAY flag
    // is set, there can be buffered up frames that need to be flushed, so we'll do that
    if (codecContext->codec->capabilities & CODEC_CAP_DELAY)
    {
        av_init_packet(&readingPacket);
        // Decode all the remaining frames in the buffer, until the end is reached
        int gotFrame = 0;
        while (avcodec_decode_audio4(codecContext, frame, &gotFrame, &readingPacket) >= 0 && gotFrame)
        {
            // We now have a fully decoded audio frame
			processFrame(&fft, codecContext, frame);
			
        }
    }
	
    // Clean up!
	for(int i = 0; i < channels; i++) {
		delete [] samples[i];
		delete [] converted_data[i];
	}
	delete [] samples;
	delete [] converted_data;

    av_free(frame);
    avcodec_close(codecContext);
    avformat_close_input(&formatContext);

	return 0;

	///////////////////////////////////////////////
}

/* 
 * This function should be implemented in ofApp
 * However, type OPENFILENAMEA could not be found 
 * even with <Windows.h> header included
 */
string FFmpeg::dialog() 
{
	/*
	 * derived from Microsoft documentation
	 * https://msdn.microsoft.com/en-us/library/windows/desktop/ms646829
	 */

	char szFile[FILENAME_MAX] = "";  
	OPENFILENAMEA ofn;  
	HWND hwnd;
	hwnd = CreateWindowA(
		"MainWClass", 
		"Open File", 
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		(HWND)NULL,
		(HMENU)NULL,
		(HINSTANCE)NULL,
		(LPVOID)NULL);
	ZeroMemory(&ofn, sizeof(ofn));  
	ofn.lStructSize = sizeof(ofn);  
	ofn.hwndOwner = hwnd;  
	ofn.lpstrFilter = "All Files (*.*)\0*.*\0";  // should be limited to audio files
	ofn.lpstrFile = szFile;  
	ofn.nMaxFile = sizeof(szFile);  
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST ;  
	ofn.lpstrDefExt = 0;  

	if(GetOpenFileNameA(&ofn)) {  
		return string(szFile);  
	}

	return string("");
}

int FFmpeg::getWindowSize() {
	return windowsize;
}

bool FFmpeg::isStereo() {
	return channels > 1;
}

int FFmpeg::getWindowCount() {
	return count / windowsize;
}

int FFmpeg::getChannelsCount() {
	return channels;
}

int FFmpeg::getFqBinCount() {
	return windowsize / 2 + 1;
}

float* FFmpeg::getMaxAmplitude() {
	return maxAmplitude;
}

vector<float*>* FFmpeg::getAmplitudes() {
	return amplitudes;
}

void FFmpeg::convertS32(const AVFrame* frame) {
	int32_t * data = (int32_t*) frame->extended_data[0];
	for(int i = 0; i < channels; ++i) {
		for(int j = 0; j < frame->nb_samples; ++j) {
			converted_data[i][j] = data[2*j+i] / 2147483648.f;
		}
	}
}

void FFmpeg::convertS32P(const AVFrame* frame) {
	int32_t ** data = (int32_t**) frame->extended_data;
	for(int i = 0; i < channels; ++i) {
		for(int j = 0; j < frame->nb_samples; ++j) {
			converted_data[i][j] = data[i][j] / 2147483648.f;
		}
	}
}

void FFmpeg::convertS16(const AVFrame* frame) {
	int16_t * data = (int16_t*) frame->extended_data[0];
	for(int i = 0; i < channels; ++i) {
		for(int j = 0; j < frame->nb_samples; ++j) {
			converted_data[i][j] = data[2*j+i] / 32768.f;
		}
	}
}

void FFmpeg::convertS16P(const AVFrame* frame) {
	int16_t ** data = (int16_t**) frame->extended_data;
	for(int i = 0; i < channels; ++i) {
		for(int j = 0; j < frame->nb_samples; ++j) {
			converted_data[i][j] = data[i][j] / 32768.f;
		}
	}
}

void FFmpeg::reset() {
	if(amplitudes) {
		int length = getWindowCount();
		for(int c = 0; c < channels; ++c) {
			for(auto window = amplitudes[c].begin(); window != amplitudes[c].end(); ++window) {
				delete [] *window;
			}
			amplitudes[c].erase(amplitudes[c].begin(), amplitudes[c].end());
		}
		delete [] amplitudes;
		delete [] maxAmplitude;
	}
}

FFmpeg::~FFmpeg() {
	reset();
}

/* 
 * Functions below are not finished
 * they were going to be used for caching files
 */
void FFmpeg::saveCachedFile(char* szPath) {
	ofstream file;
	int windowcount = getWindowCount();
	int fqcount = getFqBinCount();
	string cached_szPath = getCacheFileName(szPath);
	file.open (cached_szPath, ios::binary);
	file << szPath << endl;
	file << count << endl;
	file << windowsize << endl;
	file << channels << endl;
	file << maxAmplitude << endl;
	for(int c = 0; c < channels; ++c) {
		for(int t = 0; t < windowcount; ++t) {
			for(int f = 0; f < fqcount; ++f) {
				file << amplitudes[c][t][f] << endl;
			}
		}
	}
	file.close();
	cout << getCacheFileName(szPath) << " saved";
}

string FFmpeg::getCacheFileName(char* szPath) {
	string str(szPath);
	hash<string> str_hash;
	size_t f_hash = str_hash(str);
	string str_path("data/" + to_string(f_hash) + ".bahn");
	return str_path;
}

bool FFmpeg::fileExists(const char* szPath) {
	DWORD dwAttrib = GetFileAttributesA(szPath);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool FFmpeg::tryCachedFile(char* szPath) {
	const char* cached_szPath = getCacheFileName(szPath).c_str();
	if(fileExists(cached_szPath)) {

		return true;
	}
	return false;
}
