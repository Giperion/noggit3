// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#include <math/trig.hpp>
#include <math/vector_2d.hpp>

namespace math
{
	void rotate(float x0, float y0, float* x, float* y, radians angle)
	{
		const float xa(*x - x0);
		const float ya(*y - y0);
		*x = xa * cos(angle) - ya * sin(angle) + x0;
		*y = xa * sin(angle) + ya * cos(angle) + y0;
	}
}  // namespace math
