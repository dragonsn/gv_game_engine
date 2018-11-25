GVM_DCL_DRIVER(impexp)
namespace unit_test_image
{
void main(gvt_array< gv_string >& args)
{
	gv_global::sandbox_mama.get();
	gv_global::sandbox_mama->init();
	gv_sandbox* m_sandbox;
	{
		sub_test_timer timer("test_importer_3ds");
		m_sandbox = gv_global::sandbox_mama->create_sandbox(gv_global::sandbox_mama->get_base_sandbox());
		{
			gv_image_2d* pimage = m_sandbox->create_object< gv_image_2d >();
			pimage->init_image(gv_color::RED(), gv_vector2i(128, 128));
			;
			for (int i = 0; i < 128; i++)
			{
				for (int j = 0; j < 128; j++)
				{
					gv_color c(j * 2, 0, 0, 255);
					pimage->get_pixel(gv_vector2i(i, j)) = c;
				}
			}

			pimage->export_file("red2.tga");
			pimage->import_file("red2.tga");
			pimage->export_file("red.tga");
			pimage->export_file("red.bmp");
		}
		gv_global::sandbox_mama->delete_sandbox(m_sandbox);
		gv_id::static_purge();
		m_sandbox = NULL;
	}

	gv_global::sandbox_mama.destroy();
	gv_id::static_purge();
}
}