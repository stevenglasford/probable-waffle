/*************************************************************************//**
* @file
* 
* @mainpage prog4 - Harp Heroine
* 
* @section M001 Computer Science 1
* 
* @author Steven Glasford
* 
* @date 12-4-2017
* 
* @par Professor:
*            Larry Pyeatt
* 
* @par Course: 
*           CSC-150-M001-10AM
* 
* @par Location:
*           McLaury - 306
* 
* @section program_section Program Information
* 
* @details This program use command line arguments to pass in a .notes file, 
* which is a file of different midi note values and the times in which those 
* notes are played, as well as an output file of .wav for the sound file, 
* and then a tempo value. The program uses the Karplus Strong algorithm to 
* mimic a stringed instrument.
* 
* @section compile_section Compiling and Usage
* 
* @par Compliling Instructions
*           This program has been tested using Gnu as well as Microsoft 
* visual studio 2017 community versions. Absolutely nothing is needed to 
* be done to make it work, just compile and run it.
*  
* 
* @par Usage:
  @verbatim
  c:\> prog4.exe <input file> <output file> <tempo>
  d:\> c:\bin\prog4.exe <input file> <output file> <tempo>
  @endverbatim
* 
* @section todo_bugs_modifications_section Todo, Bugs, and Modifications
* @bug No more bugs
* 
* @par Modifications and Development Timeline:
  @verbatim
  Date          Modification 
  ------------  -------------------------------------------------------------- 
  12-3-2017     Finished the program, documentation, and turned it in
  @endverbatim
* 
*****************************************************************************/
 

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <limits.h>
#include <stdint.h>
#include <errno.h>
#include <cstring>
#include "wave.h"

using namespace std;

/*!
 * @brief This is the sampling rate for the file, which never changes
 */
const int samplingRate = 44100;
/*!
 * @brief This is how many columns are present in the large volumeMatrix
 */
const int arrayLen = 5395;
/*!
 * @brief This is how many rows are present in the large volumeMatrix
 * as well as the total number of midi notes that exist.
 */
const int maxMidi = 128;


/**************************************************************************//**
 * @author Steven Glasford
 * 
 * @par Description:
 * This function calculates the fundamental frequency for a given midi number
 * and return an integer of the fundamental frequency.
 * 
 * @param[in]    midiNum - the input midi number
 * 
 * @returns The calculated fundamental frequency
 *****************************************************************************/
int aFundFreq(int midiNum) {
	return static_cast<int>( floor(((static_cast<double>(samplingRate))/(pow(2.0,(midiNum-69.0)/12.0)*440))));
}

/**************************************************************************//**
 * @author Steven Glasford
 * 
 * @par Description:
 * This function determines when to stop writing temporary data for the wave
 * file, and, in a way, determines the duration of the wav file by checking
 * for a -1 in the midi array, and compares that entry to the time (t) array
 * and then uses the value in the t array multiplies it by 441 and the system
 * gets how many samples in total will be taken by the program to generate 
 * the wave file.
 * 
 * @param[in,out]   midi - an array of the midi values
 * @param[in,out]   t    - an array of the times in which notes are played
 * 
 * @returns The total number of samples to be taken when building the wav file
 *****************************************************************************/
int when_to_stop(int *midi, double *t){
	int i = 0;
	while (!(midi[i] == -1)) {
		i++;
	}
	return static_cast<int>(t[i] * 441);
}

/**************************************************************************//**
 * @author Steven Glasford
 * 
 * @par Description:
 * This function will initializea pluck of a string, by quickly filling 
 * the array with a bunch of random decimal values at the time in which it was
 * plucked 
 * 
 * @param[in,out]    t            - This is the time array containing all of 
 *                                  times in which notes were struck
 * @param[in,out]    midi         - This is the midi array that contains all
 *                                  the different notes that are played in
 *                                  song
 * @param[in]        place        - This is the place in which the sound
 *                                  function is currently on with respect to
 *                                  the entire song, this function compares
 *                                  this value to the value in the t array
 *                                  and initializes the pluck if they are the 
 *                                  same 
 * @param[in]        numEntries   - This is how many entries are in the t,
 *                                  volume and midi arrays
 * @param[in,out]    volume       - This is how loud the pluck should be 
 *                                  when it is first plucked, and the random
 *                                  numbers don't exceed this value
 * @param[in,out]    volumeMatrix - This is the place where the volumes are 
 *                                  stored in a matrix
 * 
 * @returns void
 *****************************************************************************/
void initialize_pluck(double *t, int *midi, int64_t place,
	int numEntries, double *volume, double volumeMatrix[][arrayLen]) {
	for (int i = 0; i < numEntries; i++) {
		//checks if a string has been plucked yet
		if (place == 441 * t[i]) {
			//if the system finds a string that has been plucked it
			//	will initialize a particular string by filling
			//	that string with random number in that midi row
			//temporarily stores the maximum volume for a particular note
			double vtemp = volume[i];
			//fills the midi row with the random numbers
			for (int j = 0; j < arrayLen; j++) {
				volumeMatrix[midi[i]][j] = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/(volume[i])/2));
				//volumeMatrix[midi[i]][j] = volume[i];
				//if ((rand() % arrayLen) > aFundFreq(midi[i])) {
                    //volumeMatrix[midi[i]][j] = 0;
                //}
			}
		}
	}
	return;
}

/**************************************************************************//**
 * @author Steven Glasford
 * 
 * @par Description:
 * This function quickly reads in the input file and decomposes the information
 * into the volume, t, and midi arrays for an ease of use later on in the 
 * program. This function also calculates the total number of notes in the
 * .notes file
 * 
 * @param[in,out]   input       - This is the input filename
 * @param[in,out]   notes       - This is the file variable, and the input
 *                                file
 * @param[in,out]   t           - This matrix contains all of the times in 
 *                                a note was played
 * @param[in,out]   midi        - This matrix contains all of the midi notes 
 *                                that are going to be played in the song
 * @param[in,out]   volume      - This is the volume array, which contains all
 *                                volumes for the different notes
 * @param[in,out]   numEntries  - This is the total number of notes that were
 *                                read into the t, volume and midi arrays
 * 
 * @return  void
 *****************************************************************************/
void read_data(char *input, ifstream& notes, double *t, int *midi,
	double *volume, int& numEntries) {
	while (notes >> t[numEntries]) {
		notes >> midi[numEntries];
		notes >> volume[numEntries];
		numEntries++;
	}
	cout << "Read in " << numEntries-1 << " number of notes" << endl;
	return;
}

/**************************************************************************//**
 * @author Steven Glasford
 * 
 * @par Description:
 * This function takes all of the information about the notes in the .notes
 * file and writes the temporary/intermediate file
 * 
 * @param[in,out]   t           - The array of all of the times in which a 
 *                                note was struck
 * @param[in,out]   midi        - The array holding all of the midi notes
 * @param[in,out]   volume      - The array of all of the volumes of the 
 *                                notes.
 * @param[in]       tempo       - The tempo of the song
 * @param[in]       numNotes    - The number of notes from the original 
 *                                .notes file
 * @param[in,out]   output      - A C string of hte output file name
 * @param[in,out]   input       - A C string of the input file name
 * 
 * @returns void
 *****************************************************************************/
void sound(double *t, int *midi, double *volume, double tempo,
	int numNotes, char *output, char *input) {
	//This is the large array full of all of the notes and volumes
	double volumeMatrix[maxMidi][arrayLen] = { 0 };

	double averageNum = 0;
	double sum = 0;

	//This is the average of all of the notes
	double average = 0;

	//This will be the point in which the simulation will just end
	int stop = when_to_stop(midi, t);

	//This variable will be counting until the very last sample
	int k = 0;
    
    //This is the frequency of the sound produced

	//open the output file
	ofstream intermediate("intermediate.txt");

	//The actual algorithm
	for (int64_t i = 0; i < stop; i++) {
		initialize_pluck(t,midi,i,numNotes,volume,volumeMatrix);
		for (int j = 0; j < maxMidi; j++) {
			//if (abs(volumeMatrix[j][i%arrayLen]) > .000001) {
			//	averageNum++;
            //}
            
            sum+=volumeMatrix[j][i%aFundFreq(j)];
            
            //rings down the volume matrix
            volumeMatrix[j][i%aFundFreq(j)] = .996*(volumeMatrix[j][i%aFundFreq(j)]+volumeMatrix[j][(i+1)%aFundFreq(j)])/2;
			//determines whether or not the sound should be played
			//	based on random numbers, it should produce a
			//	frequency equal to the frequency of the note
			//	being played
			//if ((rand() % arrayLen) > aFundFreq(j)) {
            
			//}
			
		}
		//write the bitdepth data to the intermediate file
		intermediate << sum << endl;
		sum = 0;
		averageNum = 0;
	}
	intermediate.close();
	return;
}

/*convert_to_wave()
  Author: Larry Pyeatt
  Date: 11-1-2012
  (C) All rights reserved

  convert_to_wave reads an input file consisting of a sequence of
  numbers in ASCII format, and converts it to Microsoft (tm) Wave (tm)
  format, and writes it to the output file.
*/

/* I have done minimal error checking.  I really should do more */
/**************************************************************************//** 
 * @author Larry Pyeatt
 * 
 * @par Description: 
 * This function writes the temporary file data to an actual wav file
 * 
 * @param[in]       filename    - This is the output wav file name
 * @param[in]       data        - This array stores all of the values in the 
 *                                temporary file
 * @param[in]       size        - This is how many values are in the 
 *                                intermediate/temporary file
 * @param[in]       sample_rate - This is the sample rate of the wave file
 * 
 * @returns 0   If there is a failure
 * @return  1   If successful
 * 
 *****************************************************************************/
int write_wave(const char filename[], 
		int16_t data[], int size, int sample_rate)
{
  ofstream outfile (filename,ofstream::binary);
  uint32_t chunksize;
  uint32_t subchunk1size;
  uint32_t subchunk2size;
  uint16_t audioformat;
  uint16_t numchannels;
  uint32_t samplerate;
  uint32_t byterate;
  uint16_t blockalign;
  uint16_t bitspersample;

  if(!outfile.good())
    {
      cout << "convert_to_wave: Unable to open file \"";
      cout << filename;
      cout << "\" for output." << endl;
      cout<< strerror( errno ) << "\n" << endl;
      return 0;
    }
  subchunk1size = 16;  // always 16 for PCM data
  audioformat = 1;    // this is the code for PCM
  numchannels = 1;
  samplerate = sample_rate;
  bitspersample = sizeof(int16_t)*8;
  byterate = samplerate * numchannels * bitspersample/8;
  blockalign = numchannels * bitspersample/8;
  subchunk2size = size * numchannels * sizeof(int16_t);
  chunksize = 4 + (8 + subchunk1size) + (8 + subchunk2size);

  outfile.write("RIFF",4);
  outfile.write((char*)&chunksize,4);
  outfile.write("WAVE",4);

  outfile.write("fmt ",4);
  outfile.write((char*)&subchunk1size,sizeof(subchunk1size));
  outfile.write((char*)&audioformat,sizeof(audioformat));
  outfile.write((char*)&numchannels,sizeof(numchannels));
  outfile.write((char*)&samplerate,sizeof(samplerate));
  outfile.write((char*)&byterate,sizeof(byterate));
  outfile.write((char*)&blockalign,sizeof(blockalign));
  outfile.write((char*)&bitspersample,sizeof(bitspersample));

  outfile.write("data",4);
  outfile.write((char*)&subchunk2size,sizeof(subchunk2size));
  outfile.write((char*)data,size * sizeof(int16_t));
  if(!outfile.good())
    {
      cout << "convert_to_wave: Error writing to \"";
      cout << filename;
      cout << "\"." << endl;
      cout<< strerror( errno ) << endl;
      return 0;
    }
  outfile.close();
  return 1;
}



/**************************************************************************//** 
 * @author Larry Pyeatt
 * 
 * @par Description: 
 * This function converts a temporary file to a wave file
 * 
 * @param[in]       input_filename  - This is the temporary file's name
 * @param[in]       output_filename - This is the wav file output name
 * @param[in]       sample_rate     - This is the sample rate of the wav file
 * @param[in]       noclobber       - This determines whether or not to write
 *                                    over an existing file
 * 
 * @returns 0   If there is a failure
 * @return  1   If there aren't any errors
 * 
 *****************************************************************************/
/* should return 0 on failure, but I'm in a hurry */
 int convert_to_wave(const char input_filename[],
		    const char output_filename[],
		    int sample_rate,
		    bool noclobber)
{
  ifstream input;
  float tmp,max;
  int16_t *data16;
  int size,i;
  int return_code;
  char yn;

  if(noclobber)
    {
      input.open(output_filename);
      if(input)
	{
	  do{
	    cout<<"The output file \""<<output_filename<<
	      "\" already exists."<<endl;
	    cout<<"Do you want to overwrite it? (N/y)";
	    cin>>yn;
	    yn = toupper(yn);
	  }while(yn != 'Y' && yn != 'N');
	  if(yn == 'N')
	    return 1;
	}
      input.close();
      input.clear();
    }

  input.open(input_filename);
  if(!input.good())
    {
      cout << "convert_to_wave: Unable to open file \"";
      cout << input_filename;
      cout << "\" for input." << endl;
      cout<< strerror( errno ) << endl;
      return 0;
    }
  // count how many numbers we can read from the file
  size = 0;
  max = 0;
  input >> tmp;
  while(!input.fail())
    {
      size++;
      input >> tmp;
      if(fabs(tmp)>max)
	max = fabs(tmp);
    }
  // clear the fail flag so we can read the file again
  input.clear();
  // go back to the beginning of the file
  input.seekg(ios_base::beg);
  if(!input.good())
    {
      cout << "convert_to_wave: Error reading input file \"";
      cout << input_filename;
      cout << "\"." << endl;
      cout<< strerror( errno ) << endl;
      return 0;
    }
  if(size == 0)
    {
      cout << "convert_to_wave: Input file \"";
      cout << input_filename;
      cout << "\" is empty or does not contain only numbers." << endl;
      return 0;
    }
  // allocate enough space to store all of the numbers
  data16 = new(nothrow) int16_t[size];
  if(data16 == NULL)
    {
      cout<<"convert_to_wave: Memory allocation failed."<<endl;
      cout<< strerror( errno ) << endl;
      return 0;
    }
  // read them all in as floats, then convert to 16 bit ints
  for(i=0;i<size;i++)
    {
      input >> tmp;
      data16[i] = (int16_t)((tmp/max) * (SHRT_MAX-1));
    }
  if(!input.good())
    {
      cout << "convert_to_wave: Error reading input file \"";
      cout << input_filename;
      cout << "\"." << endl;
      cout<< strerror( errno ) << endl;
      return 0;
    }
  input.close();
  return_code = write_wave(output_filename,data16,size,sample_rate);
  delete[] data16;
  return return_code;
}

/**************************************************************************//**
 * @author Steven Glasford
 * 
 * @par Description:
 * This program runs if the user inputs the incorrect number of comand line
 * arguments into the function
 * 
 * @return 6 This is the error code
 *****************************************************************************/
int usage() {
    cout << "prog4 usage: \n"
        "\tThis program takes in a file of different notes and their times,\n"
        "\tand then uses that information to generate a wav file of those\n"
        "\tsame notes to mimic the plucking of a string.\n\n"
        "\t\tprog4.exe <input notes file> <output wav file> <tempo>" << endl; 
    return 6;
}

/**************************************************************************//**
 * @author Steven Glasford
 * 
 * @par Description:
 * This changes the tempo depending on the 4th command line argument
 * 
 * @param[in,out]   t           - This is the time array
 * @param[in]       tempo       - This is the tempo value for the song
 * @param[in]       numEntries  - This is how many notes are in the file
 * 
 * @returns void
 *****************************************************************************/
void changeTempo(double *t, double tempo, int numEntries) {
    for (int i = 0; i < numEntries; i++)
        t[i] /= tempo;
    
    return;
}

/**************************************************************************//**
 * @author Steven Glasford
 * 
 * @par Description:
 * This is the main function, an it basically controls everything in the 
 * program
 * 
 * @param[in]    argc - This is how many 
 * @param[in]    argv - This matrix stores the inputs for the command line
 *                      arguments
 * 
 * @returns 6   This means that the user didn't enter enough command line
 *              arguments
 * @returns 0   The program ran successfully
 * @returns 1   The program wasn't able to open the input file
 * 
 *****************************************************************************/
int main(int argc, char** argv) {
	//if the argument count is not equal to 4, the program will exit with an error
    if (argc != 4) {
        return usage();
    }
    //char outputFilename[fileNameLength];
    double t[1000];
	int midi[1000];
	double volume[1000];
	double tempo = atof(argv[3]);
	int numNotes = 0;
	srand(time(NULL));
	ifstream notes (argv[1]);
	if (!notes.good()) {
		cout << "Sorry but there was an error trying to open the"
			" .notes file, exiting." << endl;
		return 1;
	}
	else {
		read_data(argv[1],notes,t,midi,volume,numNotes);
	}
	changeTempo(t,tempo,numNotes);
	notes.close();
	sound(t,midi,volume,tempo,numNotes,argv[2],argv[1]);
	int OK = convert_to_wave("intermediate.txt",argv[2],samplingRate);
	return 0;
}

