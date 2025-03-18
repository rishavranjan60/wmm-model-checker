# INSTALLATION

```bash
git clone https://github.com/mishok2503/wmm-model-checker.git
cd wmm-model-checker
sudo docker build -t model-checker .
sudo docker run -it model-checker
```

Program launch inside container:
```bash
cd /build
./executor --help
./executor --memory-model=ra -i rel_sequence.txt # interactive mode
./executor --memory-model=pso -c dekkers_lock.txt # model checking
```

Tests launch inside container:
```bash
cd /build/tests
./test # tests launch
```