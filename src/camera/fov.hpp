#ifndef FOV_HPP
#define FOV_HPP

#include <cstdint>

namespace FOV {

    using SystemSettingClose = uint64_t(__fastcall*)(uint64_t a1);

    inline SystemSettingClose SSC = nullptr;

    inline uint64_t SSCArgs = 0ull;


    void Initialize();


}

#endif
