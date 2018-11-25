#pragma once

#define GV_HOOK_TEST 0

namespace gv
{
/**
* Sample game for rendering a scene with a model/mesh.
*/
class gv_hook_gameplay3d : public Game, public gv_state_machine
{
	friend class gv_renderer_gameplay3d;

public:
	gv_hook_gameplay3d();
	virtual ~gv_hook_gameplay3d();

protected:
	virtual void keyEvent(Keyboard::KeyEvent evt, int key);
	virtual void touchEvent(Touch::TouchEvent evt, int x, int y,
							unsigned int contactIndex);
	virtual bool mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta);
	virtual bool updateBehaviorTree(float dt);
	virtual void drawScene();
	virtual void initializeScene();
	virtual void clearScene();
	virtual void controlEvent(Control* control, int);
	virtual void createCamera();
	virtual void createLight();

public:
	void showMsgBox(gv_string text, bool show_cancel = false, bool show_ok = true,
					gv_float duration = 0, gv_int priority = 5);
	void loadPage(gv_string address, gv_string id);
	void showPage(gv_string id, gv_string effect_id = "");
	void hidePage(gv_string id, gv_string effect_id = "");
	Form* getPage(gv_string id);
	void hideAllPage();
	void reloadShaderCache();

private:
	virtual void render(float elapsedTime);
	virtual void initialize();
	virtual void finalize();
	virtual void update(float elapsedTime);
	void initializeCommonRenderData();
	bool drawSceneNode(Node* node);
	void initializeDebugCube();
	void addRenderable(gv_component* component);
	void removeRenderable(gv_component* component);
	void updateCamera(gv_com_camera* camera);
	void drawFrameRate(Font* font, const Vector4& color, unsigned int x,
					   unsigned int y, unsigned int fps);
	void drawSplash(void* param);
	Model* createGridModel(unsigned int lineCount = 41);
	Mesh* createCubeMesh(float size = 1.0f);

public:
	bool _touched;
	int _touchX;
	gvt_ptr< Font > m_font;
	gvt_ptr< Scene > _scene;
	gvt_ptr< Node > _modelNode;
	gvt_ptr< Node > _cubeNode;
	gvt_ptr< Node > _cameraNode;
	gvt_ptr< Node > _lightNode;
	gvt_ptr< Node > _terrain;
	gvt_ptr< Node > _sky;
	gvt_ptr< Camera > _camera;
	gv_int _tickCount;
	gvt_hash_map< gv_string, Form* > _formTable;
	gvt_hash_map< gv_text, Material* > _materialTable;
	gvt_array< gvt_ref_ptr< gv_component > > m_renderable_components;
	bool m_hide_terrain;
	struct MsgBoxInfo
	{
		bool operator<(const MsgBoxInfo& o) const
		{
			return priority < o.priority;
		}
		gv_string s;
		gv_int flags;
		gv_int priority;
		gv_float duration;
	};
	gvt_array< MsgBoxInfo > _msgBoxes;
	enum RenderQueue
	{
		QUEUE_TERRAIN = 0,
		QUEUE_OPAQUE,
		QUEUE_TRANSPARENT,
		QUEUE_COUNT
	};
	bool __viewFrustumCulling;
	std::vector< Node* > _renderQueues[QUEUE_COUNT];
	gv_string m_debug_string;
	gvt_ptr< gv_com_camera > m_current_camera;

private:
	Mesh* precacheStaticMesh(gv_static_mesh* mesh);
	Effect* precacheEffect(gv_effect* effect);
	bool precacheMaterial(gv_material* mat_gv, Material*& mat);
	bool updateMaterials(gv_com_graphic* com, Model* model);
	bool updateMaterialParam(gv_material* mat_gv, Material* mat);
	gv_string_tmp getCachedTextureName(gv_texture* tex);
	gv_string_tmp getCachedTextureName(const char* file_name);
	void precacheTerrain(class gv_com_terrain_roam* terrain);
	bool buildRenderQueues(Node* node);
};
};
