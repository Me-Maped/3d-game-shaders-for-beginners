/*
  Learning panel for the shader demo. Widgets write the same values the
  keyboard shortcuts use, plus the uniforms that used to be hardcoded.
*/

#ifndef DEBUG_PANEL_H
#define DEBUG_PANEL_H

#include <string>
#include <vector>

#include "luse.h"

struct ShaderDebugState {
  bool panel_open = true;
  bool reset_requested = false;
  bool buffer_changed = false;
  bool sun_dragging = false;
  bool go_midday = false;
  bool go_midnight = false;

  bool particles_on = true;
  bool* sound_enabled = nullptr;
  bool* animate_sun = nullptr;

  int* buffer_index = nullptr;
  std::vector<std::string>* buffer_names = nullptr;

  LVecBase2f* ssao = nullptr;
  LVecBase2f* blinn_phong = nullptr;
  LVecBase2f* fresnel = nullptr;
  LVecBase2f* rim_light = nullptr;
  LVecBase2f* refraction = nullptr;
  LVecBase2f* reflection = nullptr;
  LVecBase2f* fog = nullptr;
  LVecBase2f* outline = nullptr;
  LVecBase2f* cel = nullptr;
  LVecBase2f* normal_maps = nullptr;
  LVecBase2f* bloom = nullptr;
  LVecBase2f* sharpen = nullptr;
  LVecBase2f* depth_of_field = nullptr;
  LVecBase2f* film_grain = nullptr;
  LVecBase2f* flow_maps = nullptr;
  LVecBase2f* lookup_table = nullptr;
  LVecBase2f* painterly = nullptr;
  LVecBase2f* motion_blur = nullptr;
  LVecBase2f* posterize = nullptr;
  LVecBase2f* pixelize = nullptr;
  LVecBase2f* chromatic = nullptr;

  float* fog_near = nullptr;
  float* fog_far = nullptr;
  float* foam_depth = nullptr;
  float* ior = nullptr;
  float* sun_pitch = nullptr;
  float* focus_x = nullptr;
  float* focus_y = nullptr;

  float bloom_size = 3.0f;
  float bloom_separation = 4.0f;
  float bloom_threshold = 0.6f;
  float bloom_amount = 0.6f;

  float ssao_radius = 0.6f;
  float ssao_bias = 0.005f;
  float ssao_magnitude = 1.1f;
  float ssao_contrast = 1.1f;

  float sharpen_amount = 0.3f;
  float posterize_levels = 6.0f;
  float film_grain_amount = 0.01f;
  float dof_min = 8.0f;
  float dof_max = 12.0f;
  float ca_red = 0.009f;
  float ca_green = 0.006f;
  float ca_blue = -0.006f;
  float pixel_size = 5.0f;
  float motion_size = 2.0f;
  float motion_separation = 1.0f;
  float painterly_size = 3.0f;
  float outline_separation = 1.0f;

  void reset_tunables();
};

void draw_shader_debug(ShaderDebugState& state, bool cjk_font);

#endif
