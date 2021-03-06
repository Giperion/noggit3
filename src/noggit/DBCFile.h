// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#pragma once

#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

class DBCFile
{
   public:
	explicit DBCFile(const std::string& filename);

	// Open database. It must be openened before it can be used.
	void open();

	class NotFound : public std::runtime_error
	{
	   public:
		NotFound() : std::runtime_error("Key was not found") {}
	};

	class Iterator;
	class Record
	{
	   public:
		const float& getFloat(size_t field) const
		{
			assert(field < file.fieldCount);
			return *reinterpret_cast<float*>(offset + field * 4);
		}
		const unsigned int& getUInt(size_t field) const
		{
			assert(field < file.fieldCount);
			return *reinterpret_cast<unsigned int*>(offset + field * 4);
		}
		const int& getInt(size_t field) const
		{
			assert(field < file.fieldCount);
			return *reinterpret_cast<int*>(offset + field * 4);
		}
		const char* getString(size_t field) const
		{
			assert(field < file.fieldCount);
			size_t stringOffset = getUInt(field);
			assert(stringOffset < file.stringSize);
			return file.stringTable.data() + stringOffset;
		}
		const char* getLocalizedString(size_t field, int locale = -1) const
		{
			int loc = locale;
			if (locale == -1)
			{
				assert(field < file.fieldCount - 8);
				for (loc = 0; loc < 9; loc++)
				{
					size_t stringOffset = getUInt(field + loc);
					if (stringOffset != 0) break;
				}
			}

			assert(field + loc < file.fieldCount);
			size_t stringOffset = getUInt(field + loc);
			assert(stringOffset < file.stringSize);
			return file.stringTable.data() + stringOffset;
		}

	   private:
		Record(const DBCFile& pfile, unsigned char* poffset)
			: file(pfile), offset(poffset)
		{
		}
		const DBCFile& file;
		unsigned char* offset;

		friend class DBCFile;
		friend class DBCFile::Iterator;
	};
	/** Iterator that iterates over records
	 */
	class Iterator
	{
	   public:
		Iterator(const DBCFile& file, unsigned char* offset)
			: record(file, offset)
		{
		}
		/// Advance (prefix only)
		Iterator& operator++()
		{
			record.offset += record.file.recordSize;
			return *this;
		}
		/// Return address of current instance
		Record const& operator*() const { return record; }
		const Record* operator->() const { return &record; }
		/// Comparison
		bool operator==(const Iterator& b) const
		{
			return record.offset == b.record.offset;
		}
		bool operator!=(const Iterator& b) const
		{
			return record.offset != b.record.offset;
		}

	   private:
		Record record;
	};

	inline Record getRecord(size_t id)
	{
		return Record(*this, data.data() + id * recordSize);
	}

	inline Iterator begin() { return Iterator(*this, data.data()); }
	inline Iterator end() { return Iterator(*this, data.data() + data.size()); }

	inline size_t getRecordCount() const { return recordCount; }
	inline size_t getFieldCount() const { return fieldCount; }
	inline Record getByID(unsigned int id, size_t field = 0)
	{
		for (Iterator i = begin(); i != end(); ++i)
		{
			if (i->getUInt(field) == id) return (*i);
		}
		throw NotFound();
	}

   private:
	std::string filename;
	size_t recordSize;
	size_t recordCount;
	size_t fieldCount;
	size_t stringSize;
	std::vector<unsigned char> data;
	std::vector<char> stringTable;
};
