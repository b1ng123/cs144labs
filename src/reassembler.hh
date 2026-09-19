#pragma once

#include "byte_stream.hh"
#include <cstdint>
#include <map>
#include <string>

class Reassembler
{
public:
  // Construct Reassembler to write into given ByteStream.
  explicit Reassembler( ByteStream&& output ) : output_( std::move( output ) ) {}

  /*
   * Insert a new substring to be reassembled into a ByteStream.
   *   `first_index`: the index of the first byte of the substring
   *   `data`: the substring itself
   *   `is_last_substring`: this substring represents the end of the stream
   *   `output`: a mutable reference to the Writer
   *
   * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
   * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
   * learns the next byte in the stream, it should write it to the output.
   *
   * If the Reassembler learns about bytes that fit within the stream's available capacity
   * but can't yet be written (because earlier bytes remain unknown), it should store them
   * internally until the gaps are filled in.
   *
   * The Reassembler should discard any bytes that lie beyond the stream's available capacity
   * (i.e., bytes that couldn't be written even if earlier gaps get filled in).
   *
   * The Reassembler should close the stream after writing the last byte.
   */
  /*
   * 插入一个新的子字符串以重新组装为字节流。
   * `first_index`：子字符串的第一个字节的索引
   * `data`：子字符串本身 （左闭右开[)）
   * `is_last_substring`：该子字符串表示流的结束
   * `output`：指向 Writer 的可变引用
   *
   * Reassembler 的工作是将索引的子字符串（可能是乱序的
   * 并且可能重叠）重新组装回原始字节流。一旦 Reassembler
   * 得知流中的下一个字节，它应立即将其写入输出。
   *
   * 如果 Reassembler 得知的字节适合流的可用容量
   * 但尚不能写入（因为前面的字节仍未知），它应将其
   * 内部存储，直到填补这些空缺。
   *
   * Reassembler 应丢弃任何超出流可用容量的字节
   * （即使早期空缺被填补，也无法写入的字节）。
   *
   * Reassembler 应在写入最后一个字节后关闭流。
   */
  void insert( uint64_t first_index, std::string data, bool is_last_substring );

  // How many bytes are stored in the Reassembler itself?
  // This function is for testing only; don't add extra state to support it.
  // 在重组器本身中存储了多少字节？
  // 此函数仅用于测试；不要添加额外状态来支持它。
  uint64_t count_bytes_pending() const;

  // Access output stream reader
  // 访问输出流读取器
  Reader& reader() { return output_.reader(); }
  const Reader& reader() const { return output_.reader(); }

  // Access output stream writer, but const-only (can't write from outside)
  // 访问输出流写入器，但只能做const（不能从外部写入）
  const Writer& writer() const { return output_.writer(); }

private:
  ByteStream output_;

  std::map<uint64_t, std::string> pending_ {}; // 暂存区
  uint64_t unasembled_index_ { 0 };            // 当前等待写入的流水线号
  uint64_t bytes_pending_ { 0 };               // 已缓存字节计数
  bool has_last_ { false };                    // 是否收到流结尾信号
  uint64_t last_index_ { 0 };                  // 流结尾最后一位的索引
};