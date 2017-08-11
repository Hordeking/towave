//towave - a program to extract the channels from a chiptune file
//Copyright 2015 Hordeking (substantial editions to the code)
/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Original copyright:
//Copyright 2011 Bryan Mitchell, under the same license as above.
*/

#include "wave_writer.h"
#include "gme/gme.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>

#ifdef NEWMODE
#include <getopt.h>
#endif

//A couple of flags that apply everywhere in this file.
bool verbose = false, simulate_only = false;


void print_usage(void) {
#ifdef NEWMODE
    std::cerr << "Usage: towave [-T <time>] [-t <track>] [-V <voice>] <filename>" << std::endl;
    std::cerr << std::endl;
    std::cerr << "  -t<track #>\tThe track number within the music file, starting at 1." << std::endl;
    std::cerr << "  -T<time>\tLength (in seconds) to convert (eg 3.142)." << std::endl;
    std::cerr << "  -V<voice #>\tConvert only the indicated track voice. Unimplemented." << std::endl;
    std::cerr << "  -D<target>\tDirectory you wish to write waves to." << std::endl;
    std::cerr << "  -v\t\tVerbose mode." << std::endl;
    std::cerr << "  -n\t\tSimulate output, but do not write any files." << std::endl;
    std::cerr << std::endl;
#else
	std::cerr << "Please call towave from the command prompt." << std::endl;
	std::cerr << "Proper syntax is" << std::endl;
	std::cerr << "\ttowave filename" << std::endl;
	std::cerr << "Where filename is of any type accepted by GME. (See readme)";
	std::cerr << std::endl;
#endif
	return;
}


std::string num2str(int x) {
	std::stringstream result;
	
	result << x;
	
	return result.str();
}

void writeTheWave(gme_t* emu, std::string wav_name, int tracklen, int i, int sample_rate) {
	//Ignoring silence allows us to record tracks that start in or have
	//long periods of silence. Unfortunately, this also means that
	//if a track is of finite length, we still need to have its length separately.
	gme_ignore_silence(emu, true);
	
	//Create a muting mask to isolate the channel
	int mute = -1;
	mute ^= (1 << i);
	gme_mute_voices(emu, mute);
	
	//Create a buffer to hand the data from GME to wave_write
	const int buf_size = 1024;
	short buffer[buf_size];

	//Sets up the header of the WAV file so it is, in fact, a WAV
	wave_open(sample_rate, wav_name.c_str());
	wave_enable_stereo(); //GME always outputs in stereo
	
	//Perform the magic.
	while (gme_tell(emu) < tracklen) {
		//If an error occurs during play, we still need to close out the file
		if (gme_play(emu, buf_size, buffer)) break;
		if (!simulate_only) wave_write(buffer, buf_size);
	}
	
	//Properly finishes the header and closes the internal file object
	wave_close();
}

int main ( int argc, char** argv ) {
	std::string filename, directory, tracktitle;

	const char kPathSeparator =
		#if defined(_WIN32)||defined(_WIN64)
			'\\';
		#else
			'/';
		#endif

	int tracknum=-1, tracklen=0;
	float inputtracklength = -1;
	
	//If the user didn't enter anything but the command...
	if (argc <= 1){
		print_usage();
		exit(EXIT_FAILURE);
	}

#ifdef NEWMODE	
	int option = 0;
	extern char *optarg;
	extern int optind;
	
	while ((option = getopt(argc, argv,"nD:t:T:V:v")) != -1) {
		switch (option) {
			case 'v' : //Verbose
				verbose = true;
				break;
				
			case 'n' : //Dry run.
				simulate_only = true;
				break;
				
			case 'D' : //Target directory
				directory = std::string(optarg);
				if (directory.at(directory.length()-1)!=kPathSeparator) directory+=kPathSeparator;
				break;
				
			case 't' : //Track number
				tracknum = std::atoi(optarg);
				break;
				
			case 'T' : //Convert length
				inputtracklength = std::atof(optarg);
				break;
				
			case 'V' : //Rip channel
				//TODO: find a way to request a specific channel to be ripped.
				std::cerr << "-V flag is unimplemented. Converting ALL channels." << std::endl;
				break;
				
			default: print_usage(); 
				exit(EXIT_FAILURE);
		}
	}

	if (optind < argc){	//Get next argument, turn it into a filename string.
		filename = std::string(argv[optind]);
	}
	else {	//Whoops. Failed to enter a filename. Abort.
		std::cerr << "Error: Please enter a filename." << std::endl << std::endl;
		print_usage();
		exit(EXIT_FAILURE);
	}
#else
	filename = std::string(argv[1]);
#endif


	tracktitle = filename.substr(0, filename.rfind("."));
	tracktitle = tracktitle.substr(tracktitle.rfind(kPathSeparator)+1, std::string::npos);


	//If the user didn't request a track # at the command line...
	while (tracknum<=0){
		std::cerr << "Track number (first track is 1): ";
		std::cin >> tracknum;
	}
	tracknum--; //first track for GME is 0
	
	std::cerr << std::endl;
	//If the user didn't specify the length of time to convert at the line...
	while (inputtracklength<0){
		std::cerr << "How long to record, in seconds: ";
		std::cin >> inputtracklength;
	}
	//Convert the user-entered float to a hard integer in terms of milliseconds.
	tracklen = (int) (inputtracklength * 1000);

	gme_t* emu;
	int sample_rate = 44100;
	const char* err1 = gme_open_file(filename.c_str(), &emu, sample_rate);
	
	if (err1) {
		std::cerr << err1;
		exit(EXIT_FAILURE);
	}
	
	const char* err2 = gme_start_track(emu, tracknum);
	if (err2) {
		std::cerr << err2;
		return 1;
	}
	//Run the emulator for a second while muted to eliminate opening sound glitch
	for (int len = 0; len < 1000; len = gme_tell(emu)) {
		int m = -1;
		m ^= 1;
		gme_mute_voices(emu, m);
		short buf[1024];
		gme_play(emu, 1024, buf);
	}
	
	for (int i = 0; i < gme_voice_count(emu); i++) {
		const char* err = gme_start_track(emu, tracknum);
		if (err) {
			std::cerr << err;
			return 1;
		}
		
		//the second argument is going to be the proper filename of the wave.
		
		//The filename will be a number, followed by a space and its track title.
		//This ensures both unique and (in most cases) descriptive file names.
		std::string wav_name;
		#ifdef NEWMODE
		wav_name += directory + tracktitle;
		wav_name += "-Track " + num2str(tracknum+1);
		wav_name += "-Voice ";
		#endif
		wav_name += num2str(i+1);
		wav_name += " (";
		wav_name += (std::string)gme_voice_name(emu, i);
		wav_name += ").wav";

		
		if (verbose) std::cerr << "Writing file: " << wav_name << std::endl;
		
		if(!simulate_only) writeTheWave(emu, wav_name, tracklen, i, sample_rate);
	}
	
	gme_delete(emu);
	
	return 0;
}
