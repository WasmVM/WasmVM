# Docker 開發環境

WasmVM 可以在 docker 中開發，透過以下幾個步驟安裝：

* 透過 [`toolbuddy/ssfw`](https://github.com/toolbuddy/ssfw) 安裝 *docker*
```
source <(curl -s https://raw.githubusercontent.com/toolbuddy/ssfw/master/system/install_docker.sh)
```

* 將 [images](https://hub.docker.com/r/wasmvm/develop/) 拉至本地端
```bash
[sudo] docker pull wasmvm/develop
```

* 掛載本地端 WASMVM 資料夾到 docker image 當中，並產生 docker container
```bash
docker run --rm -ti -w /home/WasmVM -v <local 端的 WasmVM 的位置>:/home/WasmVM wasmvm/develop bash
```

* 建立 build 資料夾
```
mkdir -p build && cd build
```

* 執行 cmake
```
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

* 執行 make 
```
NUM_CORES=`grep -c ^processor /proc/cpuinfo`
make -j${NUM_CORES}
```

或 

```
"$(make -j`nproc --all`)"
```

* 跑單元測試
```
ctest
```
