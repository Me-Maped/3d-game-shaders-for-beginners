/*
  Minimal Dear ImGui host for this Panda3D demo.

  ImGui is drawn as ordinary Panda geometry on render2d, so it does not
  fight the engine's OpenGL state. Call begin() after sampling input and
  end() after submitting widgets.
*/

#ifndef IMGUI_HOST_H
#define IMGUI_HOST_H

#include <vector>

#include "nodePath.h"

class GraphicsWindow;
class Texture;
class GeomVertexData;
class GeomVertexFormat;

class ImGuiHost {
  public:
    ImGuiHost(GraphicsWindow* window, NodePath render2d);
    ~ImGuiHost();

    ImGuiHost(const ImGuiHost&) = delete;
    ImGuiHost& operator=(const ImGuiHost&) = delete;

    void begin
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
      );
    void end();

    bool want_capture_mouse() const;
    bool want_capture_keyboard() const;
    bool has_cjk_font() const;

  private:
    void setup_font();
    void setup_geom();
    void setup_shader();
    NodePath create_geomnode(const GeomVertexData* vdata);
    void upload_draw_data();

    struct GeomList {
      PT(GeomVertexData) vdata;
      std::vector<NodePath> nodepaths;
    };

    bool cjk_font_     = false;
    bool button_down_[3] = {false, false, false};
    int display_w_     = 1;
    int display_h_     = 1;

    NodePath root_;
    PT(Texture) font_texture_;
    CPT(GeomVertexFormat) vformat_;
    std::vector<GeomList> geom_data_;
};

#endif
