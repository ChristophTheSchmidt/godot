/*************************************************************************/
/*  vector3.cpp                                                          */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "vector3.h"

#include "core/math/basis.h"
#include "core/math/vector2.h"
#include "core/math/vector3i.h"
#include "core/string/ustring.h"

void Vector3::rotate(const Vector3 &p_axis, const real_t p_angle) {
	*this = Basis(p_axis, p_angle).xform(*this);
}

Vector3 Vector3::rotated(const Vector3 &p_axis, const real_t p_angle) const {
	Vector3 r = *this;
	r.rotate(p_axis, p_angle);
	return r;
}

Vector3 Vector3::clamp(const Vector3 &p_min, const Vector3 &p_max) const {
	return Vector3(
			CLAMP(x, p_min.x, p_max.x),
			CLAMP(y, p_min.y, p_max.y),
			CLAMP(z, p_min.z, p_max.z));
}

void Vector3::snap(const Vector3 p_step) {
	x = Math::snapped(x, p_step.x);
	y = Math::snapped(y, p_step.y);
	z = Math::snapped(z, p_step.z);
}

Vector3 Vector3::snapped(const Vector3 p_step) const {
	Vector3 v = *this;
	v.snap(p_step);
	return v;
}

Vector3 Vector3::limit_length(const real_t p_len) const {
	const real_t l = length();
	Vector3 v = *this;
	if (l > 0 && p_len < l) {
		v /= l;
		v *= p_len;
	}

	return v;
}

Vector3 Vector3::move_toward(const Vector3 &p_to, const real_t p_delta) const {
	Vector3 v = *this;
	Vector3 vd = p_to - v;
	real_t len = vd.length();
	return len <= p_delta || len < (real_t)CMP_EPSILON ? p_to : v + vd / len * p_delta;
}

Vector2 Vector3::octahedron_encode() const {
	Vector3 n = *this;
	n /= Math::abs(n.x) + Math::abs(n.y) + Math::abs(n.z);
	Vector2 o;
	if (n.z >= 0.0f) {
		o.x = n.x;
		o.y = n.y;
	} else {
		o.x = (1.0f - Math::abs(n.y)) * (n.x >= 0.0f ? 1.0f : -1.0f);
		o.y = (1.0f - Math::abs(n.x)) * (n.y >= 0.0f ? 1.0f : -1.0f);
	}
	o.x = o.x * 0.5f + 0.5f;
	o.y = o.y * 0.5f + 0.5f;
	return o;
}

Vector3 Vector3::octahedron_decode(const Vector2 &p_oct) {
	Vector2 f(p_oct.x * 2.0f - 1.0f, p_oct.y * 2.0f - 1.0f);
	Vector3 n(f.x, f.y, 1.0f - Math::abs(f.x) - Math::abs(f.y));
	float t = CLAMP(-n.z, 0.0f, 1.0f);
	n.x += n.x >= 0 ? -t : t;
	n.y += n.y >= 0 ? -t : t;
	return n.normalized();
}

Vector2 Vector3::octahedron_tangent_encode(const float sign) const {
	Vector2 res = this->octahedron_encode();
	res.y = res.y * 0.5f + 0.5f;
	res.y = sign >= 0.0f ? res.y : 1 - res.y;
	return res;
}

Vector3 Vector3::octahedron_tangent_decode(const Vector2 &p_oct, float *sign) {
	Vector2 oct_compressed = p_oct;
	oct_compressed.y = oct_compressed.y * 2 - 1;
	*sign = oct_compressed.y >= 0.0f ? 1.0f : -1.0f;
	oct_compressed.y = Math::abs(oct_compressed.y);
	Vector3 res = Vector3::octahedron_decode(oct_compressed);
	return res;
}

Basis Vector3::outer(const Vector3 &p_with) const {
	Basis basis;
	basis.rows[0] = Vector3(x * p_with.x, x * p_with.y, x * p_with.z);
	basis.rows[1] = Vector3(y * p_with.x, y * p_with.y, y * p_with.z);
	basis.rows[2] = Vector3(z * p_with.x, z * p_with.y, z * p_with.z);
	return basis;
}

bool Vector3::is_equal_approx(const Vector3 &p_v) const {
	return Math::is_equal_approx(x, p_v.x) && Math::is_equal_approx(y, p_v.y) && Math::is_equal_approx(z, p_v.z);
}

bool Vector3::is_zero_approx() const {
	return Math::is_zero_approx(x) && Math::is_zero_approx(y) && Math::is_zero_approx(z);
}

Vector3::operator String() const {
	return "(" + String::num_real(x, false) + ", " + String::num_real(y, false) + ", " + String::num_real(z, false) + ")";
}

Vector3::operator Vector3i() const {
	return Vector3i(x, y, z);
}
