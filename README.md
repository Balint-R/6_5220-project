# 6_5220-project
Final project for MIT subject 6.5220 Randomized Algorithms


## Compiling the project
```
mkdir build && cd build
cmake ..
cmake --build .
```

## Running Code

### For synthetic data:
`
./testing_framework synth <n> <m> <sigma> <rounds>
`
- n: length of first string
- m: length of second string
- sigma: alphabet size
- rounds: number of rounds to run the algorithms for

### For real data:
`
./testing_framework real <f1> <f2> <rounds>
`
- f1: first filename
- f2: second filename
- rounds: number of rounds to run the algorithms for
