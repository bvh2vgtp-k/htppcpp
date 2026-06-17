# SIMPLE HTTP/1.1 Server in c++
Implementation of a simple HTTP/1.1 server using modern c++
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


## Getting started 
Project is using CMake, but works only with Linux sockets so you can really use it on Windows or BSD or Mac or whatever 
### Linux 

Two build types are available
```sh
cmake -B build -DCMAKE_BUILD_TYPE=Debug   
cmake -B build -DCMAKE_BUILD_TYPE=Release 

cmake --build build 
```

Run the executable 
```sh
./build/httpsrv
```

