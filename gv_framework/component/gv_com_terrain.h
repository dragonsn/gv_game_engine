#pragma once
namespace gv
{

/*
class  gv_com_terrain :public gv_com_graphic
{
public:
        gv_com_terrain	();
        ~gv_com_terrain	();
public:
        //geom related function
        virtual gv_bool		init
(	const gv_float  * pheightmap ,int w , int l, int wseg, int lseg  );
        virtual float		get_heightf
(	float	x, float y		);
        virtual float		get_heightf
(	const  gv_vector3 & v3  ) {	return get_heightf(v3.x, v3.y); }
        virtual float		get_height_interpolated	(
float x, float y		);
        virtual	void		get_normal
(	float x, float y, gv_vector3 &norm);
        virtual	void		get_normal_interpolated	(
float x, float y, gv_vector3 &norm);
        virtual float		get_width();
        virtual float		get_length();
        virtual float		get_max_height();
        virtual gv_bool		find_line_collision		(const
gv_line_seg & line_seg , float max_len, gvt_array<gv_vector3> &xpoints, gv_bool
stop_at_first_hit=TRUE)=NULL;
        virtual int			get_lseg()=NULL;
        virtual int			get_wseg()=NULL;
        virtual float		get_sector_w();
        virtual float		get_sector_l();
        virtual gv_rect		get_sector_rect(int x, int y);


        virtual void		gen_light_map_with_sun	(const
gv_vector3 &sun_dir  ,INT w , INT h ,  gvi_stream * ps)=NULL;
        virtual void		set_wire_frame_on		(gv_bool b
)=NULL;
        virtual void		set_view_dependence		(gv_bool
b )=NULL;
        virtual gvi_mesh *	get_mesh ()=NULL ;
        virtual void reset()=NULL;

        //material
        virtual void			set_material
(gv_material * pmat );
        virtual	void			update_sector_material(int id);

        //soil layer related
        virtual int				get_nb_soil_layer	 ();
        virtual void			set_nb_soil_layer	 (int nb);
        virtual	int
get_max_layer_per_sector();//this is for optimizaton,
        virtual	void			set_max_layer_per_sector(int );
        //
        virtual gv_texture *	get_static_light_map ();
        virtual void			set_static_light_map (gv_texture *);
        //
        virtual gvi_texture *	get_nth_tile_map	 (int i);
        virtual void			set_nth_tile_map	 (int i,
gvi_texture *);
        virtual void			set_nth_tile_map_matrix  ( int i,
const gv_matrix44 & mat);
        virtual	gv_matrix44		get_nth_tile_map_matrix  ( int i);

        // mask image related ,for the layer visiblity
        virtual gv_bool 			update_layer_info
(const gv_rect & rect,  int layer=0);
        virtual gvo_image	&		get_nth_mask
(int i);
        virtual void				set_nth_mask
(int i, const gvo_image &);
        virtual void				get_mask_image_size
(int & w, int &  h);
        virtual void				set_mask_image_size
(int   w, int    h); //must can mod the wseg, and hseg
        virtual	void				build_nth_mask
(int i , gv_bool discard_old=FALSE);
        virtual gvi_texture *		get_nth_mask_map
(int i);
        virtual void				set_nth_mask_map
(int i, gvi_texture * );

        // dirty map
        virtual void				enable_dirty_map
(gv_bool b);
        virtual gv_bool				b_is_dirty_map_enabled
();
        virtual	void				set_dirty_map
(int w ,int h);
        virtual void				clear_dirty_map
();
        virtual gvi_texture		*   get_dirty_map();
        virtual void				set_dirty_map(gvi_texture *);

        //
protected:
        gvt_array<gvt_array<char> > m_sector_layer_visible_info;
        gv_texture *				m_tile_maps
[MAX_TERRAIN_LAYER];
        gv_texture *				m_mask_maps
[MAX_TERRAIN_LAYER];
        gv_texture *				m_dirty_map;
        gv_texture *				m_light_map;
        gvt_array<gv_material * >	m_material_list;
        gv_matrix44
m_tile_map_matrix[MAX_TERRAIN_LAYER];
        int
m_mask_size_w, m_mask_size_h;
};
*/
}