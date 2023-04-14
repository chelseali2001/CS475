#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define XMIN     -1.
#define XMAX      1.
#define YMIN     -1.
#define YMAX      1.

// setting the number of threads:
#ifndef NUMT
#define NUMT		    2
#endif

// setting the number of nodes:
#ifndef NUMNODES
#define NUMNODES	10000
#endif

float Height( int, int );	// function prototype

const float N = 2.5f;
const float R = 1.2f;

int main( int argc, char *argv[ ] )
{
	omp_set_num_threads( NUMT );    // set the number of threads to use in parallelizing
  double time0 = omp_get_wtime( );

	// the area of a single full-sized tile:
	// (not all tiles are full-sized, though)

	float fullTileArea = (  ( ( XMAX - XMIN )/(float)(NUMNODES-1) )  *
				( ( YMAX - YMIN )/(float)(NUMNODES-1) )  );
        
  float edgeTileArea = (  ( ( XMAX - XMIN )/(float)(NUMNODES-1) )  *
				( ( YMAX - YMIN )/(float)(NUMNODES-1) )  ) / 2.;
        
  float cornerTileArea = (  ( ( XMAX - XMIN )/(float)(NUMNODES-1) )  *
				( ( YMAX - YMIN )/(float)(NUMNODES-1) )  ) / 4.;

	// sum up the weighted heights into the variable "volume"
	// using an OpenMP for-loop and a reduction:
 
  float volume = 0.;

	#pragma omp parallel for default(none), shared(fullTileArea, edgeTileArea, cornerTileArea), reduction(+:volume)
  for( int i = 0; i < NUMNODES*NUMNODES; i++ )
  {
  	int iu = i % NUMNODES;
  	int iv = i / NUMNODES;
  	float z = Height( iu, iv );
  	
    if ( (iu == 0 && iv == 0) || (iu == 0 && iv == (NUMNODES - 1)) || (iu == (NUMNODES - 1) && iv == 0) || (iu == (NUMNODES - 1) && iv == (NUMNODES - 1)) )
      volume += cornerTileArea * z;
    else if (iu ==  0 || iu == (NUMNODES - 1) || iv == 0 || iv == (NUMNODES - 1))
      volume += edgeTileArea * z;
    else
      volume += fullTileArea * z;
  }

  volume *= 2;

  double time1 = omp_get_wtime();
  double maxPerformance = (double)(NUMNODES * NUMNODES) / (time1 - time0) / 1000000.;
  fprintf(stderr, "%2d threads : %8d nodes ; volume = %6.2f ; megaheights/sec = %6.2lf\n",
                NUMT, NUMNODES, volume, maxPerformance);
    
  return 0;
}

float
Height( int iu, int iv )	// iu,iv = 0 .. NUMNODES-1
{
	float x = -1.  +  2.*(float)iu /(float)(NUMNODES-1);	// -1. to +1.
	float y = -1.  +  2.*(float)iv /(float)(NUMNODES-1);	// -1. to +1.

	float xn = pow( fabs(x), (double)N );
	float yn = pow( fabs(y), (double)N );
	float rn = pow( fabs(R), (double)N );
	float r = rn - xn - yn;
	if( r <= 0. )
	        return 0.;
	float height = pow( r, 1./(double)N );
	return height;
}