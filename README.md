## Build
To build an executable:
```bash
make # for build as default
make BUILD_TYPE=asan # addr sanitizer
make BUILD_TYPE=tsan # thread sanitizer
make BUILD_TYPE=msan # memory sanitizer
make clean # clean
```
Default executable path is `./bin/result.out`. You may overide via NAME.


## Tests
You can generate tests with
```bash
# folder and tests count
./utils/test_generator.sh ./tests/ 100
```
and run them as
```bash
# binary and tests folder
./utils/test_runner.sh ./bin/result.out ./tests/
```

Change variables for tests you may at `utils/test_generator.sh`
```bash
MIN_ITER=500
MAX_ITER=5000
MIN_THREADS=2
MAX_THREADS=12
```