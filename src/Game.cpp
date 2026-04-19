#include "Game.h"
#include "raylib.h"
#include "Renderer.h"
#include "ResourceManager.h"

#include "imgui/imgui.h"
#include  "imgui/rlImGui.h"

void Game::init() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1700, 900, "3d_game");
    SetTraceLogLevel(LOG_WARNING);
    // ToggleBorderlessWindowed();

    SetTargetFPS(300);
    HideCursor();
    setup_imgui();
    SetExitKey(KEY_NULL);
    ResourceManager::Get().init();
    m_renderer.update_meshes(m_player.m_camera.position);
}

void Game::main_loop() {
    m_renderer.update_meshes(m_player.m_camera.position);
    m_world.generate_world(m_player.m_camera.position);
    if (IsKeyPressed(KEY_C)) m_enable_player_collision = !m_enable_player_collision;
    if (IsKeyPressed(KEY_F5)) m_is_debug_menu = !m_is_debug_menu;
    if (IsKeyPressed(KEY_ESCAPE)){
        m_is_escape_menu = !m_is_escape_menu;
        if (!m_is_escape_menu) DisableCursor(); else EnableCursor();
    }
    if (!m_is_escape_menu){
        m_player.update_physics(m_enable_player_collision);
        m_player.update_position();
    }

    RayResult ray = m_player.handle_interaction();
    BeginDrawing();
    ClearBackground(SKYBLUE);
    BeginMode3D(m_player.m_camera);
        m_renderer.render_chunks(m_player.m_camera.position);
        if (ray.did_hit) DrawCubeWiresV({ray.hit_block.x + 0.5f,ray.hit_block.y + 0.5f,ray.hit_block.z + 0.5f}, {1.0f, 1.0f, 1.0f}, WHITE);
    EndMode3D();
    run_imgui();

    DrawTexture(ResourceManager::Get().CROSSHAIR_TEXTURE, GetScreenWidth() / 2 - 3, GetScreenHeight() / 2 - 3, WHITE);
    EndDrawing();
}

void Game::setup_imgui(){
    rlImGuiSetup(true);
    set_minecraft_style();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
}



void Game::run_imgui(){
    rlImGuiBegin();
    debug_menu();
    escape_menu();
    rlImGuiEnd();
}
void Game::debug_menu(){
    if (!m_is_debug_menu) return;;
    ImGuiWindowFlags window_flags;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoBackground;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_Always);
    bool open = true;
    ImGui::Begin("Debug", &open, window_flags);
    ImGui::Text("FPS: %d", GetFPS());

    ImGui::Text("x: %f y: %f z: %f", m_player.m_camera.position.x, m_player.m_camera.position.y, m_player.m_camera.position.z);
    ImGui::Text("Block in inventory: %s", World::BLOCK_MATERIALS::match_material(m_player.m_block_in_inventory).c_str());

    ImGui::End();
}

void Game::escape_menu(){
    if (!m_is_escape_menu) return;
    ImGuiStyle& style = ImGui::GetStyle();
    style.FontScaleMain = 2;

    ImGuiWindowFlags window_flags;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoBackground;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    ImGui::SetNextWindowPos(ImVec2(GetScreenWidth() / 2 - 400 , GetScreenHeight() / 2 - 350), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(800, 700), ImGuiCond_Always);
    bool open = true;
    ImGui::Begin("Escape", &open, window_flags);
    if (ImGui::Button("Quit", ImVec2{70, 30})){
        m_window_should_close = true;
    }

    ImGui::End();
    style.FontScaleMain = 1;
}
void Game::set_minecraft_style() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // 1. The Geometry (Blocky, No Curves)
    style.WindowRounding    = 0.0f;
    style.FrameRounding     = 0.0f;
    style.PopupRounding     = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabRounding      = 0.0f;
    style.TabRounding       = 0.0f;

    // 2. Thick Borders (Simulating block edges)
    // style.WindowBorderSize  = 2.0f;
    // style.FrameBorderSize   = 2.0f;
    style.PopupBorderSize   = 2.0f;
    style.ItemSpacing       = ImVec2(8.0f, 8.0f);
    // 3. The Color Palette

    // Window Background: Dark translucent gray (like the player inventory)
    colors[ImGuiCol_WindowBg]         = ImVec4(0.13f, 0.13f, 0.13f, 0.90f);
    colors[ImGuiCol_Border]           = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_BorderShadow]     = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Text: Pure white
    colors[ImGuiCol_Text]             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);

    // Buttons: Stone Grey
    colors[ImGuiCol_Button]           = ImVec4(0.55f, 0.55f, 0.55f, 1.00f); // Standard Stone
    colors[ImGuiCol_ButtonHovered]    = ImVec4(0.78f, 0.78f, 0.78f, 1.00f); // Lighter Stone / Highlight
    colors[ImGuiCol_ButtonActive]     = ImVec4(0.33f, 0.33f, 0.33f, 1.00f); // Dark Stone (Clicked)

    // Frames (Inputs, Checkboxes, Sliders backgrounds)
    colors[ImGuiCol_FrameBg]          = ImVec4(0.13f, 0.13f, 0.13f, 1.00f); // Dark inner slot
    colors[ImGuiCol_FrameBgHovered]   = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_FrameBgActive]    = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);

    // Window Title Bars
    colors[ImGuiCol_TitleBg]          = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_TitleBgActive]    = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);

    // Active Highlights (Classic Minecraft Green & Yellow)
    colors[ImGuiCol_SliderGrab]       = ImVec4(1.00f, 1.00f, 0.33f, 1.00f); // Yellow
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.33f, 1.00f, 0.33f, 1.00f); // Green
    colors[ImGuiCol_CheckMark]        = ImVec4(0.33f, 1.00f, 0.33f, 1.00f); // Green

    // Scrollbars
    colors[ImGuiCol_ScrollbarBg]      = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]    = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
}