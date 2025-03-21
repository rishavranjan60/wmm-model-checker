# INSTALLATION

```bash
git clone https://github.com/mishok2503/wmm-model-checker.git
cd wmm-model-checker
sudo docker build -t model-checker .
sudo docker run -it model-checker
```

Inside container:
```bash
cd /build
./executor --help
./executor --memory-model=ra -i rel_sequence.txt # interactive mode
./executor --memory-model=pso -c dekkers_lock.txt # model checking

for PSO TESTING
cd /build
cmake /srcs
 make -j$(nproc)  #compile the project 
 g++ -O2 -march=native -o /build/tests/pso/test_pso /build/tests/pso/test_pso.cpp -lpthread  #build the test
 g++ O0 -g -fno-inline -fno-omit-frame-pointer -o /build/tests/pso/test_pso /build/tests/pso/test_pso.cpp -lpthread
 ./tests/pso/test_pso  #run the test
 assert(success_count > 0 && "Expected reordering under PSO"); presents in the code that causing the error


```