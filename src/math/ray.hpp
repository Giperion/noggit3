// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#pragma once

#include <math/matrix_4x4.hpp>
#include <math/vector_3d.hpp>

#include <optional>

namespace math
{
	struct ray
	{
		ray(vector_3d origin, vector_3d const& direction)
			: _origin(std::move(origin)), _direction(direction.normalized())
		{
		}

		ray(matrix_4x4 const& transform, ray const& other)
			: ray((transform * math::vector_4d(other._origin, 1.0)).xyz(),
				  (transform * math::vector_4d(other._direction, 0.0)).xyz())
		{
		}

		std::optional<float> intersect_bounds(vector_3d const& _min,
											  vector_3d const& _max) const;
		std::optional<float> intersect_triangle(vector_3d const& _v0,
												vector_3d const& _v1,
												vector_3d const& _v2) const;

		vector_3d position(float distance) const
		{
			return _origin + _direction * distance;
		}

	   private:
		vector_3d _origin;
		vector_3d _direction;
	};
}  // namespace math
