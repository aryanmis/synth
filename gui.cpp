#include "gui.h"

#include <vector>
#include <algorithm> // std::clamp
#include <cstdio>

// Dear ImGui
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

static void glfwErrorCallback(int error, const char* desc) {
    std::fprintf(stderr, "GLFW error %d: %s\n", error, desc);
}

bool runGui(Synth& engine) {
    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit()) {
        std::fprintf(stderr, "glfwInit() failed\n");
        return false;
    }

#ifdef __APPLE__
    // macOS requires a forward-compatible core profile for reliable context creation.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    const char* glsl_version = "#version 150";
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    const char* glsl_version = "#version 130";
#endif

    GLFWwindow* window = glfwCreateWindow(800, 500, "Minimal Synth GUI", nullptr, nullptr);
    if (!window) {
        std::fprintf(stderr, "glfwCreateWindow() failed\n");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        std::fprintf(stderr, "ImGui_ImplGlfw_InitForOpenGL() failed\n");
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }

    if (!ImGui_ImplOpenGL3_Init(glsl_version)) {
        std::fprintf(stderr, "ImGui_ImplOpenGL3_Init() failed\n");
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }

    // GUI-side shadow list for selection + per-osc freq UI
    std::vector<float> guiOscFreqs;
    guiOscFreqs.push_back(engine.masterPitchHz.load());

    int selectedOsc = 0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Synth Controls");

        // Master pitch
        float pitch = engine.masterPitchHz.load();
        if (ImGui::SliderFloat("Master Pitch (Hz)", &pitch, 50.0f, 2000.0f, "%.1f")) {
            engine.masterPitchHz.store(pitch);
        }

        // LFO rate
        float lfo = engine.lfoRateHz.load();
        if (ImGui::SliderFloat("LFO Rate (Hz)", &lfo, 0.0f, 20.0f, "%.2f")) {
            engine.lfoRateHz.store(lfo);
        }

        // LFO depth (if present in Synth.h)
        float depth = engine.lfoDepthHz.load();
        if (ImGui::SliderFloat("LFO Depth (Hz)", &depth, 0.0f, 50.0f, "%.1f")) {
            engine.lfoDepthHz.store(depth);
        }

        ImGui::Separator();
        ImGui::Text("Oscillators");

        if (ImGui::Button("Add Oscillator")) {
            SynthCmd c{};
            c.type = SynthCmd::AddOsc;
            c.index = -1;
            c.value = 0.0f;

            if (engine.cmdQ.push(c)) {
                guiOscFreqs.push_back(pitch);
                selectedOsc = (int)guiOscFreqs.size() - 1;
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Delete Selected")) {
            if (!guiOscFreqs.empty() && selectedOsc >= 0 && selectedOsc < (int)guiOscFreqs.size()) {
                SynthCmd c{};
                c.type = SynthCmd::RemoveOsc;
                c.index = selectedOsc;
                c.value = 0.0f;

                if (engine.cmdQ.push(c)) {
                    guiOscFreqs.erase(guiOscFreqs.begin() + selectedOsc);
                    if (guiOscFreqs.empty()) selectedOsc = 0;
                    else selectedOsc = std::clamp(selectedOsc, 0, (int)guiOscFreqs.size() - 1);
                }
            }
        }

        ImGui::BeginChild("osc_list", ImVec2(250, 250), true);
        for (int i = 0; i < (int)guiOscFreqs.size(); ++i) {
            char label[64];
            std::snprintf(label, sizeof(label), "Osc %d", i);
            if (ImGui::Selectable(label, selectedOsc == i)) selectedOsc = i;
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginGroup();
        ImGui::Text("Selected Osc: %d", selectedOsc);

        if (!guiOscFreqs.empty() && selectedOsc >= 0 && selectedOsc < (int)guiOscFreqs.size()) {
            float f = guiOscFreqs[selectedOsc];
            if (ImGui::SliderFloat("Osc Freq (Hz)", &f, 20.0f, 5000.0f, "%.1f")) {
                guiOscFreqs[selectedOsc] = f;

                SynthCmd c{};
                c.type = SynthCmd::SetOscFreq;
                c.index = selectedOsc;
                c.value = f;
                engine.cmdQ.push(c);
            }
        } else {
            ImGui::TextDisabled("No oscillator selected.");
        }

        ImGui::EndGroup();
        ImGui::End();

        ImGui::Render();
        int display_w = 0, display_h = 0;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return true;
}
