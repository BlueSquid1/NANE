# must run from repo root directory so can access source code in docker container
set -e
docker run -v $(pwd):/src -u $(id -u):$(id -g) emscripten/emsdk emmake cmake -S ./NANE-Web -B ./NANE-Web -DCMAKE_BUILD_TYPE=Release
docker run -v $(pwd):/src -u $(id -u):$(id -g) emscripten/emsdk emmake make -C ./NANE-Web NANE
docker run -p 6931:6931 -v $(pwd):/src -u $(id -u):$(id -g) emscripten/emsdk emrun --no-browser ./NANE-Web/NANE.html