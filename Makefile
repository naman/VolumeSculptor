all:
	#mkdir -p out
	g++ -pthread -O3 -DNDEBUG -I . -I .. -isystem /usr/include -isystem /usr/include -isystem /usr/include -isystem /usr/include -DOPENVDB_USE_LOG4CPLUS -o out/levelset src/levelset.cpp -I . \
	    -Wl,-rpath,/usr/local/openvdb/lib -L/usr/local/openvdb /usr/local/openvdb/libopenvdb.so.3.1.0 \
	    -ldl -lm -lz -Wl,-rpath,/usr/lib/x86_64-linux-gnu -L/usr/lib/x86_64-linux-gnu -lHalf -Wl,-rpath,/usr/lib -L/usr/lib -ltbb -Wl,-rpath,/usr/lib/x86_64-linux-gnu -L/usr/lib/x86_64-linux-gnu -lboost_iostreams -lboost_system  -Wl,-rpath,/usr/lib -L/usr/lib -llog4cplus -Wl,-rpath,/usr/lib -L/usr/lib -lrt -ltbbmalloc_proxy -ltbbmalloc -ggdb

#run:
#	vglrun vdb_view grids.vdb
	
clean:
	rm out/*