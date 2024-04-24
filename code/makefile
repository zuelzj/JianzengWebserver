CXX ?=g++
DEBUG ?=1
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g
else
    CXXFLAGS += -O2

endif

server: main.cpp  ./timer/heap_timer.cpp ./http/http_conn.cpp ./log/log.cpp ./mysql/sql_connection_pool.cpp  ./webserver/webserver.cpp
	$(CXX) -o server -DET $^ $(CXXFLAGS) -lpthread -lmysqlclient

clean:
	rm  -r server
