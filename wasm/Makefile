SRC_XAUDIOPRO = xaudiopro.tar.gz

WORK_PATH := $(shell pwd)

SOURCE_REDAY:
	mkdir build && \
	cp src/$(SRC_XAUDIOPRO) build \
	cd build && \
	tar xvf $(SRC_XAUDIOPRO)

all: xaudiopro.js 

clean: 
	rm -rf build

xaudiopro: #$(SOURCE_REDAY)
	cd build/xaudiopro && \
	emconfigure ./configure \
		--prefix="$(WORK_PATH)/build/dist" \
		--host=x86-none-linux \
		&& \
	emmake make && \
	emmake make install && \
	cp ./xatools/xadenoise ../xadenoise.bc

EMCC_COMMON_ARGS = \
	--closure 1 \
	-s TOTAL_MEMORY=134217728 \
	-O3 \
	-s WASM=0 \
	-o $@


SHARED_DEPS = \
	build/dist/lib/libxacodec.so \
	build/dist/lib/libxafilter.so \
	build/dist/lib/libxautil.so

#xaudiopro.js: xaudiopro 
	#emcc build/xadenoise.bc $(SHARED_DEPS) \
		#--pre-js pre.js \
		#--post-js post.js \
		#$(EMCC_COMMON_ARGS)

xaudiopro.js: 
	emcc build/xadenoise.bc $(SHARED_DEPS) \
		--pre-js pre.js \
		--post-js post.js \
		$(EMCC_COMMON_ARGS)


