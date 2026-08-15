#include <xmmintrin.h>
#include <cstdint>
#include "dxport.hpp"
#include "mem.hpp"

namespace DXPORT {

	struct Vec3 { float x, y, z; };
	static constexpr float FLT_CHECK = 1.4210855e-14;
	static constexpr float THREE = 3.0f;
	static constexpr float ONEHALF = 1.5f;
	static constexpr const float HALF = 0.5f;

	__attribute__((target("avx,fma")))
	Vec3* Vec3NormalizeBranchless(Vec3* out, const Vec3* in) {
    		const float x = in->x;
    		const float y = in->y;
    		const float z = in->z;

    		const float length_sq = x * x + y * y + z * z;

    		const float rsqrt_approx = _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(length_sq)));

    		const float rsqrt_refined_unmasked = rsqrt_approx * (ONEHALF - HALF * length_sq * rsqrt_approx * rsqrt_approx);

    		const float mask = (length_sq < FLT_CHECK) ? 1.0f : 0.0f;

    		const float rsqrt_refined = rsqrt_refined_unmasked * (1.0f - mask);

    		out->x = x * rsqrt_refined;
    		out->y = y * rsqrt_refined;
    		out->z = z * rsqrt_refined;

    		return out;
    }

	void Initialize() {

	    uint64_t pVec3 = 0ull;
		static constexpr std::string_view Vec3Byte = "ff 15 ?? ?? ?? ?? 41 ff c7 48 ff c5";
		Memory::PatternScan(Vec3Byte, pVec3);
		if (pVec3 != 0ull) {
		    pVec3 = Memory::moduleBase + Memory::FindRVA(Memory::moduleBase, std::bit_cast<void*>(pVec3), 2, 6);
			Memory::IatMod(std::bit_cast<uint64_t*>(pVec3), Vec3NormalizeBranchless);
		}

	}

}
