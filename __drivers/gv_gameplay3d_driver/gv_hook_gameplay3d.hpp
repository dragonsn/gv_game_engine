#define GP_MAX_GPU_SKIN_BONE_NUMBER 66
#include "gv_hook_gameplay3d_rendering.hpp"

namespace gv
{
enum
{
	e_game_start = 1,
	e_game_play,
	e_game_end,
};
#if !GV_HOOK_TEST

gv_hook_gameplay3d::gv_hook_gameplay3d()
	: _touched(false), _touchX(0)
{
	_tickCount = 0;
	__viewFrustumCulling = true;
	m_hide_terrain = false;
}

gv_hook_gameplay3d::~gv_hook_gameplay3d()
{
}

Mesh* gv_hook_gameplay3d::createCubeMesh(float size)
{
	float a = size * 0.5f;
	float vertices[] = {
		-a, -a, a, 0.0, 0.0, 1.0, 0.0, 0.0, a, -a, a, 0.0, 0.0,
		1.0, 1.0, 0.0, -a, a, a, 0.0, 0.0, 1.0, 0.0, 1.0, a, a,
		a, 0.0, 0.0, 1.0, 1.0, 1.0, -a, a, a, 0.0, 1.0, 0.0, 0.0,
		0.0, a, a, a, 0.0, 1.0, 0.0, 1.0, 0.0, -a, a, -a, 0.0,
		1.0, 0.0, 0.0, 1.0, a, a, -a, 0.0, 1.0, 0.0, 1.0, 1.0, -a,
		a, -a, 0.0, 0.0, -1.0, 0.0, 0.0, a, a, -a, 0.0, 0.0, -1.0,
		1.0, 0.0, -a, -a, -a, 0.0, 0.0, -1.0, 0.0, 1.0, a, -a, -a,
		0.0, 0.0, -1.0, 1.0, 1.0, -a, -a, -a, 0.0, -1.0, 0.0, 0.0, 0.0,
		a, -a, -a, 0.0, -1.0, 0.0, 1.0, 0.0, -a, -a, a, 0.0, -1.0,
		0.0, 0.0, 1.0, a, -a, a, 0.0, -1.0, 0.0, 1.0, 1.0, a, -a,
		a, 1.0, 0.0, 0.0, 0.0, 0.0, a, -a, -a, 1.0, 0.0, 0.0, 1.0,
		0.0, a, a, a, 1.0, 0.0, 0.0, 0.0, 1.0, a, a, -a, 1.0,
		0.0, 0.0, 1.0, 1.0, -a, -a, -a, -1.0, 0.0, 0.0, 0.0, 0.0, -a,
		-a, a, -1.0, 0.0, 0.0, 1.0, 0.0, -a, a, -a, -1.0, 0.0, 0.0,
		0.0, 1.0, -a, a, a, -1.0, 0.0, 0.0, 1.0, 1.0};
	short indices[] = {0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7,
					   8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15,
					   16, 17, 18, 18, 17, 19, 20, 21, 22, 22, 21, 23};
	unsigned int vertexCount = 24;
	unsigned int indexCount = 36;
	VertexFormat::Element elements[] = {
		VertexFormat::Element(VertexFormat::POSITION, 3),
		VertexFormat::Element(VertexFormat::NORMAL, 3),
		VertexFormat::Element(VertexFormat::TEXCOORD0, 2)

	};
	Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 3), vertexCount, false);
	if (mesh == NULL)
	{
		GP_ERROR("Failed to create mesh.");
		return NULL;
	}
	mesh->setVertexData(vertices, 0, vertexCount);
	MeshPart* meshPart =
		mesh->addPart(Mesh::TRIANGLES, Mesh::INDEX16, indexCount, false);
	meshPart->setIndexData(indices, 0, indexCount);
	return mesh;
}

void gv_hook_gameplay3d::initializeDebugCube()
{

	// Create the cube mesh and model.
	Mesh* cubeMesh = createCubeMesh(1);
	Model* cubeModel = Model::create(cubeMesh);
	// Release the mesh because the model now holds a reference to it.
	SAFE_RELEASE(cubeMesh);

	// Create the material for the cube model and assign it to the first mesh
	// part.
	Material* material = cubeModel->setMaterial("res/shaders/textured.vert",
												"res/shaders/textured.frag", 0);

	// These parameters are normally set in a .material file but this example sets
	// them programmatically.
	// Bind the uniform "u_worldViewProjectionMatrix" to use the
	// WORLD_VIEW_PROJECTION_MATRIX from the scene's active _camera and the node
	// that the model belongs to.
	material->setParameterAutoBinding("u_worldViewProjectionMatrix",
									  "WORLD_VIEW_PROJECTION_MATRIX");
	material->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix",
									  "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");

	// Bind the light's direction to the material.
	material->getParameter("u_lightDirection")
		->bindValue<>(_lightNode.ptr(), &Node::getForwardVectorView);
	// Bind the light's color to the material.
	material->getParameter("u_lightColor")
		->setValue(_lightNode->getLight()->getColor());
	// Set the ambient color of the material.
	material->getParameter("u_ambientColor")->setValue(Vector3(1, 1, 1));

	// Load the texture from file.
	Texture::Sampler* sampler = material->getParameter("u_diffuseTexture")
									->setValue("res/png/box-diffuse.png", true);
	sampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);
	material->getStateBlock()->setCullFace(false);
	material->getStateBlock()->setDepthTest(true);
	material->getStateBlock()->setDepthWrite(true);

	_cubeNode = _scene->addNode("cube");
	_cubeNode->setModel(cubeModel);
	_cubeNode->rotateY(MATH_PIOVER4);
	SAFE_RELEASE(cubeModel);
}

void gv_hook_gameplay3d::initializeCommonRenderData()
{
	initializeScene();
}

void gv_hook_gameplay3d::createCamera()
{
	// Create the _camera.
	_camera = Camera::createPerspective(45.0f, getAspectRatio(), 0.1f, 1000.0f);
	_cameraNode = _scene->addNode("_camera");

	// Attach the _camera to a node. This determines the position of the _camera.
	_cameraNode->setCamera(_camera);

	// Make this the active _camera of the scene.
	_scene->setActiveCamera(_camera);
	_camera->release();

	// Move the _camera to look at the origin.
	_cameraNode->translate(0, 1, 5);
	_cameraNode->rotateX(MATH_DEG_TO_RAD(-11.25f));
};
void gv_hook_gameplay3d::createLight()
{
	// Create a white light.
	Light* light = Light::createDirectional(1.0f, 1.0f, 1.0f);
	_lightNode = _scene->addNode("light");
	_lightNode->setLight(light);
	// Release the light because the node now holds a reference to it.
	light->release();
	_lightNode->rotateX(MATH_DEG_TO_RAD(-45.0f));
};
void gv_hook_gameplay3d::initializeScene()
{
	// Create a new empty scene.
	if (getGamepad(0) && getGamepad(0)->isVirtual())
		getGamepad(0)->getForm()->setEnabled(false);

	clearScene();

	_scene = Scene::create();

	createCamera();
	createLight();

#if GV_DEBUG_VERSION
// initializeDebugCube();
#endif
};

void gv_hook_gameplay3d::initialize()
{

	GV_PROFILE_EVENT(gv_gameplay_hook__initialize, 0);
	setMultiTouch(true);

	gv_global::rnd_opt.m_max_debug_line = 10000;
	gv_global::rnd_opt.m_max_debug_tri = 1000;

	gv_debug_renderer_gameplay3d::static_get()->initialize_gameplay3d();
	m_font = gv_debug_renderer_gameplay3d::static_get()->get_debug_font();
	this->initializeCommonRenderData();
	// Create the font for drawing the framerate.
}

void gv_hook_gameplay3d::finalize()
{
	GV_PROFILE_EVENT(gv_gameplay_hook__finalize, 0);
	clearScene();
	gv_debug_renderer_gameplay3d::static_get()->finalize_gameplay3d();
	m_font = NULL;
	this->m_renderable_components.clear();
}

void gv_hook_gameplay3d::clearScene()
{
	SAFE_RELEASE(_scene);
	_modelNode = NULL;
	_cubeNode = NULL;
	_cameraNode = NULL;
	_lightNode = NULL;
	_camera = NULL;
}

void gv_hook_gameplay3d::update(float elapsedTime)
{
	GV_PROFILE_EVENT(gv_gameplay_hook__update, 0);
	// gv_state_machine::tick(elapsedTime/1000.f);
	this->updateBehaviorTree(elapsedTime);
	_tickCount++;
	// gv_string_tmp s=get_state_stack();
	// s<<"[tick]:"<<_tickCount;
	// gv_global::debug_draw->draw_string(*s,gv_vector2i(5,5),gv_color::YELLOW_B()
	// );
	// gv_global::debug_draw->draw_string(*m_debug_string,gv_vector2i(5,25),gv_color::YELLOW_B()
	// );
	// m_debug_string.clear_and_reserve();
}

bool gv_hook_gameplay3d::updateBehaviorTree(float dt)
{
	// sample code to use behavior tree to control gam logic
	GVM_BT_START
	GVM_BT_NODE_SELECT(e_game_start);
	GVM_BT_LEAF(1, false)
	GVM_BT_LEAF(2, false)
	GVM_BT_LEAF(3, goto_state(e_game_play, 0))
	GVM_BT_END_NODE

	GVM_BT_NODE_SELECT(e_game_play);
	GVM_BT_LEAF(1, false)
	GVM_BT_LEAF(2, false)
	GVM_BT_LEAF(3, goto_state(e_game_end, 0))
	GVM_BT_END_NODE

	GVM_BT_NODE_SELECT(e_game_end);
	GVM_BT_LEAF(1, false)
	GVM_BT_LEAF(2, false);
	GVM_BT_LEAF(3, nop())
	GVM_BT_END_NODE
	GVM_BT_END
	return true;
};

static gve_key map_gp_key(int key)
{
	switch (key)
	{
	case Keyboard::KEY_BACKSPACE:
		return e_key_backspace;
	case Keyboard::KEY_TAB:
		return e_key_tab;
	case Keyboard::KEY_RETURN:
		return e_key_return;
	case Keyboard::KEY_KP_ENTER:
		return e_key_return;
	case Keyboard::KEY_ESCAPE:
		return e_key_esc;
	case Keyboard::KEY_SPACE:
		return e_key_space;
	case Keyboard::KEY_ZERO:
		return e_key_0;
	case Keyboard::KEY_ONE:
		return e_key_1;
	case Keyboard::KEY_TWO:
		return e_key_2;
	case Keyboard::KEY_THREE:
		return e_key_3;
	case Keyboard::KEY_FOUR:
		return e_key_4;
	case Keyboard::KEY_FIVE:
		return e_key_5;
	case Keyboard::KEY_SIX:
		return e_key_6;
	case Keyboard::KEY_SEVEN:
		return e_key_7;
	case Keyboard::KEY_EIGHT:
		return e_key_8;
	case Keyboard::KEY_NINE:
		return e_key_9;
	case Keyboard::KEY_CAPITAL_A:
		return e_key_a;
	case Keyboard::KEY_CAPITAL_B:
		return e_key_b;
	case Keyboard::KEY_CAPITAL_C:
		return e_key_c;
	case Keyboard::KEY_CAPITAL_D:
		return e_key_d;
	case Keyboard::KEY_CAPITAL_E:
		return e_key_e;
	case Keyboard::KEY_CAPITAL_F:
		return e_key_f;
	case Keyboard::KEY_CAPITAL_G:
		return e_key_g;
	case Keyboard::KEY_CAPITAL_H:
		return e_key_h;
	case Keyboard::KEY_CAPITAL_I:
		return e_key_i;
	case Keyboard::KEY_CAPITAL_J:
		return e_key_j;
	case Keyboard::KEY_CAPITAL_K:
		return e_key_k;
	case Keyboard::KEY_CAPITAL_L:
		return e_key_l;
	case Keyboard::KEY_CAPITAL_M:
		return e_key_m;
	case Keyboard::KEY_CAPITAL_N:
		return e_key_n;
	case Keyboard::KEY_CAPITAL_O:
		return e_key_o;
	case Keyboard::KEY_CAPITAL_P:
		return e_key_p;
	case Keyboard::KEY_CAPITAL_Q:
		return e_key_q;
	case Keyboard::KEY_CAPITAL_R:
		return e_key_r;
	case Keyboard::KEY_CAPITAL_S:
		return e_key_s;
	case Keyboard::KEY_CAPITAL_T:
		return e_key_t;
	case Keyboard::KEY_CAPITAL_U:
		return e_key_u;
	case Keyboard::KEY_CAPITAL_V:
		return e_key_v;
	case Keyboard::KEY_CAPITAL_W:
		return e_key_w;
	case Keyboard::KEY_CAPITAL_X:
		return e_key_x;
	case Keyboard::KEY_CAPITAL_Y:
		return e_key_y;
	case Keyboard::KEY_CAPITAL_Z:
		return e_key_z;
	case Keyboard::KEY_A:
		return e_key_a;
	case Keyboard::KEY_B:
		return e_key_b;
	case Keyboard::KEY_C:
		return e_key_c;
	case Keyboard::KEY_D:
		return e_key_d;
	case Keyboard::KEY_E:
		return e_key_e;
	case Keyboard::KEY_F:
		return e_key_f;
	case Keyboard::KEY_G:
		return e_key_g;
	case Keyboard::KEY_H:
		return e_key_h;
	case Keyboard::KEY_I:
		return e_key_i;
	case Keyboard::KEY_J:
		return e_key_j;
	case Keyboard::KEY_K:
		return e_key_k;
	case Keyboard::KEY_L:
		return e_key_l;
	case Keyboard::KEY_M:
		return e_key_m;
	case Keyboard::KEY_N:
		return e_key_n;
	case Keyboard::KEY_O:
		return e_key_o;
	case Keyboard::KEY_P:
		return e_key_p;
	case Keyboard::KEY_Q:
		return e_key_q;
	case Keyboard::KEY_R:
		return e_key_r;
	case Keyboard::KEY_S:
		return e_key_s;
	case Keyboard::KEY_T:
		return e_key_t;
	case Keyboard::KEY_U:
		return e_key_u;
	case Keyboard::KEY_V:
		return e_key_v;
	case Keyboard::KEY_W:
		return e_key_w;
	case Keyboard::KEY_X:
		return e_key_x;
	case Keyboard::KEY_Y:
		return e_key_y;
	case Keyboard::KEY_Z:
		return e_key_z;
	}
	return e_key_max;
}

void gv_hook_gameplay3d::keyEvent(Keyboard::KeyEvent evt, int key)
{
	if (evt == Keyboard::KEY_PRESS)
	{
		gv_global::input->set_key_down(map_gp_key(key), true);
	}
	else if (evt == Keyboard::KEY_RELEASE)
	{
		gv_global::input->set_key_down(map_gp_key(key), false);
	}
}

bool gv_hook_gameplay3d::mouseEvent(Mouse::MouseEvent evt, int x, int y,
									int wheelDelta)
{

	switch (evt)
	{
	case Mouse::MOUSE_PRESS_LEFT_BUTTON:
		gv_global::input->set_key_down(e_key_lbutton, true);
		break;

	case Mouse::MOUSE_RELEASE_LEFT_BUTTON:
		gv_global::input->set_key_down(e_key_lbutton, false);
		break;

	case Mouse::MOUSE_PRESS_MIDDLE_BUTTON:
		gv_global::input->set_key_down(e_key_mbutton, true);
		break;

	case Mouse::MOUSE_RELEASE_MIDDLE_BUTTON:
		gv_global::input->set_key_down(e_key_mbutton, false);
		break;

	case Mouse::MOUSE_PRESS_RIGHT_BUTTON:
		gv_global::input->set_key_down(e_key_rbutton, true);
		break;

	case Mouse::MOUSE_RELEASE_RIGHT_BUTTON:
		gv_global::input->set_key_down(e_key_rbutton, false);
		break;

	case Mouse::MOUSE_MOVE:
		gv_global::input->set_mouse_pos(gv_vector2i(x, y));
		break;

	case Mouse::MOUSE_WHEEL:
		gv_global::input->set_mouse_wheel_delta(wheelDelta * 120);
		GVM_DEBUG_OUT("mouse wheel delta event :" << wheelDelta * 120);
		break;
	}
	return false; // pass it to the touch event handler
};

void gv_hook_gameplay3d::touchEvent(Touch::TouchEvent evt, int x, int y,
									unsigned int contactIndex)
{

	switch (evt)
	{
	case Touch::TOUCH_PRESS:
	{
		_touched = true;
		_touchX = x;
	}
	break;
	case Touch::TOUCH_RELEASE:
	{
		_touched = false;
		_touchX = 0;
	}
	break;
	case Touch::TOUCH_MOVE:
	{
		int deltaX = x - _touchX;
		_touchX = x;
	}
	break;
	default:
		break;
	};
}
static gv_string_tmp get_event_flag_string(int eventFlags)
{
	gv_string_tmp s;
	if ((eventFlags & Control::Listener::PRESS) == Control::Listener::PRESS)
	{
		s += "|PRESS";
	}

	if ((eventFlags & Control::Listener::RELEASE) == Control::Listener::RELEASE)
	{
		s += "|RELEASE";
	}

	if ((eventFlags & Control::Listener::CLICK) == Control::Listener::CLICK)
	{
		s += "|CLICK";
	}

	if ((eventFlags & Control::Listener::VALUE_CHANGED) ==
		Control::Listener::VALUE_CHANGED)
	{
		s += "|VALUE_CHANGED";
	}

	if ((eventFlags & Control::Listener::ENTER) == Control::Listener::ENTER)
	{
		s += "|ENTER";
	}

	if ((eventFlags & Control::Listener::LEAVE) == Control::Listener::LEAVE)
	{
		s += "|LEAVE";
	}
	return s;
}

void gv_hook_gameplay3d::controlEvent(Control* control, int evt)
{
	GVM_DEBUG_OUT("[ui][event] source=" << control->getId() << "  type="
										<< get_event_flag_string(evt));
};

void gv_hook_gameplay3d::showMsgBox(gv_string text, bool show_cancel,
									bool show_ok, gv_float duration,
									gv_int priority){

};

void gv_hook_gameplay3d::loadPage(gv_string address, gv_string id)
{
	Form* form = Form::create(*address);
	GV_ASSERT(form);
	_formTable.add(id, form);
	form->setEnabled(false);
};

void gv_hook_gameplay3d::showPage(gv_string id, gv_string effect_id)
{
	Form* f = _formTable[id];
	GV_ASSERT(f);
	f->setEnabled(true);
};

void gv_hook_gameplay3d::hidePage(gv_string id, gv_string effect_id)
{
	Form* f = _formTable[id];
	GV_ASSERT(f);
	f->setEnabled(false);
};

Form* gv_hook_gameplay3d::getPage(gv_string id)
{
	Form* f = _formTable[id];
	return f;
};

void gv_hook_gameplay3d::hideAllPage()
{
	gvt_hash_map< gv_string, Form* >::iterator it;
	it = _formTable.begin();
	while (it != _formTable.end())
	{
		if (!it.is_empty())
		{
			Form* f = (*it);
			f->setEnabled(false);
		}
		++it;
	}
};

#else
#include "gv_hook_gameplay3d_test.hpp"
#endif
}