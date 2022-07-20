//
// Created by foobles on 7/11/2022.
//

#ifndef SDL_GLEW_TEST_OBJ_FORMAT_HPP
#define SDL_GLEW_TEST_OBJ_FORMAT_HPP

#include <vector>
#include <optional>
#include "model.hpp"
#include "vec4.hpp"

class ObjFormat {
public:
    struct FaceVertex {
        bool operator==(FaceVertex const &other) const = default;

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

    explicit ObjFormat(char const *path);

    [[nodiscard]] Model create_model() const;

    std::vector<Vec4<float>> v;
    std::vector<TexCoord> vt;
    std::vector<Face> f;
};

#endif //SDL_GLEW_TEST_OBJ_FORMAT_HPP
