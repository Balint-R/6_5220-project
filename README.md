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
./testing_framework synth <n> <m> <eps> <sigma> <rounds> <alg_id> <gen_id>
`
- n: length of first string
- m: length of second string
- eps: desired approximation ratio
- sigma: alphabet size
- rounds: number of rounds to run the algorithms for
- alg_id: algorithm to test (optional, default tests all)
- gen_id: generator to test

#### To test all algorithms at once:
`
./testing_framework synth <n> <m> <eps> <sigma> <rounds>
`

### For real data:
#### To test a specific algorithm:
```
./testing_framework real <filename> <eps> <rounds> <alg_id>
```
- filename: filename of input file
- desired approximation ratio
- rounds: number of rounds to run the algorithms for
- alg_id: algorithm to test (optional, default tests all)

#### To test all algorithms at once:
`
./testing_framework real <filename> <eps> <rounds>
`
