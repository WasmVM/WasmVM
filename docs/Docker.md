# Docker 開發環境

WasmVM 可以在 docker 中開發，透過以下幾個步驟安裝：

* 透過 [`toolbuddy/ssfw`](https://github.com/toolbuddy/ssfw) 安裝 *docker*
```
source <(curl -s https://raw.githubusercontent.com/toolbuddy/ssfw/master/system/install_docker.sh)
```

* 將 [images](https://hub.docker.com/r/kevinbird61/wasmvm_dev/) 拉至本地端
```bash
[sudo] docker pull kevinbird61/wasmvm_dev
```

* 掛載本地端 WASMVM 資料夾到 docker image 當中
```bash
docker run --rm -ti -w /root -v <local 端的 WasmVM 的位置>:/root kevinbird61/wasmvm_dev bash
```

* make 
```
NUM_CORES=`grep -c ^processor /proc/cpuinfo`
make -j${NUM_CORES}
```

或 

```
"$(make -j`nproc --all`)"
```
