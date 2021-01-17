#include "Selection.h"

SelectionType2::SelectionType2()
	: Chunk(nullptr, 0, math::vector_3d(0.0f, 0.0f, 0.0f))
{
}

SelectionType2::SelectionType2(selected_model_type&& InModel)
	: Model(InModel),
	  Chunk(nullptr, 0, math::vector_3d(0.0f, 0.0f, 0.0f)),
	  Type(eSelectionEntryTypes::eEntry_Model)
{
}

SelectionType2::SelectionType2(selected_wmo_type&& InWmo)
	: Wmo(InWmo),
	  Chunk(nullptr, 0, math::vector_3d(0.0f, 0.0f, 0.0f)),
	  Type(eSelectionEntryTypes::eEntry_WMO)
{
}

SelectionType2::SelectionType2(selected_chunk_type&& InChunk)
	: Chunk(InChunk), Type(eSelectionEntryTypes::eEntry_MapChunk)
{
}

SelectionType2::SelectionType2(selected_model_type& InModel)
	: Chunk(nullptr, 0, math::vector_3d(0.0f, 0.0f, 0.0f)),
	  Model(InModel),
	  Type(eSelectionEntryTypes::eEntry_Model)
{
}

SelectionType2::SelectionType2(selected_wmo_type& InWmo)
	: Chunk(nullptr, 0, math::vector_3d(0.0f, 0.0f, 0.0f)),
	  Wmo(InWmo),
	  Type(eSelectionEntryTypes::eEntry_WMO)
{
}

SelectionType2::SelectionType2(selected_chunk_type& InChunk)
	: Chunk(InChunk), Type(eSelectionEntryTypes::eEntry_MapChunk)
{
}

eSelectionEntryTypes SelectionType2::which() const { return Type; }

bool SelectionType2::operator==(const SelectionType2 Other) const
{
	return Equal(Other);
}

bool SelectionType2::Equal(const SelectionType2 Other) const
{
	switch (this->Type)
	{
		case eEntry_Model:
			return this->Model == Other.Model;
		case eEntry_WMO:
			return this->Wmo == Other.Wmo;
		case eEntry_MapChunk:
			return this->Chunk == Other.Chunk;
		default:
			return false;
	}
}

bool SelectionType2::operator!=(const SelectionType2 Other) const
{
	return !Equal(Other);
}

const type_info& SelectionType2::type() const
{
	switch (Type)
	{
		case eEntry_Model:
			return typeid(selected_model_type);
		case eEntry_WMO:
			return typeid(selected_wmo_type);
		case eEntry_MapChunk:
			return typeid(selected_chunk_type);
		default:
			return typeid(selected_model_type);
	}
}

SelectionResult2::SelectionResult2() {}

void SelectionResult2::emplace_back_awda(float radius,
										 selected_model_type&& ModelType)
{
	SelectionType2 awda(ModelType);
	emplace_back(std::make_pair(radius, awda));
}

void SelectionResult2::emplace_back_awda(float radius,
										 selected_wmo_type&& ModelType)
{
	emplace_back(std::make_pair(radius, SelectionType2(ModelType)));
}

void SelectionResult2::emplace_back_awda(float radius,
										 selected_chunk_type&& ModelType)
{
	emplace_back(std::make_pair(radius, SelectionType2(ModelType)));
}

bool StrEndsWith(const std::string& str, const char* EndStr)
{
	size_t EndStrLen = strlen(EndStr);
	EndStrLen--;

	auto StrIter = str.rbegin();
	for (int i = 0; i < EndStrLen; i++, StrIter++)
	{
		if (StrIter == str.rend())
		{
			return false;
		}

		size_t ReverseCharIter = EndStrLen - i;
		if (*StrIter != EndStr[ReverseCharIter])
		{
			return false;
		}
	}

	return true;
}
