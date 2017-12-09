/*convert_to_wave()
  Author: Larry Pyeatt
  Date: 11-1-2012
  (C) All rights reserved

  convert_to_wave reads an input file consisting of a sequence of
  numbers in ASCII format, and converts it to Microsoft (tm) Wave (tm)
  format, and writes it to the output file.
*/
#include <cmath>
#include <iostream>
#include <fstream>
#include <limits.h>
#include <stdint.h>
#include <errno.h>
#include <cstring>
#include "wave.h"
using namespace std;

/* I have done minimal error checking.  I really should do more */
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
