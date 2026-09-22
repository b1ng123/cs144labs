#include "tcp_receiver.hh"
#include "debug.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  if ( message.RST ) {
    reader().set_error();
    return;
  }

  if ( message.SYN ) {
    isn_ = message.seqno;
  }

  if ( !isn_.has_value() ) {
    return;
  }

  uint64_t checkpoint = writer().bytes_pushed();
  uint64_t abs_seqno = message.seqno.unwrap( isn_.value(), checkpoint );
  uint64_t stream_index = message.SYN ? abs_seqno : abs_seqno - 1;

  reassembler_.insert( stream_index, message.payload, message.FIN );
}

TCPReceiverMessage TCPReceiver::send() const
{
  TCPReceiverMessage msg;

  msg.RST = reader().has_error();
  msg.window_size
    = static_cast<uint16_t>( min( writer().available_capacity(), static_cast<uint64_t>( UINT16_MAX ) ) );

  if ( isn_.has_value() ) {
    uint64_t abs_ackno = writer().bytes_pushed() + 1 + ( writer().is_closed() ? 1 : 0 );
    msg.ackno = Wrap32::wrap( abs_ackno, isn_.value() );
  }

  return msg;
}