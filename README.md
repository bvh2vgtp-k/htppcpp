# SIMPLE HTTP/1.1 Server in c++
Implementation of a simple HTTP/1.1 server using modern c++. This project is more like a library interface for creating http servers
## Features
* HTTP/1.1 request handling 
* Using c++23 standard 
* Using raw Linux sockets 
* Cool looking API 

Easy to use go like API. You can provide specific address to use, otherwise it will be defaulted to 0.0.0.0. Port is required and can not be omitted.
```cpp
 http::Server srv{":8080"}; //will automatically use 0.0.0.0 
```

Register handler with required method and URI
```cpp
 srv.register_handler(http::Method::GET, "/", [](){
            return http::Response()
                .status(http::status_code::OK)
                .type("text/plain; charset=utf-8")
                .body("Hello")
                .build();
        });
        srv.run(); //run the server
```


## Building
Project is using CMake, but works only with Linux sockets so it can only run on Linux
### Linux 

Two build types are available. (You can also build in release) 
```sh
#debug build with sanitizers and stuff
cmake --preset debug 

cmake --build --preset debug
```

Run the executable 
```sh
./build/debug/httpsrv
```

