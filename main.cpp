
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#pragma comment(lib,"../API/GLFW/glfw3.lib")
//#pragma comment(lib,"../API/GLEW/glew32s.lib")
//#pragma comment(lib,"Opengl32.lib")
#include <iostream>
#include <string>


#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GUI.h"


#include <Windows.h>


void SetWindowColors(HWND hwnd, COLORREF titleBarColor, COLORREF borderColor)
{
	HBRUSH  titleBarColormm =CreateSolidBrush(titleBarColor);
    // 设置标题栏颜色
    SetClassLongA(hwnd, -10,(LONG64)titleBarColormm);
    // 设置边框颜色
    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, borderColor, 0, LWA_COLORKEY);
}


GLFWwindow* Windows;
// 主函数
int main()
{
	// 初始化 GLFW
	glfwInit();

	// 设置 GLFW 窗口提示的 OpenGL 版本和配置文件
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 创建 GLFW 窗口
	Windows = glfwCreateWindow(900, 800, "窗口", NULL, NULL);

HWND hwnd = GetForegroundWindow();


        // 在创建窗口时设置标题栏和边框颜色
        SetWindowColors(hwnd, RGB(100, 100, 255), RGB(50, 50, 127));

	// 将窗口的上下文设置为当前上下文
	glfwMakeContextCurrent(Windows);

	// 禁用垂直同步
	glfwSwapInterval(0);

	// 检查 ImGui 版本
	IMGUI_CHECKVERSION();

	// 创建 ImGui 上下文
	ImGui::CreateContext(NULL);
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// 添加中文字体到 ImGui
	io.Fonts->AddFontFromFileTTF("kaiu.ttf", 18, NULL, io.Fonts->GetGlyphRangesChineseFull());

	// 配置 ImGui 的标志以启用停靠和视口
io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // 启用停靠支持
io.ConfigFlags |= ImGuiViewportFlags_NoDecoration;  // 不显示视口装饰，如标题栏和窗口边框
io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // 启用视口支持，允许创建多个窗口和工作区
io.ConfigFlags |= ImGuiCol_DockingEmptyBg;           // 指定停靠空间为空时的背景颜色

	// 设置 ImGui 的暗黑样式
	ImGui::MyStyleColorsLight();

	// 初始化用于 GLFW 和 OpenGL 的 ImGui
	ImGui_ImplGlfw_InitForOpenGL(Windows, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// 创建一个 TextEditor 并将其语言定义设置为 C
    te = new TextEditor();
	te->SetLanguageDefinition(TextEditor::LanguageDefinition::C());

	// 主循环
	while (!glfwWindowShouldClose(Windows))
	{
		// 清空 OpenGL 颜色缓冲区
		glClear(GL_COLOR_BUFFER_BIT);
		// 开始新的 ImGui 帧
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport();

		// 调用绘制 GUI 的函数
		DrawGUI();

		// 显示 ImGui 演示窗口
		ImGui::ShowDemoWindow();

		// 渲染 ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// 如果启用了 ImGui 视口，则更新和渲染其他 ImGui 平台窗口
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		// 交换 GLFW 缓冲区并轮询事件
		glfwSwapBuffers(Windows);
		glfwPollEvents();
	}
	return 0;
}
