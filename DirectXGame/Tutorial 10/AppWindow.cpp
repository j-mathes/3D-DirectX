#include "AppWindow.h"
#include <Windows.h>
#include "Vector3D.h"
#include "Matrix4x4.h"

struct vertex
{
	Vector3D position;
	Vector3D position1;
	Vector3D color;
	Vector3D color1;
};

__declspec(align(16))
struct constant
{
	Matrix4x4 m_world;
	Matrix4x4 m_view;
	Matrix4x4 m_proj;
	unsigned int m_time;
};

void AppWindow::updateQuadPosition()
{
	constant cc;
	cc.m_time = ::GetTickCount();

	cc.m_world.setTranslation(Vector3D(0, 0, 0));
	cc.m_view.setIdentity();
	cc.m_proj.setOrthoLH(
		(this->getClientWindowRect().right - this->getClientWindowRect().left) / 400.0f,
		(this->getClientWindowRect().bottom - this->getClientWindowRect().top) / 400.0f,
		-4.0f,
		4.0f
		);

	m_cb->update(GraphicsEngine::get()->getImmediateDeviceContext(), &cc);
}

void AppWindow::onCreate()
{
	Window::onCreate();
	GraphicsEngine::get()->init();
	m_swap_chain = GraphicsEngine::get()->createSwapChain();

	RECT rc = this->getClientWindowRect();
	m_swap_chain->init(this->m_hwnd,rc.right-rc.left, rc.bottom-rc.top);

	vertex list[] =
	{
		//X - Y - Z
		{Vector3D (-0.5f,-0.5f,0.0f),	Vector3D (-0.32f,-0.11f,0.0f),	Vector3D (1,0,0),	Vector3D (0,0,1)},	// POS1
		{Vector3D (-0.5f,0.5f,0.0f),	Vector3D (-0.11f,0.78f,0.0f),	Vector3D (0,1,0),	Vector3D (1,1,1)},	// POS2
		{Vector3D (0.5f,-0.5f,0.0f),	Vector3D (0.75f,-0.73f,0.0f),	Vector3D (0,0,1),	Vector3D (1,0,1)},	// POS3
		{Vector3D (0.5f,0.5f,0.0f),		Vector3D (0.88f,0.77f,0.0f),	Vector3D (1,1,0),	Vector3D (0,1,1)}	// POS4
	};

	m_vb = GraphicsEngine::get()->createVertexBuffer();
	UINT size_list = ARRAYSIZE(list);

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);

	m_vs = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);
	m_vb->load(list, sizeof(vertex), size_list, shader_byte_code, size_shader);
	GraphicsEngine::get()->releaseCompiledShader();

	GraphicsEngine::get()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps = GraphicsEngine::get()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->releaseCompiledShader();

	constant cc;
	cc.m_time = 0;
	m_cb = GraphicsEngine::get()->createConstantBuffer();
	m_cb->load(&cc, sizeof(constant));
}

void AppWindow::onUpdate()
{
	Window::onUpdate();

	// Clear the render target
	GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain, 0, 0.3f, 0.4f, 1);
	
	// Set the viewport of the render target
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	updateQuadPosition();

	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_vs, m_cb);
	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_ps, m_cb);

	// Set the default shader in the graphics pipeline to be able to draw
	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getImmediateDeviceContext()->setPixelShader(m_ps);
	
	// Set the vertices
	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffer(m_vb);
	
	// Draw
	GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleStrip(m_vb->getSizeVertexList(), 0);
	m_swap_chain->present(true);
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	m_vb->release();
	m_swap_chain->release();
	m_vs->release();
	m_ps->release();
	GraphicsEngine::get()->release();
}