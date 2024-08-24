#pragma once 
#include <algorithm>
#include <cstring>
#include "sketch.h"
#include <string.h>
#include "bobhash.h"
#include "sead.h"
#include <iostream>
#include "params.h"
#include "ICE_bucket.h"
#include "SmallActiveCounter.h"


using namespace std;
class CM_RSketch :public class_sketches {
private:
	BOBHash* bobhash[20];
	int index[20];
	sead_c* counter[20];
	int* int_counter[20];
	ICEBuckets *icebuc;
	SmallActiveCounter *SAC;
	int w,w1, d;

	int p,bit_size,max_cnt_r;

	int counter_index_size;
	uint64_t hash_value;
public:
	CM_RSketch(int _w, int _d) { //w is the size of the hash area, d is the number of hash functions in the Mediean trick
		counter_index_size = 20;
		w = _w;
		d = _d;
		bit_size = 16;
		p = 1;
		printf("fine\n");
		max_cnt_r =( 1 << bit_size) - 1;
		for (int i = 0; i < d; i++)
		{
			counter[i] = new sead_c[w];
			memset(counter[i], 0, sizeof(sead_c) * w);
			bobhash[i] = new BOBHash(i + 1000);

			int_counter[i] = new int[w];
			memset(int_counter[i], 0, sizeof(int) * w);
		}
		w1=w*(double)per_estimator_int/per_estimator_bit;
	}
	//insert function for int counters
	void Insert(const char* str, int c) {

        double add = (double)c / p;
        int pre_add = int(add);
        if (drand48() < (add - pre_add)) pre_add++; //概率进1

		for (int i = 0; i < d; i++) {
			index[i] = (bobhash[i]->run(str, package_size)) % w;
			if (int_counter[i][index[i]] + pre_add <= max_cnt_r) { ///
				int_counter[i][index[i]] += pre_add;
			}
            else{
				int_counter[i][index[i]] += pre_add;
                for(int j = 0; j < d; j++){
					for(int k=0; k < w; d++){
						double change_val = double(int_counter[j][k]) / 2;
						int pre_val = int(change_val);
						if (drand48() < (change_val - pre_val)) pre_val++ ; //概率进1
						int_counter[j][k] = pre_val;
					}
				}
				p*=2;
            }
		}
	}
	//query function for int counters
	int Query(const char* str) {
		int min_value = 1<<30;
		int temp;

		for (int i = 0; i < d; i++)
		{
			index[i] = (bobhash[i]->run(str, package_size)) % w;
			temp = int_counter[i][index[i]];
			min_value = temp*p < min_value ? temp*p : min_value;
		}
		return min_value;
	}

	//the insert function for dynamic sign bits SEAD, the function adds c to "d mapped counters"
	
	//the debug function used to print values in "d mapped counters" correspond with str
	void check(const char* str, int l_sign, LL* gamma) {
		int temp;
		int result[22];
		cout << "[[";
		//check str's correspond hash table content
		for (int i = 0; i < d; ++i) {
			int pos = (bobhash[i]->run(str, package_size)) % w;
			
			result[i] = abs(predict(counter[i][pos], gamma,false));
			int buc_num=(i*w1+index[i])/icebuc->S;
			int buc_sym=(i*w1+index[i])%icebuc->S;
			//int est=icebuc->estimate(buc_num,buc_sym);
			//cout<< est<<endl;
			//bprint(sead_c(est));
		}
		//sort(result, result + d);
		for (int i = 0; i < d; ++i) {
			cout << result[i] << " ";
			bprint(sead_c(result[i]));
		}
		cout << "]]" << endl;
	}
	
	
	
	~CM_RSketch() {
		for (int i = 0; i < d; ++i)
		{
			delete[] counter[i];
			delete[] int_counter[i];
			delete bobhash[i];
		}
		delete icebuc;
		delete[] SAC;
	}
};

// CMSKETCH_H
