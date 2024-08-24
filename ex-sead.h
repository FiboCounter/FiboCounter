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
   
//负数不采用补码，仅用符号位加绝对值表示


sead_c edge[COUNTER_SIZE+1];

LL max_predict_dyn,max_predict_sta;

LL predict(sead_c, int, LL*,bool);

LL predict(sead_c, LL*,bool);

void bprint(sead_c x);

void cprint(sead_c x);

//should be called when using sead
// if gamma changes, edge changes. edge is only used and necessary in adding c, but not 1.

void showme();

void cal_edge();

unsigned short get_pos(sead_c x,bool sp_negative = true);

void high_val_add_one(sead_c & x,unsigned short pos,bool sp_negative = true);

int get_high_val(sead_c x,unsigned short pos,bool sp_negative = true);

void adding(sead_c& x, int c, LL* gamma,bool sp_negative);

void showme(){
	printf("MAX_CNT_CO: ");
	bprint(MAX_CNT_CO);
	printf("MIN_CNT_CO: ");
	bprint(MIN_CNT_CO);
	printf("MAX_CNT_CO_P: ");
	bprint(MAX_CNT_CO_P);
	printf("MAX_CNT: ");
	bprint(MAX_CNT);
	printf("MIN_CNT: ");
	bprint(MIN_CNT);
	printf("high_bit_ones:  ");
	bprint(high_bit_ones);
}

void cal_edge(bool sp_negative=true){
	if(sp_negative){
		printf("signed !\n");
		edge[0]=0;
		edge[1]=1<<(COUNTER_SIZE-2);
		
		for(int i=2;i<=COUNTER_SIZE-1;i++){
			
			edge[i]=(edge[i-2]>>2)+(1<<(COUNTER_SIZE-3)); //每次右移两位在前面补01

			//bprint(edge[i]);
		}
	}
	else{
		printf("unsigned !\n");  
		edge[0]=0;
		edge[1]=1<<(COUNTER_SIZE-1);
		for(int i=2;i<=COUNTER_SIZE;i++){
			edge[i]=(edge[i-2]>>2)+(1<<(COUNTER_SIZE-2)); //每次右移两位在前面补01
			
			//bprint(edge[i]);
		}
	}
}

void write_sead(sead_c & x,LL c,LL * gamma,bool sp_negative){   //在x的基础上把c加进x，x为低位是0的状态，c>0
	unsigned short pos = 0;
	pos=get_pos(x,sp_negative);
	//printf("the x's pos = %hd \n",pos);

	int total_length=sp_negative?(COUNTER_SIZE - 1):COUNTER_SIZE;
	int carry_val = 1 << (total_length - pos - 2); //进位的值
	
	int high_val = get_high_val(x,pos,sp_negative);
	//printf("hign_val = %d\n",high_val);

	/*bprint(x);
	printf("c = %lld \n",c);*/

	LL distance = ((fib[pos]-high_val)*carry_val)*gamma[pos];
	//printf("the distance is %lld\n",distance);

	if(c >= distance){
		//printf("右移!\n");
		x = edge[pos+1];
		write_sead(x,c-distance,gamma,sp_negative);
		return;
	}

	double add = (double)c / gamma[pos];
	int pre_add = int(add);
	if (drand48() < (add - pre_add)) pre_add++; //概率进1
	//printf("pre_add: %d\n",pre_add);
	//x -= low_bits;    //先把x低位清空，确定进位   
	//pre_add += low_bits;

	if ((pre_add * gamma[pos]) < distance) {
		int carry_cnt = pre_add/carry_val;
		for(int i=0;i<carry_cnt;++i) high_val_add_one(x,pos,true);  
		x += (pre_add - carry_cnt*carry_val);   
		return;
	}
	else {
		x = edge[pos+1];
		return;
	}	
}

void greeting(bool sp_negative=true,int l_sign=3) {
	//showme();
	cal_edge(sp_negative);	
	/*sead_c MAX_CNT_C = sp_negative?MAX_CNT_CO:MAX_CNT_CO_P;
	bprint(MAX_CNT_C);
	max_predict_dyn=predict(MAX_CNT_C,gamma_2,sp_negative);
	bprint(max_predict_dyn);
	max_predict_sta=predict(MAX_CNT_C,l_sign,gamma_2,sp_negative);
	bprint(max_predict_sta);*/

	printf("***************\n");
	printf("**This is SEAD**\n");
	printf("***************\n\n");
	printf("Welcome!\n");
	/*ead_c x;
	while(cin >> x){
		bprint(x);
		//printf("x's high_val = %d \n",get_high_val(x,get_pos(x),0));
		printf("the x's val = %lld  \n",predict(x,gamma_2,false));
		printf("the x's pos = %hd  \n",get_pos(x,true));
		high_val_add_one(x,get_pos(x),true);
		printf("the x's val = %lld   ",predict(x,gamma_2,true));
		bprint(x);
		printf("the x's pos = %hd  \n",get_pos(x,true));
		sead_c d;
		LL sum=predict(x,gamma_2,true);
		while(cin >> d){
			d = drand48()*(1<<29);
			sum += d;
			printf("now we add the d = %d  ",d);
			adding(x,d,gamma_2,true);
			printf("then the x = %lld ,and the sum = %lld\n",predict(x,gamma_2,true),sum);
			//bprint(x);
		}
	}*/
}

void bprint(sead_c x) { //print binary value of x
	for (int i =COUNTER_SIZE-1; i >= 0; --i) {
		printf("%hhd", (x & (1<<i)) >> i);
	}
	printf("\n");
}

void cprint(sead_c x) { //print binary value of x
	for (int i = 31; i >= 0; --i) {
		printf("%hhd", (x & (1<<i)) >> i);
	}
	printf("\n");
}

unsigned short get_pos(sead_c x,bool sp_negative ) {
	unsigned short pos=0;
	x = sp_negative?(x|second_high_bit_ones):(x|high_bit_ones);
	sead_c y = x | (x >> 1);   //x连续两个0的第二个0是y的第一个0
	pos = __builtin_clz(~y)+COUNTER_SIZE-32-1;   //结束的时候要不要把两个0的位置占用？
	if(pos == COUNTER_SIZE -1 && (1 & y)) pos++;    //考虑占用最后两位，如果最后一位为0则右移了counter_size-1 位，若为1则是counter_size位
	if(sp_negative) pos--;
	return pos;
}

void high_val_add_one(sead_c & x,unsigned short pos,bool sp_negative){   //增加绝对值
	unsigned short bit_cnt = sp_negative ? (COUNTER_SIZE-1):(COUNTER_SIZE);
	sead_c sigh_bit = 0, low_bits = 0;
	if (sp_negative) sigh_bit = x & (1<<(COUNTER_SIZE-1));    //记录下符号位
	low_bits = x & ((1 << (bit_cnt - pos ))-1);    //记录下低位
	sead_c y = x & ((1 << bit_cnt)-1);  //清除无用位和符号位
	y = y & (~((1 << (bit_cnt - pos))-1));   //只把高位留下
	//printf("y :  ");
	//bprint(y);
	if(y ==((1 << bit_cnt)-(1 << (bit_cnt - pos)))){  //高位已满
		//printf("高位已满\n");
		x = edge[pos+1] | sigh_bit | low_bits;  
	}
	else{
		unsigned short last_cnt = bit_cnt - pos;    //加1相当于找到最后一个0变成1，再把后面修改为0101...
		while(y & (1 << last_cnt)) last_cnt++;
		y = y | (1 << last_cnt);
		int flag = 0;
		for (int i = last_cnt;i > (bit_cnt - pos );--i){
			y = y ^ (flag << i);
			flag = 1 - flag;
		}/*
		sead_c add_val = 1 << (bit_cnt - pos + 1 ); //+1是由于最后一位必须是1
		do{
			y += add_val;
		}
		while(pos != get_pos(y,sp_negative)); //一直加到前面没有两个连续的0    可优化，别急*/
		x = y | sigh_bit | low_bits;
	}
}

int get_high_val(sead_c x,unsigned short pos,bool sp_negative){
	unsigned short bit_cnt = sp_negative ? (COUNTER_SIZE-1):(COUNTER_SIZE);
	sead_c y = x & ((1 << bit_cnt)-1);  //清除无用位和符号位
	y = y & (~((1 << (bit_cnt - pos))-1));   //只把高位留下
	y = y >> (bit_cnt - pos);   //将高位往下移，方便计算
	//sead_c add_val = 1 << (bit_cnt - pos + 1 ); //+1是由于最后一位必须是1
	//int high_val = 0,base_val = edge[pos];
	int high_val = fib[pos] - 1;
	for(int i = 0;i < pos;++i){
		if(!(y & (1 << i))) high_val -= fib[i];    //****1*** 与****0*** 差为fib[k],k为0后的位数
	}
	// while(base_val!=y){
	// 	//bprint(base_val);
	// 	high_val++;	
	// 	high_val_add_one(base_val,pos,sp_negative);
	// }
	return high_val;
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
		ret += gamma[i] * ((1<<(total_length - l_sign)) );  //-1
	}
	return (gamma[sign_bits] * (ones & y)+ret)*(x<0?-1:1);
}

//this function returns the predict value of an 16-bit dynamic version SAC
LL predict(sead_c x, LL* gamma,bool sp_negative=true) {   
	//printf("dynamic predict: ");
	//bprint(x);
	sead_c y=x;

	unsigned short pos = get_pos(x,sp_negative);
	//printf("pos = %d\n",pos);
	//sead_c delta = sp_negative?second_high_bit:high_bit;
	int total_length=sp_negative?(COUNTER_SIZE-1):COUNTER_SIZE;
	//if(!sp_negative) printf("%d   %d     ",total_length,x);
	LL answer = 0;
	//sead_c y =(x&high_bit)?(~x + 1):x; //if x is nagative, take the 2's complement of it
	
	/*while ((y & delta) && delta >= 1) {
		++pos;
		delta = delta >> 1;
	}*/
	//sead_c y_new=sp_negative?(y|second_high_bit_ones):(y|high_bit_ones);
	//pos=sp_negative?(__builtin_clz(~y_new)-(32-COUNTER_SIZE+1)):(__builtin_clz(~y_new)-(32-COUNTER_SIZE)); //__builtin_clz 
	sead_c ones = (1<<(total_length-pos-2))-1;   //00占两位
	answer += (ones & y) * gamma[pos];
	//printf("the low bits = %d  ",ones & y);
	//bprint(ones & y);
	for (int i = 0; i < pos; ++i) {
		answer += gamma[i] * (1<<(total_length- 2 - i)) * fib[i]; //00占两位,多乘一个系数
	}
	//bprint(x);

	answer += gamma[pos] * get_high_val(x,pos,sp_negative) *(1<<(total_length- 2 - pos));

	return (sp_negative && (x&high_bit))?(-answer):answer;   
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
