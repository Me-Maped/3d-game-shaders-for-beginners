#include "imgui_host.h"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

#include "imgui.h"

#include "graphicsWindow.h"
#include "geomNode.h"
#include "geomTriangles.h"
#include "geomVertexData.h"
#include "geomVertexArrayFormat.h"
#include "geomVertexFormat.h"
#include "geomVertexArrayData.h"
#include "geomEnums.h"
#include "internalName.h"
#include "renderState.h"
#include "colorAttrib.h"
#include "colorBlendAttrib.h"
#include "cullFaceAttrib.h"
#include "scissorAttrib.h"
#include "textureAttrib.h"
#include "transparencyAttrib.h"
#include "shader.h"
#include "texture.h"
#include "samplerState.h"

namespace {

struct FontCandidate {
  const char* path;
  int face;
};

const FontCandidate kFonts[] = {
    {"/usr/share/fonts/wenquanyi/wqy-zenhei/wqy-zenhei.ttc", 0}
  , {"/usr/share/fonts/noto-cjk/NotoSansCJK-Regular.ttc", 2}
  , {"/usr/share/fonts/TTF/JetBrains-Maple-Mono-NF-XX-XX/JetBrainsMapleMono-Regular.ttf", 0}
};

void set_mouse_button(bool down, bool& previous, int button) {
  if (down == previous) { return; }
  ImGui::GetIO().AddMouseButtonEvent(button, down);
  previous = down;
}

}

ImGuiHost::ImGuiHost(GraphicsWindow* window, NodePath render2d) {
  IM_UNUSED(window);

  root_ = render2d.attach_new_node("imgui-root", 0);
  root_.set_bin("unsorted", 1000);
  root_.set_depth_test(false);
  root_.set_depth_write(false);
  root_.set_two_sided(true);
  root_.set_transparency(TransparencyAttrib::M_alpha);
  root_.set_attrib(ColorAttrib::make_vertex());
  root_.set_attrib(CullFaceAttrib::make(CullFaceAttrib::M_cull_none));
  root_.set_attrib
    ( ColorBlendAttrib::make
        ( ColorBlendAttrib::M_add
        , ColorBlendAttrib::O_incoming_alpha
        , ColorBlendAttrib::O_one_minus_incoming_alpha
        )
    );

  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.IniFilename = nullptr;
  io.LogFilename = nullptr;
  // Sliders are mouse-driven. Leave the keyboard to the demo shortcuts.
  io.ConfigFlags |= ImGuiConfigFlags_NoKeyboard;
  io.ConfigNavCaptureKeyboard = false;

  ImGui::StyleColorsDark();
  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowRounding = 6.0f;
  style.FrameRounding = 4.0f;
  style.GrabRounding = 4.0f;
  style.ScrollbarRounding = 4.0f;
  style.WindowBorderSize = 1.0f;
  style.Colors[ImGuiCol_WindowBg].w = 0.90f;

  setup_font();
  setup_geom();
  setup_shader();
}

ImGuiHost::~ImGuiHost() {
  if (ImGui::GetCurrentContext() != nullptr) {
    ImGui::DestroyContext();
  }
  if (!root_.is_empty()) {
    root_.remove_node();
  }
}

void ImGuiHost::setup_font() {
  ImGuiIO& io = ImGui::GetIO();
  ImFontConfig config;
  config.OversampleH = 1;
  config.OversampleV = 1;
  config.PixelSnapH = true;

  ImFont* font = nullptr;
  for (const FontCandidate& candidate : kFonts) {
    config.FontNo = candidate.face;
    font = io.Fonts->AddFontFromFileTTF
      ( candidate.path
      , 18.0f
      , &config
      , io.Fonts->GetGlyphRangesChineseSimplifiedCommon()
      );
    if (font != nullptr) {
      cjk_font_ = true;
      std::cout << "ImGui font: " << candidate.path << "\n";
      break;
    }
  }

  if (font == nullptr) {
    io.Fonts->AddFontDefault();
    cjk_font_ = false;
    std::cout
      << "ImGui font: default (no CJK font found, panel falls back to English)\n";
  }

  unsigned char* pixels = nullptr;
  int width = 0;
  int height = 0;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

  font_texture_ = new Texture("imgui-font");
  font_texture_->setup_2d_texture
    ( width
    , height
    , Texture::T_unsigned_byte
    , Texture::F_rgba
    );
  font_texture_->set_minfilter(SamplerState::FT_linear);
  font_texture_->set_magfilter(SamplerState::FT_linear);
  font_texture_->set_wrap_u(SamplerState::WM_clamp);
  font_texture_->set_wrap_v(SamplerState::WM_clamp);

  PTA_uchar image = font_texture_->make_ram_image();
  const size_t bytes = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
  if (image.size() < bytes) {
    std::cerr << "ImGui font texture is smaller than the atlas.\n";
    return;
  }
  std::memcpy(image.p(), pixels, bytes);
  io.Fonts->SetTexID(static_cast<ImTextureID>(reinterpret_cast<uintptr_t>(font_texture_.p())));
}

void ImGuiHost::setup_geom() {
  PT(GeomVertexArrayFormat) array_format = new GeomVertexArrayFormat
    ( InternalName::get_vertex()
    , 4
    , GeomEnums::NT_stdfloat
    , GeomEnums::C_point
    , InternalName::get_color()
    , 1
    , GeomEnums::NT_packed_dabc
    , GeomEnums::C_color
    );
  vformat_ = GeomVertexFormat::register_format(new GeomVertexFormat(array_format));
}

void ImGuiHost::setup_shader() {
  PT(Shader) shader = Shader::load
    ( Shader::SL_GLSL
    , "shaders/vertex/imgui.vert"
    , "shaders/fragment/imgui.frag"
    );
  if (shader == nullptr) {
    std::cerr << "Failed to load shaders/vertex/imgui.vert\n";
    return;
  }
  root_.set_shader(shader);
  root_.set_shader_input("displaySize", LVecBase2f(1.0f, 1.0f));
}

void ImGuiHost::begin
  ( double dt
  , int display_w
  , int display_h
  , float mouse_x
  , float mouse_y
  , bool mouse_valid
  , bool left
  , bool right
  , bool middle
  , float wheel
  ) {
  display_w_ = display_w > 0 ? display_w : 1;
  display_h_ = display_h > 0 ? display_h : 1;

  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize = ImVec2
    ( static_cast<float>(display_w_)
    , static_cast<float>(display_h_)
    );
  io.DeltaTime = dt > 0.0 ? static_cast<float>(dt) : (1.0f / 60.0f);

  if (mouse_valid) {
    io.AddMousePosEvent(mouse_x, mouse_y);
  } else {
    io.AddMousePosEvent(-10000.0f, -10000.0f);
  }
  set_mouse_button(left, button_down_[0], 0);
  set_mouse_button(right, button_down_[1], 1);
  set_mouse_button(middle, button_down_[2], 2);
  if (wheel != 0.0f) {
    io.AddMouseWheelEvent(0.0f, wheel);
  }

  ImGui::NewFrame();
}

void ImGuiHost::end() {
  ImGui::Render();
  root_.set_shader_input
    ( "displaySize"
    , LVecBase2f
        ( static_cast<float>(display_w_)
        , static_cast<float>(display_h_)
        )
    );
  upload_draw_data();
}

bool ImGuiHost::want_capture_mouse() const {
  return ImGui::GetIO().WantCaptureMouse;
}

bool ImGuiHost::want_capture_keyboard() const {
  return ImGui::GetIO().WantCaptureKeyboard;
}

bool ImGuiHost::has_cjk_font() const {
  return cjk_font_;
}

NodePath ImGuiHost::create_geomnode(const GeomVertexData* vdata) {
  PT(GeomTriangles) prim = new GeomTriangles(GeomEnums::UH_stream);
  prim->set_index_type(GeomEnums::NT_uint32);
  prim->close_primitive();

  PT(Geom) geom = new Geom(vdata);
  geom->add_primitive(prim);

  PT(GeomNode) geom_node = new GeomNode("imgui-geom");
  geom_node->add_geom(geom, RenderState::make_empty());
  return NodePath(geom_node);
}

void ImGuiHost::upload_draw_data() {
  ImDrawData* draw_data = ImGui::GetDrawData();
  if (draw_data == nullptr || !draw_data->Valid) { return; }

  NodePathCollection children = root_.get_children();
  for (int i = 0; i < children.get_num_paths(); ++i) {
    children.get_path(i).detach_node();
  }

  const float fb_width = draw_data->DisplaySize.x * draw_data->FramebufferScale.x;
  const float fb_height = draw_data->DisplaySize.y * draw_data->FramebufferScale.y;
  if (fb_width <= 0.0f || fb_height <= 0.0f) { return; }

  const ImVec2 clip_off = draw_data->DisplayPos;
  const ImVec2 clip_scale = draw_data->FramebufferScale;

  for (int list_index = 0; list_index < draw_data->CmdListsCount; ++list_index) {
    const ImDrawList* cmd_list = draw_data->CmdLists[list_index];
    if (cmd_list->VtxBuffer.Size <= 0) { continue; }

    if (list_index >= static_cast<int>(geom_data_.size())) {
      geom_data_.push_back
        ( { new GeomVertexData
              ( "imgui-vertex"
              , vformat_
              , GeomEnums::UH_stream
              )
          , {}
          }
        );
    }
    GeomList& geom_list = geom_data_[list_index];

    PT(GeomVertexArrayDataHandle) vertex_handle =
      geom_list.vdata->modify_array_handle(0);
    if (vertex_handle->get_num_rows() < cmd_list->VtxBuffer.Size) {
      vertex_handle->unclean_set_num_rows(cmd_list->VtxBuffer.Size);
    }
    std::memcpy
      ( vertex_handle->get_write_pointer()
      , cmd_list->VtxBuffer.Data
      , static_cast<size_t>(cmd_list->VtxBuffer.Size) * sizeof(ImDrawVert)
      );
    // ImGui packs RGBA. Panda's NT_packed_dabc is DirectX ARGB, which is BGRA in memory.
    auto* vertices = reinterpret_cast<ImDrawVert*>(vertex_handle->get_write_pointer());
    for (int vertex = 0; vertex < cmd_list->VtxBuffer.Size; ++vertex) {
      const uint32_t color = vertices[vertex].col;
      vertices[vertex].col =
          (color & 0xff00ff00u)
        | ((color & 0x00ff0000u) >> 16)
        | ((color & 0x000000ffu) << 16);
    }

    int command_slot = 0;
    for (int cmd_index = 0; cmd_index < cmd_list->CmdBuffer.Size; ++cmd_index) {
      const ImDrawCmd* command = &cmd_list->CmdBuffer[cmd_index];
      if (command->UserCallback != nullptr) { continue; }
      if (command->ElemCount == 0) { continue; }

      const ImVec2 clip_min
        ( (command->ClipRect.x - clip_off.x) * clip_scale.x
        , (command->ClipRect.y - clip_off.y) * clip_scale.y
        );
      const ImVec2 clip_max
        ( (command->ClipRect.z - clip_off.x) * clip_scale.x
        , (command->ClipRect.w - clip_off.y) * clip_scale.y
        );
      if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y) { continue; }

      if (command_slot >= static_cast<int>(geom_list.nodepaths.size())) {
        geom_list.nodepaths.push_back(create_geomnode(geom_list.vdata));
      }
      NodePath node_path = geom_list.nodepaths[command_slot];
      ++command_slot;
      node_path.reparent_to(root_);

      PT(GeomNode) geom_node = DCAST(GeomNode, node_path.node());
      const int elem_count = static_cast<int>(command->ElemCount);
      PT(GeomVertexArrayDataHandle) index_handle =
        geom_node
          ->modify_geom(0)
          ->modify_primitive(0)
          ->modify_vertices(elem_count)
          ->modify_handle();
      if (index_handle->get_num_rows() < elem_count) {
        index_handle->unclean_set_num_rows(elem_count);
      }

      auto* indices = reinterpret_cast<uint32_t*>(index_handle->get_write_pointer());
      const ImDrawIdx* source = cmd_list->IdxBuffer.Data + command->IdxOffset;
      for (int element = 0; element < elem_count; ++element) {
        indices[element] =
          static_cast<uint32_t>(source[element]) + command->VtxOffset;
      }

      CPT(RenderState) state = RenderState::make
        ( ScissorAttrib::make
            ( clip_min.x / fb_width
            , clip_max.x / fb_width
            , 1.0f - clip_max.y / fb_height
            , 1.0f - clip_min.y / fb_height
            )
        );
      if (command->GetTexID() != 0) {
        Texture* texture = reinterpret_cast<Texture*>(static_cast<uintptr_t>(command->GetTexID()));
        state = state->add_attrib(TextureAttrib::make(texture));
      }
      geom_node->set_geom_state(0, state);
    }
  }
}
