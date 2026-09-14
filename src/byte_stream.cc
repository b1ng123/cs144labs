#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

void Writer::push( string data )
{
  if ( is_closed_ ) {
    return;
  }

  if ( data.size() > available_capacity() ) {
    data.resize( available_capacity() );
  }

  if ( data.empty() ) {
    return;
  }

  bytes_pushed_ += data.size();
  buffer_.push( move( data ) );
  if ( front_view_.empty() ) {
    front_view_ = buffer_.front();
  }
}

void Writer::close()
{
  is_closed_ = true;
}

bool Writer::is_closed() const
{
  return is_closed_;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - reader().bytes_buffered();
}

uint64_t Writer::bytes_pushed() const
{
  return bytes_pushed_;
}

string_view Reader::peek() const
{
  return front_view_;
}

void Reader::pop( uint64_t len )
{
  len = min( len, bytes_buffered() );
  bytes_popped_ += len;

  while ( !buffer_.empty() && len > 0 ) {
    if ( len >= front_view_.size() ) {
      len -= front_view_.size();
      buffer_.pop();
      front_view_ = buffer_.empty() ? string_view {} : buffer_.front();
    } else {
      front_view_.remove_prefix( len );
      len = 0;
    }
  }
}

bool Reader::is_finished() const
{
  return is_closed_ && bytes_buffered() == 0;
}

uint64_t Reader::bytes_buffered() const
{
  return bytes_pushed_ - bytes_popped_;
}

uint64_t Reader::bytes_popped() const
{
  return bytes_popped_;
}
