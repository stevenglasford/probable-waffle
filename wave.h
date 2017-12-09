/*convert_to_wave()
  Author: Larry Pyeatt
  Date: 11-1-2012
  (C) All rights reserved

  convert_to_wave reads an input file consisting of a sequence of
  numbers in ASCII format, and converts it to Microsoft (tm) Wave (tm)
  format, and writes it to the output file.

  It returns 1 on success and 0 on failure
*/
#ifndef WAVE_H
#define WAVE_H

int convert_to_wave(const char input_filename[],
		    const char output_filename[],
		    int sample_rate,
		    bool noclobber = true);

#endif
