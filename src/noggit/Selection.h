// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#pragma once

#include <math/vector_3d.hpp>

#include <string>
#include <type_traits>
#include <vector>

class ModelInstance;
class WMOInstance;
class MapChunk;

using selected_model_type = ModelInstance*;
using selected_wmo_type = WMOInstance*;

struct selected_chunk_type
{
	selected_chunk_type(MapChunk* _chunk, int _triangle,
						math::vector_3d _position)
		: chunk(_chunk), triangle(_triangle), position(_position)
	{
	}

	selected_chunk_type(void* awdawdwdawda) {}

	MapChunk* chunk;
	int triangle;
	math::vector_3d position;

	bool operator==(selected_chunk_type const& other) const
	{
		return chunk == other.chunk;
	}

	selected_chunk_type(selected_model_type InType) {}
	selected_chunk_type(selected_wmo_type InType) {}

	operator selected_model_type() { return nullptr; }

	operator selected_wmo_type() { return nullptr; }
};

//! \note Keep in same order as variant!
enum eSelectionEntryTypes
{
	eEntry_Model,
	eEntry_WMO,
	eEntry_MapChunk
};

struct SelectionType2
{
	SelectionType2();
	SelectionType2(selected_model_type&& InModel);
	SelectionType2(selected_wmo_type&& InWmo);
	SelectionType2(selected_chunk_type&& InChunk);
	SelectionType2(selected_model_type& InModel);
	SelectionType2(selected_wmo_type& InWmo);
	SelectionType2(selected_chunk_type& InChunk);

	eSelectionEntryTypes which() const;

	selected_model_type Model = nullptr;
	selected_wmo_type Wmo = nullptr;
	selected_chunk_type Chunk;

	eSelectionEntryTypes Type = eEntry_Model;

	bool Equal(const SelectionType2 Other) const;
	bool operator==(const SelectionType2 Other) const;
	bool operator!=(const SelectionType2 Other) const;

	const type_info& type() const;
};

template <typename ObjectType>
ObjectType GetFromSelection(SelectionType2& Type)
{
	if (typeid(ObjectType).hash_code() ==
		typeid(selected_model_type).hash_code())
	{
		return (ObjectType)Type.Model;
	}
	else if (typeid(ObjectType).hash_code() ==
			 typeid(selected_wmo_type).hash_code())
	{
		return (ObjectType)Type.Wmo;
	}
	else if (typeid(ObjectType).hash_code() ==
			 typeid(selected_chunk_type).hash_code())
	{
		return (ObjectType)Type.Chunk;
	}

	void* awda = nullptr;
	return ObjectType(awda);
}

template <typename ObjectType>
ObjectType GetFromSelection(const SelectionType2& Type)
{
	SelectionType2& MutualType = const_cast<SelectionType2&>(Type);
	return GetFromSelection<ObjectType>(MutualType);
}

using SelectionEntry2 = std::pair<float, SelectionType2>;

class SelectionResult2 : public std::vector<SelectionEntry2>
{
   public:
	SelectionResult2();
	void emplace_back_awda(float radius, selected_model_type&& ModelType);
	void emplace_back_awda(float radius, selected_wmo_type&& ModelType);
	void emplace_back_awda(float radius, selected_chunk_type&& ModelType);
};

#if 0
	using selection_type = boost::variant < selected_model_type
		, selected_wmo_type
		, selected_chunk_type
	>;
	using selection_entry = std::pair<float, selection_type>;
	using selection_result = std::vector<selection_entry>;
#else
using selection_type = SelectionType2;
using selection_entry = SelectionEntry2;
using selection_result = SelectionResult2;
#endif

bool StrEndsWith(const std::string& str, const char* EndStr);
