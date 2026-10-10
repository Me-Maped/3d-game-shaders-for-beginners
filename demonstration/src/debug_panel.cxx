#include "debug_panel.h"

#include "imgui.h"

namespace {

bool flag_on(const LVecBase2f& value) {
  return value[0] >= 1.0f;
}

void set_flag(LVecBase2f& value, bool on) {
  const float bit = on ? 1.0f : 0.0f;
  value[0] = bit;
  value[1] = bit;
}

void toggle_row(const char* label, const char* shortcut, LVecBase2f* value, const char* tip) {
  if (value == nullptr) { return; }
  bool on = flag_on(*value);
  if (ImGui::Checkbox(label, &on)) {
    set_flag(*value, on);
  }
  if (tip != nullptr && ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", tip);
  }
  ImGui::SameLine(250.0f);
  ImGui::TextDisabled("%s", shortcut);
}

bool slider_float(const char* label, float* value, float min_value, float max_value, const char* tip) {
  if (value == nullptr) { return false; }
  const bool edited = ImGui::SliderFloat(label, value, min_value, max_value, "%.3f");
  if (tip != nullptr && ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", tip);
  }
  return edited;
}

void shortcut_line(const char* key, const char* what) {
  ImGui::BulletText("%s    %s", key, what);
}

}

void ShaderDebugState::reset_tunables() {
  bloom_size = 3.0f;
  bloom_separation = 4.0f;
  bloom_threshold = 0.6f;
  bloom_amount = 0.6f;
  ssao_radius = 0.6f;
  ssao_bias = 0.005f;
  ssao_magnitude = 1.1f;
  ssao_contrast = 1.1f;
  sharpen_amount = 0.3f;
  posterize_levels = 6.0f;
  film_grain_amount = 0.01f;
  dof_min = 8.0f;
  dof_max = 12.0f;
  ca_red = 0.009f;
  ca_green = 0.006f;
  ca_blue = -0.006f;
  pixel_size = 5.0f;
  motion_size = 2.0f;
  motion_separation = 1.0f;
  painterly_size = 3.0f;
  outline_separation = 1.0f;
}

void draw_shader_debug(ShaderDebugState& state, bool cjk_font) {
  state.sun_dragging = false;
  state.go_midday = false;
  state.go_midnight = false;
  state.reset_requested = false;
  state.buffer_changed = false;

  const ImGuiWindowFlags hint_flags =
      ImGuiWindowFlags_NoDecoration
    | ImGuiWindowFlags_NoInputs
    | ImGuiWindowFlags_AlwaysAutoResize
    | ImGuiWindowFlags_NoNav
    | ImGuiWindowFlags_NoMove;

  if (!state.panel_open) {
    ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.45f);
    ImGui::Begin("##shader-debug-hint", nullptr, hint_flags);
    if (cjk_font) {
      ImGui::TextUnformatted("F1  打开着色器调试面板");
    } else {
      ImGui::TextUnformatted("F1  open the shader debug panel");
    }
    ImGui::End();
    return;
  }

  ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(470.0f, 700.0f), ImGuiCond_FirstUseEver);
  const char* title = cjk_font
    ? "着色器调试  [F1 关闭]"
    : "Shader Debug  [F1 hide]";
  ImGui::Begin(title, &state.panel_open);

  if (cjk_font) {
    ImGui::TextWrapped(
      "拖滑块就能看到这一 pass 的 uniform 怎么影响画面。"
      "光标在面板上时，鼠标不再转镜头。F1 可以收起。"
    );
  } else {
    ImGui::TextWrapped(
      "Sliders write the shader uniforms directly. "
      "The mouse ignores the camera while it is over this panel. F1 hides it."
    );
  }
  ImGui::Separator();

  if (state.buffer_names != nullptr && state.buffer_index != nullptr && !state.buffer_names->empty()) {
    const int count = static_cast<int>(state.buffer_names->size());
    int index = *state.buffer_index;
    if (index < 0) { index = 0; }
    if (index >= count) { index = count - 1; }
    const char* preview = (*state.buffer_names)[index].c_str();
    const char* combo_label = cjk_font ? "当前缓冲" : "Buffer";
    if (ImGui::BeginCombo(combo_label, preview)) {
      for (int i = 0; i < count; ++i) {
        const bool selected = i == index;
        if (ImGui::Selectable((*state.buffer_names)[i].c_str(), selected)) {
          *state.buffer_index = i;
          state.buffer_changed = true;
        }
        if (selected) { ImGui::SetItemDefaultFocus(); }
      }
      ImGui::EndCombo();
    }
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip(
        cjk_font
          ? "和 Tab / Shift+Tab 一样，逐张查看 G-buffer 和后处理 pass。"
          : "Same as Tab / Shift+Tab: inspect each G-buffer and post pass."
      );
    }
  }

  ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
  if (ImGui::CollapsingHeader(cjk_font ? "快捷键" : "Shortcuts")) {
    shortcut_line("F1", cjk_font ? "开关这个面板" : "toggle this panel");
    shortcut_line("WASD / 鼠标左键", cjk_font ? "旋转镜头" : "orbit");
    shortcut_line("方向键 / 鼠标右键", cjk_font ? "平移" : "pan");
    shortcut_line("Z X / 滚轮", cjk_font ? "缩放" : "dolly");
    shortcut_line("鼠标中键", cjk_font ? "点选景深和色差的焦点" : "pick DOF / CA focus");
    shortcut_line("R", cjk_font ? "重置镜头和下面这些参数" : "reset camera and parameters");
    shortcut_line("Tab / Shift+Tab", cjk_font ? "切换 framebuffer" : "cycle framebuffers");
    shortcut_line("1 / 2", cjk_font ? "正午 / 午夜" : "midday / midnight");
    shortcut_line("3", "Fresnel");
    shortcut_line("4", cjk_font ? "边缘光 Rim" : "Rim light");
    shortcut_line("5", cjk_font ? "粒子" : "Particles");
    shortcut_line("6", "Motion Blur");
    shortcut_line("7", cjk_font ? "Kuwahara 油画" : "Kuwahara");
    shortcut_line("8", "Cel shading");
    shortcut_line("9", "Lookup table");
    shortcut_line("0", "Phong / Blinn-Phong");
    shortcut_line("Y U I O P", "SSAO / Outline / Bloom / Normal / Fog");
    shortcut_line("H J K L", "DOF / Posterize / Pixelize / Sharpen");
    shortcut_line("N M , .", "Film grain / SSR / Refraction / Flow");
    shortcut_line("/  \\", cjk_font ? "太阳动画 / 色差" : "Sun animation / chromatic aberration");
    shortcut_line("Delete", cjk_font ? "声音" : "Sound");
    shortcut_line("[ ]", cjk_font ? "雾的 near / far，按住 Shift 反向" : "fog near / far, Shift reverses");
    shortcut_line("- / =", cjk_font ? "泡沫厚度 / 折射率，按住 Shift 反向" : "foam / IOR, Shift reverses");
  }

  ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
  if (ImGui::CollapsingHeader(cjk_font ? "效果开关" : "Toggles")) {
    if (ImGui::BeginTable("toggles", 1, ImGuiTableFlags_None)) {
      auto row = [&](const char* label, const char* key, LVecBase2f* value, const char* tip) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        toggle_row(label, key, value, tip);
      };
      row(cjk_font ? "环境光遮蔽 SSAO" : "SSAO", "Y", state.ssao, cjk_font ? "屏幕空间环境光遮蔽，看缝隙和接触阴影。" : "Screen-space ambient occlusion.");
      row(cjk_font ? "泛光 Bloom" : "Bloom", "I", state.bloom, cjk_font ? "把超过阈值的亮部扩成光晕。" : "Bleed pixels above the threshold.");
      row(cjk_font ? "法线贴图" : "Normal maps", "O", state.normal_maps, cjk_font ? "用贴图扰动光照法线。" : "Perturb lighting normals.");
      row(cjk_font ? "雾 Fog" : "Fog", "P", state.fog, cjk_font ? "按距离混入天空颜色。" : "Blend toward the sky by distance.");
      row(cjk_font ? "描边 Outline" : "Outline", "U", state.outline, cjk_font ? "用深度差找轮廓。" : "Contours from depth differences.");
      row("Cel", "8", state.cel, cjk_font ? "把光照量化成几档。" : "Quantize the lighting.");
      row("Fresnel", "3", state.fresnel, cjk_font ? "掠射角越亮，水面边缘会更亮。" : "Brighter at grazing angles.");
      row(cjk_font ? "边缘光 Rim" : "Rim light", "4", state.rim_light, cjk_font ? "在轮廓上加一圈光。" : "Extra light along the silhouette.");
      row("Blinn-Phong", "0", state.blinn_phong, cjk_font ? "关掉就是普通 Phong 高光。" : "Off uses Phong instead.");
      row(cjk_font ? "镜面反射 SSR" : "Reflection", "M", state.reflection, cjk_font ? "屏幕空间反射。" : "Screen-space reflection.");
      row(cjk_font ? "折射" : "Refraction", ",", state.refraction, cjk_font ? "屏幕空间折射，受折射率影响。" : "Screen-space refraction.");
      row(cjk_font ? "流向贴图" : "Flow maps", ".", state.flow_maps, cjk_font ? "让水和水车动起来。" : "Animates water and the wheel.");
      row(cjk_font ? "景深 DOF" : "Depth of field", "H", state.depth_of_field, cjk_font ? "焦点以外变糊。中键点场景改焦点。" : "Blur away from the focus point.");
      row(cjk_font ? "锐化" : "Sharpen", "L", state.sharpen, cjk_font ? "加强邻域对比。" : "Boost local contrast.");
      row(cjk_font ? "色调分离" : "Posterize", "J", state.posterize, cjk_font ? "把亮度收成有限档。" : "Collapse luminance into bands.");
      row(cjk_font ? "像素化" : "Pixelize", "K", state.pixelize, cjk_font ? "按像素块取样。" : "Sample in pixel blocks.");
      row(cjk_font ? "油画 Kuwahara" : "Painterly", "7", state.painterly, cjk_font ? "Kuwahara 滤波，核越大越像油画。" : "Kuwahara filter.");
      row(cjk_font ? "运动模糊" : "Motion blur", "6", state.motion_blur, cjk_font ? "沿上一帧的位移拉丝。" : "Smear along last frame's motion.");
      row(cjk_font ? "胶片颗粒" : "Film grain", "N", state.film_grain, cjk_font ? "每帧加一点随机亮度。" : "Random luminance each frame.");
      row("LUT", "9", state.lookup_table, cjk_font ? "按太阳位置在两张 LUT 之间混合。" : "Blend lookup tables by sun position.");
      row(cjk_font ? "色差" : "Chromatic aberration", "\\", state.chromatic, cjk_font ? "离焦点越远，RGB 分得越开。" : "Split RGB away from the focus.");
      ImGui::EndTable();
    }

    ImGui::Checkbox(cjk_font ? "粒子" : "Particles", &state.particles_on);
    ImGui::SameLine(250.0f);
    ImGui::TextDisabled("5");
    if (state.sound_enabled != nullptr) {
      ImGui::Checkbox(cjk_font ? "声音" : "Sound", state.sound_enabled);
      ImGui::SameLine(250.0f);
      ImGui::TextDisabled("Delete");
    }
  }

  if (ImGui::CollapsingHeader(cjk_font ? "太阳" : "Sun")) {
    if (state.animate_sun != nullptr) {
      ImGui::Checkbox(cjk_font ? "太阳动画" : "Animate sun", state.animate_sun);
      ImGui::SameLine(250.0f);
      ImGui::TextDisabled("/");
    }
    if (ImGui::Button(cjk_font ? "正午" : "Midday")) { state.go_midday = true; }
    ImGui::SameLine();
    if (ImGui::Button(cjk_font ? "午夜" : "Midnight")) { state.go_midnight = true; }
    ImGui::SameLine();
    ImGui::TextDisabled("1 / 2");
    slider_float(
        cjk_font ? "俯仰角" : "Pitch"
      , state.sun_pitch
      , 0.0f
      , 360.0f
      , cjk_font ? "拖动会暂停动画。0-360 对应阳光转一圈，着色器也用这个角度混白天和夜晚。" : "Dragging pauses the animation."
    );
    if (ImGui::IsItemActive()) { state.sun_dragging = true; }
  }

  if (ImGui::CollapsingHeader(cjk_font ? "雾 / 泡沫 / 折射" : "Fog / foam / IOR")) {
    slider_float(cjk_font ? "雾 near" : "Fog near", state.fog_near, 0.0f, 20.0f, cjk_font ? "开始起雾的距离。对应 [ 键。" : "Distance where fog starts.");
    slider_float(cjk_font ? "雾 far" : "Fog far", state.fog_far, 0.1f, 40.0f, cjk_font ? "雾完全盖住的距离。对应 ] 键。" : "Distance where fog is opaque.");
    slider_float(cjk_font ? "泡沫厚度" : "Foam depth", state.foam_depth, 0.001f, 6.0f, cjk_font ? "水和岸的深度差在这个范围内才出泡沫。对应 - 键。" : "Depth range that produces foam.");
    slider_float(cjk_font ? "相对折射率" : "Index of refraction", state.ior, 0.5f, 2.5f, cjk_font ? "refract() 的 eta。1 几乎不弯，偏离 1 越多弯得越厉害。对应 = 键。" : "eta passed to refract().");
  }

  if (ImGui::CollapsingHeader("SSAO")) {
    slider_float(cjk_font ? "采样半径" : "Radius", &state.ssao_radius, 0.0f, 3.0f, cjk_font ? "采样半球的半径。大了遮挡范围变宽，也更容易脏。" : "Sample hemisphere radius.");
    slider_float(cjk_font ? "偏差 bias" : "Bias", &state.ssao_bias, 0.0f, 0.05f, cjk_font ? "深度比较的容差，用来压自遮挡噪点。" : "Depth compare tolerance.");
    slider_float(cjk_font ? "强度" : "Magnitude", &state.ssao_magnitude, 0.1f, 4.0f, cjk_font ? "遮蔽值的 pow 指数，越大暗部越重。" : "pow() on the occlusion term.");
    slider_float(cjk_font ? "对比" : "Contrast", &state.ssao_contrast, 0.1f, 4.0f, cjk_font ? "把遮蔽值从 0.5 往两边推。" : "Pushes occlusion away from 0.5.");
  }

  if (ImGui::CollapsingHeader("Bloom")) {
    slider_float(cjk_font ? "模糊半径" : "Kernel", &state.bloom_size, 0.0f, 5.0f, cjk_font ? "盒式模糊的半宽，单位是像素步数。" : "Half-size of the box blur.");
    slider_float(cjk_font ? "步长" : "Separation", &state.bloom_separation, 1.0f, 8.0f, cjk_font ? "每个样本间隔多少像素。大了光晕更散，也更便宜。" : "Pixels between samples.");
    slider_float(cjk_font ? "亮度阈值" : "Threshold", &state.bloom_threshold, 0.0f, 1.0f, cjk_font ? "低于这个亮度的像素不参与泛光。" : "Pixels dimmer than this are ignored.");
    slider_float(cjk_font ? "混合量" : "Amount", &state.bloom_amount, 0.0f, 2.0f, cjk_font ? "泛光叠回画面的比例。" : "How much bloom is mixed back.");
  }

  if (ImGui::CollapsingHeader(cjk_font ? "景深 / 色差 / 描边" : "DOF / CA / outline")) {
    slider_float(cjk_font ? "焦点 X" : "Focus X", state.focus_x, 0.0f, 1.0f, cjk_font ? "焦点的屏幕坐标。也可以用中键在画面上点。" : "Focus in screen UV.");
    slider_float(cjk_font ? "焦点 Y" : "Focus Y", state.focus_y, 0.0f, 1.0f, cjk_font ? "Y 从下往上。色差也用同一个焦点。" : "Shared with chromatic aberration.");
    slider_float(cjk_font ? "景深近距" : "DOF near", &state.dof_min, 0.0f, 40.0f, cjk_font ? "和焦点的距离小于这个值时保持清晰。" : "Stay sharp inside this distance.");
    slider_float(cjk_font ? "景深远距" : "DOF far", &state.dof_max, 0.0f, 40.0f, cjk_font ? "超过这个距离就用满模糊。" : "Fully blurred past this distance.");
    slider_float(cjk_font ? "色差 R" : "CA red", &state.ca_red, -0.05f, 0.05f, cjk_font ? "红色通道沿离开焦点的方向偏移。" : "Red channel offset.");
    slider_float(cjk_font ? "色差 G" : "CA green", &state.ca_green, -0.05f, 0.05f, cjk_font ? "绿色通道偏移。" : "Green channel offset.");
    slider_float(cjk_font ? "色差 B" : "CA blue", &state.ca_blue, -0.05f, 0.05f, cjk_font ? "蓝色通道偏移。正负方向相反才会分开。" : "Blue channel offset.");
    slider_float(cjk_font ? "描边间距" : "Outline gap", &state.outline_separation, 0.25f, 4.0f, cjk_font ? "深度比较的采样间距。越大轮廓越粗、越容易断。" : "Depth-sample separation.");
  }

  if (ImGui::CollapsingHeader(cjk_font ? "风格化" : "Stylize")) {
    slider_float(cjk_font ? "锐化强度" : "Sharpen", &state.sharpen_amount, 0.0f, 2.0f, cjk_font ? "中心权重是 1 + 4*强度，四邻是 -强度。" : "Center weight is 1 + 4*amount.");
    slider_float(cjk_font ? "色调档数" : "Posterize levels", &state.posterize_levels, 2.0f, 24.0f, cjk_font ? "亮度被收成这么多档。" : "Number of luminance bands.");
    slider_float(cjk_font ? "像素块" : "Pixel size", &state.pixel_size, 1.0f, 31.0f, cjk_font ? "块大小，着色器要求奇数。" : "Block size. The shader wants an odd number.");
    slider_float(cjk_font ? "油画核" : "Painterly size", &state.painterly_size, 0.0f, 5.0f, cjk_font ? "Kuwahara 的窗口半径，0 等于关掉。" : "Kuwahara window radius.");
    slider_float(cjk_font ? "运动模糊采样" : "Motion samples", &state.motion_size, 0.0f, 16.0f, cjk_font ? "沿运动方向取多少个样本。" : "Samples along the motion vector.");
    slider_float(cjk_font ? "运动模糊间距" : "Motion separation", &state.motion_separation, 0.0f, 4.0f, cjk_font ? "样本之间的间隔。" : "Gap between motion samples.");
    slider_float(cjk_font ? "颗粒强度" : "Grain", &state.film_grain_amount, 0.0f, 0.2f, cjk_font ? "加到颜色上的随机亮度上限。" : "Max random luminance added.");
  }

  ImGui::Separator();
  if (ImGui::Button(cjk_font ? "重置镜头和参数 (R)" : "Reset camera and parameters (R)")) {
    state.reset_requested = true;
  }

  ImGui::End();
}
