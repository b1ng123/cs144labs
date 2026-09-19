#include "reassembler.hh"
#include "debug.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // 容量右红线
  const uint64_t first_unacceptable
    = output_.reader().bytes_popped() + output_.reader().bytes_buffered() + output_.writer().available_capacity();

  // 左截断
  if ( first_index < unasembled_index_ ) {
    // 整段都是旧数据
    if ( first_index + data.size() <= unasembled_index_ ) {
      if ( is_last_substring ) {
        has_last_ = true;
        last_index_ = first_index + data.size();
      }
      // 检查此时是否能直接关闭流（比如全部已就绪）
      if ( has_last_ && unasembled_index_ == last_index_ ) {
        output_.writer().close();
      }
      return;
    }

    const size_t overlap = unasembled_index_ - first_index;
    data = data.substr( overlap );
    first_index = unasembled_index_;
  }

  // 右截断
  if ( first_index >= first_unacceptable ) {
    return;
  }
  if ( first_index + data.size() > first_unacceptable ) {
    const size_t valid_len = first_unacceptable - first_index;
    data = data.substr( 0, valid_len );
    is_last_substring = false; // 尾巴被截断了，不能算收到完整的结尾
  }

  // 记录结尾标记
  if ( is_last_substring ) {
    has_last_ = true;
    last_index_ = first_index + data.size();
  }

  // 暂存区合并
  uint64_t end_index = first_index + data.size();

  // 向左看
  auto it = pending_.upper_bound( first_index );
  if ( it != pending_.begin() ) {
    auto prev_it = std::prev( it );
    const uint64_t prev_start = prev_it->first;
    const uint64_t prev_end = prev_start + prev_it->second.size();

    if ( prev_end >= first_index ) {
      // 纯冗余
      if ( prev_end >= end_index ) {
        if ( has_last_ && unasembled_index_ == last_index_ ) {
          output_.writer().close();
        }
        return;
      }

      // 需要合并
      const uint64_t overlap = prev_end - first_index;
      data = prev_it->second + data.substr( overlap );
      first_index = prev_start;
      end_index = first_index + data.size();

      bytes_pending_ -= prev_it->second.size();
      pending_.erase( prev_it );
    }
  }

  // 向右看
  while ( it != pending_.end() && it->first <= end_index ) {
    const uint64_t next_end = it->first + it->second.size();

    if ( next_end <= end_index ) {
      bytes_pending_ -= it->second.size();
      it = pending_.erase( it );
    } else {
      const uint64_t overlap = end_index - it->first;
      data += it->second.substr( overlap );

      bytes_pending_ -= it->second.size();
      pending_.erase( it );
      break;
    }
  }

  // 完成合并
  bytes_pending_ += data.size();
  pending_.emplace( first_index, std::move( data ) );

  // 把数据推入ByteStream
  while ( !pending_.empty() && pending_.begin()->first == unasembled_index_ ) {
    auto head = pending_.begin();
    unasembled_index_ += head->second.size();
    bytes_pending_ -= head->second.size();
    output_.writer().push( std::move( head->second ) );
    pending_.erase( head );
  }

  if ( has_last_ && unasembled_index_ == last_index_ ) {
    output_.writer().close();
  }
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  return bytes_pending_;
}