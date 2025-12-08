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
#### To test a specific algorithm:
`
./testing_framework synth <n> <m> <eps> <sigma> <rounds> <id>
`
- n: length of first string
- m: length of second string
- eps: desired approximation ratio
- sigma: alphabet size
- rounds: number of rounds to run the algorithms for
- id: algorithm to test (optional, default tests all)

#### To test all algorithms at once:
`
./testing_framework synth <n> <m> <eps> <sigma> <rounds>
`

### For real data:
#### To test a specific algorithm:
`
./testing_framework real <filename> <eps> <rounds> <id>
`
- filename: filename of input file
- desired approximation ratio
- rounds: number of rounds to run the algorithms for
- id: algorithm to test (optional, default tests all)

#### To test all algorithms at once:
`
./testing_framework real <filename> <eps> <rounds>
`
