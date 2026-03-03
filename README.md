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


