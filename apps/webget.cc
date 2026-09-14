#include "socket.hh"

#include <cstdlib>
#include <iostream>
#include <span>
#include <string>

using namespace std;

void get_URL( const string& host, const string& path )
{
  // 创建套接字，并连接到目标主机的http端口
  TCPSocket socket;
  socket.connect( Address( host, "http" ) );

  // 向服务器发送HTTP GET请求
  socket.write( "GET " + path + " HTTP/1.1\r\n" );
  socket.write( "Host: " + host + "\r\n" );
  socket.write( "Connection: close\r\n" );
  socket.write( "\r\n" );

  // 读取服务器响应并打印到标准输出
  while ( !socket.eof() ) {
    string buffer;
    socket.read( buffer );
    cout << buffer;
  }
}

int main( int argc, char* argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort(); // 对于严格要求的开发者：如果 argc <= 0，请不要尝试访问 argv[0]
    }

    auto args = span( argv, argc );

    // 该程序需要两个命令行参数：hostname和URL中的"path"部分
    // 除非有这两个参数（加上程序名本身，因此总参数个数为3，arg count = 3），否则打印使用信息。
    if ( argc != 3 ) {
      cerr << "Usage: " << args.front() << " HOST PATH\n";
      cerr << "\tExample: " << args.front() << " stanford.edu /class/cs144\n";
      return EXIT_FAILURE;
    }

    // 获取命令行参数
    const string host { args[1] };
    const string path { args[2] };

    // 调用编写的函数
    get_URL( host, path );
  } catch ( const exception& e ) {
    cerr << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
