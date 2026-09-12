// ============================================================
// 此文件由 embed_to_hpp.py 自动生成，请勿手动编辑。
// 生成时间: 2026-08-23T21:09:51.800119
// 若要更新，请重新运行该脚本。
// ============================================================

#pragma once

#include <cstdint>
#include <span>
#include "frozen/unordered_map.h"
#include "frozen/string.h"

using u8 = std::uint8_t;

namespace embed {
    template <size_t Extent = std::dynamic_extent>
    using u8span = std::span<u8, Extent>;

    template <size_t Extent = std::dynamic_extent>
    using cu8span = std::span<const u8, Extent>;
    
    using SpvsMap = frozen::unordered_map<frozen::string, const std::span<const u8>, 35>;

    #define BLOOM_VERT "../shader/Bloom.vert.spv"
    #define BLOOM_DOWNSAMPLE_FRAG "../shader/BloomDownsample.frag.spv"
    #define BLOOM_UPSAMPLE_FRAG "../shader/BloomUpsample.frag.spv"
    #define CUSTOM_SAMPLING_FRAG "../shader/CustomSampling.frag.spv"
    #define DEPTH_OUTLINE_FRAG "../shader/DepthOutline.frag.spv"
    #define DEPTH_SAMPLE_FRAG "../shader/DepthSample.frag.spv"
    #define EMPTY_FRAG "../shader/Empty.frag.spv"
    #define FILL_TEXTURE_COMP "../shader/FillTexture.comp.spv"
    #define FULLSCREEN_VERT "../shader/Fullscreen.vert.spv"
    #define GRADIENT_TEXTURE_COMP "../shader/GradientTexture.comp.spv"
    #define LERP_BLEND_FRAG "../shader/LerpBlend.frag.spv"
    #define LINEAR_TO_S_R_G_B_COMP "../shader/LinearToSRGB.comp.spv"
    #define LINEAR_TO_S_T2084_COMP "../shader/LinearToST2084.comp.spv"
    #define POSITION_COLOR_VERT "../shader/PositionColor.vert.spv"
    #define POSITION_COLOR_INSTANCED_VERT "../shader/PositionColorInstanced.vert.spv"
    #define POSITION_COLOR_TRANSFORM_VERT "../shader/PositionColorTransform.vert.spv"
    #define PULL_SPRITE_BATCH_VERT "../shader/PullSpriteBatch.vert.spv"
    #define RAW_TRIANGLE_VERT "../shader/RawTriangle.vert.spv"
    #define SKYBOX_FRAG "../shader/Skybox.frag.spv"
    #define SKYBOX_VERT "../shader/Skybox.vert.spv"
    #define SOLID_COLOR_FRAG "../shader/SolidColor.frag.spv"
    #define SOLID_COLOR_DEPTH_FRAG "../shader/SolidColorDepth.frag.spv"
    #define SPRITE_BATCH_COMP "../shader/SpriteBatch.comp.spv"
    #define TEXTURED_QUAD_COMP "../shader/TexturedQuad.comp.spv"
    #define TEXTURED_QUAD_FRAG "../shader/TexturedQuad.frag.spv"
    #define TEXTURED_QUAD_VERT "../shader/TexturedQuad.vert.spv"
    #define TEXTURED_QUAD_ARRAY_FRAG "../shader/TexturedQuadArray.frag.spv"
    #define TEXTURED_QUAD_COLOR_FRAG "../shader/TexturedQuadColor.frag.spv"
    #define TEXTURED_QUAD_COLOR_WITH_MATRIX_VERT "../shader/TexturedQuadColorWithMatrix.vert.spv"
    #define TEXTURED_QUAD_WITH_MATRIX_VERT "../shader/TexturedQuadWithMatrix.vert.spv"
    #define TEXTURED_QUAD_WITH_MULTIPLY_COLOR_FRAG "../shader/TexturedQuadWithMultiplyColor.frag.spv"
    #define TONE_MAP_A_C_E_S_COMP "../shader/ToneMapACES.comp.spv"
    #define TONE_MAP_EXTENDED_REINHARD_LUMINANCE_COMP "../shader/ToneMapExtendedReinhardLuminance.comp.spv"
    #define TONE_MAP_HABLE_COMP "../shader/ToneMapHable.comp.spv"
    #define TONE_MAP_REINHARD_COMP "../shader/ToneMapReinhard.comp.spv"

    alignas(4) static constexpr u8 raw_bloom_vert[] = {
        #embed BLOOM_VERT
    };
    constexpr cu8span<sizeof(raw_bloom_vert)>
        bloom_vert(raw_bloom_vert);

    alignas(4) static constexpr u8 raw_bloom_downsample_frag[] = {
        #embed BLOOM_DOWNSAMPLE_FRAG
    };
    constexpr cu8span<sizeof(raw_bloom_downsample_frag)>
        bloom_downsample_frag(raw_bloom_downsample_frag);

    alignas(4) static constexpr u8 raw_bloom_upsample_frag[] = {
        #embed BLOOM_UPSAMPLE_FRAG
    };
    constexpr cu8span<sizeof(raw_bloom_upsample_frag)>
        bloom_upsample_frag(raw_bloom_upsample_frag);

    alignas(4) static constexpr u8 raw_custom_sampling_frag[] = {
        #embed CUSTOM_SAMPLING_FRAG
    };
    constexpr cu8span<sizeof(raw_custom_sampling_frag)>
        custom_sampling_frag(raw_custom_sampling_frag);

    alignas(4) static constexpr u8 raw_depth_outline_frag[] = {
        #embed DEPTH_OUTLINE_FRAG
    };
    constexpr cu8span<sizeof(raw_depth_outline_frag)>
        depth_outline_frag(raw_depth_outline_frag);

    alignas(4) static constexpr u8 raw_depth_sample_frag[] = {
        #embed DEPTH_SAMPLE_FRAG
    };
    constexpr cu8span<sizeof(raw_depth_sample_frag)>
        depth_sample_frag(raw_depth_sample_frag);

    alignas(4) static constexpr u8 raw_empty_frag[] = {
        #embed EMPTY_FRAG
    };
    constexpr cu8span<sizeof(raw_empty_frag)>
        empty_frag(raw_empty_frag);

    alignas(4) static constexpr u8 raw_fill_texture_comp[] = {
        #embed FILL_TEXTURE_COMP
    };
    constexpr cu8span<sizeof(raw_fill_texture_comp)>
        fill_texture_comp(raw_fill_texture_comp);

    alignas(4) static constexpr u8 raw_fullscreen_vert[] = {
        #embed FULLSCREEN_VERT
    };
    constexpr cu8span<sizeof(raw_fullscreen_vert)>
        fullscreen_vert(raw_fullscreen_vert);

    alignas(4) static constexpr u8 raw_gradient_texture_comp[] = {
        #embed GRADIENT_TEXTURE_COMP
    };
    constexpr cu8span<sizeof(raw_gradient_texture_comp)>
        gradient_texture_comp(raw_gradient_texture_comp);

    alignas(4) static constexpr u8 raw_lerp_blend_frag[] = {
        #embed LERP_BLEND_FRAG
    };
    constexpr cu8span<sizeof(raw_lerp_blend_frag)>
        lerp_blend_frag(raw_lerp_blend_frag);

    alignas(4) static constexpr u8 raw_linear_to_s_r_g_b_comp[] = {
        #embed LINEAR_TO_S_R_G_B_COMP
    };
    constexpr cu8span<sizeof(raw_linear_to_s_r_g_b_comp)>
        linear_to_s_r_g_b_comp(raw_linear_to_s_r_g_b_comp);

    alignas(4) static constexpr u8 raw_linear_to_s_t2084_comp[] = {
        #embed LINEAR_TO_S_T2084_COMP
    };
    constexpr cu8span<sizeof(raw_linear_to_s_t2084_comp)>
        linear_to_s_t2084_comp(raw_linear_to_s_t2084_comp);

    alignas(4) static constexpr u8 raw_position_color_vert[] = {
        #embed POSITION_COLOR_VERT
    };
    constexpr cu8span<sizeof(raw_position_color_vert)>
        position_color_vert(raw_position_color_vert);

    alignas(4) static constexpr u8 raw_position_color_instanced_vert[] = {
        #embed POSITION_COLOR_INSTANCED_VERT
    };
    constexpr cu8span<sizeof(raw_position_color_instanced_vert)>
        position_color_instanced_vert(raw_position_color_instanced_vert);

    alignas(4) static constexpr u8 raw_position_color_transform_vert[] = {
        #embed POSITION_COLOR_TRANSFORM_VERT
    };
    constexpr cu8span<sizeof(raw_position_color_transform_vert)>
        position_color_transform_vert(raw_position_color_transform_vert);

    alignas(4) static constexpr u8 raw_pull_sprite_batch_vert[] = {
        #embed PULL_SPRITE_BATCH_VERT
    };
    constexpr cu8span<sizeof(raw_pull_sprite_batch_vert)>
        pull_sprite_batch_vert(raw_pull_sprite_batch_vert);

    alignas(4) static constexpr u8 raw_raw_triangle_vert[] = {
        #embed RAW_TRIANGLE_VERT
    };
    constexpr cu8span<sizeof(raw_raw_triangle_vert)>
        raw_triangle_vert(raw_raw_triangle_vert);

    alignas(4) static constexpr u8 raw_skybox_frag[] = {
        #embed SKYBOX_FRAG
    };
    constexpr cu8span<sizeof(raw_skybox_frag)>
        skybox_frag(raw_skybox_frag);

    alignas(4) static constexpr u8 raw_skybox_vert[] = {
        #embed SKYBOX_VERT
    };
    constexpr cu8span<sizeof(raw_skybox_vert)>
        skybox_vert(raw_skybox_vert);

    alignas(4) static constexpr u8 raw_solid_color_frag[] = {
        #embed SOLID_COLOR_FRAG
    };
    constexpr cu8span<sizeof(raw_solid_color_frag)>
        solid_color_frag(raw_solid_color_frag);

    alignas(4) static constexpr u8 raw_solid_color_depth_frag[] = {
        #embed SOLID_COLOR_DEPTH_FRAG
    };
    constexpr cu8span<sizeof(raw_solid_color_depth_frag)>
        solid_color_depth_frag(raw_solid_color_depth_frag);

    alignas(4) static constexpr u8 raw_sprite_batch_comp[] = {
        #embed SPRITE_BATCH_COMP
    };
    constexpr cu8span<sizeof(raw_sprite_batch_comp)>
        sprite_batch_comp(raw_sprite_batch_comp);

    alignas(4) static constexpr u8 raw_textured_quad_comp[] = {
        #embed TEXTURED_QUAD_COMP
    };
    constexpr cu8span<sizeof(raw_textured_quad_comp)>
        textured_quad_comp(raw_textured_quad_comp);

    alignas(4) static constexpr u8 raw_textured_quad_frag[] = {
        #embed TEXTURED_QUAD_FRAG
    };
    constexpr cu8span<sizeof(raw_textured_quad_frag)>
        textured_quad_frag(raw_textured_quad_frag);

    alignas(4) static constexpr u8 raw_textured_quad_vert[] = {
        #embed TEXTURED_QUAD_VERT
    };
    constexpr cu8span<sizeof(raw_textured_quad_vert)>
        textured_quad_vert(raw_textured_quad_vert);

    alignas(4) static constexpr u8 raw_textured_quad_array_frag[] = {
        #embed TEXTURED_QUAD_ARRAY_FRAG
    };
    constexpr cu8span<sizeof(raw_textured_quad_array_frag)>
        textured_quad_array_frag(raw_textured_quad_array_frag);

    alignas(4) static constexpr u8 raw_textured_quad_color_frag[] = {
        #embed TEXTURED_QUAD_COLOR_FRAG
    };
    constexpr cu8span<sizeof(raw_textured_quad_color_frag)>
        textured_quad_color_frag(raw_textured_quad_color_frag);

    alignas(4) static constexpr u8 raw_textured_quad_color_with_matrix_vert[] = {
        #embed TEXTURED_QUAD_COLOR_WITH_MATRIX_VERT
    };
    constexpr cu8span<sizeof(raw_textured_quad_color_with_matrix_vert)>
        textured_quad_color_with_matrix_vert(raw_textured_quad_color_with_matrix_vert);

    alignas(4) static constexpr u8 raw_textured_quad_with_matrix_vert[] = {
        #embed TEXTURED_QUAD_WITH_MATRIX_VERT
    };
    constexpr cu8span<sizeof(raw_textured_quad_with_matrix_vert)>
        textured_quad_with_matrix_vert(raw_textured_quad_with_matrix_vert);

    alignas(4) static constexpr u8 raw_textured_quad_with_multiply_color_frag[] = {
        #embed TEXTURED_QUAD_WITH_MULTIPLY_COLOR_FRAG
    };
    constexpr cu8span<sizeof(raw_textured_quad_with_multiply_color_frag)>
        textured_quad_with_multiply_color_frag(raw_textured_quad_with_multiply_color_frag);

    alignas(4) static constexpr u8 raw_tone_map_a_c_e_s_comp[] = {
        #embed TONE_MAP_A_C_E_S_COMP
    };
    constexpr cu8span<sizeof(raw_tone_map_a_c_e_s_comp)>
        tone_map_a_c_e_s_comp(raw_tone_map_a_c_e_s_comp);

    alignas(4) static constexpr u8 raw_tone_map_extended_reinhard_luminance_comp[] = {
        #embed TONE_MAP_EXTENDED_REINHARD_LUMINANCE_COMP
    };
    constexpr cu8span<sizeof(raw_tone_map_extended_reinhard_luminance_comp)>
        tone_map_extended_reinhard_luminance_comp(raw_tone_map_extended_reinhard_luminance_comp);

    alignas(4) static constexpr u8 raw_tone_map_hable_comp[] = {
        #embed TONE_MAP_HABLE_COMP
    };
    constexpr cu8span<sizeof(raw_tone_map_hable_comp)>
        tone_map_hable_comp(raw_tone_map_hable_comp);

    alignas(4) static constexpr u8 raw_tone_map_reinhard_comp[] = {
        #embed TONE_MAP_REINHARD_COMP
    };
    constexpr cu8span<sizeof(raw_tone_map_reinhard_comp)>
        tone_map_reinhard_comp(raw_tone_map_reinhard_comp);

    constexpr SpvsMap spvs_map{
        {"Bloom.vert", bloom_vert},
        {"BloomDownsample.frag", bloom_downsample_frag},
        {"BloomUpsample.frag", bloom_upsample_frag},
        {"CustomSampling.frag", custom_sampling_frag},
        {"DepthOutline.frag", depth_outline_frag},
        {"DepthSample.frag", depth_sample_frag},
        {"Empty.frag", empty_frag},
        {"FillTexture.comp", fill_texture_comp},
        {"Fullscreen.vert", fullscreen_vert},
        {"GradientTexture.comp", gradient_texture_comp},
        {"LerpBlend.frag", lerp_blend_frag},
        {"LinearToSRGB.comp", linear_to_s_r_g_b_comp},
        {"LinearToST2084.comp", linear_to_s_t2084_comp},
        {"PositionColor.vert", position_color_vert},
        {"PositionColorInstanced.vert", position_color_instanced_vert},
        {"PositionColorTransform.vert", position_color_transform_vert},
        {"PullSpriteBatch.vert", pull_sprite_batch_vert},
        {"RawTriangle.vert", raw_triangle_vert},
        {"Skybox.frag", skybox_frag},
        {"Skybox.vert", skybox_vert},
        {"SolidColor.frag", solid_color_frag},
        {"SolidColorDepth.frag", solid_color_depth_frag},
        {"SpriteBatch.comp", sprite_batch_comp},
        {"TexturedQuad.comp", textured_quad_comp},
        {"TexturedQuad.frag", textured_quad_frag},
        {"TexturedQuad.vert", textured_quad_vert},
        {"TexturedQuadArray.frag", textured_quad_array_frag},
        {"TexturedQuadColor.frag", textured_quad_color_frag},
        {"TexturedQuadColorWithMatrix.vert", textured_quad_color_with_matrix_vert},
        {"TexturedQuadWithMatrix.vert", textured_quad_with_matrix_vert},
        {"TexturedQuadWithMultiplyColor.frag", textured_quad_with_multiply_color_frag},
        {"ToneMapACES.comp", tone_map_a_c_e_s_comp},
        {"ToneMapExtendedReinhardLuminance.comp", tone_map_extended_reinhard_luminance_comp},
        {"ToneMapHable.comp", tone_map_hable_comp},
        {"ToneMapReinhard.comp", tone_map_reinhard_comp}
    };

}; // namespace embed
