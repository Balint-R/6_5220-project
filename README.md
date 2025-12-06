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
./testing_framework synth <n> <m> <eps> <sigma> <rounds> <id>
`
- n: length of first string
- m: length of second string
- eps: desired approximation ratio
- sigma: alphabet size
- rounds: number of rounds to run the algorithms for
- id: algorithm to test (optional, default brute force)

### For real data:
`
./testing_framework real <filename> <eps> <rounds>
`
- filename: filename of input file
- desired approximation ratio
- rounds: number of rounds to run the algorithms for
- id: algorithm to test (optional, default brute force)
