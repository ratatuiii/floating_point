# Additional Lab work: Sequential word count
Authors (team):  
Dizhak Nazarii - https://github.com/NazarDizhak \
Chupa Orest - https://github.com/ratatuiii


## Prerequisites

c++, gcc

### Compilation

### For my_float class:
run this in root:
```{bash}
g++ main.cpp my_float.hpp -o {executionable name}
```

### For my_float class:
run this in root:
```{bash}
g++ main.cpp functions.cpp -o {executionable name)
```
### Usage
### my_float lab:

An example definition looks like this:
```{bash}
my_float<200, 55> v1(1.23123456712345673456345645);
```
This uses a constructor from a double. It is also possible to initialize from blank like this:
```{bash}
my_float<200, 55> v1;
```
Overall, this class supports all kkinds of typical arithmetical operations like +, -, *, / etc. and comparison <, >, ==, and their combinations.

### FP conversation:

In total, 8 functions like FP16/FP32/FP64/FP80 to c++ types, and vice versa FROM c++ types to FP. In general, heavily relies on a "union" as it easies the convertion from bits(in form of uintXX_t) to standart c++ types. Usage is very obvious, give FP, get c++ type. Except for a FP80, as it uses a vector of 10 uint8_t to form 80 bits in total. It uses memcpy to write and read bits, therefore no return, it's directly writen in the given variable when converting long double to FP80.
### Results
### my_float:
A more or less flexible class that allows to define your own floating points with custom mantissa and exponent size. Theoretically allows to initialize any level of precision desired, but in general it won't work as it relies on conversaion to doubles in order to perform arithmetical operations

### FP conversion
A very good way to convert FPs from bytes to c++ types. Also accounts for all possible exceptions as NaN, -inf, +inf, +-0 and denormalised numbers as well. In main.cpp are general test for all types of exceptions mentioned as well as general numbers.
