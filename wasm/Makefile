
WORK_PATH := $(shell pwd)

all: xaudiopro.js xaudiopro.worker.js

clean: 
	rm -rf build
	rm xaudiopro.js
	rm xaudiopro.js.mem
	rm xaudiopro-worker.js
	rm xaudiopro-worker.js.mem
	rm xaudiopro.worker.js
	rm xaudiopro.worker.js.mem

xaudiopro: 
	cd ../ && \
	emconfigure ./configure \
		--prefix="$(WORK_PATH)/build/dist" \
		--host=x86-none-linux \
		--disable-static \
		&& \
	emmake make && \
	emmake make install && \
	cp ./xatools/xadenoise.bc ./wasm/build/xadenoise.bc 

#EMCC_COMMON_ARGS = \
	#--closure 1 \
	#-O3 \
	#-s TOTAL_MEMORY=134217728 \
	#-s WASM=0 \
	#-s FORCE_FILESYSTEM=1 \
	#-o $@

EMCC_COMMON_ARGS = \
	-Os \
	-s WASM=0 \
	-s TOTAL_MEMORY=134217728 \
	-s FORCE_FILESYSTEM=1 \
	-o $@

SHARED_DEPS = \
	build/dist/lib/libxacodec.so \
	build/dist/lib/libxafilter.so \
	build/dist/lib/libxautil.so \
	build/dist/lib/librnnoise.so

xaudiopro.js: xaudiopro 
	emcc build/xadenoise.bc $(SHARED_DEPS) \
		--pre-js pre.js \
		--post-js post.js \
		$(EMCC_COMMON_ARGS)

xaudiopro.worker.js: xaudiopro 
	emcc build/xadenoise.bc $(SHARED_DEPS) \
		--pre-js pre-worker.js \
		--post-js post-worker.js \
		$(EMCC_COMMON_ARGS)



