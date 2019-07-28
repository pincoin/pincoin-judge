LIB_DIR = lib

all: sandbox

sandbox: setup.py sandbox.pyx lib/libsandbox.a
	python3 setup.py build_ext --inplace

lib/libsandbox.a:
	make -C lib libsandbox.a

clean:
	make -C lib clean
	rm -rf build *.so *.log
