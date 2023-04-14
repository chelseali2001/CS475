#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>

int	NowYear;		// 2022 - 2027
int	NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	NowNumDeer;		// number of deer in the current population
float NowGlobalWarming; // increase temperature between 0-2 degrees every year

const float GRAIN_GROWS_PER_MONTH =		9.0;
const float ONE_DEER_EATS_PER_MONTH =		1.0;

const float AVG_PRECIP_PER_MONTH =		7.0;	// average
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =			2.0;	// plus or minus noise

const float AVG_TEMP =				60.0;	// average
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				40.0;
const float MIDPRECIP =				10.0;

omp_lock_t	Lock;
int		NumInThreadTeam;
int		NumAtBarrier;
int		NumGone;

unsigned int seed = 0;

void	InitBarrier( int );
void	WaitBarrier( );

float
SQR( float x )
{
  return x*x;
}

float
Ranf( unsigned int *seedp,  float low, float high )
{
        float r = (float) rand_r( seedp );              // 0 - RAND_MAX

        return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

// specify how many threads will be in the barrier:
//	(also init's the Lock)

void
InitBarrier( int n )
{
        NumInThreadTeam = n;
        NumAtBarrier = 0;
	omp_init_lock( &Lock );
}

// have the calling thread wait here until all the other threads catch up:

void
WaitBarrier( )
{
        omp_set_lock( &Lock );
        {
                NumAtBarrier++;
                if( NumAtBarrier == NumInThreadTeam )
                {
                        NumGone = 0;
                        NumAtBarrier = 0;
                        // let all other threads get back to what they were doing
			// before this one unlocks, knowing that they might immediately
			// call WaitBarrier( ) again:
                        while( NumGone != NumInThreadTeam-1 );
                        omp_unset_lock( &Lock );
                        return;
                }
        }
        omp_unset_lock( &Lock );

        while( NumAtBarrier != 0 );	// this waits for the nth thread to arrive

        #pragma omp atomic
        NumGone++;			// this flags how many threads have returned
}

void Deer() {
  while( NowYear < 2027 )
  {
    int nextNumDeer = NowNumDeer;
    int carryingCapacity = (int)( NowHeight );
    if( nextNumDeer < carryingCapacity )
            nextNumDeer++;
    else
            if( nextNumDeer > carryingCapacity )
                    nextNumDeer--;
    
    if( nextNumDeer < 0 )
            nextNumDeer = 0;
            
    WaitBarrier( );
    
    NowNumDeer = nextNumDeer;
  
    WaitBarrier( );
    
    WaitBarrier( );
  }
}

void Grain() {
  while( NowYear < 2027 )
  {
    float tempFactor = exp(   -SQR(  ( NowTemp - MIDTEMP ) / 10.  )   );
    
    float precipFactor = exp(   -SQR(  ( NowPrecip - MIDPRECIP ) / 10.  )   );
    
    float nextHeight = NowHeight;
    nextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
    nextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
    
    if( nextHeight < 0. ) nextHeight = 0.;
            
    WaitBarrier( );
    
    NowHeight = nextHeight;
  
    WaitBarrier( );
    
    WaitBarrier( );
  }
}

void Watcher() {
  while( NowYear < 2027 )
  {            
    WaitBarrier( );
    
    WaitBarrier( );
    
    printf("Year: %d, Month: %d, Temp (C): %f, Precip (in): %f, Deer(s): %d, Height of Grain (in): %f, Global Warming Temp (C): %f\n", NowYear, NowMonth, (5./9.)*(NowTemp-32), NowPrecip, NowNumDeer, NowHeight, (5./9.)*(NowGlobalWarming-32));
    
    NowMonth++;
    
    if(NowMonth >= 12){
        NowYear++;
        NowMonth = 0;
    }
    
    float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );
    
    float temp = AVG_TEMP - AMP_TEMP * cos( ang );
    NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );
    
    float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
    NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
    if( NowPrecip < 0. )
    	NowPrecip = 0.;
     
    NowTemp += NowGlobalWarming;
    
    WaitBarrier( );
  }
}

void MyAgent() {
  while( NowYear < 2027 )
  {        
      
    float globalWarmingTemp = Ranf( &seed, 0.25, 0.5 );
    
    WaitBarrier( );
    
    NowGlobalWarming += globalWarmingTemp;
    
    WaitBarrier( );
        
    WaitBarrier( );
  }
}

int main() {
  srand (time(NULL));

  // starting date and time:
  NowMonth =    0;
  NowYear  = 2022;
  
  // starting state (feel free to change this if you want):
  NowNumDeer = 1;
  NowHeight =  1.;
  NowGlobalWarming = 0;
  
  float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );
  
  float temp = AVG_TEMP - AMP_TEMP * cos( ang );
  NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );
  
  float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
  NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
  if( NowPrecip < 0. )
  	NowPrecip = 0.;
     
  InitBarrier( 4 );
  omp_set_num_threads( 4 );	// same as # of sections
  #pragma omp parallel sections
  {
  	#pragma omp section
  	{
  		Deer( );
  	}
  
  	#pragma omp section
  	{
  		Grain( );
  	}
  
  	#pragma omp section
  	{
  		Watcher( );
  	}
  
  	#pragma omp section
  	{
  		MyAgent( );	// your own
  	}
  }       // implied barrier -- all functions must return in order
  	// to allow any of them to get past here

  return 0;
}