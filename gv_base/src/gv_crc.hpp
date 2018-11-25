#pragma warning(disable : 4244)
#pragma warning(disable : 4245)
#include <boost/boost/crc.hpp>
namespace gv
{
gv_uint gv_crc32(const char* buffer, int size)
{
	boost::crc_32_type result;
	result.process_bytes(buffer, size);
	return (gv_uint)result.checksum();
};
gv_ulong gv_crc64(const char* buffer, int size)
{
	boost::crc_32_type result;
	result.process_bytes(buffer, size);
	return (gv_uint)result.checksum();
};
}