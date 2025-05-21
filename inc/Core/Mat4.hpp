#include "Transform.hpp"

struct Mat4 {
	float data[16]{0.0f};

	inline const float* ptr() const { return data; }
	inline float* ptr() { return data; }

	static Mat4 identity();
	static Mat4 modelMatrix(const Transform& t);

	static Mat4 translation(const Vec3& t);
	static Mat4 scale(const Vec3& t);

	static Mat4 rotateX(const float angle);
	static Mat4 rotateY(const float angle);
	static Mat4 rotateZ(const float angle);

	inline Mat4 operator*(const Mat4& b) const {
		Mat4 result{};

		// Column 0
		result.data[0] = data[0] * b.data[0] + data[4] * b.data[1] + data[8]  * b.data[2] + data[12] * b.data[3];
		result.data[1] = data[1] * b.data[0] + data[5] * b.data[1] + data[9]  * b.data[2] + data[13] * b.data[3];
		result.data[2] = data[2] * b.data[0] + data[6] * b.data[1] + data[10] * b.data[2] + data[14] * b.data[3];
		result.data[3] = data[3] * b.data[0] + data[7] * b.data[1] + data[11] * b.data[2] + data[15] * b.data[3];

		// Column 1
		result.data[4] = data[0] * b.data[4] + data[4] * b.data[5] + data[8]  * b.data[6] + data[12] * b.data[7];
		result.data[5] = data[1] * b.data[4] + data[5] * b.data[5] + data[9]  * b.data[6] + data[13] * b.data[7];
		result.data[6] = data[2] * b.data[4] + data[6] * b.data[5] + data[10] * b.data[6] + data[14] * b.data[7];
		result.data[7] = data[3] * b.data[4] + data[7] * b.data[5] + data[11] * b.data[6] + data[15] * b.data[7];

		//// Column 2
		result.data[8]  = data[0] * b.data[8]  + data[4] * b.data[9]  + data[8]  * b.data[10] + data[12] * b.data[11];
		result.data[9]  = data[1] * b.data[8]  + data[5] * b.data[9]  + data[9]  * b.data[10] + data[13] * b.data[11];
		result.data[10] = data[2] * b.data[8]  + data[6] * b.data[9]  + data[10] * b.data[10] + data[14] * b.data[11];
		result.data[11] = data[3] * b.data[8]  + data[7] * b.data[9]  + data[11] * b.data[10] + data[15] * b.data[11];

		//// Column 3
		result.data[12] = data[0] * b.data[12] + data[4] * b.data[13] + data[8]  * b.data[14] + data[12] * b.data[15];
		result.data[13] = data[1] * b.data[12] + data[5] * b.data[13] + data[9]  * b.data[14] + data[13] * b.data[15];
		result.data[14] = data[2] * b.data[12] + data[6] * b.data[13] + data[10] * b.data[14] + data[14] * b.data[15];
		result.data[15] = data[3] * b.data[12] + data[7] * b.data[13] + data[11] * b.data[14] + data[15] * b.data[15];

		return result;
	}
	inline bool operator==(const Mat4& b) const {
		if (data[0] != b.data[0] ||
			data[1] != b.data[1] ||
			data[2] != b.data[2] ||
			data[3] != b.data[3] ||
			data[4] != b.data[4] ||
			data[5] != b.data[5] ||
			data[6] != b.data[6] ||
			data[7] != b.data[7] ||
			data[8] != b.data[8] ||
			data[9] != b.data[9] ||
			data[10] != b.data[10] ||
			data[11] != b.data[11] ||
			data[12] != b.data[12] ||
			data[13] != b.data[13] ||
			data[14] != b.data[14] ||
			data[15] != b.data[15])
			return false;

		return true;
	}
};