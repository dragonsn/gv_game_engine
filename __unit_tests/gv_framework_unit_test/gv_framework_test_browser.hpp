#if GV_WITH_GP

#include "gameplay3d/src/gameplay.h"

using std::string;
using std::pair;
using namespace gameplay;
class TestsGame : public Game, Control::Listener
{

public:
	typedef void (*TestGameCreatePtr)(gvt_array< gv_string >& param);

	TestsGame()
		: _font(NULL), _testSelectForm(NULL)
	{
		s_instance = this;
	}

	void initialize()
	{
		_font = Font::create("res/common/arial18.gpb");

		// Load camera script
		// getScriptController()->loadScript("res/common/camera.lua");

		// Construct a form for selecting which test to run.
		Theme* theme = Theme::create("res/common/default.theme");
		Theme::Style* formStyle = theme->getStyle("basic");
		Theme::Style* buttonStyle = theme->getStyle("buttonStyle");
		Theme::Style* titleStyle = theme->getStyle("title");

		// Note: this calls addRef() on formStyle's Theme, which we created above.
		_testSelectForm =
			Form::create("testSelect", formStyle, Layout::LAYOUT_VERTICAL);
		theme->release(); // So we can release it once we're done creating forms
						  // with it.

		_testSelectForm->setAutoHeight(true);
		_testSelectForm->setWidth(200.0f);
		_testSelectForm->setScroll(Container::SCROLL_VERTICAL);

		const size_t size = _tests->size();
		for (size_t i = 0; i < size; ++i)
		{
			Label* categoryLabel =
				Label::create((*_categories)[i].c_str(), titleStyle);
			categoryLabel->setAutoWidth(true);
			categoryLabel->setTextAlignment(Font::ALIGN_BOTTOM_LEFT);
			categoryLabel->setHeight(40);
			categoryLabel->setText((*_categories)[i].c_str());
			categoryLabel->setConsumeInputEvents(false);
			_testSelectForm->addControl(categoryLabel);
			categoryLabel->release();

			TestRecordList list = (*_tests)[i];
			const size_t listSize = list.size();
			for (size_t j = 0; j < listSize; ++j)
			{
				TestRecord testRecord = list[j];
				Button* testButton =
					Button::create(testRecord.title.c_str(), buttonStyle);
				testButton->setText(testRecord.title.c_str());
				testButton->setAutoWidth(true);
				testButton->setHeight(
					60);								  // Tall enough to touch easily on a BB10 device.
				testButton->setConsumeInputEvents(false); // This lets the user scroll
														  // the container if they swipe
														  // starting from a button.
				testButton->addListener(this, Control::Listener::CLICK);
				_testSelectForm->addControl(testButton);
				testButton->release();
			}
		}
		_testSelectForm->setState(Control::FOCUS);

		// Disable virtual gamepads.
		unsigned int gamepadCount = getGamepadCount();

		for (unsigned int i = 0; i < gamepadCount; i++)
		{
			Gamepad* gamepad = getGamepad(i, false);
			if (gamepad->isVirtual())
			{
				gamepad->getForm()->setEnabled(false);
			}
		}

		//==================================================
		if (!FileSystem::fileExists("game.config"))
		{
			gv_debug_output("!game config not found !!\n");
		}
		{
			std::vector< std::string > files;
			FileSystem::listFiles("res", files);
			for (int i = 0; i < (int)files.size(); i++)
			{
				GVM_DEBUG_OUT("find file:" << files[i].c_str());
			}
		}
	}

	void finalize()
	{
		SAFE_RELEASE(_font);
		SAFE_DELETE(_categories);
		SAFE_DELETE(_tests);
		SAFE_RELEASE(_testSelectForm);
	}

	void update(float elapsedTime)
	{
		_testSelectForm->update(elapsedTime);
	}

	void render(float elapsedTime)
	{
		// Clear the color and depth buffers
		clear(CLEAR_COLOR_DEPTH, Vector4::zero(), 1.0f, 0);
		_testSelectForm->draw();
	}

	void keyEvent(Keyboard::KeyEvent evt, int key)
	{

		if (evt == Keyboard::KEY_PRESS)
		{
			switch (key)
			{
			case Keyboard::KEY_ESCAPE:
				exit();
				break;
			}
		}
	}

	bool mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta)
	{
		return false;
	}

	void menuEvent()
	{
		exitActiveTest();
	}

	void controlEvent(Control* control, EventType evt)
	{
		if (strcmp(control->getId(), "all") == 0)
		{
			const size_t size = _tests->size();
			for (size_t i = 0; i < size; ++i)
			{
				TestRecordList list = (*_tests)[i];
				const size_t listSize = list.size();
				for (size_t j = 0; j < listSize; ++j)
				{
					TestRecord testRecord = list[j];
					if (!testRecord.funcPtr)
						continue;
					GVM_WARNING("start test " << testRecord.title.c_str() << gv_endl);
					gvt_scope_test_timer< gv_log > timer(test_log(),
														 testRecord.title.c_str());
					runTest(testRecord.funcPtr);
					GVM_WARNING("end of test " << testRecord.title.c_str() << gv_endl);
				}
			}
			return;
		}
		if (strcmp(control->getId(), "quit") == 0)
		{
			return;
		}
		const size_t size = _tests->size();
		for (size_t i = 0; i < size; ++i)
		{
			TestRecordList list = (*_tests)[i];
			const size_t listSize = list.size();
			for (size_t j = 0; j < listSize; ++j)
			{
				TestRecord testRecord = list[j];
				if (testRecord.title.compare(control->getId()) == 0)
				{
					GVM_WARNING("start test " << testRecord.title.c_str() << gv_endl);
					gvt_scope_test_timer< gv_log > timer(test_log(),
														 testRecord.title.c_str());
					runTest(testRecord.funcPtr);
					GVM_WARNING("end of test " << testRecord.title.c_str() << gv_endl);
					return;
				}
			}
		}
	}

	void runTest(TestGameCreatePtr func)
	{
		TestGameCreatePtr p = (TestGameCreatePtr)func;
		gvt_array< gv_string > param;
		p(param);
	}

	void exitActiveTest()
	{
		Gamepad* virtualGamepad = getGamepad(0, false);
		if (virtualGamepad && virtualGamepad->isVirtual())
		{
			virtualGamepad->getForm()->setEnabled(false);
		}

		// Reset some game options
		setMultiTouch(false);
	}

	static void addTest(const char* category, const char* title,
						TestGameCreatePtr func, unsigned int order)
	{
		if (_tests == NULL)
			_tests = new std::vector< TestRecordList >();
		if (_categories == NULL)
		{
			_categories = new std::vector< std::string >();
			_categories->push_back("base");
			_tests->resize(_categories->size());
		}

		string categoryString(category);
		string titleString(title);
		// simplify it to be the same category
		int index = 0;
		(*_tests)[index].push_back(TestRecord(titleString, func, order));
	}

	static TestsGame* getInstance()
	{
		return s_instance;
		;
	}

private:
	struct TestRecord
	{
		std::string title;
		TestGameCreatePtr funcPtr;
		unsigned int order;

		TestRecord()
			: funcPtr(NULL), order(0)
		{
		}
		TestRecord(std::string title, TestGameCreatePtr funcPtr, unsigned int order)
			: title(title), funcPtr(funcPtr), order(order)
		{
		}

		TestRecord& operator=(const TestRecord& copy)
		{
			title = copy.title;
			funcPtr = copy.funcPtr;
			order = copy.order;
			return *this;
		}

		bool operator<(const TestRecord& v) const
		{
			return order < v.order;
		}
	};

	/**
  * The list of category title strings.
  */
	static std::vector< std::string >* _categories;

	/**
  * The collection of test titles and the function pointers that create the
  * tests.
  * The pair represents the string title of the test and the function pointer
  * that is used to create the test.
  * The inner vector is a list of those pairs in the order that they were added.
  * The outer vector represents the list of categories that contain the list of
  * tests in the category.
  * The index of _categories maps to the index of the outer vector. (Therefore
  * their size should always be the same).
  */
	typedef std::vector< TestRecord > TestRecordList;
	static std::vector< TestRecordList >* _tests;
	static TestsGame* s_instance;

	Font* _font;
	Form* _testSelectForm;
};

std::vector< TestsGame::TestRecordList >* TestsGame::_tests = NULL;
std::vector< std::string >* TestsGame::_categories = NULL;
TestsGame* TestsGame::s_instance = NULL;

int browser_main()
{
	gv_base_config& boost_config = gv_global::config;
	boost_config.enable_mem_tracking = false;
	boost_config.enable_profiler = true;
	boost_config.profiler_buffer_size = 256 * 1024; // for test
	TestsGame* mygame = new TestsGame;
	;

	gv_base_init();
	gv_framework_init();
#undef REG_UNIT_TEST
#define REG_UNIT_TEST(x)                            \
	{                                               \
		TestsGame::addTest("base", #x, x::main, 0); \
		GVM_DEBUG_OUT(#x " added to test\n");       \
	}
	TestsGame::addTest("system", "all", 0, 0);
	TestsGame::addTest("system", "quit", 0, 0);
#include "gv_framework_console_tests.h"
	FileSystem::setResourcePath(RES_ROOT"/assets/");
	//m_sandbox->set_resource_root_path(RES_ROOT);

	{
		Game* game = Game::getInstance();
		Platform* platform = Platform::create(game);
		GP_ASSERT(platform);
		platform->enterMessagePump();
		delete platform;
	}
	gv_framework_destroy();
	gv_global::profiler->dump_snap_shot("boost_framework_test_profile_");
	gv_base_destroy();
	gvp_memory_default::static_dump_tracking();
	delete mygame;
	return 1;
}
#endif
