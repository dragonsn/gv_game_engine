//============================================================================================
//								:
//============================================================================================

#include "gv_framework\inc\gv_framework_events.h"
#include "gv_framework\renderer\gv_shader.h"
#include "gv_framework\renderer\gv_effect.h"
#include "gv_framework\renderer\gv_material.h"
#include "__drivers\gv_cegui\gv_cegui_config.h"
#include "2d_bricks_maps.h"

#pragma warning(disable : 4566)
namespace unit_test_ui_cegui
{
using namespace CEGUI;
static struct
{
	utf8* Language;
	utf8* Font;
	utf8* Text;
} LangList[] =
	{
		// A list of strings in different languages
		// Feel free to add your own language here (UTF-8 ONLY!)...
		{(utf8*)"English",
		 (utf8*) "DejaVuSans-10",
		 (utf8*) "THIS IS SOME TEXT IN UPPERCASE\n"
				 "and this is lowercase...\n"
				 "Try Catching The Brown Fox While It's Jumping Over The Lazy Dog"},
		{(utf8*)"Русский",
		 (utf8*) "DejaVuSans-10",
		 (utf8*) "Всё ускоряющаяся эволюция компьютерных технологий предъявила жёсткие требования к производителям как собственно вычислительной техники, так и периферийных устройств.\n"
				 "\nЗавершён ежегодный съезд эрудированных школьников, мечтающих глубоко проникнуть в тайны физических явлений и химических реакций.\n"
				 "\nавтор панграмм -- Андрей Николаев\n"},

		{(utf8*)"Japanese",
		 (utf8*) "Sword-26",
		 (utf8*) "xxx\n"
				 "xxx\n"
				 "xxx\n"},
		{(utf8*)"Korean",
		 (utf8*) "Batang-26",
		 (utf8*) "xxx xxx\n"
				 "xxx xxx\n"
				 "xxx xxx\n"},

		{(utf8*)"Chinese",
		 (utf8*) "msyh",
		 (utf8*) "你感谢党，感谢国家\n"
				 "这个显示终于对了...\n"}};
class my_ui_module : public gv_ui_module
{
public:
#define MIN_POINT_SIZE 10.0f
	// Sample sub-class for ListboxTextItem that auto-sets the selection brush
	// image.  This saves doing it manually every time in the code.
	class MyListItem : public ListboxTextItem
	{
	public:
		MyListItem(const String& text, CEGUI::uint item_id = 0)
			: ListboxTextItem(text, item_id)
		{
			setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
		}
	};

	void load()
	{
		// we will use of the WindowManager.
		WindowManager& winMgr = WindowManager::getSingleton();

		// load scheme and set up defaults
		SchemeManager::getSingleton().create("TaharezLook.scheme");
		System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");

		// Create a custom font which we use to draw the list items. This custom
		// font won't get effected by the scaler and such.
		FontManager::getSingleton().createFreeTypeFont("DefaultFont", 10 /*pt*/, true, "DejaVuSans.ttf");
		// Set it as the default
		System::getSingleton().setDefaultFont("DefaultFont");

		// load all the fonts (if they are not loaded yet)
		FontManager::getSingleton().createAll("*.font", "fonts");

		// load an image to use as a background
		ImagesetManager::getSingleton().createFromImageFile("BackgroundImage", "GPN-2000-001437.tga");

		// here we will use a StaticImage as the root, then we can use it to place a background image
		Window* background = winMgr.createWindow("TaharezLook/StaticImage");
		// set area rectangle
		background->setArea(URect(cegui_reldim(0), cegui_reldim(0),
								  cegui_reldim(1), cegui_reldim(1)));
		// disable frame and standard background
		background->setProperty("FrameEnabled", "false");
		background->setProperty("BackgroundEnabled", "false");
		// set the background image
		background->setProperty("Image", "set:BackgroundImage image:full_image");
		// install this as the root GUI sheet
		System::getSingleton().setGUISheet(background);

		// set tooltip styles (by default there is none)
		System::getSingleton().setDefaultTooltip("TaharezLook/Tooltip");

		// load some demo windows and attach to the background 'root'
		background->addChildWindow(winMgr.loadWindowLayout("FontDemo.layout"));

		// Add the font names to the listbox
		Listbox* lbox = static_cast< Listbox* >(winMgr.getWindow("FontDemo/FontList"));
		lbox->setFont("DefaultFont");

		FontManager::FontIterator fi = FontManager::getSingleton().getIterator();
		while (!fi.isAtEnd())
		{
			// exclude the special DefaultFont!
			if (fi.getCurrentKey() != String("DefaultFont"))
				lbox->addItem(new MyListItem(fi.getCurrentKey()));
			++fi;
		}

		// set up the font listbox callback
		lbox->subscribeEvent(Listbox::EventSelectionChanged,
							 Event::Subscriber(&my_ui_module::handleFontSelection, this));
		// select the first font
		lbox->setItemSelectState(size_t(0), true);

		// Add language list to the listbox
		lbox = static_cast< Listbox* >(winMgr.getWindow("FontDemo/LangList"));
		lbox->setFont("DefaultFont");
		for (size_t i = 0; i < (sizeof(LangList) / sizeof(LangList[0])); i++)
			// only add a language if 'preferred' font is available
			if (FontManager::getSingleton().isDefined(String(LangList[i].Font)))
				lbox->addItem(new MyListItem(LangList[i].Language, i));
		// set up the language listbox callback
		lbox->subscribeEvent(Listbox::EventSelectionChanged,
							 Event::Subscriber(&my_ui_module::handleLangSelection, this));
		// select the first language
		lbox->setItemSelectState(size_t(0), true);

		winMgr.getWindow("FontDemo/AutoScaled")->subscribeEvent(Checkbox::EventCheckStateChanged, Event::Subscriber(&my_ui_module::handleAutoScaled, this));
		winMgr.getWindow("FontDemo/Antialiased")->subscribeEvent(Checkbox::EventCheckStateChanged, Event::Subscriber(&my_ui_module::handleAntialiased, this));
		winMgr.getWindow("FontDemo/PointSize")->subscribeEvent(Scrollbar::EventScrollPositionChanged, Event::Subscriber(&my_ui_module::handlePointSize, this));

		//if (FontManager::getSingleton().isFontPresent("msyh"))
		System::getSingleton().setDefaultFont("msyh");
	}

	// method to perform any required cleanup operations.
	void cleanupSample()
	{
		// me? cleanup? what?
	}

	/** When a fonts get selected from the list, we update the name field. Of course,
		this can be done easier (by passing the selected font), but this demonstrates how 
		to query a widget's font. */
	void setFontDesc()
	{
		WindowManager& winMgr = WindowManager::getSingleton();

		MultiLineEditbox* mle = static_cast< MultiLineEditbox* >(winMgr.getWindow("FontDemo/FontSample"));

		// Query the font from the textbox
		Font* f = mle->getFont();

		// Build up the font name...
		String s = f->getProperty("Name");
		if (f->isPropertyPresent("PointSize"))
			s += "." + f->getProperty("PointSize");

		// ...and set it
		winMgr.getWindow("FontDemo/FontDesc")->setText(s);
	}

	/** Called when the used selects a different font from the font list.*/
	bool handleFontSelection(const EventArgs& e)
	{
		// Access the listbox which sent the event
		Listbox* lbox = static_cast< Listbox* >(
			static_cast< const WindowEventArgs& >(e).window);

		if (lbox->getFirstSelectedItem())
		{ // Read the fontname and get the font by that name
			Font* font = &FontManager::getSingleton().get(
				lbox->getFirstSelectedItem()->getText());

			// Tell the textbox to use the newly selected font
			WindowManager& winMgr = WindowManager::getSingleton();
			winMgr.getWindow("FontDemo/FontSample")->setFont(font);

			bool b = font->isPropertyPresent("AutoScaled");
			Checkbox* cb = static_cast< Checkbox* >(winMgr.getWindow("FontDemo/AutoScaled"));
			cb->setEnabled(b);
			if (b)
				cb->setSelected(PropertyHelper::stringToBool(font->getProperty("AutoScaled")));

			b = font->isPropertyPresent("Antialiased");
			cb = static_cast< Checkbox* >(winMgr.getWindow("FontDemo/Antialiased"));
			cb->setEnabled(b);
			if (b)
				cb->setSelected(PropertyHelper::stringToBool(font->getProperty("Antialiased")));

			b = font->isPropertyPresent("PointSize");
			Scrollbar* sb = static_cast< Scrollbar* >(
				winMgr.getWindow("FontDemo/PointSize"));
			sb->setEnabled(b);

			// Set the textbox' font to have the current scale
			if (font->isPropertyPresent("PointSize"))
				font->setProperty("PointSize",
								  PropertyHelper::intToString(
									  int(MIN_POINT_SIZE + sb->getScrollPosition())));

			setFontDesc();
		}

		return true;
	}

	bool handleAutoScaled(const EventArgs& e)
	{
		WindowManager& winMgr = WindowManager::getSingleton();

		Checkbox* cb = static_cast< Checkbox* >(
			static_cast< const WindowEventArgs& >(e).window);

		MultiLineEditbox* mle = static_cast< MultiLineEditbox* >(winMgr.getWindow("FontDemo/FontSample"));

		Font* f = mle->getFont();
		f->setProperty("AutoScaled",
					   PropertyHelper::boolToString(cb->isSelected()));

		updateTextWindows();
		return true;
	}

	bool handleAntialiased(const EventArgs& e)
	{
		WindowManager& winMgr = WindowManager::getSingleton();

		Checkbox* cb = static_cast< Checkbox* >(
			static_cast< const WindowEventArgs& >(e).window);

		MultiLineEditbox* mle = static_cast< MultiLineEditbox* >(winMgr.getWindow("FontDemo/FontSample"));

		Font* f = mle->getFont();
		f->setProperty("Antialiased",
					   PropertyHelper::boolToString(cb->isSelected()));

		updateTextWindows();
		return true;
	}

	bool handlePointSize(const EventArgs& e)
	{
		WindowManager& winMgr = WindowManager::getSingleton();

		Scrollbar* sb = static_cast< Scrollbar* >(
			static_cast< const WindowEventArgs& >(e).window);

		Font* f = winMgr.getWindow("FontDemo/FontSample")->getFont();

		f->setProperty("PointSize",
					   PropertyHelper::intToString(
						   int(MIN_POINT_SIZE + sb->getScrollPosition())));

		setFontDesc();

		updateTextWindows();
		return true;
	}

	/** User selects a new language. Change the textbox content, and start with
		the recommended font. */
	bool handleLangSelection(const EventArgs& e)
	{
		// Access the listbox which sent the event
		Listbox* lbox = static_cast< Listbox* >(
			static_cast< const WindowEventArgs& >(e).window);

		if (lbox->getFirstSelectedItem())
		{
			ListboxItem* sel_item = lbox->getFirstSelectedItem();
			size_t idx = sel_item ? sel_item->getID() : 0;
			const String fontName(LangList[idx].Font);

			WindowManager& winMgr = WindowManager::getSingleton();
			// Access the font list
			Listbox* fontList = static_cast< Listbox* >(winMgr.getWindow("FontDemo/FontList"));
			ListboxItem* lbi = fontList->findItemWithText(fontName, 0);
			// Select correct font when not set already
			if (lbi && !lbi->isSelected())
			{ // This will cause 'handleFontSelection' to get called(!)
				fontList->setItemSelectState(lbi, true);
			}

			// Finally, set the sample text for the selected language
			winMgr.getWindow("FontDemo/FontSample")->setText((utf8*)LangList[idx].Text);
		}

		return true;
	}

	//! Ensure window content and layout is updated.
	void updateTextWindows()
	{
		WindowManager& winMgr(WindowManager::getSingleton());
		MultiLineEditbox* eb = static_cast< MultiLineEditbox* >(
			winMgr.getWindow("FontDemo/FontSample"));
		// this is a hack to force the editbox to update it's state, and is
		// needed because no facility currently exists for a font to notify that
		// it's internal size or state has changed (ideally all affected windows
		// should receive EventFontChanged - this should be a TODO item!)
		eb->setWordWrapping(false);
		eb->setWordWrapping(true);
		// inform lists of updated data too
		Listbox* lb = static_cast< Listbox* >(winMgr.getWindow("FontDemo/LangList"));
		lb->handleUpdatedItemData();
		lb = static_cast< Listbox* >(winMgr.getWindow("FontDemo/FontList"));
		lb->handleUpdatedItemData();
	}
};
static const int lobby_max_player = 2048;

void init_ui()
{
	//get_sandbox()->register_processor ( gv_id("gv_ui_manager_cegui") ,gve_event_channel_ui);
}

void main(gvt_array< gv_string >& args)
{
	int idx = 0;
	gv_string layout_name = "";
	if (args.find("layout", idx))
	{
		args[idx + 1] >> layout_name;
	}

	{
		//============>>INIT=================================================>>
		gv_unit_test_context_guard context;
		init_ui();
		m_sandbox->register_processor(gv_id("gv_world_rpg2d"), gve_event_channel_world);
		gv_global::rnd_opt.m_max_debug_line = 10000;
		gv_global::rnd_opt.m_max_debug_tri = 1000;
		gv_world* my_world = gvt_cast< gv_world >(m_sandbox->get_event_processor(gve_event_channel_world));
		{
			gv_vector3 min_p = to_3d(gv_vector2(0, 0));
			min_p.y = -100;
			gv_vector3 max_p = to_3d(gv_vector2(8000, 3000));
			max_p.y = 100;
			my_world->init(1, gv_vector2i(map_width, map_height), gv_box(min_p, max_p), the_map);
		}
		gv_module* my_mod = m_sandbox->create_object< gv_module >(gv_id("test_world"));
		my_world->set_owner(my_mod);
		gv_entity* my_entity = m_sandbox->create_object< gv_entity >(gv_id("entity"), my_world);
		gv_com_skeletal_mesh* com_sk_mesh = get_sandbox()->create_object< gv_com_skeletal_mesh >(my_entity);
		gv_material* my_material = m_sandbox->create_object< gv_material >();
		gv_skeletal_mesh* my_skeletal_mesh = NULL;
		gv_static_mesh* my_static_mesh = NULL;
		gv_ani_set* my_animation = NULL;
		gv_effect* my_effect = NULL;
		gvt_random< gv_float > m_random;

		gvt_array_cached< gv_effect*, 256 > effect_table;
		int current_idx = 0;

		{ //load model & animation
			gv_string_tmp file_name = FILE_FBX_HERON;
			gv_model* model = m_sandbox->create_object< gv_model >(my_mod);
			m_sandbox->import_external_format(model, file_name);
			my_animation = model->get_animation(0);
			my_static_mesh = model->get_skeletal_mesh(0)->m_t_pose_mesh;
			my_skeletal_mesh = model->get_skeletal_mesh(0);
			if (!my_static_mesh->get_nb_normal())
				my_static_mesh->rebuild_normal();
			my_static_mesh->rebuild_binormal_tangent();
		}

		{ //load material
			gv_effect_project* effect = m_sandbox->create_object< gv_effect_project >(my_mod);
			gv_string_tmp file_name = FILE_RFX_BIRD;
			m_sandbox->import_external_format(effect, *file_name);
			my_effect = effect->get_base_effect();
			my_effect->query_renderable_effect(effect_table);
			my_effect = my_effect->get_renderable_effect(gv_id("TexturedPhong"));
			my_material->m_effect = my_effect;
		}

		{ //link animation & material
			com_sk_mesh->set_material(my_material);
			com_sk_mesh->set_resource(my_skeletal_mesh);
			com_sk_mesh->set_renderer_id(gve_render_pass_opaque, gv_id("gv_com_effect_renderer"));
			my_entity->add_component(com_sk_mesh);
			gv_com_animation* com_ani = m_sandbox->create_object< gv_com_animation >(my_entity);
			//com_ani->set_resource(my_animation);
			//my_entity->add_component(com_ani);
			//com_ani->play_animation(my_animation->get_sequence(0)->get_name_id(),0.f,true);
		}
		{ //
			my_entity->add_component(gv_id("gv_com_rpg2d_actor"));
		}
		my_entity->set_position(gv_vector3(30, 1, 30));
		my_entity->update_matrix();
		my_world->add_entity(my_entity);
		my_world->set_main_actor(my_entity);

		while (1)
		{
			gv_vector3 pos;
			pos.x = m_random.get_uniform() * 1000.f;
			pos.y = 0;
			pos.z = m_random.get_uniform() * 1000.f;
			if (my_world->teleport_entity(my_entity, pos, gv_euler::get_zero_rotation()))
				break;
		};
		float r = my_static_mesh->get_bsphere().get_radius();
		gvt_array< gv_vector3 > original_pos;

		int loop = 20000;
		bool quit = false;

		{ //set camera;
			my_entity->add_component(gv_id("gv_com_3d_lobby_controller"));
			gv_com_camera* camera = m_sandbox->create_object< gv_com_camera >(gv_id("main_camera"), my_entity);
			camera->set_fov(60.f, 1.333f, 0.1f * r, 100 * r);
			camera->set_look_at(gv_vector3(0, 2, 5.f) * r, gv_vector3(0, 0, 0));
			camera->update_projection_view_matrix();
			my_entity->add_component(camera);
			GVM_POST_EVENT(render_set_camera, render, (pe->camera = camera));
		}

		if (args.size())
			args[0] >> loop;
		static int start_count = 0;
		while (loop-- && !quit)
		{
			if (loop == 1)
			{
				gv_object_event_render_uninit* pe = new gv_object_event_render_uninit;
				m_sandbox->post_event(pe, gve_event_channel_render);
			}
			start_count++;
			if (start_count == 3)
			{
				if (layout_name.strlen())
				{
					GVM_POST_EVENT(ui_load_layout, ui, pe->layout_name = layout_name);
				}
				else
				{
					GVM_POST_EVENT(ui_load_module, ui, pe->module = new my_ui_module);
				}
			}

			quit = !m_sandbox->tick();
			static bool last_key_down = false;
			if (m_sandbox->get_input_manager()->is_key_down(e_key_space) && !last_key_down)
			{
				current_idx++;
				if (current_idx >= effect_table.size())
					current_idx = 0;
				my_material->m_effect = effect_table[current_idx];
			}
			last_key_down = m_sandbox->get_input_manager()->is_key_down(e_key_space);
		}

		//================================================
		//m_sandbox->export_module(my_mod->get_name_id() );
	}
}
}
