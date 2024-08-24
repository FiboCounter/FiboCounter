#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<string>
#include <unordered_map>
#include <fstream>
#include "sead.h"
#include "cmsketch.h"
#include "cusketch.h"
#include "csketch.h"
#include "bobhash.h"
#include "exp.h"
#include "sketch.h"
#include <limits>
#include <time.h>
#include <chrono>

// to avoid undefined reference error here
char SmallActiveCounter::q = per_estimator_int;
char SmallActiveCounter::mode = per_estimator_mode;
char SmallActiveCounter::A = per_estimator_int - per_estimator_mode;
int SmallActiveCounter::r = 1;

using namespace std;

 

int main()
{
	greeting(); 
	//cout << COUNTER_SIZE << endl;
	//cout << MIN_INT_CNT << endl;

	/*int c;
	//while(cin >> c)cout << get_val(c) << endl;
	while(cin >> c){
		bprint(c);
		cout << predict(c,4,gamma_2,0) << endl;
	}*/

	printf("Type 'CAIDA','webpage' or 'synthetic'\n");

	// gammas[0] = gamma_2;
	// gammas[1] = gamma_3;
	// gammas[2] = gamma_4;

	string dataset;
	//cin >> dataset;
	dataset = "CAIDA";
	if (dataset == "CAIDA") {
		int sketch, arr, version = -1;
		double start, ends;
		cout << "type 0~6 for 'CM','CU','C', 'Counter Tree', 'SEADCM','SEADCU' or 'SEADC' sketches" << endl;
		cin >> sketch;

		if (sketch == 6) {
			sign_flag = 1;
		}
		// else {
		// 	int * p = (int *)&COUNTER_SIZE;
		// 	int q = COUNTER_SIZE + 10;
		// 	* p = q;
		// };   //如果不要用到符号位，等效把counter_size加一位
		//sketch = 4;
		//cout << "COUNTER_SIZE  " << COUNTER_SIZE << endl;
		
		if (sketch >= 4)
		{
			cout << "type 0~1 for 'Dynamic Sign Bits' version or 'Static Sign Bits' version" << endl;
			//cin >> version;
			version = 1;
		}
		cout << "type in number of arrays in sketch(type 0 for default setting)" << endl;
		cin >> arr;

		//arr = 3;

		if (arr == 0)
			arr = 12;
		cout << "type in the range of memory usage(in KB)" << endl;
		cout << "from:" << endl;
		//cin >> start;
		cout << "to:" << endl;
		//cin >> ends;
		double err1, err2;
		long double sig1, sig2;
		ofstream file;
		cout << "type in the file name your data will output(including '.txt')" << endl;
		string file_name;
		cin >> file_name;

		file_name = "../output/" + file_name;

		file.open(file_name);

		file << "memory(in KB)  ARE  sigma    AAE    sigma" << endl;

		int n = 9000000;

		start = 10000;
		ends = 10001;
		for (int i = start; i < ends; i *= 2) {
			//printf("now well\n");
			CAIDA_experiment(n, err1, version, i, arr, sig1, sketch, err2, sig2, file_name);
			//file << i << "  " << err1 << "  " << sig1 << "  " << err2 << "  " << sig2 << endl;
			file << i << "  " << err1 << "  " << sig1 << "  " << err2 << "  " << sig2 << endl;
		}

		// start = 10;
		// ends = 100;
		// for (int i = start; i < ends; i += 10) {
		// 	CAIDA_experiment(n, err1, version, i, arr, sig1, sketch, err2, sig2);
		// 	file << i << "  " << err1 << "  " << sig1 << "  " << err2 << "  " << sig2 << endl;
		// }

		// start = 100;
		// ends = 1000;
		// for (int i = start; i < ends; i += 100) {
		// 	CAIDA_experiment(n, err1, version, i, arr, sig1, sketch, err2, sig2);
		// 	file << i << "  " << err1 << "  " << sig1 << "  " << err2 << "  " << sig2 << endl;
		// }

		// start = 10000;
		// ends = 40000;
		// for (int i = start; i < ends; i += 5000) {
		// 	CAIDA_experiment(n, err1, version, i, arr, sig1, sketch, err2, sig2, file_name);
		// 	//file << i << "  " << err1 << "  " << sig1 << "  " << err2 << "  " << sig2 << endl;
		// }


	}
	else if (dataset == "webpage") {
		int sketch, arr, version = -1;
		double start, ends;
		cout << "type 0~6 for 'CM','CU','C','Counter Tree','SEADCM','SEADCU' or 'SEADC' sketches" << endl;
		cin >> sketch;
		if (sketch >= 4)
		{
			cout << "type 0~1 for 'Dynamic Sign Bits' version or 'Static Sign Bits' version" << endl;
			cin >> version;
		}
		cout << "type in number of arrays in sketch(type 0 for default setting" << endl;
		cin >> arr;
		if (arr == 0)
			arr = 12;
		cout << "type in the range of memory usage(in KB)" << endl;
		cout << "from:" << endl;
		cin >> start;
		cout << "to:" << endl;
		cin >> ends;
		double err1, err2;
		long double sig1, sig2;
		ofstream file;
		cout << "type in the file name your data will output(including '.txt')" << endl;
		string file_name;
		cin >> file_name;
		file.open("../output/" + file_name);
		file << "memory(in KB)  ARE  sigma    AAE    sigma" << endl;
		for (int i = start; i < ends; i += (ends - start) / 5) {
			webpage_experiment(100000, err1, version, i, arr, sig1, sketch, err2, sig2);
			file << i << "  " << err1 << "  " << sig1 << "  " << err2 << "  " << sig2 << endl;
		}
	}
	else if (dataset == "synthetic") {
		int sketch, arr, version = -1;
		double start, ends;
		cout << "type 0~6 for 'CM','CU','C','Counter Tree','SEADCM','SEADCU' or 'SEADC' sketches" << endl;
		cin >> sketch;
		if (sketch >= 4)
		{
			cout << "type 0~1 for 'Dynamic Sign Bits' version or 'Static Sign Bits' version" << endl;
			cin >> version;
		}
		cout << "type in number of arrays in sketch(type 0 for default setting" << endl;
		cin >> arr;
		if (arr == 0)
			arr = 12;
		cout << "type in the range of memory usage(in KB)" << endl;
		cout << "from:" << endl;
		cin >> start;
		cout << "to:" << endl;
		cin >> ends;
		double err1, err2;
		long double sig1, sig2;
		int i;
		cout << "type in the zipf parameter a (times 10, choose one integer from 11~20)" << endl;
		cin >> i;
		cout << "experiment: a = " << (double)i / 10 << endl;
		ofstream file;
		cout << "type in the file name your data will output(including '.txt')" << endl;
		string file_name;
		cin >> file_name;
		file.open("../output/" + file_name);
		file << "memory(in KB)  ARE  sigma    AAE    sigma" << endl;
		for (double j = start; j < ends; j += (ends - start) / 5) {
			synthetic_experiment(100000.0, err1, version, i, j, arr, sig1, sketch, err2, sig2);
			file << j << "  " << err1 << "  " << sig1 << "  " << err2 << "  " << sig2 << endl;
		}

	}
	else {
		cout << "No such dataset exists! Please type in datasets we generate above or modify our functions and use sketches we write here." << endl;
	}
	return 0;
}

