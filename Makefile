LIB_DIR = lib

default: sandbox

sandbox: setup.py sandbox.pyx lib/libsandbox.a
	python3 setup.py build_ext --inplace && rm -f sandbox.c && rm -rf build

lib/libsandbox.a:
	make -C lib libsandbox.a

clean:
	rm -rf *.so *.log
