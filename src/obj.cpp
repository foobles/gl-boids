//
// Created by foobles on 7/11/2022.
//

#include "obj.hpp"

#include <stdexcept>
#include <algorithm>
#include <utility>
#include <format>
#include <cstdio>
#include <cctype>
#include <charconv>

enum class ParseResult {
    Ok,
    Error,
    NoMatch,
};

using enum ParseResult;

static bool to_int(std::string_view str, int &out) {
    auto begin = str.data();
    auto end = begin + str.size();
    auto [ptr, ec] = std::from_chars(begin, end, out);
    return (ec == std::errc()) && (ptr == end);
}

class ObjParser {
public:
    explicit ObjParser(char const *path) {
        if (fopen_s(&file_, path, "r") != 0) {
            throw std::runtime_error(std::format("Could not open file '{}'", path));
        }
    }

    ~ObjParser() noexcept {
        fclose(file_);
    }

    [[nodiscard]] std::string const &cur_line() const noexcept {
        return line_;
    }

    [[nodiscard]] bool load_next_line() {
        line_.clear();
        if (feof(file_)) {
            return false;
        }
        for (int cur = fgetc(file_); cur != EOF && cur != '\n'; cur = fgetc(file_)) {
            line_.push_back(static_cast<char>(cur));
        }
        next_token_start_ = 0;
        next_token_len_ = 0;
        consume_token();
        return true;
    }

    [[nodiscard]] std::string_view next_token() const {
        return {line_.data() + next_token_start_, static_cast<std::string_view::size_type>(next_token_len_)};
    }

    void consume_token() {
        auto is_space = [](char c) { return std::isspace(c); };
        auto begin = line_.begin();
        auto end = line_.end();
        auto tok_begin = std::find_if_not(begin + next_token_start_ + next_token_len_, end, is_space);
        auto tok_end = std::find_if(tok_begin, end, is_space);
        next_token_start_ = tok_begin - begin;
        next_token_len_ = tok_end - tok_begin;
    }

    ParseResult parse(Obj &out) {
        while (load_next_line()) {
            auto peek = next_token();
            if (peek.empty() || peek[0] == '#') {
                continue;
            }

            switch (parse_v(out)) {
                case Ok: continue;
                case Error: return Error;
                case NoMatch: break;
            }

            switch (parse_vt(out)) {
                case Ok: continue;
                case Error: return Error;
                case NoMatch: break;
            }

            switch (parse_f(out)) {
                case Ok: continue;
                case Error: return Error;
                case NoMatch: break;
            }
        }
        return Ok;
    }

    ParseResult parse_v(Obj &out) {
        if (!parse_token_keyword("v")) {
            return NoMatch;
        }

        Vec4<float> vertex = {{}};
        for (int i = 0; i < 3; ++i) {
            if (!parse_token_float(vertex[i])) {
                return Error;
            }
        }

        if (!next_token().empty()) {
            if (!parse_token_float(vertex[3])) {
                return Error;
            }
            if (!next_token().empty()) {
                return Error;
            }
        } else {
            vertex[3] = 1.0f;
        }
        out.v.push_back(vertex);
        return Ok;
    }

    ParseResult parse_vt(Obj &out) {
        if (!parse_token_keyword("vt")) {
            return NoMatch;
        }

        Obj::TexCoord uv{};
        if (!parse_token_float(uv.x)) {
            return Error;
        }

        if (!parse_token_float(uv.y)) {
            return Error;
        }

        if (!next_token().empty()) {
            return Error;
        }

        out.vt.push_back(uv);
        return Ok;
    }

    ParseResult parse_f(Obj &out) {
        if (!parse_token_keyword("f")) {
            return NoMatch;
        }

        std::vector<Obj::FaceVertex> face_vertices;
        for (auto peek = next_token(); !peek.empty(); peek = next_token()) {
            Obj::FaceVertex fv = {.v_idx = 0, .vt_idx = std::nullopt, .vn_idx = std::nullopt};
            if (!parse_token_face_vertex(fv)) {
                return Error;
            }
            face_vertices.push_back(fv);
        }
        if (face_vertices.size() < 3) {
            return Error;
        }
        out.f.emplace_back(std::move(face_vertices));
        return Ok;
    }

    bool parse_token_float(float &out) {
        auto peek = next_token();
        auto begin = peek.data();
        auto end = begin + peek.size();
        auto [ptr, ec] = std::from_chars(begin, end, out);
        if (ec != std::errc() || ptr != end) {
            return false;
        }
        consume_token();
        return true;
    }

    bool parse_token_face_vertex(Obj::FaceVertex &out) {
        auto peek = next_token();
        auto first_slash = std::find(peek.cbegin(), peek.cend(), '/');
        std::string_view first_number = {peek.cbegin(), first_slash};

        if (!to_int(first_number, out.v_idx)) {
            return false;
        }

        if (first_slash != peek.cend()){
            auto after_first_slash = first_slash + 1;
            auto second_slash = std::find(after_first_slash, peek.cend(), '/');
            std::string_view second_number = {after_first_slash, second_slash};

            if (second_slash != peek.cend()) {
                auto after_second_slash = second_slash + 1;
                std::string_view third_number = {after_second_slash, peek.cend()};

                if (!second_number.empty() && !to_int(second_number, out.vt_idx.emplace())) {
                    return false;
                }

                if (!to_int(third_number, out.vn_idx.emplace())) {
                   return false;
                }
            } else {
                if (!to_int(second_number, out.vt_idx.emplace())) {
                    return false;
                }
            }
        }
        consume_token();
        return true;
    }

    bool parse_token_keyword(std::string_view kw) {
        if (next_token() == kw) {
            consume_token();
            return true;
        } else {
            return false;
        }
    }

private:
    FILE *file_ = nullptr;
    std::string line_;
    int next_token_start_ = 0;
    int next_token_len_ = 0;
};

Obj::Obj(char const *path):
    v{},
    vt{},
    f{}
{
    ObjParser parser(path);
    if (parser.parse(*this) != Ok) {
        throw std::runtime_error(std::format("Error parsing line: {}", parser.cur_line()));
    }
}