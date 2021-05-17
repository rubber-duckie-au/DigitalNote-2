#include "smsg/batchscanner.h"

namespace DigitalNote {
namespace SMSG {

BatchScanner::BatchScanner() : foundEntry(false)
{
	
}

void BatchScanner::Put(const leveldb::Slice& key, const leveldb::Slice& value)
{
	if (key.ToString() == needle)
	{
		foundEntry = true;
		*deleted = false;
		*foundValue = value.ToString();
	};
}

void BatchScanner::Delete(const leveldb::Slice& key)
{
	if (key.ToString() == needle)
	{
		foundEntry = true;
		*deleted = true;
	};
}

} // namespace SMSG
} // namespace DigitalNote
