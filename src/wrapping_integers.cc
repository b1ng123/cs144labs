#include "wrapping_integers.hh"
#include "debug.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  return Wrap32( zero_point + static_cast<uint32_t>( n ) );
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  Wrap32 ckpt_wrapped = wrap( checkpoint, zero_point );

  int64_t offset = static_cast<int32_t>( raw_value_ - ckpt_wrapped.raw_value_ );

  if ( offset < 0 && checkpoint < static_cast<uint64_t>( -offset ) ) {
    return checkpoint + static_cast<uint64_t>( offset ) + ( 1ULL << 32 );
  }

  return checkpoint + offset;
}