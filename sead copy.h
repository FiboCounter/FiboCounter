#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <iostream>
#include "params.h"
using namespace std;
//#define  COUNTER_SIZE  16


// maybe only use some bit of it, but to ensure it can be used to experiment on more bits per counter,
// we define it as 32-bit int instead of 16-bit short.
   

sead_c edge[COUNTER_SIZE+1];

LL max_predict_dyn,max_predict_sta;

LL predict(sead_c, int, LL*,bool);

LL predict(sead_c, LL*,bool);

//should be called when using sead
// if gamma changes, edge changes. edge is only used and necessary in adding c, but not 1.
void greeting(bool sp_negative=true,int l_sign=3) {
	if(sp_negative)
		for(int i=1;i<COUNTER_SIZE;++i)
			edge[i]=(1<<(COUNTER_SIZE-1))-(1<<(COUNTER_SIZE-i-1));
	else
		for(int i=1;i<=COUNTER_SIZE;++i)
			edge[i]=(1<<COUNTER_SIZE)-(1<<(COUNTER_SIZE-i));
	sead_c MAX_CNT_C = sp_negative?MAX_CNT_CO:MAX_CNT_CO_P;
	max_predict_dyn=predict(MAX_CNT_C,gamma_2,sp_negative);
	max_predict_sta=predict(MAX_CNT_C,l_sign,gamma_2,sp_negative);
	printf("***************\n");
	printf("**This is SEAD**\n");
	printf("***************\n\n");
	printf("Welcome!\n");
}

void bprint(sead_c x) { //print binary value of x
	for (int i =COUNTER_SIZE-1; i >= 0; --i) {
		printf("%hhd", (x & (1<<i)) >> i);
	}
	printf("\n");
}

unsigned short get_pos(sead_c x,bool sp_negative ) {
	unsigned short pos=0;
	x = sp_negative?(x|second_high_bit_ones):(x|high_bit_ones);
	pos = __builtin_clz(~x)+COUNTER_SIZE-32;  
	if(sp_negative) pos--;
	return pos;
}

void write_sead(sead_c & x,LL c,LL * gamma,bool sp_negative){   //在x的基础上把c加进x，x为低位是0的状态，c>0
	unsigned short pos = 0;
	pos=get_pos(x,sp_negative);
	int total_length=sp_negative?(COUNTER_SIZE - 1):COUNTER_SIZE;
	LL distance = (1<<(COUNTER_SIZE -1 - pos)) * gamma[pos];
	if(c >= distance){
		x = edge[pos+1];
		write_sead(x,c-distance,gamma,sp_negative);
		return;
	}
	double add = (double)c / gamma[pos];
	int pre_add = int(add);
	if (drand48() < (add - pre_add)) pre_add++; //概率进1
	x += pre_add;
}

int get_val(sead_c x, int base = 0) {//return the actual unsigned value stored in one SAC
	//base is the starting scale. 0 meaning multiply by 2^0
	unsigned short pos = 0; //0 means the seperating 0-bit starts at the front
	sead_c delta = high_bit;
	while ((x & delta) && delta >= 1) {  // flip and bit op should be faster 
		++pos;
		delta = delta >> 1;
	}
	sead_c ones = (1<<(COUNTER_SIZE-1-pos))-1;
	return ((ones & x) << (pos + base)) + pos*(1<<(base+COUNTER_SIZE-1));
}

int get_signed_val(sead_c x, int base = 0) { //return the actual signed value stored in one SAC
	if (x >= MAX_CNT_CO)return MAX_CNT;
	if (x <= MIN_CNT_CO)return MIN_CNT;
	unsigned short pos = 0;
	sead_c delta = second_high_bit;
	if (x < 0) {
		sead_c y = ~x + 1;
		while ((y & delta) && delta >= 1) {
			++pos;
			delta = delta >> 1;
		}
		sead_c ones = (1<<(COUNTER_SIZE-1-pos))-1;
		return -((ones & y) << (pos + base))-pos*(1<<(base+COUNTER_SIZE-1));
	}
	else {
		while ((x & delta) && delta >= 1) {
			++pos;
			delta = delta >> 1;
		}
		sead_c ones = (1<<(COUNTER_SIZE-1-pos))-1;
		return ((ones & x) << (pos + base)) + pos*(1<<(base+COUNTER_SIZE-1));
	}
}
	
//this function returns the predict value of an 16-bit static version SAC
LL predict(sead_c x, int l_sign, LL* gamma,bool sp_negative=true) {  //l_sign is the length of sign section
	sead_c delta = sp_negative?second_high_bit:high_bit; //This variable is used to get the value of certain bit in SAC
	int total_length=sp_negative?COUNTER_SIZE-1:COUNTER_SIZE;
	int sign_bits = 0;
	double ret = 0;
	sead_c y=x<0?(~x+1):x; //if x is nagative, take the 2's complement of it
	//read the sign bits
	for (int i = 0; i < l_sign; ++i) {
		if (y & delta) {
			sign_bits += 1<<(l_sign - i - 1);
		}
		delta = delta >> 1;
	}
	sead_c ones = (1<<(total_length-l_sign))-1;
	//add up the value for all previous stages
	for (int i = 0; i < sign_bits; ++i) {
		ret += gamma[i] * ((1<<(total_length - l_sign)) - 1);
	}
	return (gamma[sign_bits] * (ones & y)+ret)*(x<0?-1:1);
}

//this function returns the predict value of an 16-bit dynamic version SAC
LL predict(sead_c x, LL* gamma,bool sp_negative=true) {
	unsigned short pos = 0;
	//sead_c delta = sp_negative?second_high_bit:high_bit;
	int total_length=sp_negative?(COUNTER_SIZE-1):COUNTER_SIZE;
	//if(!sp_negative) printf("%d   %d     ",total_length,x);
	LL answer = 0;
	sead_c y =x&((1 << total_length) - 1); //if x is nagative, take the 2's complement of it
	/*while ((y & delta) && delta >= 1) {
		++pos;
		delta = delta >> 1;
	}*/
	pos = get_pos(x,sp_negative);
	sead_c ones = (1<<(total_length-pos))-1;
	answer += (ones & y) * gamma[pos];
	for (int i = 0; i < pos; ++i) {
		answer += gamma[i] * (1<<(total_length-1 - i));
	}
	if(sp_negative){
		if(x & (1<<total_length)) return -answer;
		else return answer;
	}
	else return answer;
}



//this is the add one function for fixed version of 16-bit SAC
void add_one(sead_c& x, int l_sign, LL* gamma,bool sp_negative=true) {
	sead_c MAX_CNT_C = sp_negative?MAX_CNT_CO:MAX_CNT_CO_P;
	/* if (predict(x, l_sign,gamma,sp_negative) + 1 >= max_predict_sta) {
//		cout << "overflow!" << endl;
		x = MAX_CNT_C;
		return;
	}*/
	sead_c delta = sp_negative?second_high_bit:high_bit;
	sead_c y=x<0?(~x+1):x;
	int sign_bits = 0;
	
	for (int i = 0; i < l_sign; ++i) {
		if (y & delta) {
			sign_bits += 1<<(l_sign - i - 1);
		}
		delta = delta >> 1;
	}
	double add = 1.0 / gamma[sign_bits];
	double r = (rand()%RAND_MAX) / (double)(RAND_MAX);
	if (r < add)++x;
	return;
}

//this is the minus one function for fixed version of 16-bit SAC
void minus_one(sead_c& x, int l_sign, LL* gamma,bool sp_negative=true) {
	sead_c MIN_CNT_C = sp_negative?MIN_CNT_CO:0;
	if (predict(x, l_sign,gamma,sp_negative) - 1 <= predict(MIN_CNT_C,l_sign,gamma,sp_negative)) {
		printf("underflow!\n");
		x = MIN_CNT_C;
		return;
	}
	sead_c delta = sp_negative?second_high_bit:high_bit;
	int sign_bits = 0;
	sead_c y=x<0?(~x+1):x;
	
	for (int i = 0; i < l_sign; ++i) {
		if (x & delta) {
			sign_bits += 1<<(l_sign - i - 1);
		}
		delta = delta >> 1;
	}
	double add = 1.0 / gamma[sign_bits];
	double r = (rand()%RAND_MAX) / (double)(RAND_MAX);
	if (r < add)--x;
	return;
}


void adding(sead_c& x, int c, LL* gamma,bool sp_negative=true) {
	LL pre_x = predict(x,gamma,sp_negative);
	pre_x += c;
	x = 0;
	if (pre_x >= 0){
		write_sead (x,pre_x,gamma,sp_negative);
	}
	else{
		write_sead (x,-pre_x,gamma,sp_negative);
		x = x | high_bit;
	}
	return ;
}

//this function substract c from the 16-bit Dynamic version of SAC
void subtracting(sead_c& x, int c, LL* gamma,bool sp_negative=true) {
	LL pre_x = predict(x,gamma,sp_negative);
	pre_x -= c;
	x = 0;
	if (pre_x >= 0){
		write_sead (x,pre_x,gamma,sp_negative);
	}
	else{
		write_sead (x,-pre_x,gamma,sp_negative);
		x = x | high_bit;
	}
	return ;
}
