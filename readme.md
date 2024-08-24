COUNTER_SIZE and the probability array are in params.h
The new_number of exp.h controls the number of counters in the new algorithm to ensure that the memory is the same
The backup of the new algorithm sead.h is in ex-sead.h
The original sead file is backed up at sead copy.h
Running the original sead is a matter of pasting sead copy.h into sead.h and modifying valid_COUNTER_SIZE and the probability array
Change the dataset Remember to change the package_size in param.h and where to read the data