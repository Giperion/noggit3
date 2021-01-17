// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#include <noggit/DBC.h>
#include <noggit/Log.h>
#include <noggit/TextureManager.h>  // TextureManager, Texture
#include <noggit/World.h>
#include <noggit/liquid_layer.hpp>
#include <opengl/context.hpp>
#include <opengl/scoped.hpp>

#include <algorithm>
#include <string>

void liquid_render::prepare_draw(opengl::scoped::use_program& water_shader,
								 int liquid_id, int animtime)
{
	std::size_t texture_index = 0;
	bool need_texture_update = false;

	if (!_current_liquid_id || liquid_id != _current_liquid_id)
	{
		_current_anim_time = animtime;
		_current_liquid_id = liquid_id;

		if (_textures_by_liquid_id[liquid_id].empty())
		{
			add_liquid_id(liquid_id);
		}

		texture_index = get_texture_index(liquid_id, animtime);
		need_texture_update = true;

		water_shader.uniform("type", _liquid_id_types[liquid_id]);
		water_shader.uniform("param", _float_param_by_liquid_id[liquid_id]);
	}
	else
	{
		texture_index = get_texture_index(liquid_id, animtime);
		need_texture_update =
			texture_index != get_texture_index(liquid_id, animtime);
	}

	if (need_texture_update)
	{
		auto const& textures = _textures_by_liquid_id[liquid_id];
		water_shader.sampler("tex", GL_TEXTURE0, textures[texture_index].get());
	}
}

void liquid_render::force_texture_update() { _current_liquid_id.reset(); }

std::size_t liquid_render::get_texture_index(int liquid_id, int animtime) const
{
	return static_cast<std::size_t>(animtime / 60) %
		   _textures_by_liquid_id.at(liquid_id).size();
}

void liquid_render::add_liquid_id(int liquid_id)
{
	std::vector<scoped_blp_texture_reference>& textures =
		_textures_by_liquid_id[liquid_id];
	textures.clear();

	std::string filename = "XTextures\\river\\lake_a.%d.blp";

	DBCFile::Record lLiquidTypeRow = gLiquidTypeDB.getByID(liquid_id);

	_liquid_id_types[liquid_id] = lLiquidTypeRow.getInt(LiquidTypeDB::Type);
	_float_param_by_liquid_id[liquid_id] = math::vector_2d(0.0f, 0.0f);

	for (int i = 1; i <= 30; ++i)
	{
		try
		{
			char TexName[128];
			sprintf(TexName, "%s%d", filename, i);
			textures.emplace_back(TexName);
		}
		catch (...)
		{
			break;
		}
	}

	// make sure there's at least one texture
	if (textures.empty())
	{
		textures.emplace_back("textures/shanecube.blp");
	}
}
