//
// Created by foobles on 7/11/2022.
//

#ifndef SDL_GLEW_TEST_OBJ_HPP
#define SDL_GLEW_TEST_OBJ_HPP

#include "vec4.hpp"
#include <vector>
#include <optional>

class ObjParser;
class Obj {
public:
    struct FaceVertex {
        int v_idx{};
        std::optional<int> vt_idx;
        std::optional<int> vn_idx;
    };

    struct Face {
        std::vector<FaceVertex> vertices;
    };

    struct TexCoord {
        float x;
        float y;
    };

    explicit Obj(char const *path);

    std::vector<Vec4<float>> v;
    std::vector<TexCoord> vt;
    std::vector<Face> f;
};

#endif //SDL_GLEW_TEST_OBJ_HPP
