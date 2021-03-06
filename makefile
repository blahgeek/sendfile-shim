sendfile-shim.so: sendfile-shim.cc
	g++ -shared -fPIC "$<" -o "$@"
