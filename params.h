#pragma once


#define LL long long int

#define LOOP_NUM 1
#define KIND_OF_COUNTERS 3
 
typedef int sead_c;  

//set the size of counter according to the chosen type

const int valid_COUNTER_SIZE = 8;

const int COUNTER_SIZE = valid_COUNTER_SIZE + 1;
bool sign_flag = 0;

#define MAX_INT_CNT 2147483647
#define MIN_INT_CNT -1<<31
const int MAX_CNT_CO = (1 << (COUNTER_SIZE - 1)) - 1;  //0111111111111111
const int MIN_CNT_CO = -(1 << (COUNTER_SIZE - 1))+1; 
const int MAX_CNT_CO_P = (1 << COUNTER_SIZE) - 1;  //1111111111111111
const int MAX_CNT = (COUNTER_SIZE)*(1 << (COUNTER_SIZE - 2)); 
const int MIN_CNT = -(COUNTER_SIZE)*(1 << (COUNTER_SIZE - 2)); 
const sead_c high_bit = 1<<(COUNTER_SIZE-1);  //1000000000000000
const sead_c second_high_bit = 1<<(COUNTER_SIZE-2); //0100000000000000
const sead_c high_bit_ones=~MAX_CNT_CO_P;  //0xFFFF0000
const sead_c second_high_bit_ones=~MAX_CNT_CO; //0xFFFF8000


const int package_size = 4;

const int prealloc_flow_num = 10000000;


// average 12.5 bits for all flows. For ICEbucket, 12 bit for
// estimator, 0.5 bit for E. For SEAD, 12 bit for all the estimators
// defined in Makefile
//#define per_estimator_bit  12.5
//#define per_estimator_int  12

// if the type of gamma value is double, should change here.

//LL gamma_2[20] = {1, 2, 4, 8, 16, 32, 64, 128, 256 ,512 ,1024 , 2048, 4096, 8192, 16384, 32768, 65536};

LL gamma_2[20] = {1, 4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144,1048576, 4194304, 16777216, 67108864, 268435456};

//LL gamma_2[20] = {1,5,25,125,625,3125,15625,78125,390625,1953125,9765625,48828125,244140625,1220703125,6103515625};   //还能改

//LL gamma_2[20] = {1, 6, 36, 216, 1296, 7776, 46656, 279936, 1679616, 10077696, 60466176, 362797056, 2176782336, 13060694016, 78364164096, 470184984576}; 

//LL gamma_3[20] = {1,5,25,125,625,3125,15625,78125,390625,1953125,9765625,48828125,244140625,1220703125,6103515625}; 

//LL gamma_2[20] = {1, 7, 49, 343, 2401, 16807, 117649, 823543, 5764801, 40353607, 282475249, 1977326743, 13841287201, 96889010407, 678223072849, 4747561509943};

//LL gamma_2[20] = {1, 9, 81, 729, 6561, 59049, 531441, 4782969, 43046721, 387420489, 3486784401, 31381059609, 282429536481, 2541865828329, 22876792454961, 205891132094649}; 

LL fib[20]={1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610 , 987, 1597, 2584 ,4181 , 6765};

LL * gammas[10];

const int per_estimator_mode = 4;


const int read_packet_per_times = 30000000;
// should be adjusted according to the dataset size
// no more than 9000000 for datacenter.dat

const int read_packet_print = 3000000;
// should be adjusted according to the dataset size
// 10000000 for CAIDA_large.dat




