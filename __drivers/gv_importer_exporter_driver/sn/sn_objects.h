#pragma  once
namespace sn
{
	//============================================================================================
	//								:
	//============================================================================================
	#define SN_ZERO_ME memset(this, 0, sizeof (*this));
	typedef float SNVALUE;
	typedef unsigned long DWORD;

	struct   SN_Point3  
	{
		union
		{
			struct 
			{
				SNVALUE x, y ,z;
			}; 
			SNVALUE v[3];
		}; 
	};

	struct   SN_Color
	{
		SNVALUE r,g,b,a; 
	} ; 

	struct   SN_Quat
	{
		SNVALUE w,x,y,z;
	};

	struct   SN_Matrix3  
	{
		SNVALUE			m[4][3];
		unsigned long	flags;
	};

	class  SN_Box3 
	{
		SNVALUE x,y,z;
		SNVALUE w,l,h; 
		SN_Point3 Vertexs[8];
	};
	//============================================================================================
	//								:
	//============================================================================================
	#define	OBJ_BASIS_OBJECT	0
	#define	OBJ_MESH_OBJECT		1
	#define OBJ_MAT_OBJECT		2
	#define OBJ_K_ANI_OBJECT    3
	#define	OBJ_SIMPLE_OBJECT	4
	#define	OBJ_PARTICLE_SYSTEM_OBJECT	5
	#define SN_SKIN_OBJECT				6
	#define OBJ_GEO_OBJECT		9
	#define	OBJ_CAM_OBJECT		10
	#define	OBJ_LIG_OBJECT		11
	#define OBJ_DEFORM_OBJECT   12
	#define	OBJ_NULL_OBJECT		65535	
	#define OBJ_NO_FATHER		-1

	#define	OBJ_DIRECT_LIGHT_OBJECT		0
	#define	OBJ_FREE_LIGHT_OBJECT		1
	#define OBJ_OMNI_LIGHT_OBJECT		2
	#define OBJ_POINT_LIGHT_OBJECT		3

	class  SN_Object  
	{
	public:
		SN_Object(){type=OBJ_BASIS_OBJECT;};
		virtual ~SN_Object(){};
		virtual SN_Object * Load	( long float_address , unsigned char  ** ppData )
		{
			SN_Object    * pm; 
			SN_Object	 * pms = (SN_Object	 *)(*ppData);
			int id=this->Unique_ID;
			pm =this; 
			*pm=*pms ;
			(*ppData)  +=sizeof (SN_Object );
			pm->Index+=float_address;
			if (this->father!=0 && this->father!=-1)
			{
				this->father+=float_address ; 
			}
			this->Unique_ID=id;
			return pm;
		}; 

		int		    Save ( char * filename  )
		{
			FILE * fp=fopen(filename , "a+b"); 
			int length = (int)fwrite( this, 1,sizeof (SN_Object),fp ); 
			fclose (fp); 
			return (int)length; 
		}

		long 		type;
		long		father;
		long		Index ; 
		long		NumberOfChild;
		long		Obj_Flag; 
		SN_Matrix3	tm; 
		char		name[256];
		char	*	data;
		long		Unique_ID ; 
		SN_Box3	*	pBoundingBox ;
	};
	//============================================================================================
	//								:
	//============================================================================================
	class  SN_Camera_Object :public SN_Object 
	{
	public:
		SNVALUE  left,right,top , bottom; 
		SNVALUE	 near_plane,far_plane ; 
		SNVALUE fov ; 
		SNVALUE aspect ;
	};
	//============================================================================================
	//								:
	//============================================================================================
	class  SN_Geom_Object  :public SN_Object
	{
	public:
		long  flags; 
		long  Geom_Type ; 
	};
	//============================================================================================
	//								:
	//============================================================================================
	class   SN_Deform_Object:public SN_Object
	{
	public:
		int  Current_Frame;
		int  Start_Frame; 
		int	 End_Frame;
		int  Deform_Flags ; 
		int	 Target; 
	};
	//============================================================================================
	//								:
	//============================================================================================
	class  SN_Material:public SN_Object
	{
	public:
	// method
		SN_Material(){type=OBJ_MAT_OBJECT;};
		virtual ~SN_Material(){};
		SN_Object *  SN_Material::Load(long address ,unsigned char ** ppData)
		{
			SN_Material  *  pm; 
			SN_Material  *  pms =(SN_Material  *)(*ppData); 
			int				   id=this->Unique_ID;
			pm=this ; 
			*pm=*pms ; 
			this->Index  +=address     ;
			if (pm->father!=0 && pm->father!=-1)	this->father +=address	   ;
			if (this->Material_ID!=-1) 
			{
				Material_ID+=address ; 
				//pm->LoadFromFile(pm->FileName);
			}
			(*ppData) +=sizeof (SN_Material); 
			this->Unique_ID=id;
			return (SN_Object *)pm; 
		}

		int Save	(char * filename )
		{
			FILE * fp=fopen (filename , "a+b"); 
			fwrite(this,1, sizeof (SN_Material ),fp ); 
			fclose (fp ); 
			return (sizeof ( SN_Material	));
		}

		SN_Color		Diffuse_Color; 
		SN_Color		Ambient_Color;
		SN_Color		Specular_Color ; 
		float			Opacity ; 
		float			Shininess ;
		char			FileName[256]; 
		long			Material_ID ;
		long			lNbSubMtl; 
		void			* CuMat ; 
	};

	class   SN_Bone 
	{
	public:
		SN_Bone		(){SN_ZERO_ME;}; 
		~SN_Bone	()
		{
			if (pVertexIndex) free(pVertexIndex); 
			if (pWeight) free(	pWeight);
		};
	public:
		int			Save		(FILE * fp)
		{
			int length =0; 
			fwrite (this, 1,sizeof (SN_Bone),fp); 
			length=sizeof(SN_Bone); 
			length+=sizeof (int )*this->iNumberVertex; 
			length+=sizeof (float)*this->iNumberVertex; 
			fwrite (this->pVertexIndex, 1,sizeof (int) *this->iNumberVertex,fp); 
			fwrite (this->pWeight      ,1,sizeof (float )*this->iNumberVertex,fp);
			return	length;
		}; 
		void		Load		(long address , unsigned char  ** ppData )
		{
			SN_Bone *  ps=(SN_Bone *) *ppData; 
			*this=*ps;
			this->pVertexIndex=(int  *)malloc(this->iNumberVertex*(sizeof(int  )));
			this->pWeight     =(float *) malloc(this->iNumberVertex*(sizeof(float))); 
			*ppData+=sizeof (SN_Bone); 
			memcpy(this->pVertexIndex, *ppData,sizeof (int)*this->iNumberVertex); 
			*ppData+=sizeof(int)*this->iNumberVertex;
			memcpy(this->pWeight     , *ppData,sizeof (float)*this->iNumberVertex); 
			*ppData+=sizeof(float)*this->iNumberVertex; 
			this->iBoneID+=address; 
		}; 

	public:
		SN_Matrix3  mat_ll;       // object local -> bone local 
		int			iBoneID		 ; //the index of the		bone in object table
		int			iNumberVertex; //
		int    *	pVertexIndex ; //the index of affected point  
		float  * 	pWeight		 ; //the weight table 
	}; 

	
	//============================================================================================
	//								:
	//============================================================================================
	class SN_Vertex
	{
		
	public:
		float x,y,z;
		float u,v; 
		float r,g,b; 
		union
		{
			float nx;
			float a; 
		};
		float ny,nz;
		DWORD flags;  //reserve for further use .
	} ; 

	struct tdstTriangle
	{
		long			type;          /* */
		long			Mtl_ID; 
		long			v[3];
	};

	struct  tdstQuadrilateral
	{
		long			type;          /* */
		long			Mtl_ID; 
		long			v[4];
	};

	struct   tdstStrip
	{
		long S_Number; 
		unsigned long * pSLong; 
		long SI_Number ; 
		unsigned long * pSIndex; 
	};

	typedef		tdstTriangle Tri;
	typedef		tdstQuadrilateral Quad;

	#define		SN_NORMAL_MESH				0x1
	#define     SN_INDEXED_MESH				0x2
	#define		SN_STRIP					0x4
	#define		SN_FAN						0x8
	#define		SN_MESH_CHANGED				0x10 
	#define		SN_MESH_SELECTED			0x20 
	#define		SN_MESH_DISPLAY_WIREFRAME	0x40
	#define		SN_MESH_DISPLAY_NO_TEXTURE	0x80
	#define		SN_MESH_DISPLAY_BOUNDINGBOX	0x100
	#define		SN_MESH_DISPLAY_COORDINATE	0x200
	#define		SN_MESH_DYNAMIC				0x400
	#define		SN_MESH_NO_LIGHT			0x800
	#define		SN_MESH_MULTI_TEXTURE		0x1000
	#define		SN_MESH_HIDED				0x2000
	#define		SN_MESH_WITH_HIGHLIGHT		0x4000
	#define		SN_MESH_NO_SHADOW			0x8000
	#define		SN_MESH_USE_LIGHTMAP		0x10000

	template <class T >	void  AddToArray  (T* &pHead , int &m_Length  , T *  pt , int   count ) 
	{
		 assert (count>=0); 
		 if (count==0) return ; 
		 T * p =(T *)malloc  (sizeof(T) *( m_Length+count) );
		 if ( m_Length!=0)
		 {
			 memcpy (p , pHead , sizeof (T) *m_Length ); 
		 }
		 memcpy ((p +(m_Length)), pt , sizeof(T)*count);
		 m_Length+=count; 
		free (pHead); 
		pHead =p;
	}

	class  SN_Mesh
	{
	// constructures 
	public: 
		SN_Mesh ()
		{
			this->p_Quad=NULL;
			this->p_Tri =NULL; 
			this->p_Vertex=NULL;
		}; 
		virtual ~SN_Mesh()
		{
			if (this->p_Quad)	delete[] (this->p_Quad); 
			if (this->p_Tri )	delete[] (this->p_Tri); 
			if (this->p_Vertex) delete[] (this->p_Vertex); 
		}; 

		SN_Mesh & operator =(const SN_Mesh & m )
		{
			this->Mesh_Flags=m.Mesh_Flags; 
			this->T_Number=m.T_Number; 
			this->V_Number=m.V_Number; 
			this->Q_Number=m.Q_Number;
			this->Material_ID=m.Material_ID;
			return (*this);
		}

	public : 
		long			 Mesh_Flags; 
		int  		     V_Number ; 
		int				 T_Number;
		int				 Q_Number;
		long			 ID;
		long			 Material_ID		; // the material  
		SN_Vertex	   * 	  p_Vertex		; 
		tdstTriangle   * 	  p_Tri			;
		tdstQuadrilateral *   p_Quad		;
		void	*			  pCustomerData	; 			
	public:
		virtual SN_Mesh   * Load	(long float_address , unsigned char  ** ppData)
		{
				// read the vertex list , and add the current point 
			long  temp =V_Number;  
			V_Number=0; 
			LoadVertex((SN_Vertex * )(*ppData),temp); 
			(*ppData)+=sizeof (SN_Vertex) *V_Number; 
			
			// read the tri list  
			temp =T_Number;  
			T_Number=0; 
			LoadTri((Tri * )*ppData,temp); 
			(*ppData)+=sizeof (Tri) * T_Number; 

			if (this->Material_ID!=0) this->Material_ID+=float_address; 
			//load strips 
			if ((this->Mesh_Flags&SN_STRIP))
			{
				this->pCustomerData=new(tdstStrip ) ; 
				tdstStrip * pstrip ; 
				pstrip=(tdstStrip * ) this->pCustomerData; 
				*pstrip=*((tdstStrip *) (*ppData)); 
				*ppData+=sizeof (tdstStrip ); 

				pstrip->pSLong=(unsigned long *)malloc(sizeof(long ) * pstrip->S_Number);

				memcpy (pstrip->pSLong, (*ppData), sizeof(long ) * pstrip->S_Number);
				*ppData+=sizeof(long ) * pstrip->S_Number;
				
				pstrip->pSIndex=(unsigned long *)malloc(sizeof(long ) * pstrip->SI_Number); 
				memcpy (pstrip->pSIndex,(*ppData) ,sizeof(long )* pstrip ->SI_Number); 
				*ppData+=sizeof(long) * pstrip->SI_Number; 
			}

			tdstTriangle * ptri=p_Tri ; 
			for (int i=0; i<this->T_Number; i++,ptri++)
			{
				int mtlid=ptri->Mtl_ID&0xffff;
				ptri->Mtl_ID+=float_address;
			}
			return this;
		}; 
		virtual int		    Save	(char * filename)
		{
				int length=0;
			FILE * pStream=fopen (filename , "a+b");

			length+=(int)fwrite	(this		   ,1, sizeof ( SN_Mesh )					,pStream);
			length+= (int)fwrite	(this->p_Vertex,1, sizeof ( SN_Vertex )*this->V_Number, pStream);
			length+= (int)fwrite	(this->p_Tri   ,1, sizeof ( Tri       )*this->T_Number, pStream);
			// for now no need to record the qua 
			// fwrite		( this->p_Quad  , sizeof ( Quad		 ),this->Q_Number , pStream); 
			if ( (this->Mesh_Flags&SN_STRIP))
			{
				
				tdstStrip * pstrip ; 
				pstrip=(tdstStrip * ) this->pCustomerData; 
				length+= (int)fwrite(this->pCustomerData ,1, sizeof(tdstStrip), pStream);
				length+= (int)fwrite(pstrip->pSLong , 1,sizeof(long)*pstrip->S_Number , pStream);
				length+= (int)fwrite(pstrip->pSIndex, 1,sizeof(long)*pstrip->SI_Number,pStream);
			}
			fclose ( pStream	); 
			return length; 
		}; 
		void LoadVertex (SN_Vertex * pPoint , int count )
		{
			AddToArray (this->p_Vertex, this->V_Number, pPoint, count);  
		}
		void LoadTri    (Tri  * ptri, int count)
		{
			AddToArray (this->p_Tri, this->T_Number, ptri, count); 
		}
		
	};
	//============================================================================================
	//								:
	//============================================================================================
	class  SN_Mesh_Object:public SN_Geom_Object 
	{
	public:
		SN_Mesh * pmesh;
		SN_Mesh_Object()
		{
			this->father=-1;
			this->type=OBJ_MESH_OBJECT; 
			this->pmesh=new(SN_Mesh);
			this->pmesh->ID=this->Unique_ID;
			int isize=sizeof (SN_Mesh_Object)-sizeof (SN_Object);
			isize=sizeof (SN_Mesh_Object)-sizeof (SN_Geom_Object);
			isize=sizeof (SN_Mesh_Object)-sizeof (SN_Object);
			this->pBoundingBox=0;
		}
		~SN_Mesh_Object()
		{
			if (  this->pmesh)	delete this->pmesh;
		};
		///////////////////////////////////////////////////////////////////////
		virtual SN_Object * Load			( long address , unsigned char  ** ppData )
		{
			SN_Mesh_Object *  pt; 
			SN_Mesh_Object *  ps ; 
			SN_Mesh_Object pps;
			SN_Mesh * pMesh; 
			int				   id=this->Unique_ID;
			pMesh=this->pmesh;

			// init the  target  
			ps =(SN_Mesh_Object  *) (*ppData); 
			pt =this;
			*pt=*ps;
			(*ppData) +=sizeof (SN_Mesh_Object);

			// follow  the sequence  then the next is the mesh data
			this->pmesh=pMesh;
			*(this->pmesh)=*((SN_Mesh * ) (*ppData)); 
			(*ppData)+=sizeof (SN_Mesh); 
			pt->pmesh->Load (address , ppData); 

			if (this->father!=0 && this->father!=-1) this->father+=address ;
			this->Index +=address;
			
			this->Unique_ID=id;
			this->pmesh->ID=this->Unique_ID;
			this->tm.flags=0;
			return (SN_Object * )pt;
		}; 
		virtual int		    Save			( char * filename  )
		{
			int length=0;
			FILE * pStream=fopen (filename , "a+b");
			SN_Mesh_Object * psm=this;
			length+=(int)fwrite	(psm,1, sizeof  ( SN_Mesh_Object ), pStream); 
			fclose ( pStream	); 
			length+=psm->pmesh->Save(filename );
			return length; 
		}; 
	};
	//============================================================================================
	//								:
	//============================================================================================
	class	  SN_Skin: public SN_Deform_Object 
	{
	public:
		SN_Skin()
		{
			iNumVertex=iNumBones=NULL;
			pBones=NULL;pVertexCopy=NULL;
		};
		~SN_Skin()
		{
			if (pBones) free( pBones); 
			if (pVertexCopy)free( pVertexCopy);
		};

		SN_Object * Load			( long floataddress , unsigned char  ** ppData )
		{
			SN_Skin * ps=(SN_Skin *) *ppData; 
			*this       = *ps;
			this->pBones=(SN_Bone *) malloc(sizeof (SN_Bone)*this->iNumBones); 
			SN_Bone * pbone=this->pBones; 
			*ppData+=sizeof(SN_Skin); 
			for  ( int i =0;  i<this->iNumBones; i++, pbone++)
			{
				pbone->Load(floataddress ,ppData);
			}
			this->Target+=floataddress; 
			this->Index+=floataddress;
			return (SN_Object *) this;
		}; 
		virtual     int		   Save	( char * filename )
		{
			FILE * fp=fopen(filename, "a+b"); 
			fwrite  (this,1,sizeof (SN_Skin) ,fp); 
			int length =0;
			SN_Bone * pbone=this->pBones;
			length+=sizeof (SN_Skin); 
			for     (int i =0; i<this->iNumBones; i++, pbone++)
			{
				length+=pbone->Save(fp); 
			}
			fclose (fp);
			return length; 
		}; 
	public:
		int			  iNumVertex; 
		int			  iNumBones ;
		SN_Bone		* pBones;			// the table of bones 
		SN_Vertex	* pVertexCopy ;		// the copy of the target mesh vertex 
	};

	//============================================================================================
	//								:
	//============================================================================================
	struct Ani_Key
	{
		long		Nb_Frame; 
		SN_Matrix3	tm;
		SN_Point3	Scale ; 
		SN_Point3	Position; 
		SN_Quat		Rotation ; 	
		DWORD 		Param1; // these two param leave here to support other kind of animation .
		DWORD 		Param2; //
	}; 


	class	  SN_Kine_Ani_Object: public SN_Deform_Object 
	{
	public:
		SN_Kine_Ani_Object()
		{
			this->type=OBJ_K_ANI_OBJECT; 
			this->Number_Ani_Keys=0; 
			this->p_Ani_Keys=0; 
		};
		~SN_Kine_Ani_Object()
		{
			if(this->p_Ani_Keys) delete ( this->p_Ani_Keys);
		};
		SN_Object * Load			( long address , unsigned char  ** ppData )
		{
			SN_Kine_Ani_Object *	pt; 
			SN_Kine_Ani_Object *	ps ; 
			Ani_Key			   *	pas;
			Ani_Key			   *	pat;
			int				   id=this->Unique_ID;
			// init the  target  
			ps =	(SN_Kine_Ani_Object  *) (*ppData); 
			pt =    this;
			*pt=*ps;
			(*ppData)+=sizeof (SN_Kine_Ani_Object);

			// copy the   Ani key 
			pas=(Ani_Key *)(*ppData); 
			pat=(Ani_Key *)malloc ( sizeof(Ani_Key)*pt->Number_Ani_Keys); 
			pt->p_Ani_Keys=pat;
			for ( int  i=0; i< pt->Number_Ani_Keys;  i++,pas++,pat++)
			{
				*pat=*pas;
				(*ppData)+=sizeof (Ani_Key);
			}

			//add address
			this->Index  +=address    ;
			if (this->father!=0 && this->father!=-1)	
				this->father +=address;
			this->Target+=address;
			this->Unique_ID=id;
			return (SN_Object * )pt;
		}; 
		virtual     int		   Save	( char * filename )
		{
			int length=0;
			FILE * fp=fopen (filename , "a+b"); 
			SN_Kine_Ani_Object * pAni =this;
			length+=(int)fwrite	(pAni,1, sizeof (SN_Kine_Ani_Object ), fp); 
			length+=(int)fwrite  (pAni->p_Ani_Keys, 1, sizeof (Ani_Key)*(pAni->Number_Ani_Keys),fp);
			fclose (fp); 
			return length; 
		}; 
		int	Number_Ani_Keys ; 
		Ani_Key * p_Ani_Keys ; 
	};
	//============================================================================================
	//								:
	//============================================================================================
	class   SN_Particle_System_Object :public SN_Geom_Object
	{
		public:
			SN_Particle_System_Object(){}; 
			virtual ~SN_Particle_System_Object (){}; 
		public:
			int				ParticleCount; 
			SNVALUE			Gravity ; 
			void *			m_pFirst_Particle; 
			void *			m_pLast_Particle;
	};

	//============================================================================================
	//								:
	//============================================================================================
	 static const unsigned int  SN_CURRENT_VERSION=0x00010021;
	class  SN_FILE_HEAD 
	{
	public :
		SN_FILE_HEAD ()
		{
			this->version=SN_CURRENT_VERSION;
			this->Indentify[0]='S'; 
			this->Indentify[1]='N'; 
		};
		void Save (char * filename )
		{
			FILE *	pStream=fopen (filename,"r+b");
			fseek   (pStream,0,0); 
			fwrite  (this, 1,sizeof ( SN_FILE_HEAD), pStream); 
			fclose  ( pStream) ; 
		}; 
		char Indentify [2];  // "SN" is the indentify 
		long FileSize ;      // in char 
		long version; 
		long ObjectCount;
		char Information[256]; 
		
	};
	
	class  SN_File  
	{
	public:
		// data member 
		FILE * fp; 
		unsigned char * buffer ; 
		unsigned char * cu_point; 
		int				g_Object_Number;
		SN_Object *		g_pObject_Table[65536];
		char			name[256]; 
		SN_FILE_HEAD	filehead ; 

		SN_File()
		{
			SN_ZERO_ME;
		};
		SN_File(const char * in_name)
		{
			SN_ZERO_ME;
			strncpy(name,in_name,255);
		};
		~SN_File()
		{
			for ( int i=0; i<g_Object_Number; i++)
			{
				if (g_pObject_Table[i]) delete g_pObject_Table[i];
				g_pObject_Table[i]=NULL;
			}
		};
		int		Read_To_Memory()
		{
			fp=fopen (this ->name, "rb");
			if	 (!	fp )	return 0;
			int Old_Object_Count=g_Object_Number;
			fread		  ( &	(this->filehead),sizeof (SN_FILE_HEAD),1,fp);	
			this->buffer = (unsigned char * )new (char[	this->filehead.FileSize]); 
			this->cu_point=this->buffer; 
			fread		(	this->buffer ,this->filehead.FileSize, sizeof (char), fp); 
			while ( this->cu_point < this->buffer+this->filehead.FileSize )
			{
				SN_Object * pso; 
				pso=(SN_Object *)this->cu_point;
				int itemp=g_Object_Number; 
				switch (pso-> type  )
				{
					case OBJ_MESH_OBJECT:
						 g_pObject_Table[ g_Object_Number ] =new (SN_Mesh_Object);
						 g_pObject_Table[ g_Object_Number ]->Load(Old_Object_Count,&this->cu_point);
						 g_Object_Number++;
						 break; 
					
					case OBJ_MAT_OBJECT:
						 g_pObject_Table[ g_Object_Number ] =new (SN_Material);
						 g_pObject_Table[ g_Object_Number ]->Load(Old_Object_Count,&this->cu_point);
						 g_Object_Number++;
						break; 

					case OBJ_K_ANI_OBJECT:
						g_pObject_Table[ g_Object_Number ]	 =new (SN_Kine_Ani_Object);
						g_pObject_Table[ g_Object_Number ]->Load(Old_Object_Count,&this->cu_point);
						g_Object_Number++;
						break; 

					case OBJ_NULL_OBJECT: 
						g_pObject_Table[ g_Object_Number ]	 =new (SN_Object);
						g_pObject_Table[ g_Object_Number ]->Load(Old_Object_Count,&this->cu_point);
						g_Object_Number++;
						break;

					case OBJ_BASIS_OBJECT: 
						g_pObject_Table[ g_Object_Number ] =new (SN_Object);
						g_pObject_Table[ g_Object_Number ]->Load(Old_Object_Count,&this->cu_point);
						g_Object_Number++;
						break;

					case OBJ_PARTICLE_SYSTEM_OBJECT:
						g_pObject_Table[ g_Object_Number ] =new (SN_Particle_System_Object);
						g_pObject_Table[ g_Object_Number ]->Load(Old_Object_Count,&this->cu_point);
						g_Object_Number++;
						break;

					case SN_SKIN_OBJECT:
						g_pObject_Table[ g_Object_Number ] =new (SN_Skin);
						g_pObject_Table[ g_Object_Number ]->Load(Old_Object_Count,&this->cu_point);
						g_Object_Number++;
						break;

					case OBJ_CAM_OBJECT:
						 g_pObject_Table[ g_Object_Number ] =new (SN_Camera_Object);
						 g_pObject_Table[ g_Object_Number ]->Load(Old_Object_Count,&this->cu_point);
						 g_Object_Number++;
						break; 

					default :
		 					break;
				}
			}//next object
			delete []this->buffer ; 
			this->cu_point=0;
			return 1;
		}; 
	};
};
